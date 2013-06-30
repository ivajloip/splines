#include <QApplication>
#include <QMainWindow>
#include <MainWindow.h>
#include <QTranslator>
#include <QLocale>

#include <iostream>

int main( int argc, char **argv ) {
  QApplication a(argc, argv);

  QTranslator* translator = new QTranslator();
  std::cout << translator->load("splines_bg") << std::endl;
  a.installTranslator(translator);
  QLocale::setDefault(QLocale(QLocale::Bulgarian));

  QMainWindow* window = new MainWindow(0);

  window->show();

  return a.exec();
}
