#include "volumetricelfthread.h"
#include <math.h>

VolumetricElfThread::VolumetricElfThread(Document* doc, double* result)
{
    this->doc = doc;
    //store pointer
    p = doc->get_wfn()->p;
    this->result = result;

    nz = NULL;
    atdist2 = NULL;
    d = NULL;
    dx = NULL;
    dy = NULL;
    dz = NULL;


    initializeHelpers();
}

//MAIN THREAD=============================================
void VolumetricElfThread::run(){
    for(int i = 0; i < numPoints; i++){
        result[4*i+3] = calc_elf(result[4*i+0],
                                 result[4*i+1],
                                 result[4*i+2]);
    }
}

//ELF CALCULATION=========================================
VolumetricElfThread::~VolumetricElfThread(){
    if(nz != NULL) delete[] nz, nz = NULL;
    if(atdist2 != NULL) delete[] atdist2, atdist2 = NULL;
    if (d    != NULL) delete [] d, d = NULL;
    if (dx   != NULL) delete [] dx, dx = NULL;
    if (dy   != NULL) delete [] dy, dy = NULL;
    if (dz   != NULL) delete [] dz, dz = NULL;
}

void VolumetricElfThread::initializeHelpers(){
    if(nz != NULL) delete[] nz;
    nz = new int [doc->get_wfn()->nprim + 1];

    if(atdist2 != NULL) delete[] atdist2;
    atdist2 = new double[doc->get_wfn()->nat];

    if(d != NULL) delete[] d;
    d = new double [doc->get_wfn()->nprim];

    if(dx != NULL) delete[] dx;
    dx = new double [doc->get_wfn()->nprim];

    if(dy != NULL) delete[] dy;
    dy = new double [doc->get_wfn()->nprim];

    if(dz != NULL) delete[] dz;
    dz = new double [doc->get_wfn()->nprim];
}



double VolumetricElfThread::calc_elf(double x, double y, double z)
{
    double gradx = 0, grady = 0, gradz = 0, gradf = 0;
    double rh = 0;
    int munu = 0;
    int j;

    set_nz(x,y,z);

    for(int i = 0; i < nz[0]; i++)
    {
        j = i+1;
         d[i] =  doc->get_wfn()->f(0,nz[j],x)* doc->get_wfn()->f(1,nz[j],y)* doc->get_wfn()->f(2,nz[j],z);
        dx[i] =  doc->get_wfn()->df(0,nz[j],x)* doc->get_wfn()->f(1,nz[j],y)* doc->get_wfn()->f(2,nz[j],z);
        dy[i] =  doc->get_wfn()->f(0,nz[j],x)*doc->get_wfn()->df(1,nz[j],y)* doc->get_wfn()->f(2,nz[j],z);
        dz[i] =  doc->get_wfn()->f(0,nz[j],x)* doc->get_wfn()->f(1,nz[j],y)* doc->get_wfn()->df(2,nz[j],z);
    }

    for(int mu = 0; mu < nz[0]; mu++)
        for(int nu = mu; nu < nz[0]; nu++)
        {
            munu = (2*doc->get_wfn()->nprim - nz[mu+1] + 1)*nz[mu+1]/2 + nz[nu+1] - nz[mu+1];

            rh += p[munu]*d[mu]*d[nu];

            gradx += p[munu]*(d[mu]*dx[nu]+d[nu]*dx[mu]);
            grady += p[munu]*(d[mu]*dy[nu]+d[nu]*dy[mu]);
            gradz += p[munu]*(d[mu]*dz[nu]+d[nu]*dz[mu]);
            gradf += p[munu]*(dx[mu]*dx[nu]+dy[mu]*dy[nu]+dz[mu]*dz[nu]);

        }

    if(rh == 0) return 0;

    double fermi = 0.121300565;
    double weiszacker = (gradx*gradx + grady*grady + gradz*gradz)/rh;
    double ratio = 0.5*gradf - 0.125*weiszacker + 2.871*pow(10,-5);
           ratio = ratio*ratio*fermi*pow(rh,-10/3);

           return 1/(1+ratio);
}

void VolumetricElfThread::set_nz(double x, double y, double z)
{
    if(nz == NULL) return;

    int i,j = 1;

    for(i = 0; i < doc->get_wfn()->nat; i++)
        atdist2[i] = pow(doc->get_wfn()->atcoords[i][0] - x,2) +
                     pow(doc->get_wfn()->atcoords[i][1] - y,2) +
                     pow(doc->get_wfn()->atcoords[i][2] - z,2);

    for(i = 0; i < doc->get_wfn()->nprim; i++)
        if(atdist2[doc->get_wfn()->icent[i]] < doc->get_wfn()->ts[doc->get_wfn()->itype[i]-1]/doc->get_wfn()->alpha[i])
            nz[j] = i, j++;

    nz[0] = j-1;
}
