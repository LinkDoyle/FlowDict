#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QTranslator>
#include "ConfigParser.h"
int main(int argc, char* argv[]) {
  Config& config = Config::Get();
  if (!config.load("config.json")) {
    QMessageBox::warning(nullptr, QStringLiteral("����"),
                         QStringLiteral("��ȡ�����ļ� config.json ʧ�ܣ�"));
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
