#include "glwidget.h"
#include <QMouseEvent>
#include "mainwindow.h"
#include "wavefunction/wfndata.h"
#include <QVector3D>
#include <QQuaternion>


GLWidget::GLWidget(MainWindow* parent)
{
    lastPos = QPoint(0,0);
    papa = parent;
}

void GLWidget::initializeGL()
 {
     qglClearColor(QColor(255,255,255));

     glPointSize(10);
     glEnable(GL_POINT_SMOOTH);
 }

void GLWidget::resizeGL(int width, int height)
 {
     if(!papa) return;

     int side = qMin(width, height);
     glViewport((width - side) / 2, (height - side) / 2, side, side);

     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();

     QVector3D box = papa->doc->get_box();
     glOrtho(-box.x(), box.x(), -box.y(), box.y(), -box.z(), box.z());

     glMatrixMode(GL_MODELVIEW);
}

void GLWidget::paintGL()
 {
    if(!papa) return;
    int i = 0;
    QVector3D dv = papa->doc->get_dv();
    QVector3D vRot = papa->doc->get_vRot();    

    QList<QVector3D>  atoms = papa->doc->get_atoms();
    QList<QVector3D>  helpers = papa->doc->get_helpers();


     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     glLoadIdentity();

     glRotatef(vRot.x() / 16.0, 1.0, 0.0, 0.0);
     glRotatef(vRot.y() / 16.0, 0.0, 1.0, 0.0);
     glRotatef(vRot.z() / 16.0, 0.0, 0.0, 1.0);

     ////
     QQuaternion Rx = QQuaternion::fromAxisAndAngle(1,0,0,-vRot.x()/16.0);
     QQuaternion Ry = QQuaternion::fromAxisAndAngle(0,1,0,-vRot.y()/16.0);
     QQuaternion Rz = QQuaternion::fromAxisAndAngle(0,0,1,-vRot.z()/16.0);
     papa->doc->NCamera = Rz.rotatedVector(Ry.rotatedVector(Rx.rotatedVector(QVector3D(0,0,1))));
     ////

     glBegin(GL_POINTS);

     glColor3f (0.0, 0.0, 0.0); //atomic centers
     for(i = 0; i < atoms.count(); i++)
            glVertex3f(atoms.at(i).x()-dv.x(),
                       atoms.at(i).y()-dv.y(),
                       atoms.at(i).z()-dv.z());

     glColor3f (0.0, 1.0, 0.0); //helpers
     for(i = 0; i < helpers.count(); i++)
            glVertex3f(helpers.at(i).x()-dv.x(),
                       helpers.at(i).y()-dv.y(),
                       helpers.at(i).z()-dv.z());
     glEnd();

     renderSurface();
     renderBondPaths();
     renderBCPs();
     renderZfxGradLines();
     renderCGALSurfaces();
     renderCamera();
 }

void GLWidget::mousePressEvent(QMouseEvent *event)
 {
   if(!papa) return;

   if(papa->tb->act_rotate->isChecked())
       mousePress_Rotate(event);

   if(papa->tb->act_select->isChecked())
       mousePress_Select(event);
 }

void GLWidget::mouseMoveEvent(QMouseEvent *event)
 {
   if(!papa) return;

   if(papa->tb->act_rotate->isChecked())
       mouseMove_Rotate(event);
 }

void GLWidget::update()
{
    resizeGL(this->rect().width(),this->rect().height());
    updateGL();
}

void GLWidget::mousePress_Rotate(QMouseEvent* event)
{
    lastPos = event->pos();
}

void GLWidget::mousePress_Select(QMouseEvent* event)
{
    //TODO: all this shit is necessary to rewrite...
   if(!papa) return;

   QVector3D V = getSelection(event->x(),event->y());
   if(V == QVector3D(100000,100000,100000)) return;

   //Surface
   GeomObject * object = papa->doc->get_geomobject();
   if(object){
       QList<QVector3D>  new_selected = object->selected;

       for(int i = 0; i < new_selected.count(); i++)
           if(V == QVector3D(new_selected.at(i)))
           {
                new_selected.removeAt(i);
                object->selected = new_selected;
                papa->tb->updateState();
                updateGL();
                return;
           }

       new_selected.append(V);
       object->selected = new_selected;
   }

   //print a selected atom (in the case) to console ----
   QVector3D diff;
   for(int i = 0; i < papa->doc->get_atoms().size(); i++){
       diff = papa->doc->get_atoms().at(i) - V;
       if(diff.length() < 1e-5){
           //todo: just prints a selected atom, but never stores the result
           printf("Selected atom number: %d\n",i);
           break;
       }
   }
   //<<<----

   //BCPs
   for(int i = 0; i < papa->doc->get_BCPs()->size(); i++){
       diff = papa->doc->get_BCPs()->at(i)->ptBCP - V;
       if(diff.length() < 1e-5){
           papa->doc->get_BCPs()->at(i)->selected = !papa->doc->get_BCPs()->at(i)->selected;
       }
   }

   papa->tb->updateState();
   updateGL();
   return;
}

