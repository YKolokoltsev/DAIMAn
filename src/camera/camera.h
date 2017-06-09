#ifndef CAMERA_H
#define CAMERA_H

#include <QVector3D>
#include <QString>

class Camera
{ 

public:
    Camera();

public:
    double cx;        //width
    double cy;        //height

    QVector3D norm;   //Along this vector camera looks from the infinity;
    QVector3D Ex;     //This is an OX vector on the surface
    QVector3D Ey;     //This is an YX vector on the surface

    double fi;        //rotation of the camera (around norm vector)

    QVector3D center; //The center of the picture should lie on the line
                      //along norm, where this point sites
    double resolution;// Dots Per Angstrum


public:
    bool calcExEy();
    bool calcNorm(QVector3D Line);
    double getFi(double x, double y);

public:
    Camera& operator= (const Camera& cam)
    {
        if(this == &cam) return *this;
        this->cx = cam.cx;
        this->cy = cam.cy;
        this->norm = cam.norm;
        this->Ex = cam.Ex;
        this->Ey = cam.Ey;
        this->fi = cam.fi;
        this->center = cam.center;
        this->resolution = cam.resolution;
        return *this;
    }

};

#endif // CAMERA_H
