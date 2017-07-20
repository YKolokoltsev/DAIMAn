#include <iostream>
#include <QApplication>

#include "doc_tree.hpp"
#include "main_window.h"

using namespace std;

int main(int argc, char *argv[])
{
    auto dt = DocTree::inst();
    QApplication app(argc, argv);
    new DMainWindow();

    auto ret = app.exec();
    dt->clear();
    cout << "nodes: " << dt->node_count() << ", edges: " << dt->edge_count() << endl;
    return ret;
}

//http://openbabel.org/api/2.2.0/classOpenBabel_1_1OBElementTable.shtml#_details

/*
#include <iostream>

#include <QGuiApplication>
#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>

#include "orbital_camera_controller.h"

class MyPicker : public Qt3DRender::QObjectPicker{
Q_OBJECT
public:

    MyPicker(Qt3DCore::QEntity * entity) : Qt3DRender::QObjectPicker(entity){
        connect( this, SIGNAL(clicked(Qt3DRender::QPickEvent* )),
                 this, SLOT(clicked_sl(Qt3DRender::QPickEvent*)) );
    }

public slots:
    void clicked_sl(Qt3DRender::QPickEvent *pick){
        std::cout << "pick: " << i++ << std::endl;
    }

private:
    int i{0};
};*/

/*int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    Qt3DExtras::Qt3DWindow view;

    // Root entity
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();

    // Camera
    Qt3DRender::QCamera *cameraEntity = view.camera();
    cameraEntity->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    cameraEntity->setPosition(QVector3D(0, 0, -40.0f));
    cameraEntity->setUpVector(QVector3D(0, 1, 0));
    cameraEntity->setViewCenter(QVector3D(0, 0, 0));

    //Light
    Qt3DCore::QEntity *lightEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DRender::QPointLight *light = new Qt3DRender::QPointLight(lightEntity);
    light->setColor("white");
    light->setIntensity(100);
    lightEntity->addComponent(light);
    Qt3DCore::QTransform *lightTransform = new Qt3DCore::QTransform(lightEntity);
    lightTransform->setTranslation(cameraEntity->position());
    lightEntity->addComponent(lightTransform);
    //QObject::connect(cameraEntity, &Qt3DRender::QCamera::viewMatrixChanged, [&]()
    //{ lightTransform->setTranslation(cameraEntity->position()); }); //make light follow camera

    // MyCamera controller
    //Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(rootEntity);
    //camController->setLinearSpeed( 50.0f );
    //camController->setLookSpeed( 2*180.0f );
    //camController->setCamera(cameraEntity);
    OrbitalCameraController *camController = new OrbitalCameraController();
    camController->setCamera(cameraEntity);
    camController->setParent(rootEntity);

    // Material
    Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial;
    QColor material_color(0,0.5,1);
    material->setDiffuse(material_color);
    material->setAmbient(material_color);
    material->setSpecular(material_color);

    // Torus
    Qt3DCore::QEntity *torusEntity = new Qt3DCore::QEntity;
    Qt3DExtras::QTorusMesh *torusMesh = new Qt3DExtras::QTorusMesh;
    torusMesh->setRadius(5);
    torusMesh->setMinorRadius(1);
    torusMesh->setRings(100);
    torusMesh->setSlices(20);

    torusEntity->addComponent(torusMesh);
    torusEntity->addComponent(material);
    torusEntity->setParent(rootEntity);

    // Render
    Qt3DRender::QMultiSampleAntiAliasing antiAliasing(rootEntity);
    antiAliasing.setEnabled(true);

    view.renderSettings()->setRenderPolicy(Qt3DRender::QRenderSettings::OnDemand);
    //view.renderSettings()->pickingSettings()->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);
    view.setRootEntity(rootEntity);
    view.show();

    return app.exec();
}*/

#include "main.moc"