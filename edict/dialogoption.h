#ifndef DIALOGOPTION_H
#define DIALOGOPTION_H

#include <QDialog>

namespace Ui {
class DialogOption;
}

class DialogOption : public QDialog
{
  Q_OBJECT

public:
  explicit DialogOption(QWidget *parent = 0);
  ~DialogOption();

private slots:
  void on_DialogOption_accepted();
  void on_cbStyle_activated(const QString &arg1);
  void on_btnCleanHistory_clicked();

private:
  Ui::DialogOption *ui;
};

#endif // DIALOGOPTION_H
