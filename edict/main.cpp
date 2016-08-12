#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QTranslator>
#include "ConfigParser.h"
int main(int argc, char* argv[]) {
  Config& config = Config::Get();
  if (!config.load("config.json")) {
    QMessageBox::warning(nullptr, QStringLiteral("¥ÌŒÛ"),
                         QStringLiteral("∂¡»°≈‰÷√Œƒº˛ config.json  ß∞‹£°"));
	return -1;
  }
  QApplication app(argc, argv);
  QTranslator qt;
  qt.load("qt_zh_CN");
  app.installTranslator(&qt);
  app.setStyle("Fusion");
  MainWindow w;
  w.show();

  return app.exec();
}
