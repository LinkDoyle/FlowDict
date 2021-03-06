﻿#include "dictmanager.h"
#include "ui_dictmanager.h"

#include <QAction>
#include <QFileDialog>
#include <QMessageBox>

#include <algorithm>

#include "ConfigParser.h"
#include "dictionary.h"
#include "mdictparser.h"
#include "flowhelper.h"

DictManager::DictManager(QWidget* parent)
    : QDialog(parent),
      ui(new Ui::DictManager),
      exploreAct_(new QAction(QStringLiteral("在资源管理器中显示"))) {
  ui->setupUi(this);

  Config& config = Config::Get();
  const auto& dictionaries = config.getDictionaries();
  QTableWidget* tableWiget = ui->tableWidget;
  tableWiget->setRowCount(dictionaries.size());
  int row = 0;
  for (const auto& dict : dictionaries) {
    tableWiget->setItem(row, 0, new QTableWidgetItem(dict.title));
    tableWiget->setItem(row, 1, new QTableWidgetItem(dict.type));
    tableWiget->setItem(row, 2, new QTableWidgetItem(dict.path));
    ++row;
  }

  QTableWidget* tableWidget = ui->tableWidget;
  connect(exploreAct_, &QAction::triggered, [tableWidget]() {
    const QString& path =
        tableWidget->item(tableWidget->currentItem()->row(), 2)->text();
    FlowHelper::showInFolder(path);
  });
  tableWidget->addAction(exploreAct_);
}
DictManager::~DictManager() {
  delete ui;
  delete exploreAct_;
}

void DictManager::on_toolButton_clicked() {
  QString filename = QFileDialog::getOpenFileName();
  MDict::MdictParser parser;
  if (filename.isEmpty()) return;
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
    tableWiget->setItem(row, 1, new QTableWidgetItem(QStringLiteral("mdx")));
    tableWiget->setItem(row, 2, new QTableWidgetItem(parser.filename()));
  }
}

void DictManager::on_buttonBox_accepted() {
  Config& config = Config::Get();
  QVector<Config::Dictionary>& dictionaries = config.getDictionaries();
  dictionaries.clear();
  QTableWidget* tableWiget = ui->tableWidget;
  for (int i = 0; i < tableWiget->rowCount(); ++i) {
    const QString& title = tableWiget->item(i, 0)->text();
    const QString& type = tableWiget->item(i, 1)->text();
    const QString& path = tableWiget->item(i, 2)->text();
    dictionaries.push_back(Config::Dictionary(title, type, path));
  }
  Dictionary::Load(this);
}

void DictManager::on_tableWidget_cellClicked(int row, int) {
  MDict::MdictParser parser;
  const QString& path = ui->tableWidget->item(row, 2)->text();
  if (!parser.open(path)) {
    ui->textBrowser->setText(
        QStringLiteral("咦？！词库打不开惹，这可咋么办吖~ QvQ"));
  } else if (parser.description().isEmpty()) {
    ui->textBrowser->setText(QStringLiteral(
        "<center>并没有<br>可以<br>值得<br>显示的<br>╮(╯_╰)╭</center>"));
  } else {
    QString info;
    info += QStringLiteral("<center><h1>描述</h1></center>");
    info += parser.description();
    ui->textBrowser->setText(info);
  }
}

void DictManager::on_toolButton_2_clicked() {
  QTableWidget* tableWidget = ui->tableWidget;
  QModelIndexList selections = tableWidget->selectionModel()->selectedRows();
  for (int i = 0; i < selections.count(); ++i) {
    tableWidget->removeRow(selections.at(i).row());
  }
}
