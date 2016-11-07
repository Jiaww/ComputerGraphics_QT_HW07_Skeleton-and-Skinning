#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include <QListWidget>
#include <QFile>
#include <QFileDialog>
#include <QDir>


MyGL::MyGL(QWidget *parent)
    : GLWidget277(parent),
      geom_cylinder(this), geom_sphere(this),geom_mesh(this),
      HighlightVertex(this),HighlightHalfLine(this),HighlightHFace(this),
      prog_lambert(this), prog_flat(this),timeCount(0),RenderMode(0),Func1(0),
      gl_camera()
{
    // Connect the timer to a function so that when the timer ticks the function is executed
    connect(&timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    // Tell the timer to redraw 60 times per second
    timer.start(16);
    this->setFocusPolicy(Qt::ClickFocus);
    timeCount = 0;
    RenderMode = 0;
    Func1 = 0;
}

MyGL::~MyGL()
{
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
    geom_cylinder.destroy();
    geom_sphere.destroy();
    geom_mesh.destroy();
    HighlightVertex.destroy();
    HighlightHalfLine.destroy();
    HighlightHFace.destroy();
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.5, 0.5, 0.5, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the instances of Cylinder and Sphere.
    geom_cylinder.create();

    geom_sphere.create();

    //Create the cube in Mesh:
    geom_mesh.LoadCube();
//    int num_original_vertices = geom_mesh.ADDandMOVE();
//    geom_mesh.Quadrangualte(num_original_vertices);
//    geom_mesh.ExtrudeFace(geom_mesh.Faces[0]);
//    geom_mesh.Quadrangualte();
    //geom_mesh.insertVertex(geom_mesh.HalfEdges[0]);
    //geom_mesh.Triangular();
    //geom_mesh.RepositionVert(0, glm::vec4(-1,-1,1,1));
    //geom_mesh.BevelingEdge(geom_mesh.HalfEdges[0]);
    //geom_mesh.BevelingFace(geom_mesh.Faces[1]);
    geom_mesh.create();
    HighlightVertex.setVert(geom_mesh.Vertices[3]);
    HighlightVertex.create();
    HighlightHalfLine.setLine(geom_mesh.HalfEdges[0]);
    HighlightHalfLine.create();
    HighlightHFace.setFace(geom_mesh.Faces[1]);
    HighlightHFace.create();


    //emit signals
    emit sig_PassVertices(geom_mesh.Vertices.size());
    emit sig_PassHalfEdges(geom_mesh.HalfEdges.size());
    emit sig_PassFaces(geom_mesh.Faces.size());

//    int test = geom_mesh.Test();

    // Create and set up the diffuse shader
    prog_lambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    prog_flat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");

    // Set a color with which to draw geometry since you won't have one
    // defined until you implement the Node classes.
    // This makes your geometry render green.
    prog_lambert.setGeometryColor(glm::vec4(0,1,0,1));

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
//    vao.bind();
    glBindVertexArray(vao);


}

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    gl_camera = Camera(w, h);
    glm::mat4 viewproj = gl_camera.getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    prog_lambert.setViewProjMatrix(viewproj);
    prog_flat.setViewProjMatrix(viewproj);

    printGLErrorLog();
}

//This function is called by Qt any time your GL window is supposed to update
//For example, when the function updateGL is called, paintGL is called implicitly.
//DO NOT CONSTRUCT YOUR SCENE GRAPH IN THIS FUNCTION!
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    prog_flat.setViewProjMatrix(gl_camera.getViewProj());
    prog_lambert.setViewProjMatrix(gl_camera.getViewProj());

#define NOPE
#ifdef NOPE
    //Create a model matrix. This one scales the sphere uniformly by 3, then translates it by <-2,0,0>.
    //Note that we have to transpose the model matrix before passing it to the shader
    //This is because OpenGL expects column-major matrices, but you've
    //implemented row-major matrices.
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-2,0,0)) * glm::scale(glm::mat4(1.0f), glm::vec3(3,3,3));
    //Send the geometry's transformation matrix to the shader
    prog_lambert.setModelMatrix(model);
    //Draw the example sphere using our lambert shader
    //prog_lambert.draw(geom_sphere);

    //Draw mesh
    prog_lambert.draw(geom_mesh);
    prog_lambert.draw(HighlightVertex);
    prog_lambert.draw(HighlightHalfLine);
    prog_lambert.draw(HighlightHFace);

    //Now do the same to render the cylinder
    //We've rotated it -45 degrees on the Z axis, then translated it to the point <2,2,0>
    model = glm::translate(glm::mat4(1.0f), glm::vec3(2,2,0)) * glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0,0,1));
    prog_lambert.setModelMatrix(model);
    //prog_lambert.draw(geom_cylinder);


