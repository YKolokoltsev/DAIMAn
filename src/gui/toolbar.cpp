#include "toolbar.h"
#include "eps_writer.h"
#include <QIcon>
#include <QMessageBox>
#include <QVector3D>
#include <QInputDialog>
#include <QLineEdit>
#include <QFileInfo>
#include <QDir>

Toolbar::Toolbar(MainWindow* parent)
{
    papa = parent;

    setWindowTitle(tr("Actions menu"));

    act_rotate = NULL;
    act_select = NULL;
    m_ntriangle = NULL;
    act_dtriangle = NULL;
    m_npoint = NULL;
    act_dpoint = NULL;

    icosize = "32x32";
    icopath = "/home/morrigan/CLionProjects/DAIMAn/images/";
    menu_created = createActions();
    createMenus();
}

bool Toolbar::createActions()
{
     act_rotate = new QAction(QIcon(QString(icopath+"rotate_"+icosize+".png")),tr("Rotate"),this);
     if(act_rotate == NULL) return false;
     act_rotate->setStatusTip(tr("Rotate 3D view"));
     connect(act_rotate, SIGNAL(triggered()), this, SLOT(sl_rotate()));

     act_select = new QAction(QIcon(QString(icopath+"arrow_"+icosize+".png")),tr("Select"),this);
     if(act_select == NULL) return false;
     act_select->setStatusTip(tr("Select atomic centers and points"));
     connect(act_select, SIGNAL(triggered()), this, SLOT(sl_select()));

     m_ntriangle = new QMenu("New Triangle",this);
     if(m_ntriangle == NULL) return false;
     m_ntriangle->setIcon(QIcon(QString(icopath+"ntriangle_"+icosize+".png")));
     m_ntriangle->menuAction()->setStatusTip(tr("Create a triangle facet on three points"));
     connect(m_ntriangle->menuAction(), SIGNAL(triggered()), this, SLOT(sl_ntriangle()));

     act_dtriangle = new QAction(QIcon(QString(icopath+"dtriangle_"+icosize+".png")),tr("Remove Triangle"),this);
     if(act_dtriangle == NULL) return false;
     act_dtriangle->setStatusTip(tr("Remove a facet from three selected points"));
     connect(act_dtriangle, SIGNAL(triggered()), this, SLOT(sl_dtriangle()));

     m_npoint = new QMenu("New Point",this);
     if(m_npoint == NULL) return false;
     m_npoint->setIcon(QIcon(QString(icopath+"npoint_"+icosize+".png")));
     m_npoint->menuAction()->setStatusTip(tr("Put a point at the center of mass of the selected points"));
     connect(m_npoint->menuAction(), SIGNAL(triggered()), this, SLOT(sl_npoint()));

     act_dpoint = new QAction(QIcon(QString(icopath+"dpoint_"+icosize+".png")),tr("Remove point"),this);
     if(act_dpoint == NULL) return false;
     act_dpoint->setStatusTip(tr("Remove helper point"));
     connect(act_dpoint, SIGNAL(triggered()), this, SLOT(sl_dpoint()));

     act_nzfxsurf = new QAction(QIcon(QString(icopath+"zfxsurf_"+icosize+".png")),tr("Zero flux surfaces"),this);
     if(act_nzfxsurf == NULL) return false;
     act_nzfxsurf->setStatusTip(tr("Create zero flux surfaces for selected BCPs"));
     connect(act_nzfxsurf, SIGNAL(triggered()), this, SLOT(sl_nzfxsurf()));


     act_save_eps = new QAction(QIcon(QString(icopath+"picture_save_"+icosize+".png")),tr("Save view to EPS file"),this);
     if(act_save_eps == NULL) return false;
     act_save_eps->setStatusTip(tr("Save all viewable primitives to an eps file"));
     connect(act_save_eps, SIGNAL(triggered()), this, SLOT(sl_act_save_eps()));

     return true;
}

void Toolbar::createMenus()
{
    if(!menu_created) return;

    addAction(act_rotate);
    act_rotate->setCheckable(true);

    addAction(act_select);
    act_select->setCheckable(true);

    addSeparator();

    m_ntriangle->addAction(act_dtriangle);
    addAction(m_ntriangle->menuAction());

    m_npoint->addAction(act_dpoint);
    addAction(m_npoint->menuAction());

    addSeparator();

    addAction(act_nzfxsurf);
    addAction(act_save_eps);

    disableActions();
}

