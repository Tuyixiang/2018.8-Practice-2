#include <iostream>
#include <QApplication>
#include <QMainWindow>
#include <Qt3DWindow>
#include <QFontDatabase>
#include "MainWindow.h"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  QSurfaceFormat format;
  format.setDepthBufferSize(32);
  format.setSamples(8);
  QSurfaceFormat::setDefaultFormat(format);
  QFontDatabase::addApplicationFont(":/res/HYCuSongF.ttf");
  MainWindow mw;
  mw.show();
  return app.exec();
}
