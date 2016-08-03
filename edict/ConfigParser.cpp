#include "ConfigParser.h"

#include <QFile>
#include <QByteArray>
#include <QDebug>

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

Config::Config() {}

Config::~Config() {}

Config& Config::Get() {
  static Config config;
  return config;
}

bool Config::load(const QString& filename) {
  QFile file(filename);
  if (file.exists()) {
    if (!file.open(QIODevice::ReadOnly)) return false;
    return parse(file.readAll().data());
  } else {
    // load and dump the default configuration.
    dump(filename);
  }
  return true;
}

bool Config::dump(const QString& filename) const {
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly)) return false;
  Document document;
  document.SetObject();
  Value dictionaries;
  dictionaries.SetArray();
  for (auto dict : dictionaries_) {
    Value v;
    v.SetObject();
    v.AddMember(
        Value("title", document.GetAllocator()).Move(),
        Value(dict.title.toUtf8().constData(), document.GetAllocator()).Move(),
        document.GetAllocator());
    v.AddMember(
        Value("path", document.GetAllocator()).Move(),
        Value(dict.path.toUtf8().constData(), document.GetAllocator()).Move(),
        document.GetAllocator());
    v.AddMember(
        Value("type", document.GetAllocator()).Move(),
        Value(dict.type.toUtf8().constData(), document.GetAllocator()).Move(),
        document.GetAllocator());
    dictionaries.PushBack(v, document.GetAllocator());
  }
  document.AddMember(Value("dictionaries", document.GetAllocator()).Move(),
                     Value(dictionaries, document.GetAllocator()).Move(),
                     document.GetAllocator());
  StringBuffer buffer;
  PrettyWriter<StringBuffer> writer(buffer);
  document.Accept(writer);
  file.write(buffer.GetString(), buffer.GetSize());
  file.close();
  return true;
}

QVector<Config::Dictionary>& Config::getDictionaries() { return dictionaries_; }

bool Config::parse(const char* data) {
  dictionaries_.clear();

  Document document;
  document.Parse(data);
  if (!document.IsObject()) return false;
  if (!document.HasMember("dictionaries")) return false;
  const Value& dictionaries = document["dictionaries"];
  if (!dictionaries.IsArray()) return false;
  for (Value::ConstValueIterator itr = dictionaries.Begin();
       itr != dictionaries.End(); ++itr) {
    auto src = itr->GetObject();
    Dictionary dst;
    if (!src.HasMember("title") || !src["title"].IsString()) return false;
    dst.title = QString::fromUtf8(src["title"].GetString(),
                                  src["title"].GetStringLength());
    if (!src.HasMember("path") || !src["path"].IsString()) return false;
    dst.path = QString::fromUtf8(src["path"].GetString(),
                                 src["path"].GetStringLength());
    if (!src.HasMember("type") || !src["type"].IsString()) return false;
    dst.type = QString::fromUtf8(src["type"].GetString(),
                                 src["type"].GetStringLength());
    dictionaries_.append(dst);
  }
  return true;
}
