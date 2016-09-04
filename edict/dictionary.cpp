#include "dictionary.h"
#include <QWidget>
#include <QSharedPointer>
#include <QMessageBox>
#include <QApplication>
#include <QProgressDialog>
#include <QTime>
#include "ConfigParser.h"
#include "mdict.h"

namespace Dictionary {
static QVector<QSharedPointer<IDictionary>> Dictionaries;
QVector<QSharedPointer<IDictionary>>& Get() { return Dictionaries; }

QSharedPointer<IDictionary> GetConciseDict() {
  if(Dictionaries.isEmpty()) return QSharedPointer<IDictionary>{};
  return Dictionaries[Config::Get().conciseDictIndex()];
}

void Load(QWidget* parent) {
  const auto config_dictionaries = Config::Get().getDictionaries();
  Dictionaries.clear();
  QProgressDialog* dialog = nullptr;
  QTime time;
  time.start();
  for (const Config::Dictionary& d : config_dictionaries) {
    QSharedPointer<Dictionary::IDictionary> dict =
        MDict::makeDirectory(d.path, [&](int currentCount, int totalCount) {
          if (!dialog) {
            dialog = new QProgressDialog;
            dialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint);
            dialog->setMinimumWidth(400);
            dialog->setWindowTitle(QStringLiteral("Flow Dict"));
            dialog->setWindowModality(Qt::ApplicationModal);
            dialog->setModal(true);
            dialog->setRange(0, 101);
            dialog->setCancelButton(nullptr);
            dialog->show();
          }
          dialog->setLabelText(
              QStringLiteral(
                  "正在更新词典\"%1\"的索引缓存（当前已花时：%2 s）...")
                  .arg(d.title,
                       QString().setNum(time.elapsed() / 1000.f, 'f', 1)));
          dialog->setValue(currentCount * 100 / totalCount);
          QApplication::processEvents();
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
