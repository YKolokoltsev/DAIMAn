//
// Created by morrigan on 14/07/17.
//

#ifndef DAIMAN_ORBITALCAMERACONTROLLER_H
#define DAIMAN_ORBITALCAMERACONTROLLER_H

#include <Qt3DRender>
#include <Qt3DInput>
#include <Qt3DLogic>

class OrbitalCameraController : public Qt3DCore::QEntity{
Q_OBJECT

public:
    OrbitalCameraController(Qt3DCore::QNode *parent = nullptr);
    void setCamera(Qt3DRender::QCamera *);

public slots:
    void sl_onTriggered(float);

private:
    Qt3DRender::QCamera *m_camera;

    Qt3DInput::QAction *m_leftMouseButtonAction;
    Qt3DInput::QAction *m_rightMouseButtonAction;

    Qt3DInput::QActionInput *m_leftMouseButtonInput;
    Qt3DInput::QActionInput *m_rightMouseButtonInput;

    Qt3DInput::QAxis *m_rxAxis;
    Qt3DInput::QAxis *m_ryAxis;
    Qt3DInput::QAxis *m_tzAxis;

    Qt3DInput::QAnalogAxisInput *m_mouseRxInput;
    Qt3DInput::QAnalogAxisInput *m_mouseRyInput;
    Qt3DInput::QAnalogAxisInput *m_mouseTzXInput;
    Qt3DInput::QAnalogAxisInput *m_mouseTzYInput;

    Qt3DInput::QMouseDevice *m_mouseDevice;
    Qt3DInput::QLogicalDevice *m_logicalDevice;
    Qt3DLogic::QFrameAction *m_frameAction;
};


#endif //DAIMAN_ORBITALCAMERACONTROLLER_H
