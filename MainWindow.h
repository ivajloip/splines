#ifndef __main_window_h__

#define __main_window_h__

#include <QMainWindow>
#include <QFormLayout>
#include <SplinesCalculator.h>

#include <utility>

#include "LineEdit.h"

#define MAX_POINTS_COUNT 20

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
  SplinesCalculator* splinesCalculator;
  PointsType* _points;
  LineEdit* positions;
  LineEdit* values;

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
      int& pointsCount);

  void updateSplinesCalculator(PointsType* pointsData, int pointsCount);

  bool savePointsToFile(QString fileName, int step = 1); 

  void writeLog();

  int findBiggestNonZero();

  void updateInputs();

public:
  MainWindow(QWidget *parent=0);

protected slots:
  void importSlot();
  void exportSlot();
  void closeSlot();
  void pointUpdatedSlot(int index, int type);
};

#endif