#endif
}


void MyGL::keyPressEvent(QKeyEvent *e)
{

    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used, but I really dislike their
    // syntax so I chose to be lazy and use a long
    // chain of if statements instead
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        gl_camera.RotateAboutUp(-amount);
    } else if (e->key() == Qt::Key_Left) {
        gl_camera.RotateAboutUp(amount);
    } else if (e->key() == Qt::Key_Up) {
        gl_camera.RotateAboutRight(-amount);
    } else if (e->key() == Qt::Key_Down) {
        gl_camera.RotateAboutRight(amount);
    } else if (e->key() == Qt::Key_1) {
        gl_camera.fovy += amount;
    } else if (e->key() == Qt::Key_2) {
        gl_camera.fovy -= amount;
    } else if (e->key() == Qt::Key_W) {
        gl_camera.TranslateAlongLook(amount);
    } else if (e->key() == Qt::Key_S) {
        gl_camera.TranslateAlongLook(-amount);
    } else if (e->key() == Qt::Key_D) {
        gl_camera.TranslateAlongRight(amount);
    } else if (e->key() == Qt::Key_A) {
        gl_camera.TranslateAlongRight(-amount);
    } else if (e->key() == Qt::Key_Q) {
        gl_camera.TranslateAlongUp(-amount);
    } else if (e->key() == Qt::Key_E) {
        gl_camera.TranslateAlongUp(amount);
    } else if (e->key() == Qt::Key_R) {
        gl_camera = Camera(this->width(), this->height());
        //  swap the RenderMode
        if(RenderMode == 0){
            RenderMode = 1;
        } else {
            RenderMode = 0;
        }
        prog_lambert.setRenderMode(RenderMode);
    } else if (e->key() == Qt::Key_N) {
        //next
        HighlightHalfLine.HE = HighlightHalfLine.HE->next;
        HighlightHalfLine.create();
        emit sig_ChangeSelectedHalfEdge(HighlightHalfLine.HE->id);
    } else if (e->key() == Qt::Key_M) {
        //sym
        HighlightHalfLine.HE = HighlightHalfLine.HE->sym;
        HighlightHalfLine.create();
        emit sig_ChangeSelectedHalfEdge(HighlightHalfLine.HE->id);
    } else if (e->key() == Qt::Key_F) {
        // FACE of the currently selected half-edge
        HighlightHFace.F = HighlightHalfLine.HE->face;
        HighlightHFace.create();
        emit sig_ChangeSelectedFace(HighlightHFace.F->id);
    } else if (e->key() == Qt::Key_V) {
        // VERTEX of the currently selected half-edge
        HighlightVertex.V = HighlightHalfLine.HE->vert;
        HighlightVertex.create();
        int id_num = HighlightVertex.V->id;
        if(id_num > 7){
            if(id_num > 15)
                id_num = (id_num + 1) / 3;
            else
                id_num = (id_num + 1) / 2;
        }
        emit sig_ChangeSelectedVertex(id_num);
    } else if (e->key() == Qt::Key_H) {
        //HALF-EDGE of the currently selected vertex
        HighlightHalfLine.HE = HighlightVertex.V->edge;
        HighlightHalfLine.create();
        emit sig_ChangeSelectedHalfEdge(HighlightHalfLine.HE->id);
    } else if (e->key() == Qt::Key_Shift && Qt::Key_H) {
        //HALF-EDGE of the currently selected face
        HighlightHalfLine.HE = HighlightHFace.F->start_edge;
        HighlightHalfLine.create();
        emit sig_ChangeSelectedHalfEdge(HighlightHalfLine.HE->id);
    } else if (e->key() == Qt::Key_X){
        //Func1
        //  swap the Func1
        if(Func1 != 3){
            Func1++;
        } else {
            Func1 = 0;
        }
        prog_lambert.setFunc1(Func1);
    } else if (e->key() == Qt::Key_T){
        int num_original_Vertices = geom_mesh.ADDandMOVE();
        geom_mesh.Quadrangualte(num_original_Vertices);
        geom_mesh.create();
        HighlightHalfLine.create();
        HighlightHFace.create();
        HighlightVertex.create();
        //emit signals
        emit sig_PassVertices(geom_mesh.Vertices.size());
        emit sig_PassHalfEdges(geom_mesh.HalfEdges.size());
        emit sig_PassFaces(geom_mesh.Faces.size());

    } else if (e->key() == Qt::Key_P){
        geom_mesh.Triangular(HighlightHFace.F);
        geom_mesh.create();
        HighlightHalfLine.create();
        HighlightHFace.create();
        HighlightVertex.create();
        //emit signals
        emit sig_PassVertices(geom_mesh.Vertices.size());
        emit sig_PassHalfEdges(geom_mesh.HalfEdges.size());
        emit sig_PassFaces(geom_mesh.Faces.size());
    }
    gl_camera.RecomputeAttributes();
    update();  // Calls paintGL, among other things
}

