#include "halfedge.h"
#include <la.h>
Vertex::Vertex(){
    pos = glm::vec3(0,0,0);
    edge = nullptr;
    id = -1;
}

Vertex::Vertex(int id){
    pos = glm::vec3(0,0,0);
    edge = nullptr;
    this->id = id;
}

Vertex::Vertex(glm::vec3 pos, HalfEdge *edge, int id){
    this->pos = pos;
    this->edge = edge;
    this->id = id;
}

Face::Face(){
    start_edge = nullptr;
    color = glm::vec3(0,0,0);
    id = -1;
}

Face::Face(int id){
    start_edge = nullptr;
    color = glm::vec3(0,0,0);
    this->id = id;
}

Face::Face(HalfEdge *start_edge, int id){
    this->start_edge = start_edge;
    color = glm::vec3(0,0,0);
    this->id = id;
}

Face::Face(HalfEdge *start_edge, glm::vec3 color, int id){
    this->start_edge = start_edge;
    this->color = color;
    this->id = id;
}

HalfEdge::HalfEdge(){
    face = nullptr;
    vert = nullptr;
    next = nullptr;
    sym = nullptr;
    id = -1;
}

HalfEdge::HalfEdge(int id){
    face = nullptr;
    vert = nullptr;
    next = nullptr;
    sym = nullptr;
    this->id = id;
}

HalfEdge::HalfEdge(Face *face, Vertex *vert, HalfEdge *next, HalfEdge *sym, int id){
    this->face = face;
    this->vert = vert;
    this->next = next;
    this->sym = sym;
    this->id = id;
}
