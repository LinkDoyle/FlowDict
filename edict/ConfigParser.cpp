#include "ConfigParser.h"

#include <QFile>
#include <QByteArray>
#include <QDebug>

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

const char* kDefaultDefinitionPattern = ".*?<br>(.*?\\[.*?\\].*?)?(.*)";
using namespace rapidjson;

Config::Config()
    : conciseDictIndex_(0),
      conciseDictRegex_(kDefaultDefinitionPattern),
      searchOptions{0},
      historyMaxCount(20) {}

Config::~Config() {}

Config& Config::Get() {
  static Config config;
  return config;
}

bool Config::load(const QString& filename) {
  QFile file(filename);
  if (!file.exists()) {
    // load and dump the default configuration.
    dump(filename);
  }
  if (!file.open(QIODevice::ReadOnly)) return false;
  return parse(file.readAll().data());
}

bool Config::dump(const QString& filename) const {
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly)) return false;
  Document document;
  Document::AllocatorType& allocator = document.GetAllocator();
  document.SetObject();

  // For Dictionaries
  {
    Value dictionaries;
    dictionaries.SetArray();
    for (auto dict : dictionaries_) {
      Value v;
      v.SetObject();
      v.AddMember(Value("title", allocator).Move(),
                  Value(dict.title.toUtf8().constData(), allocator).Move(),
                  allocator);
      v.AddMember(Value("path", allocator).Move(),
                  Value(dict.path.toUtf8().constData(), allocator).Move(),
                  allocator);
      v.AddMember(Value("type", allocator).Move(),
                  Value(dict.type.toUtf8().constData(), allocator).Move(),
                  allocator);
      dictionaries.PushBack(v, allocator);
    }
    document.AddMember(Value("dictionaries", allocator).Move(),
                       Value(dictionaries, allocator).Move(), allocator);
  }

  // For Concise Dictionary
  {
    Value conciseDict;
    conciseDict.SetObject();
    conciseDict.AddMember(Value("index", allocator).Move(),
                          Value(conciseDictIndex_).Move(), allocator);
    conciseDict.AddMember(
        Value("regex", allocator).Move(),
        Value(conciseDictRegex_.toUtf8().constData(), allocator).Move(),
        allocator);
    document.AddMember(Value("conciseDict", allocator).Move(), conciseDict,
                       allocator);
  }

  // For GUI
  {
    document.AddMember(Value("BasicStyle", allocator).Move(),
                       Value(basicStyle.toUtf8().constData(), allocator).Move(),
                       allocator);
  }

  // For Search Options
  {
    Value options;
    options.SetObject();
    options.AddMember(Value("autoCorrect", allocator).Move(),
                      Value().SetBool(searchOptions.autoCorrect), allocator);
    options.AddMember(Value("caseSensitive", allocator).Move(),
                      Value().SetBool(searchOptions.caseSensitive), allocator);
    options.AddMember(Value("reverseLookup", allocator).Move(),
                      Value().SetBool(searchOptions.reverseLookup), allocator);
    options.AddMember(Value("showSimpleDefinition", allocator).Move(),
                      Value().SetBool(searchOptions.showSimpleDefinition),
                      allocator);
    options.AddMember(Value("wildcardEnable", allocator).Move(),
                      Value().SetBool(searchOptions.wildcardEnable), allocator);
    document.AddMember(Value("SearchOptions", allocator).Move(), options,
                       allocator);
  }

  // For History
  {
    document.AddMember(Value("HistoryMaxCount", allocator).Move(),
                       Value().SetUint(historyMaxCount), allocator);
    Value historyArray;
    historyArray.SetArray();
    for (const QString& key : history) {
      historyArray.PushBack(Value(key.toUtf8().constData(), allocator),
                            allocator);
    }
    document.AddMember(Value("History", allocator).Move(), historyArray.Move(),
                       allocator);
  }

  StringBuffer buffer;
  PrettyWriter<StringBuffer> writer(buffer);
  document.Accept(writer);
  file.write(buffer.GetString(), buffer.GetSize());
  file.close();
  return true;
}

QVector<Config::Dictionary>& Config::getDictionaries() { return dictionaries_; }

int Config::conciseDictIndex() const { return conciseDictIndex_; }

const QString& Config::conciseDictRegex() const { return conciseDictRegex_; }

void Config::setConciseDictIndex(int index) { conciseDictIndex_ = index; }

void Config::setConciseDictRegex(const QString& pattern) {
  conciseDictRegex_ = pattern;
}

