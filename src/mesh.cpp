#include "mesh.h"
#include <halfedge.h>
#include <la.h>
#include <iostream>
#include <random>
#include <ctime>
#include <cstdlib>
#include <map>

//Mesh::Mesh()
//{

//}

Mesh::Mesh(GLWidget277 *context) : Drawable(context)
{}

void Mesh::Triangular(Face *F){
    srand((unsigned)time(NULL));
    HalfEdge *HE0 = F->start_edge;
    int count = 1;
    int id = HE0->id;
    HE0 = HE0->next;
    while(HE0->id != id){
        count++;
        HE0 = HE0->next;
    }
    while(count > 3){
        HE0 = F->start_edge;
        //quadrangle:
        //Step1: Create two new halfedges HEA and HEB
        HalfEdge *HEA = new HalfEdge;
        HalfEdge *HEB = new HalfEdge;
        HEA->id = HalfEdges.size();
        HEB->id = HalfEdges.size() + 1;
        HEA->vert = HE0->vert;
        HalfEdge *HETemp = HE0;
        while(HETemp->next->next != HE0){
            HETemp = HETemp->next;
        }
        HEB->vert = HETemp->vert;
        HEA->sym = HEB;
        HEB->sym = HEA;
        //Step2: Create a second face Face2
        Face *FACE2 = new Face;

        //set color to tell
        float r, g, b;
        r = float(rand()%1001)/1000.0;
        g = float(rand()%1001)/1000.0;
        b = float(rand()%1001)/1000.0;
        FACE2->color = glm::vec3(r,g,b);
        FACE2->id = Faces.size();
        HEB->face = FACE2;
        HE0->face = FACE2;
        HETemp->next->face = FACE2;
        HEA->face = F;
        FACE2->start_edge = HEB;
        F->start_edge = HEA;
        //Step3: Fix up the next pointers for our half-edges
        HEB->next = HETemp->next;
        HETemp->next = HEA;
        HEA->next = HE0->next;
        HE0->next = HEB;
        //Step4: push back the edges and face.(Face should be inserted before the begin(),
        //                                      because after sepratation, the remain face may still have more than 3 edges)
        HalfEdges.push_back(HEA);
        HalfEdges.push_back(HEB);
        Faces.push_back(FACE2);
        count--;
    }

}

void Mesh::create(){
    std::vector <int> mesh_idx;
    std::vector <glm::vec4> vert_pos;
    std::vector <glm::vec4> vert_nor;
    std::vector <glm::vec4> vert_col;
    std::vector <glm::ivec2> vert_inf;
    std::vector <glm::vec2> vert_wei;

    count = 0;
    //Set Indices colors normals, positions by faces:
    for(int i = 0; i < Faces.size(); i++){
        int set = vert_pos.size();
        int k = 0;// verts num of this face
        glm::vec3 color = Faces[i]->color;
        HalfEdge *edge = Faces[i]->start_edge;
        glm::vec3 normal = glm::normalize(glm::cross(edge->next->vert->pos - edge->vert->pos,
                                                     edge->next->next->vert->pos - edge->next->vert->pos));

//        //push back a fake normal for the first vert
//        vert_nor.push_back(glm::vec4(0,0,0,1));
        //push back first vert pos and color
        vert_pos.push_back(glm::vec4(edge->vert->pos,1));
        vert_col.push_back(glm::vec4(color,1));
        vert_nor.push_back(glm::vec4(normal, 1));
        //push back the influence of Joints
        glm::ivec2 inf;
        glm::vec2 weights;
        if(edge->vert->J_I[0].J != nullptr && edge->vert->J_I[1].J != nullptr){
            inf[0] = edge->vert->J_I[0].J->id;
            inf[1] = edge->vert->J_I[1].J->id;
            weights[0] = edge->vert->J_I[0].w;
            weights[1] = edge->vert->J_I[1].w;
        }
        vert_inf.push_back(inf);
        vert_wei.push_back(weights);

        k++;
        while(edge->next != Faces[i]->start_edge){
            edge = edge->next;
            vert_pos.push_back(glm::vec4(edge->vert->pos, 1));
            vert_col.push_back(glm::vec4(color, 1));
            vert_nor.push_back(glm::vec4(normal, 1));
            //push back the influence of Joints
            glm::ivec2 inf;
            glm::vec2 weights;
            if(edge->vert->J_I[0].J != nullptr && edge->vert->J_I[1].J != nullptr){
                inf[0] = edge->vert->J_I[0].J->id;
                inf[1] = edge->vert->J_I[1].J->id;
                weights[0] = edge->vert->J_I[0].w;
                weights[1] = edge->vert->J_I[1].w;
                std::cout<<edge->vert->J_I[0].J->name.toStdString()<<inf[0]<<" weight: "<< weights[0]<<"  "<<edge->vert->J_I[1].J->name.toStdString()<<inf[1]<<" weight: "<<weights[1]<<"\n";
            }
            vert_inf.push_back(inf);
            vert_wei.push_back(weights);
            k++;
        }
        //Triangluate:
        for(int j = 0; j+2 < k; j++){
            mesh_idx.push_back(set);
            mesh_idx.push_back(set+j+1);
            mesh_idx.push_back(set+j+2);
            }
    }
    count = mesh_idx.size();
    GLuint mesh_idx_array[count];
    glm::vec4 mesh_vert_col[vert_pos.size()];
    glm::vec4 mesh_vert_pos[vert_pos.size()];
    glm::vec4 mesh_vert_nor[vert_pos.size()];
    glm::ivec2 mesh_vert_inf[vert_pos.size()];
    glm::vec2 mesh_vert_wei[vert_pos.size()];

    for(int i = 0; i < count; i++){
        mesh_idx_array[i] = mesh_idx[i];
    }
    for(int i = 0; i < vert_pos.size(); i++){
        mesh_vert_pos[i] = vert_pos[i];
        mesh_vert_nor[i] = vert_nor[i];
        mesh_vert_col[i] = vert_col[i];
        mesh_vert_inf[i] = vert_inf[i];
        mesh_vert_wei[i] = vert_wei[i];
    }

    // Create a VBO on our GPU and store its handle in bufIdx
    generateIdx();
    // Tell OpenGL that we want to perform subsequent operations on the VBO referred to by bufIdx
    // and that it will be treated as an element array buffer (since it will contain triangle indices)
    context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    // Pass the data stored in cyl_idx into the bound buffer, reading a number of bytes equal to
    // SPH_IDX_COUNT multiplied by the size of a GLuint. This data is sent to the GPU to be read by shader programs.
    context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), mesh_idx_array, GL_STATIC_DRAW);

    // The next few sets of function calls are basically the same as above, except bufPos and bufNor are
    // array buffers rather than element array buffers, as they store vertex attributes like position.
    generatePos();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    context->glBufferData(GL_ARRAY_BUFFER, vert_pos.size() * sizeof(glm::vec4), mesh_vert_pos, GL_STATIC_DRAW);

    generateNor();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufNor);
    context->glBufferData(GL_ARRAY_BUFFER, vert_pos.size() * sizeof(glm::vec4), mesh_vert_nor, GL_STATIC_DRAW);

    //Create a VBO of color
    generateCol();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    context->glBufferData(GL_ARRAY_BUFFER, vert_pos.size() * sizeof(glm::vec4), mesh_vert_col, GL_STATIC_DRAW);
    //Create a VBO of influencer
    generateInf();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufInfluencer);
    context->glBufferData(GL_ARRAY_BUFFER, vert_pos.size() * sizeof(glm::ivec2), mesh_vert_inf, GL_STATIC_DRAW);
    //Create a VBO of Weights
    generateWei();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufWeights);
    context->glBufferData(GL_ARRAY_BUFFER, vert_pos.size() * sizeof(glm::vec2), mesh_vert_wei, GL_STATIC_DRAW);
}

