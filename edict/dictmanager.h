#ifndef DICTMANAGER_H
#define DICTMANAGER_H

#include <QDialog>

class QAction;

namespace Ui {
class DictManager;
}

class DictManager : public QDialog {
  Q_OBJECT

 public:
  explicit DictManager(QWidget *parent = 0);
  ~DictManager();

 private slots:
  void on_toolButton_clicked();
  void on_buttonBox_accepted();
  void on_tableWidget_cellClicked(int row, int column);
  void on_toolButton_2_clicked();

private:
  Ui::DictManager *ui;
  QAction *exploreAct_;
};

#endif  // DICTMANAGER_H
