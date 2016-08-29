#include "ccompleter.h"

#include <QAbstractItemModel>
#include <QListView>
#include <QComboBox>
#include <QApplication>
#include <QKeyEvent>
#include <QScrollBar>
#include <qDebug>

CCompleter::CCompleter()
    : view_(new QListView), target_(nullptr) {
  view_->hide();
  view_->setParent(0, Qt::Popup | Qt::FramelessWindowHint);
  view_->setFocusPolicy(Qt::NoFocus);
  view_->setFocusProxy(target_);
  view_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  view_->setSelectionMode(QAbstractItemView::SingleSelection);
  connect(view_, &QListView::activated, this, [this](const QModelIndex &index) {
    QComboBox *target = qobject_cast<QComboBox *>(target_);
    if (target_) {
      target->setEditText(index.data().toString());
      target->setFocus();
      emit target->activated(target->currentText());
    }
    view_->hide();
  });
  qApp->installEventFilter(this);
}

CCompleter::~CCompleter()
{
  delete view_;
}

bool CCompleter::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::NonClientAreaMouseButtonPress) {
    view_->hide();
    return QObject::eventFilter(obj, event);
  }
  if (obj == target_ && event->type() == QEvent::FocusOut) {
    return true;
  }
  if (obj != view_) return QObject::eventFilter(obj, event);
  if (view_->isVisible()) {
    switch (event->type()) {
      case QEvent::KeyPress: {
        auto key = static_cast<QKeyEvent *>(event);
        switch (key->key()) {
          case Qt::Key_Escape: {
            view_->hide();
            break;
          }
          case Qt::Key_Down:
          case Qt::Key_Up:
            break;
          case Qt::Key_End:
          case Qt::Key_Home:
            if (key->modifiers() & Qt::ControlModifier) return false;
            break;
          default:
            break;
        }

        (static_cast<QObject *>(target_))->event(event);
        if (!target_->hasFocus()) {
          view_->hide();
          if (event->isAccepted()) return true;
        }
        break;
      }
      case QEvent::MouseButtonPress: {
        if (!view_->underMouse()) {
          view_->hide();
          return true;
        }
        break;
      }
      case QEvent::InputMethod:
      case QEvent::ShortcutOverride:
        QApplication::sendEvent(target_, event);
        break;
      default:
        return false;
    }
  }
  return false;
}
void CCompleter::setModel(QAbstractItemModel *model) { view_->setModel(model); }

void CCompleter::setTarget(QWidget *widget) {
  target_ = widget;
  view_->setFocusProxy(target_);
  view_->setFocusPolicy(target_->focusPolicy());
}

void CCompleter::complete() const {
  if (!target_) return;
  if (view_->model()->rowCount() == 0) view_->hide();
  int sizeRow = view_->sizeHintForRow(0);
  view_->setMaximumSize(target_->width(), sizeRow > 0 ? sizeRow * 10 : 0);
  view_->setMinimumSize(target_->width(), 0);
  view_->move(target_->mapToGlobal(QPoint(0, target_->height())));
  QRect rect = view_->geometry();
  rect.setWidth(target_->width());
  rect.setHeight(view_->sizeHintForRow(0) * view_->model()->rowCount() +
                 view_->horizontalScrollBar()->sizeHint().height());
  view_->setGeometry(rect);
  view_->show();
}

void CCompleter::completed() const { view_->hide(); }