void Mesh::LoadCube(){
    //Eight Vertices:
    Vertex *V0 = new Vertex(0), *V1 = new Vertex(1),
            *V2 = new Vertex(2), *V3 = new Vertex(3),
            *V4 = new Vertex(4), *V5 = new Vertex(5),
            *V6 = new Vertex(6), *V7 = new Vertex(7);

    //set pos
    V0->pos = glm::vec3(-0.5, -0.5,  0.5);
    V1->pos = glm::vec3(-0.5, -0.5, -0.5);
    V2->pos = glm::vec3( 0.5, -0.5, -0.5);
    V3->pos = glm::vec3( 0.5, -0.5,  0.5);
    V4->pos = glm::vec3(-0.5,  0.5,  0.5);
    V5->pos = glm::vec3( 0.5,  0.5,  0.5);
    V6->pos = glm::vec3( 0.5,  0.5, -0.5);
    V7->pos = glm::vec3(-0.5,  0.5, -0.5);

    //set id:
    V0->id = 0;
    V1->id = 1;
    V2->id = 2;
    V3->id = 3;
    V4->id = 4;
    V5->id = 5;
    V6->id = 6;
    V7->id = 7;


//    //set nor_id
//    V0->nor_id = 0;
//    V1->nor_id = 1;
//    V2->nor_id = 2;
//    V3->nor_id = 3;
//    V4->nor_id = 4;
//    V5->nor_id = 5;
//    V6->nor_id = 6;
//    V7->nor_id = 7;


    //bottom face:
    Face *F0 = new Face(0);
    F0->color = glm::vec3(1,0,1);
    HalfEdge *HE0 = new HalfEdge(0), *HE1 = new HalfEdge(1), *HE2 = new HalfEdge(2), *HE3 = new HalfEdge(3);
    F0->start_edge = HE0;
    //set next
    HE0->next = HE1;
    HE1->next = HE2;
    HE2->next = HE3;
    HE3->next = HE0;
    //set face
    HE0->face = F0;
    HE1->face = F0;
    HE2->face = F0;
    HE3->face = F0;
    //set Vertex
    HE0->vert = V0;
    V0->edge = HE0;
    HE1->vert = V1;
    V1->edge = HE1;
    HE2->vert = V2;
    V2->edge = HE2;
    HE3->vert = V3;
    V3->edge = HE3;


    //front face:
    Face *F1 = new Face(1);
    F1->color = glm::vec3(1,0,0);
    HalfEdge *HE4 = new HalfEdge(4), *HE5 = new HalfEdge(5), *HE6 = new HalfEdge(6), *HE7 = new HalfEdge(7);
    F1->start_edge = HE4;
    //set next
    HE4->next = HE5;
    HE5->next = HE6;
    HE6->next = HE7;
    HE7->next = HE4;
    //set face
    HE4->face = F1;
    HE5->face = F1;
    HE6->face = F1;
    HE7->face = F1;
    //set Vertex
    HE4->vert = V3;
    V3->edge = HE4;
    HE5->vert = V5;
    V5->edge = HE5;
    HE6->vert = V4;
    V4->edge = HE6;
    HE7->vert = V0;
    V0->edge = HE7;


    //right face:
    Face *F2 = new Face(2);
    HalfEdge *HE8 = new HalfEdge(8), *HE9 = new HalfEdge(9), *HE10 = new HalfEdge(10), *HE11 = new HalfEdge(11);
    F2->color = glm::vec3(0,0,1);
    F2->start_edge = HE9;
    //set next
    HE8->next = HE9;
    HE9->next = HE10;
    HE10->next = HE11;
    HE11->next = HE8;
    //set face
    HE8->face = F2;
    HE9->face = F2;
    HE10->face = F2;
    HE11->face = F2;
    //set Vertex
    HE8->vert = V2;
    V2->edge = HE8;
    HE9->vert = V6;
    V6->edge = HE9;
    HE10->vert = V5;
    V5->edge = HE10;
    HE11->vert = V3;
    V3->edge = HE11;


    //top face:
    Face *F3 = new Face(3);
    HalfEdge *HE12 = new HalfEdge(12), *HE13 = new HalfEdge(13), *HE14 = new HalfEdge(14), *HE15 = new HalfEdge(15);
    F3->color = glm::vec3(1,1,1);
    F3->start_edge = HE12;
    //set next
    HE12->next = HE13;
    HE13->next = HE14;
    HE14->next = HE15;
    HE15->next = HE12;
    //set face
    HE12->face = F3;
    HE13->face = F3;
    HE14->face = F3;
    HE15->face = F3;
    //set Vertex
    HE12->vert = V5;
    V5->edge = HE12;
    HE13->vert = V6;
    V6->edge = HE13;
    HE14->vert = V7;
    V7->edge = HE14;
    HE15->vert = V4;
    V4->edge = HE15;


    //back face:
    Face *F4 = new Face(4);
    HalfEdge *HE16 = new HalfEdge(16), *HE17 = new HalfEdge(17), *HE18 = new HalfEdge(18), *HE19 = new HalfEdge(19);
    F4->color = glm::vec3(0,1,1);
    F4->start_edge = HE16;
    //set next
    HE16->next = HE17;
    HE17->next = HE18;
    HE18->next = HE19;
    HE19->next = HE16;
    //set face
    HE16->face = F4;
    HE17->face = F4;
    HE18->face = F4;
    HE19->face = F4;
    //set Vertex
    HE16->vert = V1;
    V1->edge = HE16;
    HE17->vert = V7;
    V7->edge = HE17;
    HE18->vert = V6;
    V6->edge = HE18;
    HE19->vert = V2;
    V2->edge = HE19;


    //left face:
    Face *F5 = new Face(5);
    HalfEdge *HE20 = new HalfEdge(20), *HE21 = new HalfEdge(21), *HE22 = new HalfEdge(22), *HE23 = new HalfEdge(23);
    F5->color = glm::vec3(1,1,0);
    F5->start_edge = HE20;
    //set next
    HE20->next = HE21;
    HE21->next = HE22;
    HE22->next = HE23;
    HE23->next = HE20;
    //set face
    HE20->face = F5;
    HE21->face = F5;
    HE22->face = F5;
    HE23->face = F5;
    //set Vertex
    HE20->vert = V4;
    V4->edge = HE20;
    HE21->vert = V7;
    V7->edge = HE21;
    HE22->vert = V1;
    V1->edge = HE22;
    HE23->vert = V0;
    V0->edge = HE23;


    //set SYM:
    HE0->sym = HE4;
    HE1->sym = HE23;
    HE2->sym = HE16;
    HE3->sym = HE8;
    HE4->sym = HE0;
    HE5->sym = HE11;
    HE6->sym = HE12;
    HE7->sym = HE20;
    HE8->sym = HE3;
    HE9->sym = HE19;
    HE10->sym = HE13;
    HE11->sym = HE5;
    HE12->sym = HE6;
    HE13->sym = HE10;
    HE14->sym = HE18;
    HE15->sym = HE21;
    HE16->sym = HE2;
    HE17->sym = HE22;
    HE18->sym = HE14;
    HE19->sym = HE9;
    HE20->sym = HE7;
    HE21->sym = HE15;
    HE22->sym = HE17;
    HE23->sym = HE1;

    //push_back:
    //Faces
    Faces.push_back(F0);
    Faces.push_back(F1);
    Faces.push_back(F2);
    Faces.push_back(F3);
    Faces.push_back(F4);
    Faces.push_back(F5);
    //Vertices
    Vertices.push_back(V0);
    Vertices.push_back(V1);
    Vertices.push_back(V2);
    Vertices.push_back(V3);
    Vertices.push_back(V4);
    Vertices.push_back(V5);
    Vertices.push_back(V6);
    Vertices.push_back(V7);

    //HalfEdges
    HalfEdges.push_back(HE0);
    HalfEdges.push_back(HE1);
    HalfEdges.push_back(HE2);
    HalfEdges.push_back(HE3);
    HalfEdges.push_back(HE4);
    HalfEdges.push_back(HE5);
    HalfEdges.push_back(HE6);
    HalfEdges.push_back(HE7);
    HalfEdges.push_back(HE8);
    HalfEdges.push_back(HE9);
    HalfEdges.push_back(HE10);
    HalfEdges.push_back(HE11);
    HalfEdges.push_back(HE12);
    HalfEdges.push_back(HE13);
    HalfEdges.push_back(HE14);
    HalfEdges.push_back(HE15);
    HalfEdges.push_back(HE16);
    HalfEdges.push_back(HE17);
    HalfEdges.push_back(HE18);
    HalfEdges.push_back(HE19);
    HalfEdges.push_back(HE20);
    HalfEdges.push_back(HE21);
    HalfEdges.push_back(HE22);
    HalfEdges.push_back(HE23);
}

