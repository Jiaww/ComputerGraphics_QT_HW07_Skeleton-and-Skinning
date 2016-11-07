#include "mydoublespinbox.h"
#include <QSpinBox>
#include <QDoubleSpinBox>
MyDoubleSpinbox::MyDoubleSpinbox(QWidget *parent)
  : QDoubleSpinBox(parent)
{

}

void MyDoubleSpinbox::slot_SetClear(){
    MyDoubleSpinbox::setValue(0.0);
}