void GLWidget::mouseMove_Rotate(QMouseEvent* event)
{
    if(!papa) return;
    QVector3D vRot = papa->doc->get_vRot();

     int dx = event->x() - lastPos.x();
     int dy = event->y() - lastPos.y();

     if (event->buttons() & Qt::LeftButton) {
         vRot.setX(vRot.x()+ 8 * dy);
         vRot.setY(vRot.y()+ 8 * dx);
     } else if (event->buttons() & Qt::RightButton) {
         vRot.setX(vRot.x() + 8*dy);
         vRot.setZ(vRot.z() + 8*dx);
         ;
     }
     lastPos = event->pos();
     papa->doc->set_vRot(vRot);

    updateGL();
}

//returns the nearest 3D point (atom, helper, BCP)
QVector3D GLWidget::getSelection(int cx, int cy)
{
   if(!papa) return QVector3D(100000,100000,100000);

   double side = qMin(this->rect().width(), this->rect().height());
   if(side == 0) return QVector3D(100000,100000,100000);

   double x = cx - (this->rect().width()  - side)/2 - side/2;
   double y = cy - (this->rect().height() - side)/2 - side/2;
          y = -y;

   QVector3D box = papa->doc->get_box();
   x = x*2*box.x()/side; y = y*2*box.x()/side;

   QVector3D vRot = papa->doc->get_vRot();
   QQuaternion Rx = QQuaternion::fromAxisAndAngle(1,0,0,-vRot.x()/16.0);
   QQuaternion Ry = QQuaternion::fromAxisAndAngle(0,1,0,-vRot.y()/16.0);
   QQuaternion Rz = QQuaternion::fromAxisAndAngle(0,0,1,-vRot.z()/16.0);


   QVector3D Norm = Rz.rotatedVector(Ry.rotatedVector(Rx.rotatedVector(QVector3D(0,0,1))));
   QVector3D L    = Rz.rotatedVector(Ry.rotatedVector(Rx.rotatedVector(QVector3D(x,y,0))));


   QList<QVector3D>  points = papa->doc->get_atoms()+
                              papa->doc->get_helpers();
   for(int i = 0; i < papa->doc->get_BCPs()->size(); i++){
       points.append(papa->doc->get_BCPs()->at(i)->ptBCP);
   }

   QVector3D dv = papa->doc->get_dv();

   double min = 10000;
   double cur = 0;
   int    j = 0;

   for(int i = 0; i < points.count(); i++)
   {
       cur = sqrt(  pow(points.at(i).x()-dv.x() - L.x(),2)+
                    pow(points.at(i).y()-dv.y() - L.y(),2)+
                    pow(points.at(i).z()-dv.z() - L.z(),2)-

                    pow((points.at(i).x()-dv.x())*Norm.x() +
                        (points.at(i).y()-dv.y())*Norm.y() +
                        (points.at(i).z()-dv.z())*Norm.z(),2));

       if(cur < min)
       {
           j = i;
           min = cur;
       }
   }

   if(min < 5)
       return QVector3D(points.at(j));

   return QVector3D(100000,100000,100000);

}


/////**************************** RENDERING FUNCTIONS FOR DIFFERENT OBJECTS/////////////////////
void GLWidget::renderSurface()
{
    if(!papa) return;
    Surface * surf = papa->doc->get_surface();
    if(!surf) return;

    QVector3D dv = papa->doc->get_dv();
    int i,j = 0;


    glBegin(GL_POINTS);

    glColor3f (1.0, 0.0, 0.0); //selected
    for(i = 0; i < surf->selected.count(); i++)
            glVertex3f(surf->selected.at(i).x()-dv.x(),
                       surf->selected.at(i).y()-dv.y(),
                       surf->selected.at(i).z()-dv.z());

     glEnd();


     glColor3f (0.5, 0.5, 0.5); //triangles
     for(i = 0; i < surf->triangles.count(); i++)
     {
         glBegin(GL_LINE_LOOP);
         glVertex3f(surf->triangles.at(i).x1.x() - dv.x(),
                    surf->triangles.at(i).x1.y() - dv.y(),
                    surf->triangles.at(i).x1.z() - dv.z());
         glVertex3f(surf->triangles.at(i).x2.x() - dv.x(),
                    surf->triangles.at(i).x2.y() - dv.y(),
                    surf->triangles.at(i).x2.z() - dv.z());
         glVertex3f(surf->triangles.at(i).x3.x() - dv.x(),
                    surf->triangles.at(i).x3.y() - dv.y(),
                    surf->triangles.at(i).x3.z() - dv.z());
         glEnd();
     }

     glPointSize(1);
     glBegin(GL_POINTS);
     glColor3f (0, 1, 0); //basins
     for(i = 0; i < surf->triangles.count(); i++)
     for(j = 0; j < surf->triangles.at(i).rhobas.count(); j++)
     glVertex3f(surf->triangles.at(i).rhobas.at(j).x() - dv.x(),
                surf->triangles.at(i).rhobas.at(j).y() - dv.y(),
                surf->triangles.at(i).rhobas.at(j).z() - dv.z());

     glEnd();
     glPointSize(10);
}

