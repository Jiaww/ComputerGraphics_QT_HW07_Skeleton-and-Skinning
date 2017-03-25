#include "joint.h"
#include <iostream>
#include <QTreeWidget>
#include <QApplication>
#include <QString>
#include <QTimer>
#include <la.h>

Joint::Joint(QString name)
{
    this->name = name;
    Parent = nullptr;
    WF = nullptr;
    BF = nullptr;
    Position = glm::vec4(0,0,0,1);
    Orientation = glm::quat(1,0,0,0);
    BindMatrix = glm::mat4(1.0f);
    setText(0, name);
}

glm::mat4 Joint::GetLocalTransformation(){
    glm::mat4 F = glm::mat4_cast(Orientation);
    //Convert the orientation to the rotation matrix
    //
    F[3][0] = Position[0];
    F[3][1] = Position[1];
    F[3][2] = Position[2];
    return F;
}

glm::mat4 Joint::GetOverallTranformation(){
    if(Parent == nullptr){
        return GetLocalTransformation();
    }
    return Parent->GetOverallTranformation() * GetLocalTransformation();
}

//WireFrame

WireFrame::WireFrame(GLWidget277 *context) : Drawable(context)
{
    color = glm::vec4(1,1,1,1);
}

GLenum WireFrame::drawMode(){
    return GL_LINES;
}

void WireFrame::create(){
    GLuint idx[120]={};
    glm::vec4 WF_vert_color[60];
    glm::vec4 WF_vert_nor[60];
    glm::vec4 WF_vert_pos[60];

    glm::vec4 v;
    glm::vec4 origin = glm::vec4(J->GetOverallTranformation()[0][3],J->GetOverallTranformation()[1][3],J->GetOverallTranformation()[2][3],1);
    glm::mat4 transform = J->GetOverallTranformation();
//    std::cout<<"Matrix:"<<transform[0][0]<<" "<<transform[0][1]<<" "<<transform[0][2]<<" "<<transform[0][3]<<"\n";
//    std::cout<<"Matrix:"<<transform[1][0]<<" "<<transform[1][1]<<" "<<transform[1][2]<<" "<<transform[1][3]<<"\n";
//    std::cout<<"Matrix:"<<transform[2][0]<<" "<<transform[2][1]<<" "<<transform[2][2]<<" "<<transform[2][3]<<"\n";
//    std::cout<<"Matrix:"<<transform[3][0]<<" "<<transform[3][1]<<" "<<transform[3][2]<<" "<<transform[3][3]<<"\n";

    count = 120;
    // Create three circle
    for(int i = 0; i < 20; i++){
        v = glm::rotate(glm::mat4(1.0f), glm::radians(i * 18.0f), glm::vec3(1,0,0)) * glm::vec4(0,0.5f,0,1);
        WF_vert_pos[i] = transform * glm::vec4(glm::vec3(v),1);
        WF_vert_nor[i] = glm::vec4(0,0,1,1);
        WF_vert_color[i] = color;
    }
    for(int i = 0; i < 20; i++){
        v = glm::rotate(glm::mat4(1.0f), glm::radians(i * 18.0f), glm::vec3(0,1,0)) * glm::vec4(0,0,0.5f,1);
        WF_vert_pos[i+20] = transform * glm::vec4(glm::vec3(v),1);
        WF_vert_nor[i+20] = glm::vec4(0,0,1,1);
        WF_vert_color[i+20] = color;
    }
    for(int i = 0; i < 20; i++){
        v = glm::rotate(glm::mat4(1.0f), glm::radians(i * 18.0f), glm::vec3(0,0,1)) * glm::vec4(0.5f,0,0,1);
        WF_vert_pos[i+40] = transform * glm::vec4(glm::vec3(v),1);
        WF_vert_nor[i+40] = glm::vec4(0,0,1,1);
        WF_vert_color[i+40] = color;
    }
    // Create the indexes of each circle
    for(int i = 0; i < 20; i++){
        if(i == 19){
            idx[2*i] = 19;
            idx[2*i + 1] = 0;
        }else{
            idx[2*i] = i;
            idx[2*i + 1] = i+1;
        }
    }
    for(int i = 20; i < 40; i++){
        if(i == 39){
            idx[2*i] = 39;
            idx[2*i + 1] = 20;
        }else{
            idx[2*i] = i;
            idx[2*i + 1] = i+1;
        }
    }
    for(int i = 40; i < 60; i++){
        if(i == 59){
            idx[2*i] = 59;
            idx[2*i + 1] = 40;
        }else{
            idx[2*i] = i;
            idx[2*i + 1] = i+1;
        }
    }
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
    context->glBufferData(GL_ARRAY_BUFFER, 60 * sizeof(glm::vec4), WF_vert_pos, GL_STATIC_DRAW);

    generateNor();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufNor);
    context->glBufferData(GL_ARRAY_BUFFER, 60 * sizeof(glm::vec4), WF_vert_nor, GL_STATIC_DRAW);
    //Create a VBO of color
    generateCol();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    context->glBufferData(GL_ARRAY_BUFFER, 60 * sizeof(glm::vec4), WF_vert_color, GL_STATIC_DRAW);
}


//BoneFrame
BoneFrame::BoneFrame(GLWidget277 *context) : Drawable(context)
{}

GLenum BoneFrame::drawMode(){
    return GL_LINES;
}

void BoneFrame::create(){
    GLuint idx[2]={0,1};
    glm::vec4 vert_pos[2];
    glm::vec4 vert_color[2];
    glm::vec4 vert_nor[2];

    vert_pos[0] = glm::vec4(Parent->GetOverallTranformation()[3][0],Parent->GetOverallTranformation()[3][1],Parent->GetOverallTranformation()[3][2],1);
    vert_color[0] = glm::vec4(1,0,1,1);//magenta
    vert_nor[0] = glm::vec4(0,1,0,1);

    vert_pos[1] = glm::vec4(Child->GetOverallTranformation()[3][0],Child->GetOverallTranformation()[3][1],Child->GetOverallTranformation()[3][2],1);
    vert_color[1] = glm::vec4(1,1,0,1);//yellow
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

void Joint::addChild(QTreeWidgetItem *c)
{
    QTreeWidgetItem::addChild(c);
    Children.push_back((Joint*)c);
}
