#include "flowhelper.h"

#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QtGlobal>
#include <QProcess>
#include <QString>
#include <QUrl>

namespace FlowHelper {

void showInFolder(const QString &path) {
#if defined(Q_OS_WIN)
  QString param = QStringLiteral("explorer /select,");
  param += QDir::toNativeSeparators(path);
  QProcess::startDetached(param);
#else
  QDesktopServices::openUrl(
      QUrl::fromLocalFile(QFileInfo(path).absolutePath()));
#endif
}
}
