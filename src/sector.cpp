#include "sector.h"
#include "math.h"

Sector::Sector()
{
    di = 0;
    dj = 0;
    fi = 0;
    alpha = 0;
    dfi = 0;
}

void Sector::set_num(int num)
{
    int ndi[] = {1, 1, 0,-1,-1,-1, 0, 1};
    int ndj[] = {0, 1, 1, 1, 0,-1,-1,-1};
    di = ndi[num];
    dj = ndj[num];

    double pi = 3.14159265;
    double a = 0.927295, b = 0.643501;

    fi = num*(a+b)/2;
    fi = fi*180/pi;

    alpha = ceil((double)num/2) - floor((double)num/2) < 0.9? b : a;
    alpha = alpha*180/pi;
}

void Sector::set_dfi(double fi0)
{
    dfi = fabs(fi - fi0);
    dfi = dfi > 180 ? 360 - dfi : dfi;
    dfi = dfi - alpha/2;
}

double Sector::calc_fi(double x, double y)
{
    double len = sqrt(x*x+y*y);
    if(len == 0) return 0;

    double pi = 3.141592654;
    double abs = (y >= 0) ? 1 : -1;

    return (pi + (acos(x/len) - pi)*abs)*180/pi;
}

bool Sector::inrange(int nx, int ny, int a, int b)
{
    if(a+di >= nx || a + di < 0) return false;
    if(b+dj >= ny || b + dj < 0) return false;
    return true;
}
