#include "ui/MainWindow.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

  QApplication app(argc, argv);
  app.setApplicationName("Color Converter");
  app.setApplicationVersion("1.0.0");
  QApplication::setStyle(QStyleFactory::create("Fusion"));

  MainWindow window;
  window.show();

  return app.exec();
}