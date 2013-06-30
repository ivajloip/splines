#ifndef __main_window_h__

#define __main_window_h__

#include <QMainWindow>
#include <QFormLayout>
#include <SplinesCalculator.h>
#include <QComboBox>
#include <QLineEdit>

#include <utility>

#include "LineEdit.h"

#define MAX_POINTS_COUNT 30
#define MAX_FILENAME_LENGTH 2048

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
  SplinesCalculator* splinesCalculator;
  PointsType* _points;
  LineEdit* positions;
  LineEdit* values;
  QLineEdit* outFileNameInput;
  QComboBox* stepInput;
  QComboBox* inputStepInput;

  QAction* saveAction;

  void createInputs(QFormLayout* layout);
    
  QAction* createMenuItem(QString label,
      QString iconLocation,
      QString shortCut,
      QWidget *addTo,
      const char * func,
      QWidget* signalTo);

  void createMenu();

  bool readPointsFromFile(QString fileName,
      PointsType*& points,
      int& pointsCount,
      int inputTableStep);

  void updateSplinesCalculator(PointsType* pointsData, int pointsCount);

  bool savePointsToFile(QString fileName,
      PointsType* points,
      int pointsCount,
      int step = 1,
      int type = 1,
      int pointsOnLine = 1); 

  void writeLog();

  int findBiggestNonZero();

  void updateInputs();

  void getDirectoryForFile(QString fileName, QString &dir);

  bool directoryIsOk(QString dir);


public:
  MainWindow(QWidget *parent=0);

protected slots:
  void importSlot();
  void exportSlot();
  void closeSlot();
  void pointUpdatedSlot(int index, int type);
};

#endif
