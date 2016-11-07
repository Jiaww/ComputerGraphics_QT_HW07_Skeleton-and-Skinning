#include <QListWidget>
#include <QListWidgetItem>
#ifndef MYLISTVIEW_H
#define MYLISTVIEW_H


class MyListWidget:public QListWidget
{
    Q_OBJECT
private:
public:
    MyListWidget(QWidget *p);
public slots:
    void slot_ReceiveVertices(int);
    void slot_ReceiveHalfEdges(int);
    void slot_ReceiveFaces(int);
    void slot_ReceiveChangeVertex(int);
    void slot_ReceiveChangeHalfEdge(int);
    void slot_ReceiveChangeFace(int);
};

#endif // MYLISTVIEW_H

