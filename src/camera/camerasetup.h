#ifndef CAMERASETUP_H
#define CAMERASETUP_H

#include <QDialog>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QLabel>
#include "gui/mainwindow.h"

class MainWindow;

class CameraSetup : public QDialog
{

    Q_OBJECT

public:
    CameraSetup(MainWindow * parent, Qt::WindowFlags f = 0);

private:
    QPushButton* createButton(const QString &text, const char *member);
    QDoubleSpinBox* createBox(double val);

private slots:
    void on_SurfNormal();
    void on_Parallel();
    void on_Center();

    void on_Ok();
    void on_Applay();
    void on_Cancel();

private:
    MainWindow * papa;

    QLabel*         p_Size;
    QLabel*         p_DCX_L;
    QDoubleSpinBox* p_DCX;
    QLabel*         p_DCY_L;
    QDoubleSpinBox* p_DCY;

    QLabel*         p_Normal;
    QLabel*         p_XN_L;
    QDoubleSpinBox* p_XN;
    QLabel*         p_YN_L;
    QDoubleSpinBox* p_YN;
    QLabel*         p_ZN_L;
    QDoubleSpinBox* p_ZN;
    QPushButton*    p_BSurfNormal;

    QLabel*         p_Center;
    QLabel*         p_XC_L;
    QDoubleSpinBox* p_XC;
    QLabel*         p_YC_L;
    QDoubleSpinBox* p_YC;
    QLabel*         p_ZC_L;
    QDoubleSpinBox* p_ZC;
    QPushButton*    p_BSurfCenter;

    QLabel*         p_Angle;
    QLabel*         p_DegL;
    QDoubleSpinBox* p_Deg;
    QPushButton*    p_BParallel;

    QPushButton*    p_Ok;
    QPushButton*    p_Applay;
    QPushButton*    p_Cancel;

};

#endif // CAMERASETUP_H
