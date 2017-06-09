#include <QHBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include "camerasetup.h"

CameraSetup::CameraSetup(MainWindow * parent, Qt::WindowFlags f):QDialog((QWidget*) parent,f)
{
    papa = parent;

    p_Size = new QLabel(tr("Size"));
    p_DCX_L = new QLabel(tr("Width:"));
    p_DCX = createBox(10);
    p_DCY_L = new QLabel(tr("Height:"));
    p_DCY = createBox(10);

    p_Normal = new QLabel(tr("Normal"));
    p_XN_L = new QLabel(tr("Xn:"));
    p_XN = createBox(0);
    p_YN_L = new QLabel(tr("Yn:"));
    p_YN = createBox(0);
    p_ZN_L = new QLabel(tr("Zn:"));
    p_ZN = createBox(0);
    p_BSurfNormal = createButton(tr("Normal  "), SLOT(on_SurfNormal()));

    p_Angle = new QLabel(tr("Angle"));
    p_DegL = new QLabel(tr("OX^OX':"));
    p_Deg = createBox(0);
    p_BParallel = createButton(tr("Parallel"), SLOT(on_Parallel()));

    p_Center = new QLabel(tr("Center"));
    p_XC_L = new QLabel(tr("Xc:"));
    p_XC = createBox(0);
    p_YC_L = new QLabel(tr("Yc:"));
    p_YC = createBox(0);
    p_ZC_L = new QLabel(tr("Zc:"));
    p_ZC = createBox(0);
    p_BSurfCenter = createButton(tr("Center"),SLOT(on_Center()));

    p_Ok = createButton(tr("Ok"),SLOT(on_Ok()));
    p_Applay = createButton(tr("Applay"),SLOT(on_Applay()));
    p_Cancel = createButton(tr("Cancel"),SLOT(on_Cancel()));

    QGridLayout *mainLayout = new QGridLayout();
    QHBoxLayout *buttonsLayout = new QHBoxLayout();

    mainLayout->addWidget(p_Size,  0,0,1,1,Qt::AlignLeft);
    mainLayout->addWidget(p_DCX_L, 0,1,1,1,Qt::AlignRight);
    mainLayout->addWidget(p_DCX,   0,2,1,1,Qt::AlignCenter);
    mainLayout->addWidget(p_DCY_L, 0,3,1,1,Qt::AlignRight);
    mainLayout->addWidget(p_DCY,   0,4,1,1,Qt::AlignCenter);

    mainLayout->addWidget(p_Normal,1,0,1,1,Qt::AlignLeft);
    mainLayout->addWidget(p_XN_L  ,1,1,1,1,Qt::AlignRight);
    mainLayout->addWidget(p_XN    ,1,2,1,1,Qt::AlignCenter);
    mainLayout->addWidget(p_YN_L  ,1,3,1,1,Qt::AlignRight);
    mainLayout->addWidget(p_YN    ,1,4,1,1,Qt::AlignCenter);
    mainLayout->addWidget(p_ZN_L  ,1,5,1,1,Qt::AlignRight);
    mainLayout->addWidget(p_ZN    ,1,6,1,1,Qt::AlignCenter);
    mainLayout->addWidget(p_BSurfNormal,1,7,1,1,Qt::AlignCenter);

    mainLayout->addWidget(p_Center,2,0,1,1,Qt::AlignLeft);
    mainLayout->addWidget(p_XC_L,  2,1,1,1,Qt::AlignRight);
    mainLayout->addWidget(p_XC,    2,2,1,1,Qt::AlignCenter);
    mainLayout->addWidget(p_YC_L,  2,3,1,1,Qt::AlignRight);
    mainLayout->addWidget(p_YC,    2,4,1,1,Qt::AlignCenter);
    mainLayout->addWidget(p_ZC_L,  2,5,1,1,Qt::AlignRight);
    mainLayout->addWidget(p_ZC,    2,6,1,1,Qt::AlignCenter);
    mainLayout->addWidget(p_BSurfCenter,2,7,1,1,Qt::AlignCenter);

    mainLayout->addWidget(p_Angle, 3,0,1,1,Qt::AlignLeft);
    mainLayout->addWidget(p_DegL,  3,1,1,1,Qt::AlignRight);
    mainLayout->addWidget(p_Deg ,  3,2,1,1,Qt::AlignCenter);
    mainLayout->addWidget(p_BParallel,3,7,1,1,Qt::AlignCenter);

    buttonsLayout->addWidget(p_Ok,0,Qt::AlignRight);
    buttonsLayout->addWidget(p_Applay,0,Qt::AlignRight);
    buttonsLayout->addWidget(p_Cancel,0,Qt::AlignRight);

    mainLayout->addLayout(buttonsLayout,4,0,1,8,Qt::AlignRight);


    setLayout(mainLayout);
}

