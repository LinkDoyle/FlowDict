#ifndef MDICT_H
#define MDICT_H
#include "dictionary.h"
#include <memory>
#include <functional>
#include <QString>
namespace MDict {
QSharedPointer<Dictionary::IDictionary> makeDirectory(
    const QString &filename, std::function<void(int, int)> callback);
}

#endif  // MDICT_H
