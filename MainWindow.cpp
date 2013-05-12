#include <QKeyEvent>
#include <QMenuBar>
#include <QAction>
#include <QFileDialog>
#include <QInputDialog>
#include <MainWindow.h>

#include <fstream>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  this->resize(800, 640);
  this->setWindowTitle("Splines approximation");
  
  // Create the menu
  QMenuBar* menubar = menuBar();
  QMenu* file = menubar->addMenu("&File");
  createMenuItem("Open file", NULL, "Ctrl+O", file, SLOT(importSlot()), this);
  file->addSeparator();
  createMenuItem("Save result", NULL, "Ctrl+S", file, SLOT(exportSlot()), this);
  file->addSeparator();
  createMenuItem("Exit", NULL, "Esc", file, SLOT(closeSlot()), this);
}

void MainWindow::createMenuItem(QString label,
  QString iconLocation,
  QString shortCut,
  QWidget *addTo,
  const char * func,
  QWidget* signalTo) {
  QAction* tmp = new QAction(label, this);
  tmp->setShortcut(shortCut);
  connect(tmp, SIGNAL(triggered()), signalTo, func);
  addTo->addAction(tmp);
}

void MainWindow::closeSlot() {
  this->close();
}

void MainWindow::importSlot() {
  QString fileName = QFileDialog::getOpenFileName(this,
      tr("Open File"),
      "",
      tr("Files (*.*)"));

  std::cout << fileName.toStdString() << std::endl;

  std::ifstream in(fileName.toStdString().c_str());

  if (!in) {
    // TODO: log error
    return;
  }

  std::cout << "Reading\n";

  while(!in.eof()) {
    char tmp;
    in >> tmp;
    std::cout << tmp;
  }

  std::cout << std::endl;
}

void MainWindow::exportSlot() {
  int step = QInputDialog::getInt(this, tr("Title"), tr("Label"), 1, 1, 10000);

  QString fileName = QFileDialog::getSaveFileName(this,
      tr("Save File"),
      "",
      tr("Files (*.*)"));

  std::cout << step << std::endl;
  std::cout << fileName.toStdString().c_str() << std::endl;
}
