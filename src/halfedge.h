#ifndef HALFEDGE_H
#define HALFEDGE_H
#include <la.h>

class HalfEdge;
class Vertex{
public:
    int id;
    int sharpness = 0;
    glm::vec3 pos;
    HalfEdge *edge;
    Vertex();
    Vertex(int id);
    Vertex(glm::vec3 pos, HalfEdge *edge, int id);
};

class Face{
public:
    int id;
    bool sharp = false;
    HalfEdge *start_edge;
    Vertex *Face_Point;
    glm::vec3 color;
    Face();
    Face(int id);
    Face(HalfEdge *start_edge, int id);
    Face(HalfEdge *start_edge, glm::vec3 color, int id);
};

class HalfEdge
{
public:
    int id;
    bool sharp = false;
    Face *face;
    Vertex *vert;
    HalfEdge *next;
    HalfEdge *sym;
    HalfEdge();
    HalfEdge(int id);
    HalfEdge(Face *face, Vertex *vert, HalfEdge *next, HalfEdge *sym, int id);
};

#endif // HALFEDGE_H
