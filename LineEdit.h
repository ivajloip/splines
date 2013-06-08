#ifndef __line_edit_h__

#define __line_edit_h__

#include <QLineEdit>
#include "SplinesCalculator.h"

class LineEdit : public QLineEdit {
    Q_OBJECT
private:
  int index;
  int type;

public:
  LineEdit(QWidget *parent=0, int index = -1, int type = -1);

  void setIndex(int index);

  void setType(int type);

signals:
  void valueChanged(int index, int type);

public slots:
  void changed(const QString& text);
};

#endif
