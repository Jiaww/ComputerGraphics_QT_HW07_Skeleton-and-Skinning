#ifndef MYGL_H
#define MYGL_H

#include <glwidget277.h>
#include <utils.h>
#include <shaderprogram.h>
#include <scene/cylinder.h>
#include <scene/sphere.h>
#include "camera.h"
#include <mesh.h>
#include <points.h>

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QList>
#include <QListWidget>
#include <joint.h>
#include <iostream>
#include <QString>

class MyGL
    : public GLWidget277
{
    Q_OBJECT
private:
    Cylinder geom_cylinder;// The instance of a unit cylinder we can use to render any cylinder
    Sphere geom_sphere;// The instance of a unit sphere we can use to render any sphere

    Mesh geom_mesh;//Mesh unit
    Points HighlightVertex;
    HalfLines HighlightHalfLine;
    HFace HighlightHFace;

    Joint *Root;
    Joint *Selected;
    ShaderProgram prog_lambert;// A shader program that uses lambertian reflection
    ShaderProgram prog_flat;// A shader program that uses "flat" reflection (no shadowing at all)

    ShaderProgram prog_skeleton;//A shader program to do skeleton shading

    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
                // Don't worry too much about this. Just know it is necessary in order to render geometry.

    Camera gl_camera;

    /// Timer linked to timerUpdate(). Fires approx. 60 times per second
    QTimer timer;

    int timeCount;
    int RenderMode;
    int Func1;
    //Decide when to shade using prog_skeleton
    bool Skeleton_Shader = false;

public:

    explicit MyGL(QWidget *parent = 0);
    ~MyGL();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    //Traverse the Skeleton
    void Traverse(Joint *J);
    void Traverse_Draw(Joint *J);
    void Test();
    //Load QJson file of Skeleton:
    void LoadQJsonSkeleton();
    //Skinning Function
    //Set BindMatrix before skinning
    void SetBindMatrix();
    void Skinning();
    void setSkeletonShader(){Skeleton_Shader = true;}
    void SendBindTOGPU();
    void SendTransTOGPU();
protected:
    void keyPressEvent(QKeyEvent *e);
signals:
    void sig_PassVertices(int);
    void sig_PassHalfEdges(int);
    void sig_PassFaces(int);
    void sig_ChangeSelectedVertex(int);
    void sig_ChangeSelectedHalfEdge(int);
    void sig_ChangeSelectedFace(int);
    //signal of joint
    void sig_RootNode(QTreeWidgetItem*);
    //Show the Transformation of Joint
    void sig_TransFirstLine(QString);
    void sig_TransSecondLine(QString);
    void sig_TransThirdLine(QString);
    void sig_TransFourthLine(QString);

private slots:
    /// Slot that gets called ~60 times per second
    void timerUpdate();
    //VertexItem Clicked
    void slot_VertexClicked(QListWidgetItem*);
    void slot_HalfEdgeClicked(QListWidgetItem*);
    void slot_FaceClicked(QListWidgetItem*);
    void slot_PosXChanged(double);
    void slot_PosYChanged(double);
    void slot_PosZChanged(double);
    void slot_ColXChanged(double);
    void slot_ColYChanged(double);
    void slot_ColZChanged(double);

    void slot_LoadOBJFile();
    void slot_ExportOBJFile();

    void slot_AddVertex();
    void slot_ExtrudeFace();
    //planarity testing
    void slot_Planarity();

    //sharp:
    void slot_SetSharpVert();
    void slot_SetSharpEdge();
    void slot_SetSharpFace();

    //Beveling:
    void slot_BevelingEdge();
    void slot_BevelingFace();

    //Load QJson of Skeleton
    void slot_LoadSkeleton();

    //Skinning
    void slot_Skinning();

    //ItemSelected
    void slot_SelectedJoint(QTreeWidgetItem*);

    //Rotate of Joint
    void slot_Joint_Rotate_X();
    void slot_Joint_Rotate_Y();
    void slot_Joint_Rotate_Z();
    //Translate of Joint
    void slot_Joint_Translate_X();
    void slot_Joint_Translate_Y();
    void slot_Joint_Translate_Z();

};


#endif // MYGL_H
