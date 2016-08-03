#ifndef DICTWEBPAGE_H
#define DICTWEBPAGE_H

#include <QWebEnginePage>

class DictWebPage : public QWebEnginePage {
  Q_OBJECT
 public:
  DictWebPage();
  bool acceptNavigationRequest(const QUrl &url,
                               QWebEnginePage::NavigationType type, bool);
 signals:
  void linkClicked(const QUrl &);
};

#endif  // DICTWEBPAGE_H