void Mesh::RepositionVert(Vertex *V, glm::vec4 new_pos){
    V->pos = glm::vec3(new_pos);
}


void Mesh::setFromFile(std::vector<glm::vec4> vert_pos, std::vector<glm::vec4> vert_uv,
                       std::vector<glm::vec4> vert_nor, std::vector<QStringList> faces_con){
    Vertices.clear();
    Faces.clear();
    HalfEdges.clear();
    //create Vertices by vert_pos:
    for(int i = 0; i < vert_pos.size(); i++){
        Vertex *V = new Vertex;
        V->pos = glm::vec3(vert_pos[i]);
        V->id = i;
        Vertices.push_back(V);
    }
    srand((unsigned)time(NULL));
    for(int i = 0; i < faces_con.size(); i++){
        // every line is a QStringList, representing a Face
        Face *F = new Face;
        float r, g, b;
        r = float(rand()%2001-1000)/1000.0;
        g = float(rand()%2001-1000)/1000.0;
        b = float(rand()%2001-1000)/1000.0;
        F->color = glm::vec3(r,g,b);
        int FirstHE = HalfEdges.size();
        for(int j = 1; j < faces_con[i].size(); j++){
            QStringList strlist = faces_con[i][j].split('/');
            //set V
            Vertex *V = Vertices[strlist[0].toInt() - 1];
            //set HE
            HalfEdge *HE = new HalfEdge;
            HE->face = F;
            HE->vert = V;
            V->edge = HE;
            HE->id = HalfEdges.size();
            //set the HE->next
            if(j != 1)
                HalfEdges[HalfEdges.size()-1]->next = HE;
            if(j == faces_con[i].size() -1){
                HE->next = HalfEdges[FirstHE];
            }
            HalfEdges.push_back(HE);
        }
        F->start_edge = HalfEdges[HalfEdges.size()-1];
        F->id = Faces.size();
        Faces.push_back(F);
    }
    std::cout<<Vertices.size()<<"\n"<<Faces.size()<<"\n"<<HalfEdges.size()<<"\n";
}

Vertex *Mesh::insertVertex(HalfEdge *HE1){
    //find V1, V2
    HalfEdge *HE2 = HE1->sym;
    Vertex *V1 = HE1->vert, *V2 = HE2->vert;
    //create V3
    Vertex *V3 = new Vertex;
    V3->pos = (V1->pos + V2->pos)/2.0f;
    //create HE1B,HE2B
    HalfEdge *HE1B = new HalfEdge, *HE2B = new HalfEdge;
    HE1B->vert = V1;
    HE2B->vert = V2;
    HE1B->face = HE1->face;
    HE2B->face = HE2->face;
    //set sym
    HE1->sym = HE2B;
    HE2B->sym = HE1;
    HE2->sym = HE1B;
    HE1B->sym = HE2;
    //set next
    HE1B->next = HE1->next;
    HE1->next = HE1B;
    HE2B->next = HE2->next;
    HE2->next = HE2B;
    HE1->vert = V3;
    HE2->vert = V3;
    //push back to the mesh structure
    HE1B->id = HalfEdges.size();
    HalfEdges.push_back(HE1B);
    HE2B->id = HalfEdges.size();
    HalfEdges.push_back(HE2B);
    V3->id = Vertices.size();
    V3->edge = HE1;
    V1->edge = HE1B;
    V2->edge = HE2B;
    Vertices.push_back(V3);
    if(HE1->sharp){
        HE1B->sharp = true;
        HE2B->sharp = true;
        V3->sharpness += 2;
    }
    return V3;
}


int Mesh::ADDandMOVE(){
    //ADD centroid(Face point):
    for(int i = 0; i < Faces.size(); i++){
        glm::vec3 pos;
        float length = 0;
        HalfEdge *HE = Faces[i]->start_edge;
        do{
            pos = pos + HE->vert->pos;
            length++;
            HE = HE->next;
        }while(HE != Faces[i]->start_edge);
        pos = pos / length;
        Vertex *FV = new Vertex;
        FV->pos = pos;
        Faces[i]->Face_Point = FV;
        if(Faces[i]->sharp) FV->sharpness += 3;
    }
    //ADD edge points:
    //Create a array to record whether this edge have created its edge point yet.
    bool Used[HalfEdges.size()] = {};
    //original number of halfedges and vertices
    int num_HalfEdges = HalfEdges.size();
    int num_Vertices = Vertices.size();
    for(int i = 0; i < num_HalfEdges; i++){
        //calculate the edge-point and insert mid-point to this edge:
        if(Used[HalfEdges[i]->id]||Used[HalfEdges[i]->sym->id])
            continue;
        Used[HalfEdges[i]->id] = true;
        Used[HalfEdges[i]->sym->id] = true;
        //calculate the edge-point
        glm::vec3 ep_pos;
        HalfEdge *HEF = HalfEdges[i];
        while(HEF->next != HalfEdges[i])
            HEF = HEF->next;
        ep_pos = (HalfEdges[i]->vert->pos +
                  HEF->vert->pos +
                  HalfEdges[i]->face->Face_Point->pos +
                  HalfEdges[i]->sym->face->Face_Point->pos) / 4.0f;
        Vertex *V_mid = insertVertex(HalfEdges[i]);
        if(HalfEdges[i]->face->sharp || HalfEdges[i]->sym->face->sharp){
            V_mid->sharpness = 3;
        }

        //Move mid-point to ep_pos:
//        std::cout<<"Before: "<<V_mid->pos[0]<<" "<<V_mid->pos[1]<<" "<<V_mid->pos[2]<<"\n";
        if(V_mid->sharpness < 3){
            if(V_mid->sharpness == 2){
                ep_pos = 0.75f * V_mid->pos + 0.125f * HalfEdges[i]->vert->pos + 0.125f * HEF->vert->pos;
                RepositionVert(V_mid, glm::vec4(ep_pos,1));
            }
            else{
                RepositionVert(V_mid, glm::vec4(ep_pos,1));
            }
        }
//        std::cout<<"After:  "<<V_mid->pos[0]<<" "<<V_mid->pos[1]<<" "<<V_mid->pos[2]<<"\n";
    }
   // calculate new vertices pos:
    for(int i = 0; i < num_Vertices; i++){
        glm::vec3 e_sum;
        glm::vec3 f_sum;
        int num = 0;
        Vertex *V = Vertices[i];
        HalfEdge *HEX = V->edge;
        //sharp endpoints:
        glm::vec3 endpoint1,endpoint2;
        bool flag = false;
        //compute the sum of e and the sum of f
        do{
            num++;
            e_sum += HEX->sym->vert->pos;
            f_sum += HEX->face->Face_Point->pos;
//            std::cout<<"MidPoint: "<<HEX->sym->vert->pos[0]<<" "<<HEX->sym->vert->pos[1]<<" "<<HEX->sym->vert->pos[2]<<"\n";
//            std::cout<<"FacPoint: "<<HEX->face->Face_Point->pos[0]<<" "<<HEX->face->Face_Point->pos[1]<<" "<<HEX->face->Face_Point->pos[2]<<"\n";
            HEX = HEX->next->sym;
            if(HEX->sharp) {
                if(!flag){
                    endpoint1 = HEX->vert->pos;
                    flag = true;
                }
                else{
                    endpoint2 = HEX->sym->vert->pos;
                }
            }
        }while(HEX != V->edge);
        glm::vec3 vert_new_pos;
        vert_new_pos = (float(num) - 2.0f) * V->pos / float(num) +
                        e_sum / float(num*num) + f_sum / float(num*num);
//        std::cout<<vert_new_pos[0]<<" "<<vert_new_pos[1]<<" "<<vert_new_pos[2]<<"\n";
        //move to the new pos

        if(V->sharpness < 3){
            if(V->sharpness == 2){
                vert_new_pos = 0.75f * V->pos + 0.125f * endpoint1 + 0.125f * endpoint2;
//                std::cout<<vert_new_pos[0]<<" "<<vert_new_pos[1]<<" "<<vert_new_pos[2]<<"\n";
                RepositionVert(V, glm::vec4(vert_new_pos,1));
            }
            else{
                RepositionVert(V, glm::vec4(vert_new_pos,1));
            }
        }

    }
    return num_Vertices;

}

