#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>

#include "dictwebpage.h"
#include "mdict.h"

namespace Ui {
class MainWindow;
}

class CCompleter;
class QLabel;
class QStringListModel;

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = 0);
  void changeEvent(QEvent *e) override;
  void closeEvent(QCloseEvent *event) override;
  void clearStatusBarLabels() const;
  void findInPage(const QString &text) const;
  void search(const QString &text) const;
  ~MainWindow();
 private slots:
  void on_action_A_triggered();
  void on_action_M_triggered();
  void on_comboBox_activated(const QString &);
  void on_comboBox_editTextChanged(const QString &);
  void on_dictWebPage_linkClicked(const QUrl &);
  void on_systemTrayIcon_activated(QSystemTrayIcon::ActivationReason);
  void on_toolButton_clicked();
  void on_webEngineView_loadFinished(bool);

 private:
  DictWebPage *dictWebPage_;
  QAction *caseSensitiveAct_;
  QAction *changeSearchModeAct_;
  QAction *correctAct_;
  QAction *reverseLookupAct_;
  QAction *showSimpleDefinitionAct_;
  QAction *wildcardAct_;
  CCompleter *completer_;
  QLabel *caseSensitiveLabel_;
  QLabel *correctLabel_;
  QLabel *reverseLookupLabel_;
  QLabel *wildcardLabel_;
  QMenu *findInPageMenu_;
  QMenu *searchMenu_;
  QMenu *systemTrayIconMenu_;
  QStringListModel *wordCompleterList_;
  QSystemTrayIcon *systemTrayIcon_;
  QWebChannel *webChannel_;
  Ui::MainWindow *ui;
};

#endif  // MAINWINDOW_H
