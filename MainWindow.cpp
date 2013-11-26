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
#include <QVariant>
#include <QFile>

#include <fstream>
#include <iostream>
#include <algorithm>
#include <locale>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  this->resize(450, 480);
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

  outFileNameInput = new QLineEdit();
  layout->addRow(tr("File"), outFileNameInput);

  inputStepInput = new QComboBox();
  inputStepInput->addItem(tr("mm"), QVariant(1));
  inputStepInput->addItem(tr("cm"), QVariant(10));
  layout->addRow(tr("Input Measure"), inputStepInput);

  stepInput = new QComboBox();
  stepInput->addItem(tr("mm"), QVariant(1));
  stepInput->addItem(tr("cm"), QVariant(10));
  stepInput->setCurrentIndex(1);
  layout->addRow(tr("Measure"), stepInput);

  QLabel *label = new QLabel(tr("Y"));
  layout->addRow(tr("X"), label);

  createInputs(layout);

  updateInputs("");
  
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
  QMenu* file = menubar->addMenu(tr("&File"));
  createMenuItem(tr("Open file"), NULL, "Ctrl+O", file, SLOT(importSlot()), this);
  file->addSeparator();
  this->saveAction = createMenuItem(tr("Save result"),
      NULL,
      "Ctrl+S",
      file,
      SLOT(exportSlot()),
      this);
  this->saveAction->setEnabled(false);
  file->addSeparator();
  createMenuItem(tr("Exit"), NULL, "Esc", file, SLOT(closeSlot()), this);
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

  if (fileName.isEmpty()) {
    return;
  }

  int pointsCount;
  int currentInputIndex = inputStepInput->currentIndex();
  int inputTableStep = stepInput->itemData(currentInputIndex).toInt();

  if (!readPointsFromFile(fileName, _points, pointsCount, inputTableStep)) {
    QMessageBox::critical(this,
        tr("Error"), 
        tr("Failed to read the points, please check the log for more information"));
    std::cerr << "Failed to read the file\n";

    return;
  }

  std::cout << "Points count is " << pointsCount << std::endl;

  std::sort(_points, _points + pointsCount);

  updateInputs(fileName);

  this->saveAction->setEnabled(true);

  QMessageBox::information(this, tr("Notice"), tr("Import succeeded!"));
}

void MainWindow::exportSlot() {
  int currentIndex = stepInput->currentIndex();
  int step = stepInput->itemData(currentIndex).toInt();

  QString fileName = outFileNameInput->text();

  std::cout << step << std::endl;
  std::cout << fileName.toStdString().c_str() << std::endl;

  int _pointsCount = findBiggestNonZero();

  updateSplinesCalculator(_points, _pointsCount);

  std::cout << "Calculating points\n";
  int pointsCount = splinesCalculator->getResultPointsCount();
  PointsType* points = splinesCalculator->getResultPoints();

  QString dir; 
  getDirectoryForFile(fileName, dir);

  if (!directoryIsOk(dir)) {
    QMessageBox::critical(this,
        tr("Error"),
        tr("%1 does not exist, please choose a directory that exists").arg(dir));

    return;
  }

  if (!savePointsToFile(fileName, points, pointsCount, step)) {
    QMessageBox::critical(this,
        tr("Error"), 
        tr("Failed to write the file, maybe you don't have permissions"));

    std::cerr << "Failed to save the file\n";

    return;
  }

  if (!savePointsToFile(fileName,points, pointsCount,  step, 2)) {
    QMessageBox::critical(this,
        tr("Error"), 
        tr("Failed to write the file, maybe you don't have permissions"));

    std::cerr << "Failed to save the file\n";

    return;
  }

  if (!savePointsToFile(fileName, points, pointsCount, step, 2, 4)) {
    QMessageBox::critical(this,
        tr("Error"), 
        tr("Failed to write the file, maybe you don't have permissions"));

    std::cerr << "Failed to save the file\n";

    return;
  }

  if (!savePointsToFile(fileName, _points, _pointsCount, step, 3)) {
    QMessageBox::critical(this,
        tr("Error"), 
        tr("Failed to write the file, maybe you don't have permissions"));

    std::cerr << "Failed to save the file\n";

    return;
  }

  writeLog();

  QMessageBox::information(this, tr("Notice"), tr("Saved!"));
}


