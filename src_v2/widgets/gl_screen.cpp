//
// Created by morrigan on 20/06/17.
//

#include "gl_screen.h"

DGlScreen::DGlScreen(node_desc_t client_edge_idx) : curr_scene(nullptr) {
    reg(this, true);
    client_edge = std::move(get_weak_obj_ptr<DClientSplitter>(this,client_edge_idx));
    client_edge->ptr.lock()->addWidget(QWidget::createWindowContainer(this));
    defaultFrameGraph()->setClearColor(QColor(QRgb(0x4d4d4f)));

    rootEntity = new Qt3DCore::QEntity;

    // Camera
    cameraEntity = camera();
    cameraEntity->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    cameraEntity->setPosition(QVector3D(0, 0, -40.0f));
    cameraEntity->setUpVector(QVector3D(0, 1, 0));
    cameraEntity->setViewCenter(QVector3D(0, 0, 0));

    camController = new OrbitalCameraController();
    camController->setCamera(cameraEntity);
    camController->setParent(rootEntity);

    // Rendering setup
    renderSettings()->setRenderPolicy(Qt3DRender::QRenderSettings::OnDemand);
    renderSettings()->pickingSettings()->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);
    renderSettings()->pickingSettings()->setPickResultMode(Qt3DRender::QPickingSettings::AllPicks);

    // Here rendering begins
    setRootEntity(rootEntity);
}

void DGlScreen::sl_viewMatrixChanged(){
    if(curr_scene){
        curr_scene->viewMatrixChanged(cameraEntity);
    }
}

void DGlScreen::reg_scene(DSceneEntity* scene){
    scene->setEnabled(false);
    scene->setParent(rootEntity);
}

void DGlScreen::unreg_scene(DSceneEntity* scene){
    if(curr_scene == scene){
        curr_scene = nullptr;
    }
    scene->setEnabled(false);
    scene->setParent((Qt3DCore::QNode*) nullptr);
}

void DGlScreen::switch_scene(node_desc_t wfx_idx){
    //todo: try to remove scene from root node, at least because of ObjectPicker
    //however it is much more optimal than it is now
    QObject::disconnect(cameraEntity, &Qt3DRender::QCamera::viewMatrixChanged, 0, 0);

    if(curr_scene) curr_scene->setEnabled(false);
    for_each_child<DSceneEntity>(wfx_idx,[&](DSceneEntity* scene){
        scene->setEnabled(true);
        scene->setCamera(cameraEntity);
        curr_scene = scene;
    });

    QObject::connect(cameraEntity, &Qt3DRender::QCamera::viewMatrixChanged,
                     this, &DGlScreen::sl_viewMatrixChanged);
}