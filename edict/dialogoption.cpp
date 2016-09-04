#include "dialogoption.h"
#include "ui_dialogoption.h"

#include <QApplication>
#include <QString>
#include <QStyleFactory>

#include "ConfigParser.h"
#include "dictionary.h"

DialogOption::DialogOption(QWidget* parent)
    : QDialog(parent), ui(new Ui::DialogOption) {
  ui->setupUi(this);
  layout()->setSizeConstraint(QLayout::SetFixedSize);
  const Config& config = Config::Get();

  QString currentStyle = QApplication::style()->objectName();
  QStringList styles = QStyleFactory::keys();
  for(const QString& style : styles) {
    ui->cbStyle->addItem(style);
    if(currentStyle.compare(style, Qt::CaseInsensitive) == 0)
      ui->cbStyle->setCurrentText(style);
  }
  const auto& dictionaries = Dictionary::Get();
  if (!dictionaries.isEmpty()) {
    for (const auto& dict : dictionaries) {
      ui->cbConciseDict->addItem(dict->info().title);
    }
    ui->cbConciseDict->setCurrentIndex(config.conciseDictIndex());
    ui->lineEdit->setText(config.conciseDictRegex());
  } else {
    ui->groupBox->setEnabled(false);
  }

  ui->sbHistoryMaxCount->setValue(config.historyMaxCount);
}

DialogOption::~DialogOption() { delete ui; }

void DialogOption::on_cbStyle_activated(const QString &style)
{
  QApplication::setStyle(style);
}

void DialogOption::on_DialogOption_accepted() {
  Config& config = Config::Get();
  config.setConciseDictIndex(ui->cbConciseDict->currentIndex());
  config.setConciseDictRegex(ui->lineEdit->text());
  config.basicStyle = ui->cbStyle->currentText();
  config.historyMaxCount = ui->sbHistoryMaxCount->value();
}

void DialogOption::on_btnCleanHistory_clicked()
{
    Config::Get().history.clear();
}
