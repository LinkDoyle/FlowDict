#include "dictmanager.h"
#include "ui_dictmanager.h"

#include <QFileDialog>
#include <QMessageBox>

#include "config.h"
#include "ConfigParser.h"
#include "mdictparser.h"

DictManager::DictManager(QWidget* parent)
    : QDialog(parent), ui(new Ui::DictManager) {
  ui->setupUi(this);

  Config& config = Config::Get();
  const auto& dictionaries = config.getDictionaries();
  QTableWidget* tableWiget = ui->tableWidget;
  tableWiget->setRowCount(dictionaries.size());
  int row = 0;
  for (const auto& dict : dictionaries) {
    tableWiget->setItem(row, 0, new QTableWidgetItem(dict.title));
    tableWiget->setItem(row, 1, new QTableWidgetItem(dict.path));
    ++row;
  }
}

DictManager::~DictManager() { delete ui; }

void DictManager::on_toolButton_clicked() {
  QString filename = QFileDialog::getOpenFileName();
  Mdict::MdictParser parser;
  if (!parser.open(filename)) {
    QMessageBox::warning(0, QStringLiteral("导入失败"),
                         QStringLiteral("导入词典\"%1\"失败！").arg(filename));
  } else {
    QString info;
    info += QStringLiteral("导入成功!<br>标题：");
    info += parser.title();
    info += QStringLiteral("<br>描述：");
    info += parser.description();
    QMessageBox::information(0, QStringLiteral("词库信息"), info);

    QTableWidget* tableWiget = ui->tableWidget;
    int row = tableWiget->rowCount();
    tableWiget->setRowCount(row + 1);
    tableWiget->setItem(row, 0, new QTableWidgetItem(parser.title()));
    tableWiget->setItem(row, 1, new QTableWidgetItem(parser.filename()));
  }
}

void DictManager::on_buttonBox_accepted() {
  QTableWidget* tableWiget = ui->tableWidget;
  for (int i = 0; i < tableWiget->rowCount(); ++i) {
    const QString& title = tableWiget->item(i, 0)->text();
    const QString& path = tableWiget->item(i, 0)->text();
  }
}
