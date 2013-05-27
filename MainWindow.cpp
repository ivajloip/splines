#include <QKeyEvent>
#include <QMenuBar>
#include <QAction>
#include <QFileDialog>
#include <QInputDialog>
#include <MainWindow.h>
#include <SplinesCalculator.h>

#include <fstream>
#include <iostream>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  this->resize(640, 480);
  this->setWindowTitle("Splines approximation");

  createMenu();
  
  splinesCalculator = NULL;
}

void MainWindow::createMenu() {
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

  std::cout << std::endl;

  PointsType* points;
  int pointsCount;

  readPointsFromFile(fileName, points, pointsCount);

  std::sort(points, points + pointsCount);

  updateSplinesCalculator(points, pointsCount);
}

void MainWindow::exportSlot() {
  bool ok;
  int step = QInputDialog::getInt(this,
      tr("Title"),
      tr("Please select the step to be used"),
      1,
      1,
      10000,
      1, 
      &ok);

  if (!ok) {
    return;
  }

  QString fileName = QFileDialog::getSaveFileName(this,
      tr("Save File"),
      "",
      tr("Files (*.*)"));

  std::cout << step << std::endl;
  std::cout << fileName.toStdString().c_str() << std::endl;

  savePointsToFile(fileName, step);
}


void MainWindow::savePointsToFile(QString fileName, int step) {
  std::ofstream out(fileName.toStdString().c_str());

  if (!out) {
    std::cerr << "Error while saving file: " << fileName.toStdString() <<
      std::endl;
    return;
  }

  int pointsCount = splinesCalculator->getResultPointsCount();
  PointsType* points = splinesCalculator->getResultPoints();

  out << pointsCount / step << std::endl;

  for (int i = 0; i < pointsCount; i += step) {
    out << points[i].first / step << " " << points[i].second << std::endl;
  }

  out.close();
}


void MainWindow::readPointsFromFile(QString fileName, PointsType*& points, int& pointsCount) {
  std::ifstream in(fileName.toStdString().c_str());

  if (!in) {
    std::cerr << "Error while loading file: " << fileName.toStdString() <<
      std::endl;

    return;
  }

  std::cout << "Reading file: " << fileName.toStdString() << std::endl;

  int m;
  in >> pointsCount;
  std::cout << pointsCount << std::endl;

  points = new PointsType[m];

  for (int i = 0; i < pointsCount; i++) {
    in >> points[i].first;
    in >> points[i].second;

    std::cout << points[i].first << " " << points[i].second << std::endl;
  }

  in.close();

}


void MainWindow::updateSplinesCalculator(PointsType* points, int pointsCount) {
  if (splinesCalculator != NULL) {
    std::cout << "Deleting old instance\n";

    delete splinesCalculator;
  }

  std::cout << "Constructing SplinesCalcualtor with:\n";

  for (int i = 0; i < pointsCount; i++) {
    std::cout << points[i].first << " " << points[i].second << std::endl;
  }

  splinesCalculator = new SplinesCalculator(points, pointsCount);
}

bool cmpPoints(PointsType point1, PointsType point2) {
  return point1.first < point2.first;
}
