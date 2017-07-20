//
// Created by morrigan on 14/07/17.
//

#include <iostream>
#include "orbital_camera_controller.h"

OrbitalCameraController::OrbitalCameraController(Qt3DCore::QNode *parent) :
        m_camera(nullptr),
        m_leftMouseButtonAction(new Qt3DInput::QAction),
        m_rightMouseButtonAction(new Qt3DInput::QAction),
        m_leftMouseButtonInput(new Qt3DInput::QActionInput),
        m_rightMouseButtonInput(new Qt3DInput::QActionInput),
        m_rxAxis(new Qt3DInput::QAxis),
        m_ryAxis(new Qt3DInput::QAxis),
        m_tzAxis(new Qt3DInput::QAxis),
        m_mouseRxInput(new Qt3DInput::QAnalogAxisInput),
        m_mouseRyInput(new Qt3DInput::QAnalogAxisInput),
        m_mouseTzXInput(new Qt3DInput::QAnalogAxisInput),
        m_mouseTzYInput(new Qt3DInput::QAnalogAxisInput),
        m_mouseDevice(new Qt3DInput::QMouseDevice),
        m_logicalDevice(new Qt3DInput::QLogicalDevice),
        m_frameAction(new Qt3DLogic::QFrameAction) {

    // Left Mouse Button Action
    m_leftMouseButtonInput->setButtons(QVector<int>() << Qt::LeftButton);
    m_leftMouseButtonInput->setSourceDevice(m_mouseDevice);
    m_leftMouseButtonAction->addInput(m_leftMouseButtonInput);

    // Right Mouse Button Action
    m_rightMouseButtonInput->setButtons(QVector<int>() << Qt::RightButton);
    m_rightMouseButtonInput->setSourceDevice(m_mouseDevice);
    m_rightMouseButtonAction->addInput(m_rightMouseButtonInput);

    // Mouse X
    m_mouseRxInput->setAxis(Qt3DInput::QMouseDevice::X);
    m_mouseRxInput->setSourceDevice(m_mouseDevice);
    m_rxAxis->addInput(m_mouseRxInput);

    // Mouse Y
    m_mouseRyInput->setAxis(Qt3DInput::QMouseDevice::Y);
    m_mouseRyInput->setSourceDevice(m_mouseDevice);
    m_ryAxis->addInput(m_mouseRyInput);

    // Mouse Wheel X
    m_mouseTzXInput->setAxis(Qt3DInput::QMouseDevice::WheelX);
    m_mouseTzXInput->setSourceDevice(m_mouseDevice);
    m_tzAxis->addInput(m_mouseTzXInput);

    // Mouse Wheel Y
    m_mouseTzYInput->setAxis(Qt3DInput::QMouseDevice::WheelY);
    m_mouseTzYInput->setSourceDevice(m_mouseDevice);
    m_tzAxis->addInput(m_mouseTzYInput);

    //Logical Device
    m_logicalDevice->addAction(m_leftMouseButtonAction);
    m_logicalDevice->addAction(m_rightMouseButtonAction);
    m_logicalDevice->addAxis(m_rxAxis);
    m_logicalDevice->addAxis(m_ryAxis);
    m_logicalDevice->addAxis(m_tzAxis);

    // FrameAction
    QObject::connect(m_frameAction, SIGNAL(triggered(float)),
                     this, SLOT(sl_onTriggered(float)) );


    // Disable the logical device when the entity is disabled
    QObject::connect(this, &Qt3DCore::QEntity::enabledChanged,
                     m_logicalDevice, &Qt3DInput::QLogicalDevice::setEnabled);

    // Finish up entity tree
    addComponent(m_frameAction);
    addComponent(m_logicalDevice);
}

void OrbitalCameraController::setCamera(Qt3DRender::QCamera *camera){
    if (m_camera != camera) {
        m_camera = camera;
    }
}

float zoomDistance(QVector3D firstPoint, QVector3D secondPoint)
{
    return (secondPoint - firstPoint).lengthSquared();
}

void OrbitalCameraController::sl_onTriggered(float dt){
    if(m_rightMouseButtonAction->isActive() && (m_camera != nullptr)) {
        // Orbit
        auto dy_axis = QVector3D::crossProduct(m_camera->viewVector(), m_camera->upVector());
        dy_axis.normalize();
        m_camera->panAboutViewCenter((m_ryAxis->value() * 180.0f/50.0f), dy_axis);

        auto dx_axis = QVector3D::crossProduct(m_camera->viewVector(), m_camera->upVector());
        dx_axis = QVector3D::crossProduct(m_camera->viewVector(), dx_axis);
        dx_axis.normalize();
        m_camera->panAboutViewCenter((m_rxAxis->value() * 180.0f/50.0f), dx_axis);
    }else if(m_tzAxis->value() != 0){
        if ( zoomDistance(m_camera->position(), m_camera->viewCenter()) > 1) {
            // Dolly up to limit
            m_camera->translate(QVector3D(0, 0, m_tzAxis->value()), m_camera->DontTranslateViewCenter);
        } else {
            m_camera->translate(QVector3D(0, 0, 1), m_camera->DontTranslateViewCenter);
        }
    }
}
