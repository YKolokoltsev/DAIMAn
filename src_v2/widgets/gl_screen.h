//
// Created by morrigan on 20/06/17.
//

#ifndef DAIMAN_DGLSCREEN_H
#define DAIMAN_DGLSCREEN_H

//https://doc.qt.io/qt-5/qt3d-basicshapes-cpp-example.html
//https://www.digitalocean.com/community/tutorials/ubuntu-16-04-lts-ru

//#include <QtOpenGL/QGLWidget> (old base)
#include <QWidget>
#include <Qt3DExtras/Qt3DWindow>

#include <Qt3DExtras>
#include <Qt3DCore>

#include "base_vertex.hpp"
#include "graph.h"
#include "ref_decorator.hpp"
#include "client_splitter.h"
#include "top3dentity.hpp"
#include "orbital_camera_controller.h"
#include "scene_entity.h"

//todo: Qt3DWindow shell be primary, and this one has to be a widget...
class DGlScreen : public Qt3DExtras::Qt3DWindow, public BaseObj{
Q_OBJECT

    //typedef std::shared_ptr<Qt3DCore::QEntity> t_scene_ptr;
public:
    DGlScreen(node_desc_t);
    //t_scene_ptr get_scene(){return curr_scene;}
    void switch_scene(node_desc_t);
    void reg_scene(DSceneEntity* );
    void unreg_scene(DSceneEntity* );

private slots:
    void sl_viewMatrixChanged();

private:
    ext_weak_ptr_t<DClientSplitter> client_edge;

    //***********NEW************
    OrbitalCameraController *camController;
    DSceneEntity* curr_scene; //??
    Qt3DCore::QEntity* rootEntity;
    Qt3DRender::QCamera* cameraEntity;
};


#endif //DAIMAN_DGLSCREEN_H