void Mesh::Quadrangualte(int num_original_Vertices){
    std::vector<Face *> sub_Faces;
    srand((unsigned)time(NULL));
    for(int i = 0; i < Faces.size(); i++){
        //record the num of the old edges
        int num_old_edges = HalfEdges.size();
        HalfEdge *HE = Faces[i]->start_edge;
        if(HE->vert->id >= num_original_Vertices)
            HE = HE->next;
        HalfEdge *Start_Edge = HE;
        //to store the start vertices of each new quadrangle
        std::vector <Vertex *> Start_Vertex;
        int count = 0;
        Vertex *Vtemp = new Vertex;
        Start_Vertex.push_back(Vtemp);
        do{
            if(count % 2 == 1){
                if(HE->next == Start_Edge){
                    Start_Vertex[0] = HE->vert;
                } else{
                    Start_Vertex.push_back(HE->vert);
                }
            }
            count++;
            HE = HE->next;
        }while(HE != Start_Edge);
        //push back the face_point to the Vertices vector
        Faces[i]->Face_Point->id = Vertices.size();
        Vertices.push_back(Faces[i]->Face_Point);
        int num = 0;
        do{
            Face *F = new Face;
            HalfEdge *HE1 = HE;
            HalfEdge *HE2 = HE->next;
            HalfEdge *HE3 = new HalfEdge;
            HalfEdge *HE4 = new HalfEdge;
            //update the HE
            HE = HE->next->next;
            //Set Vertex for HE3 and HE4
            HE3->vert = Faces[i]->Face_Point;
            HE4->vert = Start_Vertex[num];
            num++;
            //Set next for HE1,2,3,4
            HE2->next = HE3;
            HE3->next = HE4;
            HE4->next = HE1;
            //Set face for HE1,2,3,4
            HE1->face = F;
            HE2->face = F;
            HE3->face = F;
            HE4->face = F;
            //Set id for HE3,4
            HE3->id = HalfEdges.size();
            HE4->id = HalfEdges.size()+1;
            //Set Face F
//            float r, g, b;
//            r = float(rand()%1001)/1000.0;
//            g = float(rand()%1001)/1000.0;
//            b = float(rand()%1001)/1000.0;
//            F->color = glm::vec3(r,g,b);
            F->color = glm::vec3(0,0.75,0.5);
            F->id = sub_Faces.size();
            F->start_edge = HE1;
            F->Face_Point = nullptr;
            //set Face_Point 's Edge
            Faces[i]->Face_Point->edge = HE3;
            //push back the HE3, HE4, F
            HalfEdges.push_back(HE3);
            HalfEdges.push_back(HE4);
            sub_Faces.push_back(F);
            if(Faces[i]->sharp){
                F->sharp = true;
            }
        }while(HE != Start_Edge);
        //Update the sym of the new edges
        //record the number of new edges, then the new added edges' index are from num_old_edges to num_new_edges-1
        int num_new_edges = HalfEdges.size();
        for(int m = num_old_edges; m < num_new_edges; m++){
            for(int n = m+1; n < num_new_edges; n++){
                glm::vec3 start_pos1, start_pos2;
                HalfEdge *H1 = HalfEdges[m], *H2 = HalfEdges[n];
                while(H1->next!=HalfEdges[m]){
                    H1 = H1->next;
                }
                start_pos1 = H1->vert->pos;
                while(H2->next!=HalfEdges[n]){
                    H2 = H2->next;
                }
                start_pos2 = H2->vert->pos;
                if(start_pos1 == HalfEdges[n]->vert->pos && start_pos2 == HalfEdges[m]->vert->pos){
                    HalfEdges[m]->sym = HalfEdges[n];
                    HalfEdges[n]->sym = HalfEdges[m];
                    break;
                }
            }
        }

    }
    while(Faces.size()!= 0){
        Faces.pop_back();
    }
    for(int i = 0; i < sub_Faces.size(); i++){
        Faces.push_back(sub_Faces[i]);
    }
    std::cout<<"Vertices: "<<Vertices.size()<<"\n"<<"Faces: "<<Faces.size()<<"\n"<<"HalfEdges: "<<HalfEdges.size()<<"\n";
}


