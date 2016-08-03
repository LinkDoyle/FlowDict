#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>

#include "config.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  QTranslator qt;
  qt.load("qt_zh_CN");
  app.installTranslator(&qt);
  app.setStyle("Fusion");
  MainWindow w;
  w.show();

  return app.exec();
}
