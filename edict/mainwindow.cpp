#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFile>
#include <QTime>
#include <QMessageBox>
#include <QString>
#include <QCloseEvent>
#include <QRegExp>
#include <QWebEngineView>
#include <QWebChannel>
#include <QMenu>
#include <QCompleter>
#include <QStringListModel>

#include "dialogabout.h"
#include "dictmanager.h"
#include "dictionary.h"
#include "ConfigParser.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      dictWebPage_(new DictWebPage),
      webChannel_(new QWebChannel),
      systemTrayIcon_(new QSystemTrayIcon),
      systemTrayIconMenu_(new QMenu) {
  ui->setupUi(this);

  connect(ui->webEngineView, &QWebEngineView::loadFinished, this,
          &MainWindow::on_webEngineView_loadFinished);
  connect(dictWebPage_, &DictWebPage::linkClicked, this,
          &MainWindow::on_dictWebPage_linkClicked);
  dictWebPage_->setUrl(QUrl("qrc:/html/page.htm"));
  dictWebPage_->setWebChannel(webChannel_);

  systemTrayIconMenu_->addMenu(ui->menu);
  systemTrayIconMenu_->addMenu(ui->menu_2);
  systemTrayIconMenu_->addAction(QStringLiteral("退出(&X)"), this,
                                 &MainWindow::close);
  systemTrayIcon_->setContextMenu(systemTrayIconMenu_);
  connect(systemTrayIcon_, &QSystemTrayIcon::activated, this,
          &MainWindow::on_systemTrayIcon_activated);
  systemTrayIcon_->setToolTip(this->windowTitle());
  systemTrayIcon_->setIcon(QIcon(":/images/dict.png"));
  systemTrayIcon_->show();
  Dictionary::Load(this);
}

MainWindow::~MainWindow() {
  delete ui;
  delete dictWebPage_;
  delete webChannel_;
  delete systemTrayIcon_;
  delete systemTrayIconMenu_;
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

void MainWindow::on_webEngineView_loadFinished(bool)
{
  if(ui->webEngineView->url().url() == QStringLiteral("qrc:/html/cover.htm")){
    this->show();
  }
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

void MainWindow::changeEvent(QEvent* e) {
  if ((e->type() == QEvent::WindowStateChange) && this->isMinimized()) {
    this->hide();
  }
  QMainWindow::changeEvent(e);
}

void MainWindow::on_systemTrayIcon_activated(
    QSystemTrayIcon::ActivationReason reason) {
  switch (reason) {
    case QSystemTrayIcon::DoubleClick:
    case QSystemTrayIcon::Trigger:
      if (this->isVisible()) {
        this->hide();
      } else {
        this->raise();
        this->activateWindow();
        this->showNormal();
      }
      break;
    default:
      break;
  }
}

void MainWindow::on_comboBox_editTextChanged(const QString &text)
{
    const auto& dictionaries = Dictionary::Get();
    if(dictionaries.isEmpty()) return;
    QStringList keys = dictionaries[0]->keysWithPrefix(text, 15);
    QCompleter *completer = new QCompleter(keys, this);
    completer->setModel(new QStringListModel(keys, this));
    ui->comboBox->setCompleter(completer);
}