void GLWidget::renderBondPaths(){
    if(!papa) return;
    BondNetwork* pBN = papa->doc->get_bondNetwork();
    if(pBN == NULL) return;

    QVector3D dv = papa->doc->get_dv();

    //bond path's
    for(int i = 0; i < pBN->bondPaths.count(); i++){
        glBegin(GL_LINE_STRIP);
        glColor3f (0.0, 0.0, 1.0);
        for(int j = 0; j < pBN->bondPaths.at(i).pts.count(); j++){
            glVertex3f(pBN->bondPaths.at(i).pts.at(j).x() - dv.x(),
                       pBN->bondPaths.at(i).pts.at(j).y() - dv.y(),
                       pBN->bondPaths.at(i).pts.at(j).z() - dv.z());
        }
        glEnd();

    }
}

void GLWidget::renderBCPs(){
    if(!papa) return;
    QList<BCP*>* BCPs = papa->doc->get_BCPs();
    if(BCPs == NULL) return;

    QVector3D dv = papa->doc->get_dv();


    //bond critical points
    glPointSize(4);
    glBegin(GL_POINTS);

    for(int i = 0; i < BCPs->count(); i++){
        if(BCPs->at(i)->selected){
            glColor3f (1, 0, 0);
        }else{
            glColor3f (0, 1, 0);
        }
        glVertex3f(BCPs->at(i)->ptBCP.x() - dv.x(),
                   BCPs->at(i)->ptBCP.y() - dv.y(),
                   BCPs->at(i)->ptBCP.z() - dv.z());
    }
    glEnd();
    glPointSize(10);

    //Hessian eigenvectors at each BCP
    QVector3D vec, bcp;
    for(int i = 0; i < BCPs->count(); i++){
        if(!BCPs->at(i)->renderEigenvectors) continue;

        bcp = BCPs->at(i)->ptBCP - dv;

        glBegin(GL_LINE_STRIP);
        glColor3f (0.0, 1.0, 0.0);
        glVertex3f(bcp.x(),bcp.y(),bcp.z());
        vec = bcp + BCPs->at(i)->Ecx;
        glVertex3f(vec.x(),vec.y(),vec.z());
        glEnd();

        glBegin(GL_LINE_STRIP);
        glColor3f (0.0, 0.0, 1.0);
        glVertex3f(bcp.x(),bcp.y(),bcp.z());
        vec = bcp + BCPs->at(i)->Ecy;
        glVertex3f(vec.x(),vec.y(),vec.z());
        glEnd();

        glBegin(GL_LINE_STRIP);
        glColor3f (1.0, 0.0, 0.0);
        glVertex3f(bcp.x(),bcp.y(),bcp.z());
        vec = bcp + BCPs->at(i)->Ecz;
        glVertex3f(vec.x(),vec.y(),vec.z());
        glEnd();
    }
}


void GLWidget::renderZfxGradLines(){
    if(!papa) return;
    QList<ZfxSurfGrad*>* sgs = papa->doc->get_zfxsurfgrads();
    if(sgs == NULL) return;

    QVector3D dv = papa->doc->get_dv();

    for(int i = 0; i < sgs->size(); i++){
        sgs->at(i)->draw(dv);
    }
}


void GLWidget::renderCGALSurfaces(){
    if(!papa) return;
    QList<CGALSurface*>* srf = papa->doc->get_cgalSurfaces();
    if(srf == NULL) return;

    QVector3D dv = papa->doc->get_dv();

    for(int i = 0; i < srf->size(); i++){
        srf->at(i)->draw_mesh(dv);
    }
}

void GLWidget::renderCamera()
{
    if(!papa) return;
    GeomObject * object = papa->doc->get_geomobject();
    if(!object) return;

    QVector3D square[4];
    int i = 0;
    QVector3D dv = papa->doc->get_dv();

    square[0] =   object->camera.Ex*object->camera.cx/2 - object->camera.Ey*object->camera.cy/2;
    square[1] =   object->camera.Ex*object->camera.cx/2 + object->camera.Ey*object->camera.cy/2;
    square[2] = - object->camera.Ex*object->camera.cx/2 + object->camera.Ey*object->camera.cy/2;
    square[3] = - object->camera.Ex*object->camera.cx/2 - object->camera.Ey*object->camera.cy/2;

    QQuaternion Rot = QQuaternion::fromAxisAndAngle(object->camera.norm,object->camera.fi*180/3.141592654);

    for(i = 0; i < 4; i++)
    {
        square[i] = Rot.rotatedVector(square[i]);
        square[i] += object->camera.center;// - object->camera.norm/2;
        square[i] = square[i] - dv;
    }

    glBegin(GL_LINE_LOOP);
    for(i = 0; i < 4; i++) glVertex3f(square[i].x(),square[i].y(),square[i].z());
    glEnd();

    dv = object->camera.center - dv;
    for(i = 0; i < 4; i++)
    {
        glBegin(GL_LINE);
        glVertex3f(square[i].x(),square[i].y(),square[i].z());
        glVertex3f(dv.x(),dv.y(),dv.z());
        glEnd();
    }
}
