#include "camera.h"
#include "math.h"

Camera::Camera()
{
    cx = cy = 10;
    fi = 0;
    resolution = 100;
    center = QVector3D(0,0,0);
    norm   = QVector3D(0,0,1);
    Ex     = QVector3D(1,0,0);
    Ey     = QVector3D(0,1,0);
}

bool Camera::calcExEy()
{
    norm.normalize();
    if(norm.length() < 0.99 && norm.length() > 1.01 ) return false;

    double x[3];
    x[0] = norm.x();
    x[1] = norm.y();
    x[2] = norm.z();
    double min = x[0]*x[0];

    for(int i = 0; i < 3; i++)
    {
        if(min > x[i]*x[i] || i == 0)
        {
            min = x[i]*x[i];
            Ex = QVector3D( ((i != 0)? 0 : 1),
                            ((i != 1)? 0 : 1),
                            ((i != 2)? 0 : 1));
            Ex = Ex - x[i]*norm;
        }
    }

    Ex.normalize();
    if(Ex.length() < 0.99 && Ex.length() > 1.01 ) return false;

    Ey = QVector3D::crossProduct(norm,Ex);
    Ey.normalize();
    if(Ey.length() < 0.99 && Ey.length() > 1.01 ) return false;

    return true;
}

bool Camera::calcNorm(QVector3D Line)
{
    if(Line.length() <= 0) return false;
    double x[3];
    double sum = 0, max = -1;

    x[0] = Line.x(); x[1] = Line.y(); x[2] = Line.z();
    sum = x[0] + x[1] + x[2];

    for(int i = 0; i < 3; i++)
    {
        if(max < x[i]*x[i] && x[i] != 0)
        {
            max = x[i]*x[i];
            norm = QVector3D( ((i != 0)? 1 : 1 - sum/x[i]),
                              ((i != 1)? 1 : 1 - sum/x[i]),
                              ((i != 2)? 1 : 1 - sum/x[i])  );
        }
    }

    return calcExEy();
}

double Camera::getFi(double x, double y)
{
    double len = sqrt(x*x+y*y);
    if(len == 0) return 0;

    double pi = 3.141592654;
    double abs = (y >= 0) ? 1 : -1;

    return (pi + (acos(x/len) - pi)*abs);
}
