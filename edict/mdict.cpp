#include "mdict.h"
#include <clocale>
#include <functional>
#include <algorithm>
#include <QTextCodec>
#include <QVector>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include "datrie.h"
#include "mdictparser.h"
namespace MDict {

class Alphabet {
 public:
  static uint32_t toIndex(char c) {
    assert(c >= ' ');
    assert(static_cast<unsigned char>(c) < 255);
    return static_cast<unsigned char>(c) - ' ';
  }
  static char toChar(uint32_t index) { return index + ' '; }
  static uint32_t begin() { return 0; }
  static uint32_t end() { return static_cast<unsigned char>(0xFF) - ' ' + 1; }
};

class MDictReader : public Dictionary::IDictionary {
 public:
  MDictReader();
  virtual void getArticleText(const QString &headword,
                              QString &text) const override;
  virtual const Info &info() const override { return info_; }
  virtual ~MDictReader();

  virtual QStringList keysWithPrefix(const QString &prefix, int max) const override;
  bool loadFile(const QString &filename,
                std::function<void(int, int)> callback);

 private:
  Info info_;
  QPointer<QFile> file_;
  QPointer<QFile> cache_file_;
  QString encoding_;
  uchar *index_cache_address_;
  DATrieReader<char, Alphabet> index_reader_;
  MdictParser::StyleSheets styleSheets_;
  const MdictParser::RecordInfo *recordInfos_;
};

bool createDirectoryCache(const QString &dict_filename,
                          const QString &cache_filename,
                          std::function<void(int, int)> callback);
class ArticleHandler : public MDict::MdictParser::RecordHandler {
 public:
  ArticleHandler(DATrieWriter<char, Alphabet> &writer,
                 std::vector<MDict::MdictParser::RecordInfo> &recordInfos,
                 std::function<void(void)> callback)
      : writer_(writer), recordInfos_(recordInfos), callback_(callback) {}

  virtual void handleRecord(QString const &headWord,
                            MDict::MdictParser::RecordInfo const &recordInfo) {
    QVector<unsigned int> v = headWord.toUcs4();
    // Fix for QString instance which contains non-BMP characters
    // Qt will created unexpected null characters may confuse btree indexer.
    // Related: https://bugreports.qt-project.org/browse/QTBUG-25536
    int n = v.size();
    while (n > 0 && v[n - 1] == 0) n--;
    if (n != v.size()) v.resize(n);
    QString keyword = QString::fromUcs4(v.constData(), v.size()).trimmed();

    writer_.insert(keyword.toUtf8().constData(), recordInfos_.size());
    recordInfos_.push_back(recordInfo);

    if (callback_) callback_();
  }

 private:
  DATrieWriter<char, Alphabet> &writer_;
  std::vector<MDict::MdictParser::RecordInfo> &recordInfos_;
  std::function<void(void)> callback_;
};

MDictReader::MDictReader()
    : file_(nullptr),
      cache_file_(nullptr),
      index_cache_address_(nullptr),
      recordInfos_(nullptr) {}

MDictReader::~MDictReader() {
  if (!file_.isNull() && cache_file_) {
    file_->unmap(index_cache_address_);
  }
}
bool MDictReader::loadFile(const QString &filename,
                           std::function<void(int, int)> callback) {
  file_ = new QFile(filename);
  QFileInfo fileinfo(*file_.data());
  QString cache_filename = QDir::currentPath();
  cache_filename += QStringLiteral("/dict/");
  cache_filename += fileinfo.fileName();
  cache_filename += QStringLiteral(".idx");

  cache_file_ = new QFile(cache_filename);
  QDir().mkpath(QFileInfo(*cache_file_.data()).absolutePath());

  MDict::MdictParser parser;
  if (!parser.open(filename)) return false;
  encoding_ = parser.encoding();

  if (!file_->open(QIODevice::ReadOnly)) return false;
  if (!cache_file_->exists())
    createDirectoryCache(filename, cache_filename, callback);
  if (!cache_file_->open(QIODevice::ReadOnly)) return false;
  index_cache_address_ = cache_file_->map(0, cache_file_->size());
  if (index_reader_.loadFromMemory(
          reinterpret_cast<char *>(index_cache_address_)) != 0)
    return false;
  recordInfos_ = reinterpret_cast<const MdictParser::RecordInfo *>(
      index_reader_.getExtraData());

  styleSheets_ = std::move(parser.styleSheets());
  info_.path = filename;
  info_.title = parser.title();
  info_.type = "mdx";
  return true;
}

void MDictReader::getArticleText(const QString &headword, QString &text) const {
  uint32_t index;
  if (index_reader_.retrieve(headword.toUtf8().constData(), index) == -1)
    return;
  const MDict::MdictParser::RecordInfo &recordInfo = recordInfos_[index];
  MDict::ScopedMemMap compressed(*file_.data(), recordInfo.compressedBlockPos,
                                 recordInfo.compressedBlockSize);
  QByteArray decompressed;
  MDict::MdictParser::parseCompressedBlock(
      recordInfo.compressedBlockSize, (char *)compressed.startAddress(),
      recordInfo.decompressedBlockSize, decompressed);

  text = MdictParser::toUtf16(
      encoding_, decompressed.constData() + recordInfo.recordOffset,
      recordInfo.recordSize);

  text = MdictParser::substituteStylesheet(text, styleSheets_);
}

QStringList MDictReader::keysWithPrefix(const QString &prefix, int max) const {
  auto keys = index_reader_.keysWithPrefix(prefix.toUtf8().constData(), max);
  QStringList stringList;
  for (auto key : keys) {
    stringList.push_back(QString::fromStdString(key));
  }
  return stringList;
}
QSharedPointer<Dictionary::IDictionary> makeDirectory(
    const QString &filename, std::function<void(int, int)> callback) {
  QSharedPointer<MDictReader> reader = QSharedPointer<MDictReader>::create();
  if (!reader->loadFile(filename, callback))
    return QSharedPointer<Dictionary::IDictionary>(nullptr);
  return reader;
}

bool createDirectoryCache(const QString &dict_filename,
                          const QString &cache_filename,
                          std::function<void(int, int)> callback) {
  MDict::MdictParser parser;
  if (!parser.open(dict_filename)) return false;

  DATrieWriter<char, Alphabet> writer;
  std::vector<MdictParser::RecordInfo> recordInfos;
  int currentCount = 0, totalCount = parser.wordCount();
  ArticleHandler articleHandler(writer, recordInfos, [&]() {
    callback(std::min(++currentCount, totalCount), totalCount);
  });
  MdictParser::HeadWordIndex headWordIndex;

  // enumerating word and its definition
  while (parser.readNextHeadWordIndex(headWordIndex)) {
    parser.readRecordBlock(headWordIndex, articleHandler);
  }

  writer.dumpToFile(cache_filename.toLocal8Bit().constData(),
                    reinterpret_cast<const char *>(recordInfos.data()),
                    recordInfos.size() * sizeof(MdictParser::RecordInfo));

  return true;
}
}
