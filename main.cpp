#include <QApplication>
#include <QMainWindow>

int main( int argc, char **argv ) {
  QApplication a(argc, argv);
  QMainWindow* window = new QMainWindow(0);

  window->show();

  return a.exec();
}
