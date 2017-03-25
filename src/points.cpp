#include "points.h"
#include<la.h>


Points::Points(GLWidget277 *context) : Drawable(context)
{}

void Points::setVert(Vertex *V){
    this->V = V;
}

GLenum Points::drawMode(){
    glDisable(GL_DEPTH_TEST);
    return GL_POINTS;
}

void Points::create(){
    GLuint idx[1]={0};
    glm::vec4 vert_pos[1];
    glm::vec4 vert_color[1];
    glm::vec4 vert_nor[1];
    vert_nor[0] = glm::vec4(0,1,0,1);
    vert_pos[0] = glm::vec4(V->pos,1);
    count = 1;
    vert_color[0] = glm::vec4(1,1,1,1);
    // Create a VBO on our GPU and store its handle in bufIdx
    generateIdx();
    // Tell OpenGL that we want to perform subsequent operations on the VBO referred to by bufIdx
    // and that it will be treated as an element array buffer (since it will contain triangle indices)
    context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    // Pass the data stored in cyl_idx into the bound buffer, reading a number of bytes equal to
    // SPH_IDX_COUNT multiplied by the size of a GLuint. This data is sent to the GPU to be read by shader programs.
    context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), idx, GL_STATIC_DRAW);

    // The next few sets of function calls are basically the same as above, except bufPos and bufNor are
    // array buffers rather than element array buffers, as they store vertex attributes like position.
    generatePos();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    context->glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::vec4), vert_pos, GL_STATIC_DRAW);

    generateNor();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufNor);
    context->glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::vec4), vert_nor, GL_STATIC_DRAW);
    //Create a VBO of color
    generateCol();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    context->glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::vec4), vert_color, GL_STATIC_DRAW);
}

HalfLines::HalfLines(GLWidget277 *context) : Drawable(context)
{}

GLenum HalfLines::drawMode(){
    glDisable(GL_DEPTH_TEST);
    return GL_LINES;
}

void HalfLines::setLine(HalfEdge *HE){
    this->HE = HE;
}

void HalfLines::create(){
    GLuint idx[2]={0,1};
    glm::vec4 vert_pos[2];
    glm::vec4 vert_color[2];
    glm::vec4 vert_nor[2];

    vert_pos[0] = glm::vec4(HE->vert->pos,1);
    vert_color[0] = glm::vec4(1,0,0,1);//red
    vert_nor[0] = glm::vec4(0,1,0,1);
    HalfEdge *edge = HE;
    while(edge->next != HE)
        edge = edge->next;
    vert_pos[1] = glm::vec4(edge->vert->pos,1);
    vert_color[1] = glm::vec4(1,1,0,1);
    vert_nor[1] = glm::vec4(0,1,0,1);

    count = 2;
    // Create a VBO on our GPU and store its handle in bufIdx
    generateIdx();
    // Tell OpenGL that we want to perform subsequent operations on the VBO referred to by bufIdx
    // and that it will be treated as an element array buffer (since it will contain triangle indices)
    context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    // Pass the data stored in cyl_idx into the bound buffer, reading a number of bytes equal to
    // SPH_IDX_COUNT multiplied by the size of a GLuint. This data is sent to the GPU to be read by shader programs.
    context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), idx, GL_STATIC_DRAW);

    // The next few sets of function calls are basically the same as above, except bufPos and bufNor are
    // array buffers rather than element array buffers, as they store vertex attributes like position.
    generatePos();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    context->glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec4), vert_pos, GL_STATIC_DRAW);

    generateNor();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufNor);
    context->glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec4), vert_nor, GL_STATIC_DRAW);

    //Create a VBO of color
    generateCol();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    context->glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec4), vert_color, GL_STATIC_DRAW);
}


HFace::HFace(GLWidget277 *context) : Drawable(context)
{}

GLenum HFace::drawMode(){
    glDisable(GL_DEPTH_TEST);
    return GL_LINES;
}

void HFace::setFace(Face *F){
    this->F = F;
}

void HFace::create(){
    int Vert_num=0;
    HalfEdge *H = F->start_edge;
    do{
        Vert_num++;
        H=H->next;
    }while(H != F->start_edge);

    GLuint idx[Vert_num * 2];
    for(int j = 0; j < Vert_num; j++){
        if(j<Vert_num-1){
            idx[2*j] = j;
            idx[2*j + 1] = j+1;
        }
        else{
            idx[2*j] = j;
            idx[2*j + 1] = 0;
        }

    }
    glm::vec4 vert_pos[Vert_num];
    glm::vec4 vert_color[Vert_num];
    glm::vec4 vert_nor[Vert_num];
    HalfEdge *HE = F->start_edge;
    int i = 0;
    do{
    vert_pos[i] = glm::vec4(HE->vert->pos[0],HE->vert->pos[1],HE->vert->pos[2],1);
    vert_color[i] = glm::vec4(0,1,1,1);
    vert_nor[i] = glm::vec4(0,1,0,1);
    i++;
    HE = HE->next;
    }while(HE != F->start_edge);

    count = Vert_num * 2;
    // Create a VBO on our GPU and store its handle in bufIdx
    generateIdx();
    // Tell OpenGL that we want to perform subsequent operations on the VBO referred to by bufIdx
    // and that it will be treated as an element array buffer (since it will contain triangle indices)
    context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    // Pass the data stored in cyl_idx into the bound buffer, reading a number of bytes equal to
    // SPH_IDX_COUNT multiplied by the size of a GLuint. This data is sent to the GPU to be read by shader programs.
    context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), idx, GL_STATIC_DRAW);

    // The next few sets of function calls are basically the same as above, except bufPos and bufNor are
    // array buffers rather than element array buffers, as they store vertex attributes like position.
    generatePos();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    context->glBufferData(GL_ARRAY_BUFFER, count / 2 * sizeof(glm::vec4), vert_pos, GL_STATIC_DRAW);

    generateNor();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufNor);
    context->glBufferData(GL_ARRAY_BUFFER, count / 2 * sizeof(glm::vec4), vert_nor, GL_STATIC_DRAW);

    //Create a VBO of color
    generateCol();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    context->glBufferData(GL_ARRAY_BUFFER, count / 2 * sizeof(glm::vec4), vert_color, GL_STATIC_DRAW);
}
