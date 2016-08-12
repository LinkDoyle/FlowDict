#include "dictionary.h"
#include <QWidget>
#include <QSharedPointer>
#include <QMessageBox>
#include <QCoreApplication>
#include <QProgressDialog>
#include "ConfigParser.h"
#include "mdict.h"

namespace Dictionary {
static QVector<QSharedPointer<IDictionary>> Dictionaries;
QVector<QSharedPointer<IDictionary>>& Get() { return Dictionaries; }

void Load(QWidget* parent) {
  Dictionaries.clear();
  QProgressDialog* dialog = nullptr;
  for (const Config::Dictionary& d : Config::Get().getDictionaries()) {
    QSharedPointer<Dictionary::IDictionary> dict =
        MDict::makeDirectory(d.path, [&](int currentCount, int totalCount) {
          if (!dialog) {
            dialog = new QProgressDialog;
            dialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint);
            dialog->setWindowTitle(QStringLiteral("Flow Dict"));
            dialog->setLabelText(
                QStringLiteral("请稍后，正在更新词典索引缓存..."));
            dialog->setWindowModality(Qt::WindowModal);
            dialog->setRange(0, 100);
            dialog->setCancelButton(nullptr);
            dialog->show();
          }
          dialog->setValue(currentCount * 100 / totalCount);
        });
    if (dict) {
      Dictionaries.push_back(dict);
    } else {
      QMessageBox::warning(parent, QStringLiteral("警告"),
                           QStringLiteral("读取词典 \"%1\" 失败!").arg(d.path));
    }
  }
  if (dialog) delete dialog;
}
}