void Mesh::ExtrudeFace(Face *F){
    HalfEdge *HE = F->start_edge;
    glm::vec3 normal = glm::normalize(glm::cross(HE->next->vert->pos - HE->vert->pos,
                                                 HE->next->next->vert->pos - HE->next->vert->pos));
    int num_old_edges = HalfEdges.size();
    //extrude edge by edge
    do{
        //if HE is the first edge to extrude
        if(HE == F->start_edge){
            //create 2 new Vertices
            Vertex *V3 = new Vertex, *V4 = new Vertex;
            V3->id = Vertices.size();
            V4->id = Vertices.size()+1;

            HalfEdge *HE1 = HE;
            HalfEdge *HE2 = HE->sym;
            Vertex *V1 = HE1->vert;
            HE1->vert = V3;
            HalfEdge *HE1_prev = HE1;
            Vertex *V2 = HE2->vert;
            V3->pos = V1->pos + 0.1f * normal;
            V4->pos = V2->pos + 0.1f * normal;
            while(HE1_prev->next != HE1){
                HE1_prev = HE1_prev->next;
            }
            HE1_prev->vert = V4;
            //create 2 new halfedges
            HalfEdge *HE1B = new HalfEdge, *HE2B = new HalfEdge;
            HE1->sym = HE1B;
            HE2->sym = HE2B;
            HE1B->sym = HE1;
            HE2B->sym = HE2;
            HE1B->vert = V4;
            HE2B->vert = V1;
            HE1B->id = HalfEdges.size();
            HE2B->id = HalfEdges.size()+1;
            //create 1 new face and other 2 new halfedges
            Face *new_F = new Face;
            HalfEdge *HE3 = new HalfEdge, *HE4 = new HalfEdge;
            F->id = Faces.size();
            HE3->id = HalfEdges.size()+2;
            HE4->id = HalfEdges.size()+3;
            //set Face pointer of the HE1B,2B,3,4
            HE1B->face = new_F;
            HE2B->face = new_F;
            HE3->face = new_F;
            HE4->face = new_F;
            HE3->vert = V3;
            HE4->vert = V2;
            HE1B->next = HE4;
            HE4->next = HE2B;
            HE2B->next = HE3;
            HE3->next = HE1B;
            //set the pointer of the V1,V2,V3,V4
            V1->edge = HE2B;
            V2->edge = HE2;
            V3->edge = HE1;
            V4->edge = HE1B;
            //set the start_edge of new_F and F
            new_F->color = glm::vec3(0,0.5,0.5);
            new_F->start_edge = HE1B;
            new_F->Face_Point = nullptr;
            //push back the new_F, HE1B,HE2B,HE3,HE4,V3,V4;
            Faces.push_back(new_F);
            HalfEdges.push_back(HE1B);
            HalfEdges.push_back(HE2B);
            HalfEdges.push_back(HE3);
            HalfEdges.push_back(HE4);
            Vertices.push_back(V3);
            Vertices.push_back(V4);
        }
        //if HE is the last edge to extrude, we no longer need to
        else if(HE->next == F->start_edge){
            //initialize
            HalfEdge *HE1 = HE;
            HalfEdge *HE2 = HE->sym;
            while(HE2->next != HE->sym)
                HE2 = HE2->next;
            Vertex *V1 = HE2->vert;
            HE2 = HE2->next;
            Vertex *V2 = HE2->vert;
            Vertex *V3 = HE1->next->sym->vert;
            HalfEdge *HE1_prev = HE1;
            while(HE1_prev->next != HE1){
                HE1_prev = HE1_prev->next;
            }
            Vertex *V4 = HE1_prev->vert;
            HE1->vert = V3;
            //create 2 halfedges
            HalfEdge *HE1B = new HalfEdge, *HE2B = new HalfEdge;
            HE1B->id = HalfEdges.size();
            HE2B->id = HalfEdges.size() + 1;
            HE1->sym = HE1B;
            HE1B->sym = HE1;
            HE2->sym = HE2B;
            HE2B->sym = HE2;
            HE1B->vert = V4;
            HE2B->vert = V1;
            //create 1 new face and 2 new halfedges HE3, HE4
            Face *new_F = new Face;
            HalfEdge *HE3 = new HalfEdge, *HE4 = new HalfEdge;
            new_F->id = Faces.size();
            HE3->id = HalfEdges.size()+2;
            HE4->id = HalfEdges.size()+3;
            HE1B->face = F;
            HE2B->face = F;
            HE3->face = F;
            HE4->face = F;
            HE3->vert = V3;
            HE4->vert = V2;
            HE1B->next = HE4;
            HE4->next = HE2B;
            HE2B->next = HE3;
            HE3->next = HE1B;
            //set the pointer of the V1,V2
            V1->edge = HE2B;
            V2->edge = HE2;
            //set the start_edge of new_F and F
            new_F->color = glm::vec3(0.5,0,0.5);
            new_F->start_edge = HE1B;
            new_F->Face_Point = nullptr;
            //push back the new_F, HE1B,HE2B,HE3,HE4;
            Faces.push_back(new_F);
            HalfEdges.push_back(HE1B);
            HalfEdges.push_back(HE2B);
            HalfEdges.push_back(HE3);
            HalfEdges.push_back(HE4);
        }
        //the middle edges, which means V4 has already been created
        else{
            //create new Vertex V3
            Vertex *V3 = new Vertex;
            V3->id = Vertices.size();
            HalfEdge *HE1 = HE;
            HalfEdge *HE2 = HE->sym;
            while(HE2->next != HE->sym)
                HE2 = HE2->next;
            Vertex *V1 = HE2->vert;
            HE2 = HE2->next;
            Vertex *V2 = HE2->vert;
            HE1->vert = V3;
            HalfEdge *HE1_prev = HE1;
            V3->pos = V1->pos + 0.1f * normal;
            while(HE1_prev->next != HE1){
                HE1_prev = HE1_prev->next;
            }
            Vertex *V4 = HE1_prev->vert;
            //create 2 halfedges
            HalfEdge *HE1B = new HalfEdge, *HE2B = new HalfEdge;
            HE1B->id = HalfEdges.size();
            HE2B->id = HalfEdges.size() + 1;
            HE1->sym = HE1B;
            HE1B->sym = HE1;
            HE2->sym = HE2B;
            HE2B->sym = HE2;
            HE1B->vert = V4;
            HE2B->vert = V1;
            //create 1 new face and 2 new halfedges HE3, HE4
            Face *new_F = new Face;
            HalfEdge *HE3 = new HalfEdge, *HE4 = new HalfEdge;
            new_F->id = Faces.size();
            HE3->id = HalfEdges.size()+2;
            HE4->id = HalfEdges.size()+3;
            HE1B->face = F;
            HE2B->face = F;
            HE3->face = F;
            HE4->face = F;
            HE3->vert = V3;
            HE4->vert = V2;
            HE1B->next = HE4;
            HE4->next = HE2B;
            HE2B->next = HE3;
            HE3->next = HE1B;
            //set the pointer of the V1,V2,V3
            V1->edge = HE2B;
            V2->edge = HE2;
            V3->edge = HE1;
            //set the start_edge of new_F and F
            new_F->color = glm::vec3(0.5,0,0.5);
            new_F->start_edge = HE1B;
            new_F->Face_Point = nullptr;
            //push back the new_F, HE1B,HE2B,HE3,HE4;
            Faces.push_back(new_F);
            HalfEdges.push_back(HE1B);
            HalfEdges.push_back(HE2B);
            HalfEdges.push_back(HE3);
            HalfEdges.push_back(HE4);
            Vertices.push_back(V3);
        }
        HE = HE->next;
    }while(HE!=F->start_edge);
    int num_new_edges = HalfEdges.size();
    //set sym
    for(int m = num_old_edges; m < num_new_edges; m++){
        for(int n = m+1; n < num_new_edges; n++){
            glm::vec3 start_pos1, start_pos2;
            HalfEdge *H1 = HalfEdges[m], *H2 = HalfEdges[n];
            while(H1->next!=HalfEdges[m]){
                H1 = H1->next;
            }
            start_pos1 = H1->vert->pos;
            while(H2->next!=HalfEdges[n]){
                H2 = H2->next;
            }
            start_pos2 = H2->vert->pos;
            if(start_pos1 == HalfEdges[n]->vert->pos && start_pos2 == HalfEdges[m]->vert->pos){
                HalfEdges[m]->sym = HalfEdges[n];
                HalfEdges[n]->sym = HalfEdges[m];
                break;
            }
        }
    }
}

