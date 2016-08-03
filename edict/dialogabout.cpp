#include "dialogabout.h"
#include "ui_dialogabout.h"

DialogAbout::DialogAbout(QWidget *parent)
    : QDialog(parent), ui(new Ui::DialogAbout) {
  ui->setupUi(this);
  layout()->setSizeConstraint(QLayout::SetFixedSize);
  ui->label->setText(ui->label->text().arg(__DATE__, __TIME__));
  ui->label->setTextInteractionFlags(Qt::TextBrowserInteraction);
  ui->label->setOpenExternalLinks(true);
}

DialogAbout::~DialogAbout() { delete ui; }
