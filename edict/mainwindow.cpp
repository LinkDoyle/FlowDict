#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFile>
#include <QTime>
#include <QMessageBox>
#include <QString>
#include <QCloseEvent>

#include "dialogabout.h"
#include "dictmanager.h"
#include "MdictParser.h"
#include "ConfigParser.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      dictWebPage_(new DictWebPage) {
  ui->setupUi(this);

  connect(dictWebPage_, &DictWebPage::linkClicked, this,
          &MainWindow::on_dictWebPage_linkClicked);
  ui->webEngineView->setPage(dictWebPage_);

  Config& config = Config::Get();
  if (!config.load("config.json"))
    QMessageBox::warning(this, QStringLiteral("警告"),
                         QStringLiteral("读取配置文件 config.json 失败!"));
  for (const auto& dict : config.getDictionaries()) {
    if (!reader_.loadFile(dict.path)) {
      QMessageBox::warning(
          this, QStringLiteral("警告"),
          QStringLiteral("读取词典 \"%1\" 失败!").arg(dict.path));
    }
    break;
  }
}

MainWindow::~MainWindow() {
  delete ui;
  delete dictWebPage_;
}

void MainWindow::on_action_A_triggered() {
  DialogAbout* dialog = new DialogAbout;
  dialog->setAttribute(Qt::WA_DeleteOnClose);
  dialog->exec();
}

void MainWindow::on_action_M_triggered() {
  DictManager* dialog = new DictManager;
  dialog->setAttribute(Qt::WA_DeleteOnClose);
  dialog->exec();
}

void MainWindow::closeEvent(QCloseEvent* event) {
  Config& config = Config::Get();
  if (!config.dump("config.json"))
    QMessageBox::warning(this, QStringLiteral("警告"),
                         "保存配置文件 config.json 失败!");

  event->accept();
}

void MainWindow::on_dictWebPage_linkClicked(const QUrl& url) {
  // TODO: word through url
  int pos = ui->comboBox->findText(url.host());
  if (pos == -1) {
    ui->comboBox->addItem(url.host());
    pos = ui->comboBox->count() - 1;
  }
  ui->comboBox->setCurrentIndex(pos);
}

void MainWindow::on_comboBox_currentIndexChanged(const QString& text) {
  statusBar()->showMessage(QStringLiteral("查询中..."));
  QTime time;
  time.start();

  QString article;
  reader_.getArticleText(text, article);
  QString html = "<html><body>";
  html += article;
  html += "</body></html>";
  dictWebPage_->setHtml(html);

  QString info = QString("Time used:%1ms").arg(time.elapsed());
  statusBar()->showMessage(info);
}
