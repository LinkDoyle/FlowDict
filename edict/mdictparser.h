// https://bitbucket.org/xwang/mdict-analysis
// https://github.com/zhansliu/writemdict/blob/master/fileformat.md
// https://github.com/goldendict/goldendict/blob/master/mdictparser.cc
// Octopus MDict Dictionary File (.mdx)and Resource File (.mdd)Analyser

#ifndef MDICTPARSER_H
#define MDICTPARSER_H

#include <string>
#include <vector>
#include <map>
#include <utility>

#include <QPointer>
#include <QString>
#include <QFile>

#include "datrie.h"

namespace MDict {
using std::string;
using std::vector;
using std::pair;
using std::map;

// A helper class to handle memory map for QFile
class ScopedMemMap {
  QFile& file;
  uchar* address;

 public:
  ScopedMemMap(QFile& file, qint64 offset, qint64 size)
      : file(file), address(file.map(offset, size)) {}

  ~ScopedMemMap() {
    if (address) file.unmap(address);
  }

  inline uchar* startAddress() { return address; }
};

class MdictParser {
 public:
  enum { kParserVersion = 0x000000d };

  struct RecordIndex {
    qint64 startPos;
    qint64 endPos;
    qint64 shadowStartPos;
    qint64 shadowEndPos;
    qint64 compressedSize;
    qint64 decompressedSize;

    inline bool operator==(qint64 rhs) const {
      return (shadowStartPos <= rhs) && (rhs < shadowEndPos);
    }

    inline bool operator<(qint64 rhs) const { return shadowEndPos <= rhs; }

    inline bool operator>(qint64 rhs) const { return shadowStartPos > rhs; }

    static size_t bsearch(vector<RecordIndex> const& offsets, qint64 val);
  };

  struct RecordInfo {
    qint64 compressedBlockPos;
    qint64 recordOffset;

    qint64 decompressedBlockSize;
    qint64 compressedBlockSize;
    qint64 recordSize;
  };

  class RecordHandler {
   public:
    virtual void handleRecord(QString const& name,
                              RecordInfo const& recordInfo) = 0;
  };

  typedef vector<pair<qint64, qint64> > BlockInfoVector;
  typedef vector<pair<qint64, QString> > HeadWordIndex;
  typedef map<qint32, pair<QString, QString> > StyleSheets;

  inline QString const& title() const { return title_; }

  inline QString const& description() const { return description_; }

  inline StyleSheets const& styleSheets() const { return styleSheets_; }

  inline quint32 wordCount() const { return wordCount_; }

  inline QString const& encoding() const { return encoding_; }

  inline QString const& filename() const { return filename_; }

  inline bool isRightToLeft() const { return rtl_; }

  MdictParser();
  ~MdictParser();

  bool open(const QString& filename);
  bool readNextHeadWordIndex(HeadWordIndex& headWordIndex);
  bool readRecordBlock(HeadWordIndex& headWordIndex,
                       RecordHandler& recordHandler);

  // helpers
  static QString toUtf16(const char* fromCode, const char* from,
                         size_t fromSize);
  static inline QString toUtf16(QString const& fromCode, const char* from,
                                size_t fromSize) {
    return toUtf16(fromCode.toLatin1().constData(), from, fromSize);
  }
  static bool parseCompressedBlock(qint64 compressedBlockSize,
                                   const char* compressedBlockPtr,
                                   qint64 decompressedBlockSize,
                                   QByteArray& decompressedBlock);
  static QString& substituteStylesheet(QString& article,
                                       StyleSheets const& styleSheets);
  // static inline string substituteStylesheet(string const & article,
  // StyleSheets const & styleSheets )
  // {
  //   QString s = QString::fromUtf8(article.c_str());
  //   substituteStylesheet(s, styleSheets );
  //   return string(s.toUtf8().constData());
  // }
 private:
  qint64 readNumber(QDataStream& in);
  static quint32 readU8OrU16(QDataStream& in, bool isU16);
  static bool checkAdler32(const char* buffer, unsigned int len,
                           quint32 checksum);
  static bool decryptHeadWordIndex(char* buffer, qint64 len);
  bool readHeader(QDataStream& in);
  bool readHeadWordBlockInfos(QDataStream& in);
  bool readRecordBlockInfos();
  BlockInfoVector decodeHeadWordBlockInfo(QByteArray const& headWordBlockInfo);
  HeadWordIndex splitHeadWordBlock(QByteArray const& block);

 private:
  QString filename_;
  QPointer<QFile> file_;
  StyleSheets styleSheets_;
  BlockInfoVector headWordBlockInfos_;
  BlockInfoVector::iterator headWordBlockInfosIter_;
  vector<RecordIndex> recordBlockInfos_;

  QString encoding_;
  QString title_;
  QString description_;

  double version_;
  qint64 numHeadWordBlocks_;
  qint64 headWordBlockInfoSize_;
  qint64 headWordBlockSize_;
  qint64 headWordBlockInfoPos_;
  qint64 headWordPos_;
  qint64 totalRecordsSize_;
  qint64 recordPos_;

  quint32 wordCount_;
  int numberTypeSize_;
  int encrypted_;
  bool rtl_;
};

}
#endif  // MDICTPARSER_H
