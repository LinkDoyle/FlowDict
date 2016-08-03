#ifndef DICTIONARY_H
#define DICTIONARY_H

class QString;
namespace Dictionary {

class IDictionary {
 public:
  IDictionary();
  virtual void getArticleText(const QString& headword, QString& text) = 0;
};
}

#endif  // DICTIONARY_H