void Toolbar::initActions()
{
    if(!menu_created) return;
    disableActions();
    act_rotate->setDisabled(false);
    act_rotate->setChecked(true);
    act_select->setDisabled(false);
    act_select->setChecked(false);
    act_save_eps->setDisabled(false);
}

void Toolbar::disableActions()
{
    if(!menu_created) return;
    act_rotate->setDisabled(true);
    act_select->setDisabled(true);
    act_dtriangle->setDisabled(true);
    m_ntriangle->menuAction()->setDisabled(true);
    act_dpoint->setDisabled(true);
    m_npoint->menuAction()->setDisabled(true);
    act_nzfxsurf->setDisabled(true);
    act_save_eps->setDisabled(true);
}

void Toolbar::updateState()
{
    if(!papa) return;

    //BCP actions
    bool hasSelectedBCPs = false;
    QList<BCP*>* BCPs = papa->doc->get_BCPs();
    for(int i = 0; i < BCPs->size(); i++){
        if(BCPs->at(i)->selected){
            hasSelectedBCPs = true;
            break;
        }
    }
    act_nzfxsurf->setDisabled(!hasSelectedBCPs);

    //surface actions
    GeomObject * object = papa->doc->get_geomobject();
    if(object){
        int n = object->selected.count();

        act_dpoint->setDisabled(n == 0);
        m_npoint->menuAction()->setDisabled(n == 0);

        act_dtriangle->setDisabled(n != 3);
        m_ntriangle->menuAction()->setDisabled(n != 3);
    }
}

//ACTIONS============================================================

void Toolbar::sl_rotate()
{
    if(!menu_created) return;

    act_rotate->setChecked(true);
    act_select->setChecked(false);

}

void Toolbar::sl_select()
{
    if(!menu_created) return;

    act_rotate->setChecked(false);
    act_select->setChecked(true);
}

void Toolbar::sl_ntriangle()
{
    if(!papa) return;
    GeomObject * object = papa->doc->get_geomobject();
    if(!object) return;
    Surface * surf = papa->doc->get_surface();
    if(!surf) return;

    int i = 0, j = 0, k = 0;
    QList<Triangle> inter = surf->triangles;
    QList<QVector3D> selected = object->selected;
    if(selected.count() != 3) return;

    if(selected.at(0) == selected.at(1) ||
       selected.at(0) == selected.at(2) ||
       selected.at(1) == selected.at(2)) return;

    for(i = 0; i < inter.count(); i++)
    {
        k = 0;
        for(j = 0; j < 3; j++)
            if(inter.at(i).x1 == selected.at(j))
                k = k+1;

        for(j = 0; j < 3; j++)
            if(inter.at(i).x2 == selected.at(j))
                k = k+1;

        for(j = 0; j < 3; j++)
            if(inter.at(i).x3 == selected.at(j))
                k = k+1;

        if(k == 3) return;
    }

    inter.append(Triangle(selected.at(0),selected.at(1),selected.at(2)));
    surf->triangles = inter;
    object->selected.clear();
    papa->glWidget->updateGL();
}

void Toolbar::sl_dtriangle()
{
   if(!papa) return;
    GeomObject * object = papa->doc->get_geomobject();
    if(!object) return;
    Surface * surf = papa->doc->get_surface();
    if(!surf) return;

    int i = 0, j = 0, k = 0;
    QList<Triangle> inter = surf->triangles;
    QList<QVector3D> selected = object->selected;
    if(selected.count() != 3) return;

    int n = inter.count();

    if(selected.at(0) == selected.at(1) ||
       selected.at(0) == selected.at(2) ||
       selected.at(1) == selected.at(2)) return;

    for(i = 0; i < inter.count(); i++)
    {
        k = 0;
        for(j = 0; j < 3; j++)
            if(inter.at(i).x1 == selected.at(j))
                k = k+1;

        for(j = 0; j < 3; j++)
            if(inter.at(i).x2 == selected.at(j))
                k = k+1;

        for(j = 0; j < 3; j++)
            if(inter.at(i).x3 == selected.at(j))
                k = k+1;

        if(k == 3)
            inter.removeAt(i);
    }

    if(n > inter.count()) selected.clear();

    surf->triangles = inter;
    object->selected = selected;
    papa->glWidget->updateGL();
}

