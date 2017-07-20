//
// Created by morrigan on 15/07/17.
//

#include "scene_entity.h"

DSceneEntity::DSceneEntity(node_desc_t wfx_idx, node_desc_t gl_idx) :
        camPosition(0, 0, -40.0f),
        camUpVector(0, 1, 0),
        camViewCenter(0, 0, 0)
{
    reg(this, false);
    parent = std::move(get_weak_obj_ptr<DWfxItem>(this, wfx_idx));
    gl_screen = std::move(get_weak_obj_ptr<DGlScreen>(this, gl_idx));

    // Light
    lightEntity = new Qt3DCore::QEntity;
    Qt3DRender::QPointLight *light = new Qt3DRender::QPointLight(lightEntity);
    light->setColor("white");
    light->setIntensity(1);
    lightEntity->addComponent(light);

    lightTransform = new Qt3DCore::QTransform(lightEntity);
    lightEntity->addComponent(lightTransform);
    lightEntity->setParent(this);

    //reg scene
    gl_screen->ptr.lock()->reg_scene(this);
}

DSceneEntity::~DSceneEntity(){
    gl_screen->ptr.lock()->unreg_scene(this);
}

void DSceneEntity::setCamera(Qt3DRender::QCamera* camera){
    camera->setPosition(camPosition);
    camera->setUpVector(camUpVector);
    camera->setViewCenter(camViewCenter);
    lightTransform->setTranslation(camera->position());
}

void DSceneEntity::viewMatrixChanged(Qt3DRender::QCamera* camera){
    //Light always follow the camera
    lightTransform->setTranslation(camera->position());

    //Store current camera position
    camPosition = camera->position();
    camUpVector = camera->upVector();
    camViewCenter = camera->viewCenter();
}