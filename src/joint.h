#include <iostream>
#include <QTreeWidget>
#include <QString>
#include <QTimer>
#include <la.h>
#include <glwidget277.h>
#include <drawable.h>

#ifndef JOINT_H
#define JOINT_H

class WireFrame;
class BoneFrame;

class Joint : public QTreeWidgetItem{
public:
    int id = -1;
    QString name;
    Joint *Parent;
    std::vector<Joint *>Children;
    glm::vec4 Position;
    glm::quat Orientation;
    glm::mat4 BindMatrix;
    virtual void addChild(QTreeWidgetItem*);
    WireFrame *WF;
    BoneFrame *BF;
    Joint(QString name);

    glm::mat4 GetLocalTransformation();
    glm::mat4 GetOverallTranformation();

};

class WireFrame : public Drawable{
public:
    Joint *J;
    glm::vec4 color;
    WireFrame(GLWidget277* context);
    virtual void create();
    virtual GLenum drawMode();
};

class BoneFrame : public Drawable{
public:
    Joint *Parent, *Child;
    BoneFrame(GLWidget277* context);
    virtual void create();
    virtual GLenum drawMode();
};

#endif // JOINT_H