void MyGL::timerUpdate()
{
    // This function is called roughly 60 times per second.
    // Use it to perform any repeated actions you want to do,
    // such as
    timeCount++;
    prog_lambert.setUnifTime(timeCount);
    if(RenderMode == 1 || Func1 != 0)
        this->update();
}

void MyGL::slot_VertexClicked(QListWidgetItem *p){
    QString name = p->text();
    int num = name.toInt();
    Vertex *V = geom_mesh.Vertices[num];
    HighlightVertex.setVert(V);
    HighlightVertex.create();
    this->update();
}

void MyGL::slot_HalfEdgeClicked(QListWidgetItem *p){
    QString name = p->text();
    int num = name.toInt();
    HalfEdge *HE = geom_mesh.HalfEdges[num];
    HighlightHalfLine.setLine(HE);
    HighlightHalfLine.create();
    this->update();
}

void MyGL::slot_FaceClicked(QListWidgetItem *p){
    QString name = p->text();
    int num = name.toInt();
    Face *F = geom_mesh.Faces[num];
    HighlightHFace.setFace(F);
    HighlightHFace.create();
    this->update();
}

void MyGL::slot_PosXChanged(double x){
    glm::vec4 new_pos(HighlightVertex.V->pos, 1);
    new_pos[0] += x;
    geom_mesh.RepositionVert(HighlightVertex.V, new_pos);
    HighlightVertex.create();
    HighlightHalfLine.create();
    HighlightHFace.create();
    geom_mesh.create();
    this->update();
}

void MyGL::slot_PosYChanged(double y){
    glm::vec4 new_pos(HighlightVertex.V->pos, 1);
    new_pos[1] += y;
    geom_mesh.RepositionVert(HighlightVertex.V, new_pos);
    HighlightVertex.create();
    HighlightHalfLine.create();
    HighlightHFace.create();
    geom_mesh.create();
    this->update();
}

void MyGL::slot_PosZChanged(double z){
    glm::vec4 new_pos(HighlightVertex.V->pos, 1);
    new_pos[2] += z;
    geom_mesh.RepositionVert(HighlightVertex.V, new_pos);
    HighlightVertex.create();
    HighlightHalfLine.create();
    HighlightHFace.create();
    geom_mesh.create();
    this->update();
}

void MyGL::slot_ColXChanged(double x){
    HighlightHFace.F->color[0] += x;
    HalfEdge *HE = HighlightHFace.F->start_edge;
    do{
        HE = HE->next;
    }while(HE != HighlightHFace.F->start_edge);
    HighlightVertex.create();
    HighlightHalfLine.create();
    HighlightHFace.create();
    geom_mesh.create();
    this->update();
}

void MyGL::slot_ColYChanged(double y){
    HighlightHFace.F->color[1] += y;
    HalfEdge *HE = HighlightHFace.F->start_edge;
    do{
        HE = HE->next;
    }while(HE != HighlightHFace.F->start_edge);
    HighlightVertex.create();
    HighlightHalfLine.create();
    HighlightHFace.create();
    geom_mesh.create();
    this->update();
}

void MyGL::slot_ColZChanged(double z){
    HighlightHFace.F->color[2] += z;
    HalfEdge *HE = HighlightHFace.F->start_edge;
    do{
        HE = HE->next;
    }while(HE != HighlightHFace.F->start_edge);
    HighlightVertex.create();
    HighlightHalfLine.create();
    HighlightHFace.create();
    geom_mesh.create();
    this->update();
}

