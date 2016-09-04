#include "mainwindow.h"

#include <QApplication>
#include <QMessageBox>

#include "ConfigParser.h"
int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  Config& config = Config::Get();
  if (!config.load("config.json")) {
    QMessageBox::warning(nullptr, QStringLiteral("错误"),
                         QStringLiteral("读取配置文件 config.json 失败！"));
    return -1;
  }

  app.setStyle(config.basicStyle);
  MainWindow w;
  return app.exec();
}