bool Config::parse(const char* data) {
  dictionaries_.clear();

  Document document;
  document.Parse(data);
  if (!document.IsObject()) return false;

  // For Dictionaries
  Value::ConstMemberIterator itr = document.FindMember("dictionaries");
  if (itr == document.MemberEnd()) return false;
  const Value& dictionaries = itr->value;
  if (!dictionaries.IsArray()) return false;
  for (const auto& dict : dictionaries.GetArray()) {
    if (!dict.IsObject()) return false;
    Dictionary dst;

    Value::ConstMemberIterator itr = dict.FindMember("title");
    if (itr == dict.MemberEnd()) return false;
    const Value& title = itr->value;
    if (!title.IsString()) return false;
    dst.title = QString::fromUtf8(title.GetString(), title.GetStringLength());

    itr = dict.FindMember("path");
    if (itr == dict.MemberEnd()) return false;
    const Value& path = itr->value;
    if (!path.IsString()) return false;
    dst.path = QString::fromUtf8(path.GetString(), path.GetStringLength());

    itr = dict.FindMember("type");
    if (itr == dict.MemberEnd()) return false;
    const Value& type = itr->value;
    if (!type.IsString()) return false;
    dst.type = QString::fromUtf8(type.GetString(), type.GetStringLength());

    dictionaries_.append(dst);
  }

  // For Concsise Dictary
  itr = document.FindMember("conciseDict");
  if (itr != document.MemberEnd()) {
    const Value& conciseDict = itr->value;
    if (!conciseDict.IsObject()) return false;

    itr = conciseDict.FindMember("index");
    if (itr == conciseDict.MemberEnd()) return false;
    const Value& index = itr->value;
    if (!index.IsInt()) return false;
    conciseDictIndex_ = index.GetInt();
    if (conciseDictIndex_ < 0) conciseDictIndex_ = 0;

    itr = conciseDict.FindMember("regex");
    if (itr == conciseDict.MemberEnd()) return false;
    const Value& pattern = itr->value;
    if (!pattern.IsString()) return false;
    conciseDictRegex_ =
        QString::fromUtf8(pattern.GetString(), pattern.GetStringLength());
  }

  // For GUI

  itr = document.FindMember("BasicStyle");
  if (itr != document.MemberEnd()) {
    const Value& value = itr->value;
    if (!value.IsString()) return false;
    basicStyle = QString::fromUtf8(value.GetString(), value.GetStringLength());
  }

  // For Search Options
  itr = document.FindMember("SearchOptions");
  if (itr != document.MemberEnd()) {
    const Value& options = itr->value;
    if (!options.IsObject()) return false;

    itr = options.FindMember("autoCorrect");
    if (itr == options.MemberEnd()) return false;
    const Value& autoCorrect = itr->value;
    if (!autoCorrect.IsBool()) return false;
    searchOptions.autoCorrect = autoCorrect.GetBool();

    itr = options.FindMember("caseSensitive");
    if (itr == options.MemberEnd()) return false;
    const Value& caseSensitive = itr->value;
    if (!caseSensitive.IsBool()) return false;
    searchOptions.caseSensitive = caseSensitive.GetBool();

    itr = options.FindMember("reverseLookup");
    if (itr == options.MemberEnd()) return false;
    const Value& reverseLookup = itr->value;
    if (!reverseLookup.IsBool()) return false;
    searchOptions.autoCorrect = reverseLookup.GetBool();

    itr = options.FindMember("showSimpleDefinition");
    if (itr == options.MemberEnd()) return false;
    const Value& showSimpleDefinition = itr->value;
    if (!showSimpleDefinition.IsBool()) return false;
    searchOptions.autoCorrect = showSimpleDefinition.GetBool();

    itr = options.FindMember("wildcardEnable");
    if (itr == options.MemberEnd()) return false;
    const Value& wildcardEnable = itr->value;
    if (!wildcardEnable.IsBool()) return false;
    searchOptions.autoCorrect = wildcardEnable.GetBool();
  }

  // For History
  itr = document.FindMember("HistoryMaxCount");
  if (itr != document.MemberEnd() && itr->value.IsUint()) {
    historyMaxCount = itr->value.GetUint();
  }
  itr = document.FindMember("History");
  if (itr != document.MemberEnd()) {
    const Value& array = itr->value;
    if (array.IsArray()) {
      for (const auto& key : array.GetArray()) {
        if (key.IsString())
          history.append(
              QString::fromUtf8(key.GetString(), key.GetStringLength()));
      }
    }
  }

  return true;
}

Config::Dictionary::Dictionary() {}

Config::Dictionary::Dictionary(const QString& title, const QString& type,
                               const QString& path)
    : title(title), type(type), path(path) {}

bool Config::Dictionary::operator==(const Config::Dictionary& rhs) const {
  return rhs.path == this->path && rhs.title == this->title &&
         rhs.type == this->type;
}