bool Mesh::Planarity_Testing(Face *F){
    double epsilon_tolerance = glm::length(glm::vec3(0.01,0.01,0.01));
    HalfEdge *edge = F->start_edge;
    std::vector <glm::vec3> verts_normal;
            //push back a fake normal for the first vert
            verts_normal.push_back(glm::vec3(0,0,0));
            //calculate the next vert's normal:
            verts_normal.push_back(glm::vec3(glm::normalize(glm::cross((edge->next->vert->pos - edge->vert->pos),
                                          (edge->next->next->vert->pos - edge->next->vert->pos)))));
            while(edge->next != F->start_edge){
                edge = edge->next;
                //calculate the next vert's normal:
                if(edge->next != F->start_edge){
                    verts_normal.push_back(glm::vec3(glm::normalize(glm::cross((edge->next->vert->pos - edge->vert->pos),
                                                                           (edge->next->next->vert->pos - edge->next->vert->pos)))));

                }
                else{
                    //update the first vert's normal:
                    verts_normal[0] = glm::vec3(glm::normalize(glm::cross((edge->next->vert->pos - edge->vert->pos),
                                                                        (edge->next->next->vert->pos - edge->next->vert->pos))));
                }
            }
            for(int i = 0; i < verts_normal.size(); i++){
                for(int j = i+1; j < verts_normal.size(); j++){
                    if(glm::length(verts_normal[i] - verts_normal[j]) > epsilon_tolerance)
                        return false;
                }
            }
            return true;
}

//Sharp
void Mesh::setSharp_Vertex(Vertex *V){
    V->sharpness += 3;
}

void Mesh::setSharp_Edge(HalfEdge *HE){
    if(!HE->sharp){
        HE->sharp = true;
        HE->vert->sharpness++;
    }
    if(!HE->sym->sharp){
        HE->sym->sharp = true;
        HE->sym->vert->sharpness++;
    }
}

void Mesh::setSharp_Face(Face *F){
    F->sharp = true;
    HalfEdge *HE = F->start_edge;
    do{
        setSharp_Edge(HE);
        HE = HE->next;
    }while(HE != F->start_edge);
}

