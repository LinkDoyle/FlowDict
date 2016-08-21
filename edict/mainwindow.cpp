#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFile>
#include <QTime>
#include <QMessageBox>
#include <QString>
#include <QCloseEvent>
#include <QRegExp>
#include <QWebChannel>
#include "dialogabout.h"
#include "dictmanager.h"
#include "dictionary.h"
#include "ConfigParser.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      dictWebPage_(new DictWebPage),
      webChannel_(new QWebChannel) {
  ui->setupUi(this);

  connect(dictWebPage_, &DictWebPage::linkClicked, this,
          &MainWindow::on_dictWebPage_linkClicked);
  dictWebPage_->setUrl(QUrl("qrc:/html/page.htm"));
  dictWebPage_->setWebChannel(webChannel_);
  Dictionary::Load(this);
}

MainWindow::~MainWindow() {
  delete ui;
  delete dictWebPage_;
  delete webChannel_;
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
  ui->webEngineView->setPage(dictWebPage_);
  dictWebPage_->runJavaScript(QStringLiteral("clearArticles();"));
  statusBar()->showMessage(QStringLiteral("查询中..."));
  QTime time;
  time.start();

  QRegExp rx(R"(\\([\s\S])|(["\r\n]))");
  QString article;
  for (const auto& dict : Dictionary::Get()) {
    article.clear();
    dict->getArticleText(text, article);
    article.replace(rx, "\\\\1\\2");
    dictWebPage_->runJavaScript(QStringLiteral("addArticle(\"%1\",\"%2\");")
                                    .arg(dict->info().title, article));
  }
  QString info = QString("Time used:%1ms").arg(time.elapsed());
  statusBar()->showMessage(info);
}
