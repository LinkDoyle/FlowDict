#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>

#include "dictwebpage.h"
#include "mdict.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  void closeEvent(QCloseEvent *event) override;
  void changeEvent(QEvent *e) override;
 private slots:
  void on_action_A_triggered();
  void on_action_M_triggered();
  void on_dictWebPage_linkClicked(const QUrl &);
  void on_webEngineView_loadFinished(bool);
  void on_comboBox_currentIndexChanged(const QString &);
  void on_systemTrayIcon_activated(QSystemTrayIcon::ActivationReason);

 private:
  Ui::MainWindow *ui;
  DictWebPage *dictWebPage_;
  QWebChannel *webChannel_;
  QSystemTrayIcon *systemTrayIcon_;
  QMenu *systemTrayIconMenu_;
};

#endif  // MAINWINDOW_H
