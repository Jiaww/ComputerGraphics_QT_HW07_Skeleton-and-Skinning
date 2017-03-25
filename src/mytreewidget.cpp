#include "mytreewidget.h"

MyTreeWidget::MyTreeWidget(QWidget *p) : QTreeWidget(p), newNodeName()
{

}

void MyTreeWidget::slot_ReturnRootNode(QTreeWidgetItem *n){
    addTopLevelItem(n);
}
