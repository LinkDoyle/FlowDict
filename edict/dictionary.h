#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <QVector>
#include <QWidget>
#include <QSharedPointer>
#include <QString>

namespace Dictionary {

enum class State {
  OK = 0,
  Fail = -1,
  NoCache = -2
};

class IDictionary {
 public:
  struct Info {
    QString path;
    QString title;
    QString type;
  };
  IDictionary() = default;
  virtual ~IDictionary() = default;
  virtual void getArticleText(const QString& headword, QString& text) const = 0;
  virtual const Info& info() const = 0;
  virtual QStringList keysWithPrefix(const QString &prefix, int max = -1) const = 0;
};

QVector<QSharedPointer<IDictionary>>& Get();
void Load(QWidget* parent);
}

#endif  // DICTIONARY_H