void MyGL::slot_LoadOBJFile(){
    std::vector<glm::vec4> vert_pos;
    std::vector<glm::vec4> vert_uv;
    std::vector<glm::vec4> vert_nor;
    std::vector<QStringList> faces_con;

    QString filename = QFileDialog::getOpenFileName(0, QString("Load OBJ File"), QDir::currentPath().append(QString("../..")), QString("*.obj"));
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)){
        qWarning("Could not open the OBJ file.");
        return;
    }
    while(!file.atEnd()){
        QByteArray line = file.readLine();
        QString str(line);
        QStringList strlist = str.split(" ");
        if(strlist[0] == "v"){
            vert_pos.push_back(glm::vec4(strlist[1].toDouble(), strlist[2].toDouble(), strlist[3].toDouble(), 1));
        }
        else if(strlist[0] == "vt"){
            vert_uv.push_back(glm::vec4(strlist[1].toDouble(), strlist[2].toDouble(), 0, 1));
        }
        else if(strlist[0] == "vn"){
            vert_nor.push_back(glm::vec4(strlist[1].toDouble(), strlist[2].toDouble(), strlist[3].toDouble(), 1));
        }
        else if(strlist[0] == "f"){
            //build the face:
            faces_con.push_back(strlist);
        }
    }

    geom_mesh.setFromFile(vert_pos,vert_uv,vert_nor,faces_con);
    //set sym
    for(int i = 0; i < geom_mesh.HalfEdges.size(); i++){
        for(int j = i+1; j < geom_mesh.HalfEdges.size(); j++){
            glm::vec3 start_pos1, start_pos2;
            HalfEdge *H1 = geom_mesh.HalfEdges[i], *H2 = geom_mesh.HalfEdges[j];
            while(H1->next!=geom_mesh.HalfEdges[i]){
                H1 = H1->next;
            }
            start_pos1 = H1->vert->pos;
            while(H2->next!=geom_mesh.HalfEdges[j]){
                H2 = H2->next;
            }
            start_pos2 = H2->vert->pos;
            if(start_pos1 == geom_mesh.HalfEdges[j]->vert->pos && start_pos2 == geom_mesh.HalfEdges[i]->vert->pos){
                geom_mesh.HalfEdges[i]->sym = geom_mesh.HalfEdges[j];
                geom_mesh.HalfEdges[j]->sym = geom_mesh.HalfEdges[i];
                break;
            }
        }
    }
//    HighlightVertex.V = nullptr;
//    HighlightHFace.F = nullptr;
//    HighlightHalfLine.HE = nullptr;
    HighlightVertex.setVert(geom_mesh.Vertices[0]);
    HighlightHalfLine.setLine(geom_mesh.HalfEdges[0]);
    HighlightHFace.setFace(geom_mesh.Faces[0]);
    HighlightHalfLine.create();
    HighlightHFace.create();
    HighlightVertex.create();
    geom_mesh.create();
    //emit signals
    emit sig_PassVertices(geom_mesh.Vertices.size());
    emit sig_PassHalfEdges(geom_mesh.HalfEdges.size());
    emit sig_PassFaces(geom_mesh.Faces.size());

    this->update();
}

void MyGL::slot_ExportOBJFile(){
//    QFile data("../export.obj");
//    if (data.open(QFile::WriteOnly | QIODevice::Truncate)) {
//        QTextStream out(&data);
//        //out v_pos
//        for(int i = 0; i < geom_mesh.Vertices.size(); i++){
//            out<<"v ";
//            out<<geom_mesh.Vertices[i]->pos[0]<<" ";
//            out<<geom_mesh.Vertices[i]->pos[1]<<" ";
//            out<<geom_mesh.Vertices[i]->pos[2];
//            out<<"\n";
//        }
//        out<<"\n";
//        //out v_uv

//        //out v_nor
//        for(int i = 0; i < geom_mesh.Vertices.size(); i++){
//            out<<"vn ";
//            out<<geom_mesh.Vertices[i]->nor[0]<<" ";
//            out<<geom_mesh.Vertices[i]->nor[1]<<" ";
//            out<<geom_mesh.Vertices[i]->nor[2];
//            out<<"\n";
//        }
//        out<<"\n";
//        //out f
//        for(int i = 0; i < geom_mesh.Faces.size(); i++){
//            out<<"f";
//            HalfEdge *HE = geom_mesh.Faces[i]->start_edge;
//            do{
//                Vertex *V = HE->vert;
//                out<<" "<<(V->id+1)<<"/"<<"0"<<"/"<<(V->nor_id+1);
//                HE = HE->next;
//            }while(HE!=geom_mesh.Faces[i]->start_edge);
//            out<<"\n";
//        }
//    }
//    data.close();
}

