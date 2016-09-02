#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QTranslator>

#include "ConfigParser.h"
int main(int argc, char* argv[]) {
  Config& config = Config::Get();
  if (!config.load("config.json")) {
    QMessageBox::warning(nullptr, QStringLiteral("错误"),
                         QStringLiteral("读取配置文件 config.json 失败！"));
    return -1;
  }
  QApplication app(argc, argv);
  QTranslator qt;
  qt.load("qt_zh_CN");
  app.installTranslator(&qt);
  app.setStyle("Fusion");
  MainWindow w;
  return app.exec();
}
