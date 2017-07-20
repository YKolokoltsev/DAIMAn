//
// Created by morrigan on 15/07/17.
//

#ifndef DAIMAN_SCENEENTITY_H
#define DAIMAN_SCENEENTITY_H

#include <Qt3DCore>

#include "base_vertex.hpp"
#include "ref_decorator.hpp"
#include "wfx_item.h"
#include "orbital_camera_controller.h"
#include "gl_screen.h"

class DGlScreen;
class DSceneEntity : public Qt3DCore::QEntity, public BaseObj{
Q_OBJECT
public:
    DSceneEntity(node_desc_t, node_desc_t);
    ~DSceneEntity();
    void setCamera(Qt3DRender::QCamera*);
    void viewMatrixChanged(Qt3DRender::QCamera*);

private:
    ext_weak_ptr_t<DWfxItem> parent;
    ext_weak_ptr_t<DGlScreen> gl_screen;

    Qt3DCore::QTransform *lightTransform;
    Qt3DCore::QEntity *lightEntity;

    QVector3D camPosition;
    QVector3D camUpVector;
    QVector3D camViewCenter;
};


#endif //DAIMAN_SCENEENTITY_H