void MyGL::slot_AddVertex(){
    geom_mesh.insertVertex(HighlightHalfLine.HE);
    geom_mesh.create();
    HighlightHalfLine.create();
    HighlightHFace.create();
    HighlightVertex.create();
    //emit signals
    emit sig_PassVertices(geom_mesh.Vertices.size());
    emit sig_PassHalfEdges(geom_mesh.HalfEdges.size());
    emit sig_PassFaces(geom_mesh.Faces.size());
    this->update();
}

void MyGL::slot_ExtrudeFace(){
    geom_mesh.ExtrudeFace(HighlightHFace.F);
    geom_mesh.create();
    HighlightHalfLine.create();
    HighlightHFace.create();
    HighlightVertex.create();
    //emit signals
    emit sig_PassVertices(geom_mesh.Vertices.size());
    emit sig_PassHalfEdges(geom_mesh.HalfEdges.size());
    emit sig_PassFaces(geom_mesh.Faces.size());
    this->update();
}

void MyGL::slot_Planarity(){
    if(!geom_mesh.Planarity_Testing(HighlightHFace.F)){
        geom_mesh.Triangular(HighlightHFace.F);
    }
    geom_mesh.create();
    HighlightHalfLine.create();
    HighlightHFace.create();
    HighlightVertex.create();
    //emit signals
    emit sig_PassVertices(geom_mesh.Vertices.size());
    emit sig_PassHalfEdges(geom_mesh.HalfEdges.size());
    emit sig_PassFaces(geom_mesh.Faces.size());
    this->update();
}

void MyGL::slot_SetSharpVert(){
    geom_mesh.setSharp_Vertex(HighlightVertex.V);
    geom_mesh.create();
    HighlightHalfLine.create();
    HighlightHFace.create();
    HighlightVertex.create();
    //emit signals
    emit sig_PassVertices(geom_mesh.Vertices.size());
    emit sig_PassHalfEdges(geom_mesh.HalfEdges.size());
    emit sig_PassFaces(geom_mesh.Faces.size());
    this->update();
}

void MyGL::slot_SetSharpEdge(){
    geom_mesh.setSharp_Edge(HighlightHalfLine.HE);
    geom_mesh.create();
    HighlightHalfLine.create();
    HighlightHFace.create();
    HighlightVertex.create();
    //emit signals
    emit sig_PassVertices(geom_mesh.Vertices.size());
    emit sig_PassHalfEdges(geom_mesh.HalfEdges.size());
    emit sig_PassFaces(geom_mesh.Faces.size());
    this->update();
}

void MyGL::slot_SetSharpFace(){
    geom_mesh.setSharp_Face(geom_mesh.Faces[1]);
    geom_mesh.setSharp_Face(geom_mesh.Faces[4]);
    geom_mesh.create();
    HighlightHalfLine.create();
    HighlightHFace.create();
    HighlightVertex.create();
    //emit signals
    emit sig_PassVertices(geom_mesh.Vertices.size());
    emit sig_PassHalfEdges(geom_mesh.HalfEdges.size());
    emit sig_PassFaces(geom_mesh.Faces.size());
    this->update();
}

void MyGL::slot_BevelingEdge(){
    geom_mesh.BevelingEdge(HighlightHalfLine.HE);
    geom_mesh.create();
    HighlightHalfLine.create();
    HighlightHFace.create();
    HighlightVertex.create();
    //emit signals
    emit sig_PassVertices(geom_mesh.Vertices.size());
    emit sig_PassHalfEdges(geom_mesh.HalfEdges.size());
    emit sig_PassFaces(geom_mesh.Faces.size());
    this->update();
}

void MyGL::slot_BevelingFace(){
    geom_mesh.BevelingFace(HighlightHFace.F);
    geom_mesh.create();
    HighlightHalfLine.create();
    HighlightHFace.create();
    HighlightVertex.create();
    //emit signals
    emit sig_PassVertices(geom_mesh.Vertices.size());
    emit sig_PassHalfEdges(geom_mesh.HalfEdges.size());
    emit sig_PassFaces(geom_mesh.Faces.size());
    this->update();
}
