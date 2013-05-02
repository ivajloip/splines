#include <QApplication>
#include <QMainWindow>
#include <MainWindow.h>

int main( int argc, char **argv ) {
  QApplication a(argc, argv);
  QMainWindow* window = new MainWindow(0);

  window->show();

  return a.exec();
}
