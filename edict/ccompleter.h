#ifndef CCOMPLETER_H
#define CCOMPLETER_H

#include <QObject>
#include <QListView>

class CCompleter : public QObject{
  Q_OBJECT

 public:
  CCompleter();
  ~CCompleter();
  bool eventFilter(QObject *obj, QEvent *event);
  void setModel(QAbstractItemModel *c);
  void setTarget(QWidget *widget);
  void complete() const;
  void completed() const;

 private:
  QListView *view_;
  QWidget *target_;
};

#endif  // CCOMPLETER_H
