#ifndef DICTMANAGER_H
#define DICTMANAGER_H

#include <QDialog>

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

private:
  Ui::DictManager *ui;
};

#endif  // DICTMANAGER_H
