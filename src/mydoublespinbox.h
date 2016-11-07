#ifndef MYDOUBLESPINBOX_H
#define MYDOUBLESPINBOX_H
#include <QWidget>
#include <QDoubleSpinBox>

class MyDoubleSpinbox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    MyDoubleSpinbox(QWidget *parent);
public slots:
    void slot_SetClear();
};

#endif // MYDOUBLESPINBOX_H