void MainWindow::getDirectoryForFile(QString fileName, QString &dir) {
  int lastSeparator = fileName.lastIndexOf("/");
  if (lastSeparator == -1) {
    lastSeparator = fileName.lastIndexOf("\\");
  }

  if (lastSeparator == -1) {
    dir = ".";
    return;
  }

  std::cout << "Index " << lastSeparator << std::endl;

  dir = fileName.left(lastSeparator);
}

bool MainWindow::directoryIsOk(QString dir) {
  if (dir == "") {
    return true;
  }

  std::cout << dir.toStdString() << std::endl;

  QFile* tmp = new QFile(dir);

  return tmp->exists();
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
    PointsType* points,
    int pointsCount,
    int step,
    int type,
    int pointsOnLine) {
  char newFileName[MAX_FILENAME_LENGTH];
  char *suffix;
  const char* fileNameAsChars = fileName.toStdString().c_str();
  if (strlen(fileNameAsChars) < 1) {
    return false;
  }

  if (type == 1) {
    suffix = ((char *)"out");
  } else if (type == 3) {
    suffix = ((char *)"in");
  } else if (pointsOnLine == 1) {
    suffix = ((char *)"1col");
  } else {
    suffix = ((char *)"4col");
  }

  snprintf(newFileName, MAX_FILENAME_LENGTH, "%s_%s.%s\0", fileNameAsChars, 
      suffix, ((type & 1) == 0) ? "html" : "txt");

  std::locale::global(std::locale(""));
  std::ofstream out(newFileName);

  if (!out) {
    return false;
  }

  std::cout << "Writing result to file\n";

  // we want all the input points, no matter what
  if (type == 3) {
    step = 1;
  }

  if ((type & 1) == 1) {
    out << (pointsCount + step - 1) / step << std::endl;
  } else {
    out << "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">\n";
    out << "<html><head>\n";
    out << "<meta http-equiv=\"Content-Type\" content=\"text/html;charset=UTF-8\">\n";
    out << "<title> Denev & co table</title>\n";
    out << "</head>\n<body>\n<center>\n";
    out << "<table border>\n";
    out << "<tr>\n";

    for (int j = 0; j < pointsOnLine; j++) {
      out << "<th>" << ((step == 10) ? tr("cm").toUtf8().constData() : tr("mm").toUtf8().constData())
          << "</th><th>" << tr("liters").toUtf8().constData() << "</th>\n";
    }

    out << "</tr>\n";
  }

  for (int i = 0; i < pointsCount; i += step) {
    if ((type & 1) == 1) {
      out << points[i].first / step << " " << points[i].second << std::endl;
    } else if (type == 2) {
      out << "<tr>\n";

      int j;

      for (j = 0; j < pointsOnLine && i + (j * step) < pointsCount; j++) {
        out << "<td align=\"right\">" << points[i + (j * step)].first / step
            << "</td><td align=\"right\">"
            << long(points[i + (j * step)].second) << "</td>\n";
      }
      
      for (; j < pointsOnLine; j++) {
        out << "<td align=\"right\">&nbsp;</td><td align=\"right\">"
            << "&nbsp;</td>\n";
      }

      
      i += (pointsOnLine * step) - step;

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
    int& pointsCount,
    int inputTableStep) {
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
  std::cout << inputTableStep << std::endl;
  std::cout << pointsCount << std::endl;

  for (int i = 0; i < pointsCount; i++) {
    in >> points[i].first;
    in >> points[i].second;

    if (points[i].first == 0) {
        pointsCount--;
        i--;
        std::cout << "Fixing .out reading\n";
        continue;
    }

    points[i].first *= inputTableStep;

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

void MainWindow::updateInputs(QString fileName) {
  char tmp[16];

  std::cout << "Updating inputs" << std::endl;

  this->outFileNameInput->setText(fileName);

  for (int i = 0; i < MAX_POINTS_COUNT; i++) {
    snprintf(tmp, 16, "%d", _points[i].first);

    std::cout << _points[i].first << " ";

    positions[i].setText(tmp);

    snprintf(tmp, 16, "%.3Lf", _points[i].second);

    std::cout << _points[i].second << std::endl;

    values[i].setText(tmp);
  }
}
