#include <QKeyEvent>
#include <QMenuBar>
#include <QAction>
#include <QFileDialog>
#include <QInputDialog>
#include <MainWindow.h>
#include <SplinesCalculator.h>
#include <QMessageBox>
#include <QScrollArea>
#include <QLayout>
#include <QLineEdit>
#include <QWidget>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QLabel>

#include <fstream>
#include <iostream>
#include <algorithm>
#include <locale>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  this->resize(640, 480);
  this->setWindowTitle("Splines approximation");

  createMenu();

  QScrollArea *scrollArea = new QScrollArea(this);

  _points = new PointsType[MAX_POINTS_COUNT + 2];
  positions = new LineEdit[MAX_POINTS_COUNT];
  values = new LineEdit[MAX_POINTS_COUNT];

  this->setCentralWidget(scrollArea);

  QWidget *tmp = new QWidget;
  scrollArea->setWidget(tmp);

  QFormLayout *layout = new QFormLayout;
  tmp->setLayout(layout);
  layout->setSizeConstraint(QLayout::SetMinimumSize);

  QLabel *label = new QLabel(tr("Y"));
  layout->addRow(tr("X"), label);

  createInputs(layout);

  updateInputs();
  
  std::cout << "Bar\n";
  
  splinesCalculator = NULL;
}

void MainWindow::createInputs(QFormLayout* layout) {
  for (int i = 0; i < MAX_POINTS_COUNT; i++) {
    positions[i].setIndex(i);
    positions[i].setType(0);
    positions[i].setValidator(new QIntValidator);
    positions[i].setMinimumWidth(200);
    connect(&positions[i], SIGNAL(valueChanged(int, int)), this, SLOT(pointUpdatedSlot(int, int)));

    values[i].setIndex(i);
    values[i].setType(1);
    values[i].setValidator(new QDoubleValidator);
    values[i].setMinimumWidth(200);
    connect(&values[i], SIGNAL(valueChanged(int, int)), this, SLOT(pointUpdatedSlot(int, int)));
    layout->addRow(&positions[i], &values[i]);
  }
}

void MainWindow::createMenu() {
  // Create the menu
  QMenuBar* menubar = menuBar();
  QMenu* file = menubar->addMenu("&File");
  createMenuItem("Open file", NULL, "Ctrl+O", file, SLOT(importSlot()), this);
  file->addSeparator();
  this->saveAction = createMenuItem("Save result",
      NULL,
      "Ctrl+S",
      file,
      SLOT(exportSlot()),
      this);
  this->saveAction->setEnabled(false);
  file->addSeparator();
  createMenuItem("Exit", NULL, "Esc", file, SLOT(closeSlot()), this);
}

QAction* MainWindow::createMenuItem(QString label,
    QString iconLocation,
    QString shortCut,
    QWidget *addTo,
    const char * func,
    QWidget* signalTo) {
  QAction* tmp = new QAction(label, this);
  tmp->setShortcut(shortCut);
  connect(tmp, SIGNAL(triggered()), signalTo, func);
  addTo->addAction(tmp);

  return tmp;
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

  int pointsCount;

  if (!readPointsFromFile(fileName, _points, pointsCount)) {
    QMessageBox::critical(this,
        tr("Error"), 
        tr("Failed to read the points, please check the log for more information"));
    std::cerr << "Failed to read the file\n";

    return;
  }

  std::sort(_points, _points + pointsCount);

  updateInputs();

  this->saveAction->setEnabled(true);

  QMessageBox::information(this, tr("Notice"), tr("Import succeeded!"));
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

  updateSplinesCalculator(_points, findBiggestNonZero());

  if(!savePointsToFile(fileName, step)) {
    QMessageBox::critical(this,
        tr("Error"), 
        tr("Failed to save the points, please check the log for more information"));

    std::cerr << "Failed to save the file\n";

    return;
  }

  if(!savePointsToFile(fileName, step, 2)) {
    QMessageBox::critical(this,
        tr("Error"), 
        tr("Failed to save the points, please check the log for more information"));

    std::cerr << "Failed to save the file\n";

    return;
  }

  if(!savePointsToFile(fileName, step, 2, 4)) {
    QMessageBox::critical(this,
        tr("Error"), 
        tr("Failed to save the points, please check the log for more information"));

    std::cerr << "Failed to save the file\n";

    return;
  }

  writeLog();

  QMessageBox::information(this, tr("Notice"), tr("Saved!"));
}

