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

 private:
  Config();
  bool parse(const char* data);
  QVector<Dictionary> dictionaries_;
};

#endif  // CONFIG_H
