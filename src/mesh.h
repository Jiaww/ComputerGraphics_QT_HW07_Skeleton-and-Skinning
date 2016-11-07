#ifndef MESH_H
#define MESH_H

#include <drawable.h>
#include <halfedge.h>
#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <la.h>


class Mesh : public Drawable
{
public:
    std::vector <Face *> Faces;
    std::vector <HalfEdge *> HalfEdges;
    std::vector <Vertex *> Vertices;
    //Mesh();
    Mesh(GLWidget277 *context);
    virtual void create();
    bool Planarity_Testing(Face *F);
    void Triangular(Face *F);
    Vertex * insertVertex(HalfEdge *HE1);
    void Quadrangualte(int num_original_Vertices);
    void ExtrudeFace(Face *F);

    void LoadCube();
    int Test(){
        return Faces.size();
    }
    //ADD Vertices for subdivision:
    int ADDandMOVE();
    void RepositionVert(Vertex *V, glm::vec4 new_pos);

    void setFromFile(std::vector<glm::vec4> vert_pos, std::vector<glm::vec4> vert_uv,
                     std::vector<glm::vec4> vert_nor, std::vector<QStringList> faces_con);

    //sharp:
    void setSharp_Vertex(Vertex *V);
    void setSharp_Edge(HalfEdge *HE);
    void setSharp_Face(Face *F);

    //beveling edge
    void BevelingEdge(HalfEdge *HE);
    void BevelingFace(Face *F);
};

#endif // MESH_H
