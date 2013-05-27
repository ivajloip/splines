#ifndef __main_window_h__

#define __main_window_h__

#include <QMainWindow>
#include <SplinesCalculator.h>

#include <utility>

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
  SplinesCalculator* splinesCalculator;
    
  void createMenuItem(QString label,
      QString iconLocation,
      QString shortCut,
      QWidget *addTo,
      const char * func,
      QWidget* signalTo);

  void createMenu();

  void readPointsFromFile(QString fileName,
      PointsType*& points,
      int& pointsCount);

  void updateSplinesCalculator(PointsType* pointsData, int pointsCount);

  void savePointsToFile(QString fileName, int step = 1); 

public:
  MainWindow(QWidget *parent=0);

protected slots:
  void importSlot();
  void exportSlot();
  void closeSlot();
};

#endif