void MainWindow::pointUpdatedSlot(int index, int type) {
  std::cout << "Point changed: " << index << " " << type << std::endl;

  if (type == 1) {
    _points[index].second = atof(values[index].text().toStdString().c_str());
  } else {
    _points[index].first = atoi(positions[index].text().toStdString().c_str());
  }

  if (findBiggestNonZero() > 0) {
    this->saveAction->setEnabled(true);
  }
}

int MainWindow::findBiggestNonZero() {
  int i = 0;
  while (i < MAX_POINTS_COUNT && _points[i].first > 0) {
    i++;
  }

  return i;
}

bool MainWindow::savePointsToFile(QString fileName,
    int step,
    int type,
    int pointsOnLine) {
  char newFileName[MAX_FILENAME_LENGTH];
  const char* fileNameAsChars = fileName.toStdString().c_str();
  if (strlen(fileNameAsChars) < 1) {
    return false;
  }

  snprintf(newFileName, MAX_FILENAME_LENGTH, "%s%s%s\0", fileNameAsChars, 
      (pointsOnLine != 1) ? "short" : "", (type != 1) ? ".html" : "");

  std::locale::global(std::locale(""));
  std::ofstream out(newFileName);

  if (!out) {
    QMessageBox::critical(this,
        tr("Error"), 
        tr("Failed to save the points, please check the log for more information"));

    std::cerr << "Error while saving file: " << fileName.toStdString() <<
      std::endl;
    return false;
  }

  std::cout << "Calculating points\n";
  int pointsCount = splinesCalculator->getResultPointsCount();
  PointsType* points = splinesCalculator->getResultPoints();
  std::cout << "Writing result to file\n";

  if (type == 1) {
    out << pointsCount / step << std::endl;
  } else {
    out << "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">\n";
    out << "<html><head>\n";
    out << "<meta http-equiv=\"Content-Type\" content=\"text/html;charset=UTF-8\">\n";
    out << "<title> Denev & co table</title>\n";
    out << "</head>\n<body>\n<center>\n";
    out << "<table border>\n";
    out << "<tr>\n";

    for (int j = 0; j < pointsOnLine; j++) {
//      wchar_t tmp[] = L"литри";
      out << "<th>cm</th><th>" << "liters" << "</th>\n";
    }

    out << "</tr>\n";
  }

  for (int i = 0; i < pointsCount; i += step) {
    if (type == 1) {
      out << points[i].first / step << " " << points[i].second << std::endl;
    } else if (type == 2) {
      out << "<tr>\n";

      for (int j = 0; j < pointsOnLine && i + (j * step) < pointsCount; j++) {
        out << "<td align=\"right\">" << points[i + (j * step)].first / step
            << "</td><td align=\"right\">"
            << points[i + (j * step)].second << "</td>\n";
      }
      
      i += pointsOnLine - step;

      out << "</tr>\n";
    }
  }

  out.close();
  
  return true;
}

void MainWindow::writeLog() {
  std::ofstream out("log.log", std::ofstream::out | std::ofstream::app);
  out << splinesCalculator->errorCalculation() << std::endl;

  out.close();
}

bool MainWindow::readPointsFromFile(QString fileName,
    PointsType*& points,
    int& pointsCount) {
  const char* fileNameAsChars = fileName.toStdString().c_str();
  if (strlen(fileNameAsChars) < 1) {
    return false;
  }

  for (int i = 0; i < MAX_POINTS_COUNT; i++) {
      points[i].first = 0;
      points[i].second = 0.0;
  }

  std::ifstream in(fileNameAsChars);

  if (!in) {
    std::cerr << "Error while loading file: " << fileName.toStdString() <<
      std::endl;

    return false;
  }

  std::cout << "Reading file: " << fileName.toStdString() << std::endl;

  in >> pointsCount;
  std::cout << pointsCount << std::endl;

  for (int i = 0; i < pointsCount; i++) {
    in >> points[i].first;
    in >> points[i].second;

    std::cout << points[i].first << " " << points[i].second << std::endl;
  }

  in.close();

  std::cout << "Points read\n";

  for (int i = 0; i < pointsCount; i++) {
      std::cout << points[i].first << " " << points[i].second << std::endl;
  }

  return true;
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

void MainWindow::updateInputs() {
  char tmp[16];

  std::cout << "Updating inputs" << std::endl;

  for (int i = 0; i < MAX_POINTS_COUNT; i++) {
    snprintf(tmp, 16, "%d", _points[i].first);

    std::cout << _points[i].first << " ";

    positions[i].setText(tmp);

    snprintf(tmp, 16, "%.3Lf", _points[i].second);

    std::cout << _points[i].second << std::endl;

    values[i].setText(tmp);
  }
}
