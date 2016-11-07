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
    ShaderProgram prog_lambert;// A shader program that uses lambertian reflection
    ShaderProgram prog_flat;// A shader program that uses "flat" reflection (no shadowing at all)

    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
                // Don't worry too much about this. Just know it is necessary in order to render geometry.

    Camera gl_camera;

    /// Timer linked to timerUpdate(). Fires approx. 60 times per second
    QTimer timer;

    int timeCount;
    int RenderMode;
    int Func1;


public:
    explicit MyGL(QWidget *parent = 0);
    ~MyGL();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void Test();
protected:
    void keyPressEvent(QKeyEvent *e);
signals:
    void sig_PassVertices(int);
    void sig_PassHalfEdges(int);
    void sig_PassFaces(int);
    void sig_ChangeSelectedVertex(int);
    void sig_ChangeSelectedHalfEdge(int);
    void sig_ChangeSelectedFace(int);
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


};


#endif // MYGL_H