void Toolbar::sl_npoint()
{
   if(!papa) return;
   GeomObject * object = papa->doc->get_geomobject();
   if(!object) return;

   int i = 0;
   QVector3D new_helper(0,0,0);
   QList<QVector3D> new_helpers = papa->doc->get_helpers();
   QList<QVector3D> selected = object->selected;
   if(selected.count() == 0) return;

   for(i = 0; i < selected.count(); i++)
       new_helper += selected.at(i);
   new_helper = new_helper/selected.count();

   for(i = 0; i < new_helpers.count(); i++)
       if(new_helper == new_helpers.at(i))
           return;

   new_helpers.append(new_helper);
   papa->doc->set_Helpers(new_helpers);
   object->selected.clear();
   papa->glWidget->updateGL();
}

void Toolbar::sl_dpoint()
{
   if(!papa) return;
   GeomObject * object = papa->doc->get_geomobject();
   if(!object) return;
   Surface * surf = papa->doc->get_surface();

   QList<QVector3D> new_helpers = papa->doc->get_helpers();
   QList<QVector3D> selected = object->selected;
   QList<Triangle> inter; if(surf) inter = surf->triangles;

   if(selected.count() == 0) return;

   for(int i = 0; i < selected.count(); i++)
       for(int j = 0; j < new_helpers.count(); j++)
           if(selected.at(i) == new_helpers.at(j))
           {
               for(int k = 0; k < inter.count();)
               {
                   if(inter.at(k).x1 == new_helpers.at(j) ||
                      inter.at(k).x2 == new_helpers.at(j) ||
                      inter.at(k).x3 == new_helpers.at(j))
                        inter.removeAt(k);
                   else
                   k = k +1;
               }
               new_helpers.removeAt(j);
           }

   object->selected.clear();
   if(surf) surf->triangles = inter;
   papa->doc->set_Helpers(new_helpers);
   papa->glWidget->updateGL();
}

void Toolbar::sl_nzfxsurf(){
    if(!papa) return;

    QList<BCP*>* BCPs = papa->doc->get_BCPs();
    QList<ZfxSurfGrad*>* Zfxs =  papa->doc->get_zfxsurfgrads();
    QList<CGALSurface*>* Cgss =  papa->doc->get_cgalSurfaces();

    bool exist;
    ZfxSurfGrad* newZfx;
    CGALSurface* newCgss;
    Point_with_normal_3_list rand_points;

    for(int i = 0; i < BCPs->size(); i++){
        exist = false;
        if(BCPs->at(i)->selected){
            for(int j = 0; j < Zfxs->size(); j++){
                if(Zfxs->at(j)->compRc(BCPs->at(i)->ptBCP)){
                    exist = true;
                }
            }
            if(!exist){
                newZfx = new ZfxSurfGrad(BCPs->at(i)->ptBCP,
                                         BCPs->at(i)->Ecx,
                                         BCPs->at(i)->Ecy,
                                         BCPs->at(i)->Ecz,
                                         papa->doc->get_wfn());
                Zfxs->append(newZfx);

                //newZfx->fillPoints(20);

                //rand_points.clear();
                //newZfx->fillPoints(20, &rand_points);

                newCgss = new CGALSurface();
                newZfx->fillPolyhedron(newCgss->getSurface());

                newCgss->print_polyhedron_info();
                //newCgss->removeZeroTriangles();
                //newCgss->print_polyhedron_info();

                //newCgss->parameterize();
                //newCgss->save_parameterization_eps("/home/data/tmp/parameterize.eps",500);
                //newCgss->setMeshVectors(BCPs->at(i)->ptBCP,BCPs->at(i)->Ecx,BCPs->at(i)->Ecy);
                //newCgss->buildMesh(1,5);

                Cgss->append(newCgss);

            }
        }
    }

    papa->glWidget->updateGL();
}