void Mesh::BevelingEdge(HalfEdge *HE){
    Face *F = HE->face;
    glm::vec3 normal = (glm::cross(HE->next->vert->pos - HE->vert->pos,
                                                 HE->next->next->vert->pos - HE->next->vert->pos));
    //Step 1: reposition back
    HalfEdge *s_HE = HE;
    do{
        HE->vert->pos = HE->vert->pos - 0.2f * normal;
        HE = HE->next;
    }while(HE != s_HE);
    //Step 2: calculate the change of the HE's endpoints
    HalfEdge *Dprev_HE = HE;
    while(Dprev_HE->next->next != HE)
        Dprev_HE = Dprev_HE->next;
    glm::vec3 change1, change2;
    change1 = 0.2f * (HE->next->vert->pos - HE->vert->pos);
    change2 = 0.2f * (Dprev_HE->vert->pos - Dprev_HE->next->vert->pos);
    //Step 3: extrude
    int num_old_edges = HalfEdges.size();
    //extrude edge by edge
    do{
        //if HE is the first edge to extrude
        if(HE == F->start_edge){
            //create 2 new Vertices
            Vertex *V3 = new Vertex, *V4 = new Vertex;
            V3->id = Vertices.size();
            V4->id = Vertices.size()+1;
            HalfEdge *HE1 = HE;
            HalfEdge *HE2 = HE->sym;
            Vertex *V1 = HE1->vert;
            HE1->vert = V3;
            HalfEdge *HE1_prev = HE1;
            Vertex *V2 = HE2->vert;
            V3->pos = V1->pos + 0.2f * normal;
            V4->pos = V2->pos + 0.2f * normal;
            while(HE1_prev->next != HE1){
                HE1_prev = HE1_prev->next;
            }
            HE1_prev->vert = V4;
            //create 2 new halfedges
            HalfEdge *HE1B = new HalfEdge, *HE2B = new HalfEdge;
            HE1->sym = HE1B;
            HE2->sym = HE2B;
            HE1B->sym = HE1;
            HE2B->sym = HE2;
            HE1B->vert = V4;
            HE2B->vert = V1;
            HE1B->id = HalfEdges.size();
            HE2B->id = HalfEdges.size()+1;
            //create 1 new face and other 2 new halfedges
            Face *new_F = new Face;
            HalfEdge *HE3 = new HalfEdge, *HE4 = new HalfEdge;
            F->id = Faces.size();
            HE3->id = HalfEdges.size()+2;
            HE4->id = HalfEdges.size()+3;
            //set Face pointer of the HE1B,2B,3,4
            HE1B->face = new_F;
            HE2B->face = new_F;
            HE3->face = new_F;
            HE4->face = new_F;
            HE3->vert = V3;
            HE4->vert = V2;
            HE1B->next = HE4;
            HE4->next = HE2B;
            HE2B->next = HE3;
            HE3->next = HE1B;
            //set the pointer of the V1,V2,V3,V4
            V1->edge = HE2B;
            V2->edge = HE2;
            V3->edge = HE1;
            V4->edge = HE1B;
            //set the start_edge of new_F and F
            new_F->color = F->color;
            new_F->start_edge = HE1B;
            new_F->Face_Point = nullptr;
            //push back the new_F, HE1B,HE2B,HE3,HE4,V3,V4;
            Faces.push_back(new_F);
            HalfEdges.push_back(HE1B);
            HalfEdges.push_back(HE2B);
            HalfEdges.push_back(HE3);
            HalfEdges.push_back(HE4);
            Vertices.push_back(V3);
            Vertices.push_back(V4);
            if(V3 == s_HE->vert){
                V3->pos = V3->pos + change1;
                change1 = glm::vec3(0.0f,0.0f,0.0f);
            }
            if(V4 == s_HE->vert){
                V4->pos = V4->pos + change1;
                change1 = glm::vec3(0.0f,0.0f,0.0f);
            }
            if(V3 == Dprev_HE->next->vert){
                V3->pos = V3->pos + change2;
                change2 = glm::vec3(0.0f,0.0f,0.0f);
            }
            if(V4 == Dprev_HE->next->vert){
                V4->pos = V4->pos + change2;
                change2 = glm::vec3(0.0f,0.0f,0.0f);
            }

        }
        //if HE is the last edge to extrude, we no longer need to
        else if(HE->next == F->start_edge){
            //initialize
            HalfEdge *HE1 = HE;
            HalfEdge *HE2 = HE->sym;
            while(HE2->next != HE->sym)
                HE2 = HE2->next;
            Vertex *V1 = HE2->vert;
            HE2 = HE2->next;
            Vertex *V2 = HE2->vert;
            Vertex *V3 = HE1->next->sym->vert;
            HalfEdge *HE1_prev = HE1;
            while(HE1_prev->next != HE1){
                HE1_prev = HE1_prev->next;
            }
            Vertex *V4 = HE1_prev->vert;
            HE1->vert = V3;
            //create 2 halfedges
            HalfEdge *HE1B = new HalfEdge, *HE2B = new HalfEdge;
            HE1B->id = HalfEdges.size();
            HE2B->id = HalfEdges.size() + 1;
            HE1->sym = HE1B;
            HE1B->sym = HE1;
            HE2->sym = HE2B;
            HE2B->sym = HE2;
            HE1B->vert = V4;
            HE2B->vert = V1;
            //create 1 new face and 2 new halfedges HE3, HE4
            Face *new_F = new Face;
            HalfEdge *HE3 = new HalfEdge, *HE4 = new HalfEdge;
            new_F->id = Faces.size();
            HE3->id = HalfEdges.size()+2;
            HE4->id = HalfEdges.size()+3;
            HE1B->face = F;
            HE2B->face = F;
            HE3->face = F;
            HE4->face = F;
            HE3->vert = V3;
            HE4->vert = V2;
            HE1B->next = HE4;
            HE4->next = HE2B;
            HE2B->next = HE3;
            HE3->next = HE1B;
            //set the pointer of the V1,V2
            V1->edge = HE2B;
            V2->edge = HE2;
            //set the start_edge of new_F and F
            new_F->color = F->color;
            new_F->start_edge = HE1B;
            new_F->Face_Point = nullptr;
            //push back the new_F, HE1B,HE2B,HE3,HE4;
            Faces.push_back(new_F);
            HalfEdges.push_back(HE1B);
            HalfEdges.push_back(HE2B);
            HalfEdges.push_back(HE3);
            HalfEdges.push_back(HE4);
            if(V3 == s_HE->vert){
                V3->pos = V3->pos + change1;
                change1 = glm::vec3(0.0f,0.0f,0.0f);
            }
            if(V4 == s_HE->vert){
                V4->pos = V4->pos + change1;
                change1 = glm::vec3(0.0f,0.0f,0.0f);
            }
            if(V3 == Dprev_HE->next->vert){
                V3->pos = V3->pos + change2;
                change2 = glm::vec3(0.0f,0.0f,0.0f);
            }
            if(V4 == Dprev_HE->next->vert){
                V4->pos = V4->pos + change2;
                change2 = glm::vec3(0.0f,0.0f,0.0f);
            }
        }
        //the middle edges, which means V4 has already been created
        else{
            //create new Vertex V3
            Vertex *V3 = new Vertex;
            V3->id = Vertices.size();
            HalfEdge *HE1 = HE;
            HalfEdge *HE2 = HE->sym;
            while(HE2->next != HE->sym)
                HE2 = HE2->next;
            Vertex *V1 = HE2->vert;
            HE2 = HE2->next;
            Vertex *V2 = HE2->vert;
            HE1->vert = V3;
            HalfEdge *HE1_prev = HE1;
            V3->pos = V1->pos + 0.2f * normal;
            while(HE1_prev->next != HE1){
                HE1_prev = HE1_prev->next;
            }
            Vertex *V4 = HE1_prev->vert;
            //create 2 halfedges
            HalfEdge *HE1B = new HalfEdge, *HE2B = new HalfEdge;
            HE1B->id = HalfEdges.size();
            HE2B->id = HalfEdges.size() + 1;
            HE1->sym = HE1B;
            HE1B->sym = HE1;
            HE2->sym = HE2B;
            HE2B->sym = HE2;
            HE1B->vert = V4;
            HE2B->vert = V1;
            //create 1 new face and 2 new halfedges HE3, HE4
            Face *new_F = new Face;
            HalfEdge *HE3 = new HalfEdge, *HE4 = new HalfEdge;
            new_F->id = Faces.size();
            HE3->id = HalfEdges.size()+2;
            HE4->id = HalfEdges.size()+3;
            HE1B->face = F;
            HE2B->face = F;
            HE3->face = F;
            HE4->face = F;
            HE3->vert = V3;
            HE4->vert = V2;
            HE1B->next = HE4;
            HE4->next = HE2B;
            HE2B->next = HE3;
            HE3->next = HE1B;
            //set the pointer of the V1,V2,V3
            V1->edge = HE2B;
            V2->edge = HE2;
            V3->edge = HE1;
            //set the start_edge of new_F and F
            new_F->color = F->color;
            new_F->start_edge = HE1B;
            new_F->Face_Point = nullptr;
            //push back the new_F, HE1B,HE2B,HE3,HE4;
            Faces.push_back(new_F);
            HalfEdges.push_back(HE1B);
            HalfEdges.push_back(HE2B);
            HalfEdges.push_back(HE3);
            HalfEdges.push_back(HE4);
            Vertices.push_back(V3);
            if(V3 == s_HE->vert){
                V3->pos = V3->pos + change1;
                change1 = glm::vec3(0.0f,0.0f,0.0f);
            }
            if(V4 == s_HE->vert){
                V4->pos = V4->pos + change1;
                change1 = glm::vec3(0.0f,0.0f,0.0f);
            }
            if(V3 == Dprev_HE->next->vert){
                V3->pos = V3->pos + change2;
                change2 = glm::vec3(0.0f,0.0f,0.0f);
            }
            if(V4 == Dprev_HE->next->vert){
                V4->pos = V4->pos + change2;
                change2 = glm::vec3(0.0f,0.0f,0.0f);
            }
        }
        HE = HE->next;
    }while(HE!=F->start_edge);
    int num_new_edges = HalfEdges.size();
    //set sym
    for(int m = num_old_edges; m < num_new_edges; m++){
        for(int n = m+1; n < num_new_edges; n++){
            glm::vec3 start_pos1, start_pos2;
            HalfEdge *H1 = HalfEdges[m], *H2 = HalfEdges[n];
            while(H1->next!=HalfEdges[m]){
                H1 = H1->next;
            }
            start_pos1 = H1->vert->pos;
            while(H2->next!=HalfEdges[n]){
                H2 = H2->next;
            }
            start_pos2 = H2->vert->pos;
            if(start_pos1 == HalfEdges[n]->vert->pos && start_pos2 == HalfEdges[m]->vert->pos){
                HalfEdges[m]->sym = HalfEdges[n];
                HalfEdges[n]->sym = HalfEdges[m];
                break;
            }
        }
    }
}


