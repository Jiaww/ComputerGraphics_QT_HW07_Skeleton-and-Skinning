#include <QList>
#include <QListWidget>
#include <QListWidgetItem>
#include "mylistwidget.h"

MyListWidget::MyListWidget(QWidget *p) : QListWidget(p)
{

}

void MyListWidget::slot_ReceiveVertices(int a){
    clear();
    for(int i = 0; i < a; i++){
        QString x;
        x.setNum(i,10);
        QListWidgetItem* lst = new QListWidgetItem(x);
        addItem(lst);
    }
}

void MyListWidget::slot_ReceiveHalfEdges(int a){
    clear();
    for(int i = 0; i < a; i++){
        QString x;
        x.setNum(i,10);
        QListWidgetItem* lst = new QListWidgetItem(x);
        addItem(lst);
    }
}

void MyListWidget::slot_ReceiveFaces(int a){
    clear();
    for(int i = 0; i < a; i++){
        QString x;
        x.setNum(i,10);
        QListWidgetItem* lst = new QListWidgetItem(x);
        addItem(lst);
    }
}

void MyListWidget::slot_ReceiveChangeVertex(int a){
    setCurrentRow(a);
}
void MyListWidget::slot_ReceiveChangeHalfEdge(int a){
    setCurrentRow(a);
}
void MyListWidget::slot_ReceiveChangeFace(int a){
    setCurrentRow(a);
}
