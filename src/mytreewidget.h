#ifndef MYTREEWIDGET_H
#define MYTREEWIDGET_H
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <mygl.h>

class MyTreeWidget : public QTreeWidget
{
    Q_OBJECT    //This macro tells Qt's Meta-Object Compiler to produce a source file with Qt meta-object data in it,
                //such as signal and slot data with which other Qt classes can interface.
                //In this case, the file created is moc_mainwindow.cpp, where the moc stands for meta-object compiler.
                //You should never have to make changes to this type of file, and in fact any changes you make will be reverted
                //when Qt re-compiles the class.

private:
    QString newNodeName;
public:
    MyTreeWidget(QWidget* p);
public slots:       // Slots are callable just like regular functions, but they can also be
                    // linked to signals emitted from other Q_OBJECTs, or even this Q_OBJECT itself.
                    // When linked to a signal, a slot gets called any time that signal is emitted,
                    // and it is called on the data passed by the emitted signal.
                    // For organizational purposes, I like to prefix all my slot names with slot_.


    void slot_ReturnRootNode(QTreeWidgetItem*n);

};

#endif // MYTREEWIDGET_H