void Mesh::BevelingFace(Face *F){
    HalfEdge *HE = F->start_edge;
    glm::vec3 normal = (glm::cross(HE->next->vert->pos - HE->vert->pos,
                                                 HE->next->next->vert->pos - HE->next->vert->pos));
    //Step 1: reposition back
    int length = 0;
    HalfEdge *s_HE = HE;
    do{
        HE->vert->pos = HE->vert->pos - 0.2f * normal;
        HE = HE->next;
        length++;
    }while(HE != s_HE);
    //Step 2: calculate the change of the HE's endpoints
    std::vector <glm::vec3> change;
    do{
        HalfEdge *prev_HE = HE;
        while(prev_HE->next != HE)
            prev_HE = prev_HE->next;
        glm::vec3 change1;
        change1 = 0.2f * ((HE->next->vert->pos - HE->vert->pos) + (prev_HE->vert->pos - HE->vert->pos));
        change.push_back(change1);
        HE = HE->next;
    }while(HE!=F->start_edge);
    //Step 3: extrude
    int num_old_edges = HalfEdges.size();
    //extrude edge by edge
    do{
        //if HE is the first edge to extrude
        if(HE == F->start_edge){
            //create 2 new Vertices
            Vertex *V3 = new Vertex, *V4 = new Vertex;
            V3->id = Vertices.size();
            V4->id = Vertices.size()+1;
            HalfEdge *HE1 = HE;
            HalfEdge *HE2 = HE->sym;
            Vertex *V1 = HE1->vert;
            HE1->vert = V3;
            HalfEdge *HE1_prev = HE1;
            Vertex *V2 = HE2->vert;
            V3->pos = V1->pos + 0.2f * normal;
            V4->pos = V2->pos + 0.2f * normal;
            while(HE1_prev->next != HE1){
                HE1_prev = HE1_prev->next;
            }
            HE1_prev->vert = V4;
            //create 2 new halfedges
            HalfEdge *HE1B = new HalfEdge, *HE2B = new HalfEdge;
            HE1->sym = HE1B;
            HE2->sym = HE2B;
            HE1B->sym = HE1;
            HE2B->sym = HE2;
            HE1B->vert = V4;
            HE2B->vert = V1;
            HE1B->id = HalfEdges.size();
            HE2B->id = HalfEdges.size()+1;
            //create 1 new face and other 2 new halfedges
            Face *new_F = new Face;
            HalfEdge *HE3 = new HalfEdge, *HE4 = new HalfEdge;
            F->id = Faces.size();
            HE3->id = HalfEdges.size()+2;
            HE4->id = HalfEdges.size()+3;
            //set Face pointer of the HE1B,2B,3,4
            HE1B->face = new_F;
            HE2B->face = new_F;
            HE3->face = new_F;
            HE4->face = new_F;
            HE3->vert = V3;
            HE4->vert = V2;
            HE1B->next = HE4;
            HE4->next = HE2B;
            HE2B->next = HE3;
            HE3->next = HE1B;
            //set the pointer of the V1,V2,V3,V4
            V1->edge = HE2B;
            V2->edge = HE2;
            V3->edge = HE1;
            V4->edge = HE1B;
            //set the start_edge of new_F and F
            new_F->color = F->color;
            new_F->start_edge = HE1B;
            new_F->Face_Point = nullptr;
            //push back the new_F, HE1B,HE2B,HE3,HE4,V3,V4;
            Faces.push_back(new_F);
            HalfEdges.push_back(HE1B);
            HalfEdges.push_back(HE2B);
            HalfEdges.push_back(HE3);
            HalfEdges.push_back(HE4);
            Vertices.push_back(V3);
            Vertices.push_back(V4);

        }
        //if HE is the last edge to extrude, we no longer need to
        else if(HE->next == F->start_edge){
            //initialize
            HalfEdge *HE1 = HE;
            HalfEdge *HE2 = HE->sym;
            while(HE2->next != HE->sym)
                HE2 = HE2->next;
            Vertex *V1 = HE2->vert;
            HE2 = HE2->next;
            Vertex *V2 = HE2->vert;
            Vertex *V3 = HE1->next->sym->vert;
            HalfEdge *HE1_prev = HE1;
            while(HE1_prev->next != HE1){
                HE1_prev = HE1_prev->next;
            }
            Vertex *V4 = HE1_prev->vert;
            HE1->vert = V3;
            //create 2 halfedges
            HalfEdge *HE1B = new HalfEdge, *HE2B = new HalfEdge;
            HE1B->id = HalfEdges.size();
            HE2B->id = HalfEdges.size() + 1;
            HE1->sym = HE1B;
            HE1B->sym = HE1;
            HE2->sym = HE2B;
            HE2B->sym = HE2;
            HE1B->vert = V4;
            HE2B->vert = V1;
            //create 1 new face and 2 new halfedges HE3, HE4
            Face *new_F = new Face;
            HalfEdge *HE3 = new HalfEdge, *HE4 = new HalfEdge;
            new_F->id = Faces.size();
            HE3->id = HalfEdges.size()+2;
            HE4->id = HalfEdges.size()+3;
            HE1B->face = F;
            HE2B->face = F;
            HE3->face = F;
            HE4->face = F;
            HE3->vert = V3;
            HE4->vert = V2;
            HE1B->next = HE4;
            HE4->next = HE2B;
            HE2B->next = HE3;
            HE3->next = HE1B;
            //set the pointer of the V1,V2
            V1->edge = HE2B;
            V2->edge = HE2;
            //set the start_edge of new_F and F
            new_F->color = F->color;
            new_F->start_edge = HE1B;
            new_F->Face_Point = nullptr;
            //push back the new_F, HE1B,HE2B,HE3,HE4;
            Faces.push_back(new_F);
            HalfEdges.push_back(HE1B);
            HalfEdges.push_back(HE2B);
            HalfEdges.push_back(HE3);
            HalfEdges.push_back(HE4);
        }
        //the middle edges, which means V4 has already been created
        else{
            //create new Vertex V3
            Vertex *V3 = new Vertex;
            V3->id = Vertices.size();
            HalfEdge *HE1 = HE;
            HalfEdge *HE2 = HE->sym;
            while(HE2->next != HE->sym)
                HE2 = HE2->next;
            Vertex *V1 = HE2->vert;
            HE2 = HE2->next;
            Vertex *V2 = HE2->vert;
            HE1->vert = V3;
            HalfEdge *HE1_prev = HE1;
            V3->pos = V1->pos + 0.2f * normal;
            while(HE1_prev->next != HE1){
                HE1_prev = HE1_prev->next;
            }
            Vertex *V4 = HE1_prev->vert;
            //create 2 halfedges
            HalfEdge *HE1B = new HalfEdge, *HE2B = new HalfEdge;
            HE1B->id = HalfEdges.size();
            HE2B->id = HalfEdges.size() + 1;
            HE1->sym = HE1B;
            HE1B->sym = HE1;
            HE2->sym = HE2B;
            HE2B->sym = HE2;
            HE1B->vert = V4;
            HE2B->vert = V1;
            //create 1 new face and 2 new halfedges HE3, HE4
            Face *new_F = new Face;
            HalfEdge *HE3 = new HalfEdge, *HE4 = new HalfEdge;
            new_F->id = Faces.size();
            HE3->id = HalfEdges.size()+2;
            HE4->id = HalfEdges.size()+3;
            HE1B->face = F;
            HE2B->face = F;
            HE3->face = F;
            HE4->face = F;
            HE3->vert = V3;
            HE4->vert = V2;
            HE1B->next = HE4;
            HE4->next = HE2B;
            HE2B->next = HE3;
            HE3->next = HE1B;
            //set the pointer of the V1,V2,V3
            V1->edge = HE2B;
            V2->edge = HE2;
            V3->edge = HE1;
            //set the start_edge of new_F and F
            new_F->color = F->color;
            new_F->start_edge = HE1B;
            new_F->Face_Point = nullptr;
            //push back the new_F, HE1B,HE2B,HE3,HE4;
            Faces.push_back(new_F);
            HalfEdges.push_back(HE1B);
            HalfEdges.push_back(HE2B);
            HalfEdges.push_back(HE3);
            HalfEdges.push_back(HE4);
            Vertices.push_back(V3);
        }
        HE = HE->next;
    }while(HE!=F->start_edge);
    int cnt = 0;
    do{
        HE->vert->pos = HE->vert->pos + change[cnt];
        cnt++;
        HE = HE->next;
    }while(HE!=F->start_edge);
    int num_new_edges = HalfEdges.size();
    //set sym
    for(int m = num_old_edges; m < num_new_edges; m++){
        for(int n = m+1; n < num_new_edges; n++){
            glm::vec3 start_pos1, start_pos2;
            HalfEdge *H1 = HalfEdges[m], *H2 = HalfEdges[n];
            while(H1->next!=HalfEdges[m]){
                H1 = H1->next;
            }
            start_pos1 = H1->vert->pos;
            while(H2->next!=HalfEdges[n]){
                H2 = H2->next;
            }
            start_pos2 = H2->vert->pos;
            if(start_pos1 == HalfEdges[n]->vert->pos && start_pos2 == HalfEdges[m]->vert->pos){
                HalfEdges[m]->sym = HalfEdges[n];
                HalfEdges[n]->sym = HalfEdges[m];
                break;
            }
        }
    }

}
