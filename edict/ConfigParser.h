#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <QString>
#include <QVector>

class Config {
 public:
  static Config& Get();

  struct Dictionary {
    Dictionary();
    Dictionary(const QString& title, const QString& type, const QString& path);
    bool operator==(const Dictionary& rhs) const;
    QString title;
    QString type;
    QString path;
  };
  Config(const Config& rhs) = delete;
  Config& operator=(const Config& rhs) = delete;
  ~Config();

  bool load(const QString& filename);
  bool dump(const QString& filename) const;
  QVector<Dictionary>& getDictionaries();
  int conciseDictIndex() const;
  const QString& conciseDictRegex() const;
  void setConciseDictIndex(int);
  void setConciseDictRegex(const QString& pattern);

  QString basicStyle;
  uint32_t historyMaxCount;
  QStringList history;
  struct SearchOptions {
    bool autoCorrect : 1;
    bool caseSensitive : 1;
    bool reverseLookup : 1;
    bool showSimpleDefinition : 1;
    bool wildcardEnable : 1;
  } searchOptions;

 private:
  Config();
  bool parse(const char* data);
  QVector<Dictionary> dictionaries_;
  int conciseDictIndex_;
  QString conciseDictRegex_;
};

#endif  // CONFIG_H
