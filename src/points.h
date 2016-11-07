#ifndef POINTS_H
#define POINTS_H
#include "drawable.h"
#include <la.h>
#include <mesh.h>

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class Points : public Drawable
{
public:
    Vertex *V;
    Points(GLWidget277* context);
    virtual void create();
    virtual GLenum drawMode();
    void setVert(Vertex *);
};

class HalfLines : public Drawable
{
public:
    HalfEdge *HE;
    HalfLines(GLWidget277* context);
    virtual void create();
    virtual GLenum drawMode();
    void setLine(HalfEdge *);
};

class HFace : public Drawable{
public:
    Face *F;
    HFace(GLWidget277* context);
    virtual void create();
    virtual GLenum drawMode();
    void setFace(Face *);
};

#endif // POINTS_H
