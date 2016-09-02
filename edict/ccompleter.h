#ifndef CCOMPLETER_H
#define CCOMPLETER_H

#include <QObject>
#include <QListView>

class HTMLDelegate;

class CCompleter : public QObject {
  Q_OBJECT

 public:
  CCompleter();
  ~CCompleter();
  bool eventFilter(QObject *obj, QEvent *event);
  void setModel(QAbstractItemModel *c);
  void setTarget(QWidget *widget);
  void complete() const;
  void completed() const;
  void setSimpleDefinitionVisible(bool visible);

 private:
  QListView *view_;
  QWidget *target_;
  QAbstractItemDelegate *oldItemDelete_;
  HTMLDelegate *styledItemDelegate_;
};

#endif  // CCOMPLETER_H