QPushButton* CameraSetup::createButton(const QString &text, const char *member)
 {
     QPushButton *button = new QPushButton(text);
     connect(button, SIGNAL(clicked()), this, member);
     return button;
 }

QDoubleSpinBox* CameraSetup::createBox(double val)
{
    QDoubleSpinBox* box = new QDoubleSpinBox();
    box->setDecimals(5);
    box->setRange(-99999,99999);
    box->setSingleStep(0.01);
    box->setValue(val);
    return box;
}

void CameraSetup::on_SurfNormal()
{
    QMessageBox msgBox(papa);
    bool res = false;

    if(!papa) return;
    GeomObject * object = papa->doc->get_geomobject();
    if(!object) return;

    if(object->selected.count() > 3)
    {
        msgBox.setText("You can't define a plane with more than three points");
        msgBox.exec();
    }

    if(object->selected.count() == 2)
       res = object->camera.calcNorm(object->selected.at(1) - object->selected.at(0));

    if(object->selected.count() == 3)
    {
       QVector3D norm = object->camera.norm;
       object->camera.norm = QVector3D::normal(object->selected.at(0),object->selected.at(1),object->selected.at(2));
       if(QVector3D::dotProduct(norm,object->camera.norm) > 0) object->camera.norm *= -1;
       res = object->camera.calcExEy();
    }

    if(!res)
    {
       object->camera.norm = QVector3D(0,0,1);
       object->camera.calcExEy();
       msgBox.setText("Bad plane points or just a one point defined, using defaults");
       msgBox.exec();
    }

    p_XN->setValue(object->camera.norm.x()); p_XN->update();
    p_YN->setValue(object->camera.norm.y()); p_YN->update();
    p_ZN->setValue(object->camera.norm.z()); p_ZN->update();

    papa->glWidget->updateGL();
}

void CameraSetup::on_Parallel()
{
    QMessageBox msgBox(papa);

    if(!papa) return;
    GeomObject * object = papa->doc->get_geomobject();
    if(!object) return;

    if(!object->camera.calcExEy())
    {
        msgBox.setText("Define surface normal first");
        msgBox.exec();
        return;
    }

    if(object->selected.count() != 2)
    {
        msgBox.setText("You must select two points exactly");
        msgBox.exec();
        return;
    }

    QVector3D X = object->selected.at(1) - object->selected.at(0);
    double fi = 0;
    fi = object->camera.getFi(QVector3D::dotProduct(X,object->camera.Ex),
                              QVector3D::dotProduct(X,object->camera.Ey));
    object->camera.fi = fi;
    p_Deg->setValue(fi*180/3.141592654);  p_Deg->update();

    papa->glWidget->updateGL();
}

void CameraSetup::on_Center()
{
    QMessageBox msgBox(papa);

    if(!papa) return;
    GeomObject * object = papa->doc->get_geomobject();
    if(!object) return;

    if(object->selected.count() != 1)
    {
        msgBox.setText("Define a center point exactly with one selected point");
        msgBox.exec();
        return;
    }

   object->camera.center = object->selected.at(0);
   p_XC->setValue(object->camera.center.x()); p_XC->update();
   p_YC->setValue(object->camera.center.y()); p_YC->update();
   p_ZC->setValue(object->camera.center.z()); p_ZC->update();

   papa->glWidget->updateGL();
}

void CameraSetup::on_Ok()
{
    ;
}

void CameraSetup::on_Applay()
{
    if(!papa) return;
    GeomObject * object = papa->doc->get_geomobject();
    if(!object) return;

    object->camera.norm = QVector3D(p_XN->value(),p_YN->value(),p_ZN->value());
    object->camera.norm.normalize();
    p_XN->setValue(object->camera.norm.x()); p_XN->update();
    p_YN->setValue(object->camera.norm.y()); p_YN->update();
    p_ZN->setValue(object->camera.norm.z()); p_ZN->update();
    object->camera.calcExEy();


    object->camera.center = QVector3D(p_XC->value(),p_YC->value(),p_ZC->value());
    object->camera.fi = p_Deg->value()*3.141592654/180;
    object->camera.cx = p_DCX->value();
    object->camera.cy = p_DCY->value();

    papa->glWidget->updateGL();
}

void CameraSetup::on_Cancel()
{
    ;
}
