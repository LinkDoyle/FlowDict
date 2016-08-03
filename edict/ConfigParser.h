#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <QString>
#include <QVector>

class Config {
 public:
  struct Dictionary {
    QString path;
    QString title;
    QString type;
  };
  Config(const Config& rhs) = delete;
  Config& operator=(const Config& rhs) = delete;
  ~Config();
  static Config& Get();
  bool load(const QString& filename);
  bool dump(const QString& filename) const;
  QVector<Dictionary>& getDictionaries();

 private:
  Config();
  bool parse(const char* data);
  QVector<Dictionary> dictionaries_;
};

#endif  // CONFIG_H