void Toolbar::sl_act_save_eps(){
    bool ok;
    QString name;
    int i = 0;
    EpsWriter eps;

    QList<BondNetwork>* bondNetworks;
    QList<BCP*>* BCPs;
    QList<ZfxSurfGrad*>* zfxSurfGrads;
    QList<CGALSurface*>* cgalSurfaces;

    QList<QList<QVector3D>*> lines;
    QList<QVector3D>* line;
    Polyhedron* polyhedron;

    QString bcp_file_base_name =
            QFileInfo(papa->doc->get_wfn_file_path()).dir().absolutePath() + "/" +
            QFileInfo(papa->doc->get_wfn_file_path()).completeBaseName() + "_";


    do{
        name = bcp_file_base_name + QString("%1").arg(i) + QString("%1").arg(".eps");
        i++;
    }while(QFile(name).exists());

    bcp_file_base_name = name;

    name = QInputDialog::getText(this,
                          QString("New Bonds Set Name"),
                          NULL,
                          QLineEdit::Normal,
                          QFileInfo(bcp_file_base_name).fileName(),
                          &ok);
    if(ok){
        bcp_file_base_name = QFileInfo(papa->doc->get_wfn_file_path()).dir().absolutePath() + "/" + name;

        bondNetworks = papa->doc->get_bondNetworks();
        BCPs = papa->doc->get_BCPs();
        zfxSurfGrads = papa->doc->get_zfxsurfgrads();
        cgalSurfaces = papa->doc->get_cgalSurfaces();

        //Bond paths (rho(BCP) > 0.025)
        for(i = 0; i < bondNetworks->size(); i++){
            if(bondNetworks->at(i).is_active){
                for(int j = 0; j < bondNetworks->at(i).bondPaths.size(); j++){
                    if(bondNetworks->at(i).bondPaths.at(j).rhoBCP > 0.025){
                        line = new QList<QVector3D>(bondNetworks->at(i).bondPaths.at(j).pts);
                        lines.append(line);
                    }
                    printf("rhpBCP = % .e\n",bondNetworks->at(i).bondPaths.at(j).rhoBCP);
                }
                eps.appendLines(&lines,3,QColor(0,0,0));
                clearLines(&lines);
            }
        }

        //Bond paths (rho(BCP) < 0.025)
        for(i = 0; i < bondNetworks->size(); i++){
            if(bondNetworks->at(i).is_active){
                for(int j = 0; j < bondNetworks->at(i).bondPaths.size(); j++){
                    if(bondNetworks->at(i).bondPaths.at(j).rhoBCP <= 0.025){
                        line = new QList<QVector3D>(bondNetworks->at(i).bondPaths.at(j).pts);
                        lines.append(line);
                    }
                }
                eps.appendLines(&lines,3,QColor(0,0,0),10);
                clearLines(&lines);
            }
        }

        //Zero flux surfaces (and the others)
        for(i = 0; i < cgalSurfaces->size(); i++){
            polyhedron = cgalSurfaces->at(i)->getSurface();
            for(Halfedge_handle h = polyhedron->halfedges_begin(); h != polyhedron->halfedges_end(); ++h){
                line = new QList<QVector3D>;
                line->append(QVector3D(h->vertex()->point().x(),h->vertex()->point().y(),h->vertex()->point().z()));
                line->append(QVector3D(h->opposite()->vertex()->point().x(),h->opposite()->vertex()->point().y(),h->opposite()->vertex()->point().z()));
                lines.append(line);
            }
            eps.appendLines(&lines,0.5,QColor(0,0,0));
            clearLines(&lines);
        }

        //BCPs
        line = new QList<QVector3D>;
        for(i = 0; i < BCPs->size(); i++){
            line->append(QVector3D(BCPs->at(i)->ptBCP.x(),BCPs->at(i)->ptBCP.y(),BCPs->at(i)->ptBCP.z()));
        }
        eps.appendPoints(line,5,QColor(0,0,0));
        delete line;

        //Atoms
        line = new QList<QVector3D>;
        for(int i = 0; i < papa->doc->get_wfn()->nat; i++){
            line->append(QVector3D(papa->doc->get_wfn()->atcoords[i][0],
                                   papa->doc->get_wfn()->atcoords[i][1],
                                   papa->doc->get_wfn()->atcoords[i][2]));
        }
        eps.appendPoints(line,15,QColor(0,0,0));
        delete line;

        eps.saveEps(bcp_file_base_name,papa->doc->NCamera,100);
        //eps.saveEps(bcp_file_base_name,QVector3D(1,0,0),100);
    }
}
