#ifndef __main_window_h__

#define __main_window_h__

#include <QMainWindow>
#include <QFormLayout>
#include <SplinesCalculator.h>
#include <QComboBox>
#include <QLineEdit>

#include <utility>

#include "LineEdit.h"

#define MAX_POINTS_COUNT 50
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
      int pointsOnLine = 1,
      char separator = 0); 

  void writeLog();

  int findBiggestNonZero();

  void updateInputs(QString fileName);

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


#include <locale>       // std::locale

class Numpunct : public std::numpunct<char> {
  char _separator;

  public: 
    Numpunct(char separator) {
      _separator = separator;
    }

  protected:
    virtual char do_thousands_sep() const {
        return _separator;
    }

    virtual std::string do_grouping() const {
        return "\03";
    }
};

#endif
