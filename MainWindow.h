#ifndef __main_window_h__

#define __main_window_h__

#include <QMainWindow>

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    
  void createMenuItem(QString label,
      QString iconLocation,
      QString shortCut,
      QWidget *addTo,
      const char * func,
      QWidget* signalTo);

public:
  MainWindow(QWidget *parent=0);

protected slots:
  void importSlot();
  void exportSlot();
  void closeSlot();
};

#endif
