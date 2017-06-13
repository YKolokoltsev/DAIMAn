#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QVector3D>
#include <QObject>

#include "wavefunction/wfndata.h"
#include "camera/camera.h"
#include "cslwl.h"
#include "zfxsurfgrad.h"
#include "cgal_surface.h"

 typedef struct Triangle
 {
     QVector3D x1;
     QVector3D x2;
     QVector3D x3;

     QList<QVector3D> elfbas;
     QList<QVector3D> rhobas;

     Triangle(QVector3D nx1 = QVector3D(0,0,0), QVector3D nx2 = QVector3D(0,0,0),QVector3D nx3 = QVector3D(0,0,0))
     {
         x1 = nx1;
         x2 = nx2;
         x3 = nx3;
     }

     Triangle& operator= (const Triangle& tri)
     {
      if(this == &tri) return *this;
      this->x1 = tri.x1;
      this->x2 = tri.x2;
      this->x3 = tri.x3;
      this->elfbas = tri.elfbas;
      this->rhobas = tri.rhobas;

      return *this;
     }


 } Triangle;

typedef struct Path
{
    QList<QVector3D> pts;

    Path()
    {
        pts.clear();
    }

    Path& operator= (const Path& path)
    {
     if(this == &path) return *this;
     this->pts = path.pts;

     return *this;
    }

} Path;

class BondPath : public Path
{
public:
    BondPath(Cslwl* cslwl){
        const int lines = 20;
        for(int i=0; i <= lines; i++){
            pts.append(cslwl->R((double)i/(double)lines));
        }

        rhoBCP = cslwl->rhoRc;
        selected = false;
    }

    BondPath(BondPath* pBP){
        this->pts = pBP->pts;
        this->selected = pBP->selected;
        this->rhoBCP = pBP->rhoBCP;
    }

public:
    double rhoBCP;
    bool selected;
};

class BCP{

public:
    BCP(Cslwl* cslwl){
        this->ptBCP = cslwl->Rc;

        this->Ecx = QVector3D(cslwl->Ecx.x(),cslwl->Ecx.y(),cslwl->Ecx.z());
        this->Ecy = QVector3D(cslwl->Ecy.x(),cslwl->Ecy.y(),cslwl->Ecy.z());
        this->Ecz = QVector3D(cslwl->Ecz.x(),cslwl->Ecz.y(),cslwl->Ecz.z());

        selected = false;
        renderEigenvectors =false;
    }

    BCP& operator= (const BCP& bcp){
        if(this == &bcp) return *this;

        this->ptBCP = bcp.ptBCP;
        this->Ecx = bcp.Ecx;
        this->Ecy = bcp.Ecy;
        this->Ecz = bcp.Ecz;

        this->selected = bcp.selected;
        this->renderEigenvectors = renderEigenvectors;
        return *this;
    }

public:
    QVector3D ptBCP;  //Bond Critical Point

    QVector3D Ecz;    //critical point axis along bond line in direction of positive hessian eigenvector
    QVector3D Ecx;    //first negative hessian eigenvector
    QVector3D Ecy;    //second negative hessian eigenvector

    bool selected;
    bool renderEigenvectors;
};

class BondNetwork{
public:
    BondNetwork(QString name){
     this->name = name;
     bondPaths.clear();
     is_active = false;
    }

    BondNetwork& operator= (const BondNetwork& bn)
    {
     if(this == &bn) return *this;
     this->name = bn.name;
     this->bondPaths = bn.bondPaths;
     this->is_active = bn.is_active;
     return *this;
    }

public:
    QString name;
    QList<BondPath> bondPaths;
    bool is_active;
};

 class GeomObject
 {
     public:
     GeomObject()
     {
         selected.empty();
     }
     QList<QVector3D>  selected;   //list of selected points
     Camera             camera;
 };

 class Surface : public GeomObject
 {
  public:
     Surface(QString new_name)
     {
         ::GeomObject();
         name = new_name;
         triangles.empty();
         is_active = false;
     }
     Surface& operator= (const Surface& surf)
{
      if(this == &surf) return *this;
      this->name = surf.name;
      this->triangles = surf.triangles;
      this->is_active = surf.is_active;
      this->camera = surf.camera;
      this->selected = surf.selected;
      return *this;
}

   public:
     QString           name;       //surface name (in the tree view)
     QList<Triangle>   triangles;  //triangles
     bool              is_active;  //this surface will be processed by the
                                   //other functions only if it is active
 };

class Document : public QObject
{
public:
    Document();
    ~Document();

public:
    void set_WFN(WFNData* data);
    void set_vRot(QVector3D new_vRot){vRot = QVector3D(new_vRot);}
    void set_Helpers(QList<QVector3D>  new_helpers){helpers = new_helpers;}
    void set_Surfaces(QList<Surface> new_surf){surfaces = new_surf;}
    void set_wfn_file_path(QString path){wfn_file_path = path;}
    void cleanup();

    QVector3D get_dv(){return dv;}
    QVector3D get_box(){return box;}
    QVector3D get_vRot(){return vRot;}

    QList<QVector3D>   get_atoms(){return atoms;}
    QList<QVector3D>   get_helpers(){return helpers;}
    QList<Surface>*     get_surfaces(){return &surfaces;}
    QString get_wfn_file_path(){return wfn_file_path;}
    QList<BondNetwork>* get_bondNetworks(){return &bondNetworks;}
    QList<ZfxSurfGrad*>* get_zfxsurfgrads(){return &zfxSurfGrads;}
    QList<CGALSurface*>* get_cgalSurfaces(){return &cgalSurfaces;}
    QList<BCP*>* get_BCPs(){return &BCPs;}

    GeomObject*        get_geomobject();
    Surface*           get_surface();
    BondNetwork*       get_bondNetwork();

    WFNData* get_wfn(){return wfn;}


private:
    //ACTUAL DATA

    //********* OBJECTS
    QList<Surface>   surfaces;       //list of triangles (intersections)
    QList<BondNetwork> bondNetworks; //list of interatomic bonds
    QList<BCP*> BCPs;                 //list of bond critical points
    QList<ZfxSurfGrad*> zfxSurfGrads; //list of interatomic zero flux surfaces
    QList<CGALSurface*> cgalSurfaces; //list of logical triangulated surfaces for rendering
    QString wfn_file_path;            //full path to wfn file (to simplify future opening of additional files)
    WFNData* wfn;                    //wavefunction data
    //********* HELPER OBJECTS
    QList<QVector3D>  atoms;         //list of not selected atoms;
    QList<QVector3D>  helpers;       //list of not selected helpers;

    //*********VISUALIZATION********
    QVector3D dv;                     //Shift of the molecule (to the center of mass)
    QVector3D box;                    //Bounding box of the molecule
    QVector3D vRot;                   //Rotation of the view along axes;

public:
    QVector3D NCamera;                //vector always pointing into camera
};

#endif // DOCUMENT_H
