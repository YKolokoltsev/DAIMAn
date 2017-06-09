#include "document.h"

Document::Document()
{
    wfn = NULL;                      //wavefunction data

    //*********HELPER OBJECTS
    surfaces.clear();                 //list of surfaces (intersections)
    atoms.clear();                    //list of the atomas (not selected)
    helpers.clear();                  //list of the helpers(not selected)

    //*********VISUALIZATION********
    dv   = QVector3D(0,0,0);          //Shift of the molecule (to the center of mass)
    box  = QVector3D(1,1,1);          //Bounding box of the molecule
    vRot = QVector3D(0,0,0);          //Rotation of the view along axes;
}

Document::~Document()
{
    cleanup();
}

void Document::cleanup()
{
    if(wfn != NULL) delete wfn, wfn = NULL;

    dv   = QVector3D(0,0,0);
    box  = QVector3D(1,1,1);
    vRot = QVector3D(0,0,0);

    surfaces.clear();
    bondNetworks.clear();
    atoms.clear();
    helpers.clear();

    for(int i = 0; i < zfxSurfGrads.size(); i++){
        delete zfxSurfGrads.at(i);
    }
    zfxSurfGrads.clear();

    for(int i = 0; i < cgalSurfaces.size(); i++){
        delete cgalSurfaces.at(i);
    }
    cgalSurfaces.clear();

    for(int i = 0; i < BCPs.size(); i++){
        delete BCPs.at(i);
    }
    BCPs.clear();
}

void Document::set_WFN(WFNData* data)
{
    cleanup();

    if(data == NULL) return;
    wfn = data;

    double max = 0, next = 0;

    max = (wfn->getMinMaxXYZ(0,false)/2 - wfn->getMinMaxXYZ(0,true)/2)*1.3;
    next = (wfn->getMinMaxXYZ(1,false)/2 - wfn->getMinMaxXYZ(1,true)/2)*1.3;
    max = next>max?next:max;
    next = (wfn->getMinMaxXYZ(2,false)/2 - wfn->getMinMaxXYZ(2,true)/2)*1.3;
    max = next>max?next:max;

    box = QVector3D(max,max,max);
    dv  = QVector3D(wfn->getMidXYZ(0),wfn->getMidXYZ(1),wfn->getMidXYZ(2));

    for(int i = 0; i < wfn->nat; i++)
        atoms.append(QVector3D(wfn->atcoords[i][0],wfn->atcoords[i][1],wfn->atcoords[i][2]));
}

GeomObject* Document::get_geomobject()
{
    for(int i = 0; i < surfaces.count(); i++)
        if(surfaces.at(i).is_active) return (GeomObject*) &surfaces.at(i);

    return NULL;
}

Surface* Document::get_surface()
{
    for(int i = 0; i < surfaces.count(); i++)
        if(surfaces.at(i).is_active) return (Surface*) &surfaces.at(i);

    return NULL;
}

BondNetwork* Document::get_bondNetwork(){
    for(int i = 0; i < bondNetworks.count(); i++){
        if(bondNetworks.at(i).is_active) return (BondNetwork*) & bondNetworks.at(i);
    }
    return NULL;
}
