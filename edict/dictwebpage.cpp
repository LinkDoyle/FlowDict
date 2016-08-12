#include "dictwebpage.h"

DictWebPage::DictWebPage() {}

bool DictWebPage::acceptNavigationRequest(const QUrl &url,
                                          QWebEnginePage::NavigationType type,
                                          bool) {
  if (type == QWebEnginePage::NavigationTypeLinkClicked) {
    emit linkClicked(url);
    return false;
  }
  return true;
}
