#include "LineEdit.h"
#include <cstdlib>
#include <iostream>

LineEdit::LineEdit(QWidget *parent, int index, int type) : QLineEdit(parent) {
  this->index = index;
  this->type = type;
  connect(this, SIGNAL(textChanged(QString)), this, SLOT(changed(QString)));
  setText("0");
}

void LineEdit::changed(const QString& text) {
  emit valueChanged(index, type); 
}

void LineEdit::setIndex(int index) {
  this->index = index;
}

void LineEdit::setType(int type) {
  this->type = type;
}
