#ifndef __main_window_h__

#define __main_window_h__

#include <QMainWindow>
#include <SplinesCalculator.h>

#include <utility>

typedef std::pair<int, PointsType*> PointsData;

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

  void readPointsFromFile(QString fileName, PointsData& pointsData);

  void updateSplinesCalculator(PointsData pointsData);

  void savePointsToFile(QString fileName, int step = 1); 

public:
  MainWindow(QWidget *parent=0);

protected slots:
  void importSlot();
  void exportSlot();
  void closeSlot();
};

#endif
