#include "wfndata.h"
#include "string.h"
#include "stdio.h"
#include "math.h"

WFNData::WFNData(){
}

WFNData::~WFNData(){
    clear();
}

void WFNData::clear(){
    int i = 0;

    if(title != NULL)
    {
        delete [] title;
        title = NULL;
    }

    if(keywords != NULL)
    {
        delete [] keywords;
        keywords = NULL;
    }

    if(atnames != NULL)
    {
        for(i = 0; i < nat; i++)
            if(atnames[i] != NULL)
                delete [] atnames[i];
        delete [] atnames;
        atnames = NULL;
    }

    if(atnumbers != NULL)
    {
        delete [] atnumbers;
        atnumbers = NULL;
    }

    if(atcharges != NULL)
    {
        delete [] atcharges;
        atcharges = NULL;
    }

    if(atcoords != NULL)
    {
        for(i = 0; i < nat; i++)
            if(atcoords[i] != NULL)
                delete [] atcoords[i];

        delete [] atcoords;
        atcoords = NULL;

    }

    if(model != NULL)
    {
        delete [] model;
        model = NULL;
    }

    if(icent != NULL)
    {
        delete [] icent;
        icent = NULL;
    }

    if(itype != NULL)
    {
        delete [] itype;
        itype = NULL;
    }

    if(alpha != NULL)
    {
        delete [] alpha;
        alpha = NULL;
    }

    if(occup != NULL)
    {
        delete [] occup;
        occup = NULL;
    }

    if(orben != NULL)
    {
        delete [] orben;
        orben = NULL;
    }

    if(orspin != NULL)
    {
        delete [] orspin;
        orspin = NULL;
    }

    if(c != NULL)
    {
        for(i = 0; i < this->norb; i++)
            if(c[i] != NULL)
                delete [] c[i];
        delete [] c;
        c = NULL;
    }

    if(p != NULL)
    {
        delete [] p;
        p = NULL;
    }

    if(ir != NULL)
    {
        delete [] ir;
        ir = NULL;
    }

    if(d != NULL)
    {
        delete [] d;
        d = NULL;
    }

    if(dx != NULL)
    {
        delete [] dx;
        dx = NULL;
    }

    if(dy != NULL)
    {
        delete [] dy;
        dy = NULL;
    }

    if(dz != NULL)
    {
        delete [] dz;
        dz = NULL;
    }

    if(d2x != NULL)
    {
        delete [] d2x;
        d2x = NULL;
    }

    if(d2y != NULL)
    {
        delete [] d2y;
        d2y = NULL;
    }

    if(d2z != NULL)
    {
        delete [] d2z;
        d2z = NULL;
    }

    if(d2xy != NULL)
    {
        delete [] d2xy;
        d2xy = NULL;
    }

    if(d2xz != NULL)
    {
        delete [] d2xz;
        d2xz = NULL;
    }

    if(d2yz != NULL)
    {
        delete [] d2yz;
        d2yz = NULL;
    }

    if(ts != NULL)
    {
        delete [] ts;
        ts = NULL;
    }

    if(nz != NULL)
    {
        delete [] nz;
        nz = NULL;
    }


    nat = 0;
    nprim = 0;
    norb = 0;
    npert = 0;
    netcharge = 0;
    nelectrons = 0;
    nalpha = 0;
    nbeta = 0;
    multi = 0;
    etotal = 0;
    rvirial = 0;
}

double WFNData::getMinMaxXYZ(int coord, bool min)
{
    if(atcoords == NULL || nat == 0) return 0;

    double answer = atcoords[0][coord];

    for(int i = 0; i < nat; i++)
    {
        if(min)
            if(atcoords[i][coord] < answer)
                answer = atcoords[i][coord];
        if(!min)
            if(atcoords[i][coord] > answer)
                answer = atcoords[i][coord];
    }

    return answer;
}

double WFNData::getMidXYZ(int coord)
{
    if(atcoords == NULL || nat == 0) return 0;

    double answer = 0;

    for(int i = 0; i < nat; i++)
        answer += atcoords[i][coord];

    answer = answer/nat;

    return answer;
}

bool WFNData::calc_helpers()
{
    int i;

    if (nprim <= 0) return false;
    if (norb <= 0) return false;
    if (orspin == NULL ) return false;
    if (itype == NULL) return false;
    if (c == NULL) return false;
    if (alpha == NULL) return false;
    if (nat <= 0) return false;

    if (p    != NULL) delete [] p;
    if (ir   != NULL) delete [] ir;
    if (d    != NULL) delete [] d;
    if (dx   != NULL) delete [] dx;
    if (dy   != NULL) delete [] dy;
    if (dz   != NULL) delete [] dz;
    if (d2x  != NULL) delete [] d2x;
    if (d2y  != NULL) delete [] d2y;
    if (d2z  != NULL) delete [] d2z;
    if (d2xy != NULL) delete [] d2xy;
    if (d2xz != NULL) delete [] d2xz;
    if (d2yz != NULL) delete [] d2yz;
    if (ts   != NULL) delete [] ts;
    if (nz   != NULL) delete [] nz;


    p = new double [(int)ceil(nprim*(nprim+1)/2)];
    if(p == NULL) return false;

    nz = new int [nprim + 1];
    if(nz == NULL) return false;

    ir = new int [3*nprim];
    if(ir == NULL) return false;

    d = new double [nprim];
    if(d == NULL) return false;

    dx = new double [nprim];
    if(dx == NULL) return false;

    dy = new double [nprim];
    if(dy == NULL) return false;

    dz = new double [nprim];
    if(dz == NULL) return false;

    d2x = new double [nprim];
    if(d2x == NULL) return false;

    d2y = new double [nprim];
    if(d2y == NULL) return false;

    d2z = new double [nprim];
    if(d2z == NULL) return false;

    d2xy = new double [nprim];
    if(d2xy == NULL) return false;

    d2xz = new double [nprim];
    if(d2xz == NULL) return false;

    d2yz = new double [nprim];
    if(d2yz == NULL) return false;

    ts = new double [20];
    if(ts == NULL) return false;


    for(i = 0; i < 20; i++)
    {
        if(i == 0)          ts[i] =  6.25;
        if(i > 0 && i < 4)  ts[i] =  9.12;
        if(i > 3 && i < 10) ts[i] = 12.18;
        if(i > 9 && i < 20) ts[i] = 13.76;
    }


    double occ[3]; occ[0] = 1; occ[1] = 1; occ[2] = 2;
    double pmatrix = 0;
    int munu = 0;
    int ix[] = {0, 1,0,0, 2,0,0,1,1,0, 3,0,0,2,2,0,1,1,0,1}; //[20]
    int iy[] = {0, 0,1,0, 0,2,0,1,0,1, 0,3,0,1,0,2,2,0,1,1};
    int iz[] = {0, 0,0,1, 0,0,2,0,1,1, 0,0,3,0,1,1,0,2,2,1};

    for(int mu = 0; mu < nprim; mu++)
    {
        ir[mu]         = ix[itype[mu]-1];// itype starts from 1, but in c++ arrays start from 0
        ir[nprim+mu]   = iy[itype[mu]-1];
        ir[2*nprim+mu] = iz[itype[mu]-1];

        for(int nu = mu; nu < nprim; nu++)
        {
            pmatrix = 0;
            for(i = 0; i < norb; i++)
                pmatrix = pmatrix + occ[orspin[i]-1]*c[i][mu]*c[i][nu];
            if(mu != nu)
                pmatrix = 2*pmatrix;
            p[munu] = pmatrix;

            munu++;
        }
    }

    return true;
}

//returns the cartesian GTO value for the given primitive "mu" at point "val" for coordinate "icoord"
double  WFNData::f(int icoord, int mu, double val)
{
    double xa = val - atcoords[icent[mu]][icoord];
    double xa2 = xa*xa;
    double arg = -alpha[mu]*xa2;
    double expo = exp(arg);
    int    type = ir[icoord*nprim+mu];

    if(type == 3)
      return xa*xa2*expo;

    if(type == 2)
      return xa2*expo;

    if(type == 1)
      return xa*expo;

    if(type == 0)
      return expo;

    return 0;
}

double WFNData::intinf(int icoord, int mu, int nu)
{
    double x1 = atcoords[icent[mu]][icoord];
    double x2 = atcoords[icent[nu]][icoord];
    double alfa = alpha[mu]+alpha[nu];
    double c = (alpha[mu]*x1+alpha[nu]*x2)/alfa;
    double mult = exp(-alpha[mu]*alpha[nu]*pow(x2-x1,2)/alfa);
    int a = ir[icoord*nprim+mu];
    int b = ir[icoord*nprim+nu];

    if(b < a)
    {
        a = b;
        b = ir[icoord*nprim+mu];
        x1 = x2;
        x2 = atcoords[icent[mu]][icoord];
    }

    double ret = 0;
    double pi = 3.14159265359;

    if(a == 0 && b == 0)
    ret = pow(pi/alfa,0.5);

    if(a == 0 && b == 1)
    ret = pow(pi/alfa,0.5)*(c-x2);

    if(a == 0 && b == 2)
    ret = pow(pi/(4*pow(alfa,3)),0.5)*(1+2*alfa*pow(c-x2,2));

    if(a == 0 && b == 3)
    ret = pow(pi/(4*pow(alfa,3)),0.5)*(3+2*alfa*pow(c-x2,2))*(c-x2);

    if(a == 1 && b == 1)
    ret = pow(pi/(4*pow(alfa,3)),0.5)*(1+2*alfa*(c-x1)*(c-x2));

    if(a == 1 && b == 2)
    ret = pow(pi/(4*pow(alfa,3)),0.5)*(3*c-x1-2*x2+2*alfa*(c-x1)*pow(c-x2,2));

    if(a == 1 && b == 3)
    ret = pow(pi/(16*pow(alfa,5)),0.5)*(3+6*alfa*(c-x2)*(2*c-x1-x2)+4*pow(alfa,2)*(c-x1)*pow(c-x2,3));

    if(a == 2 && b == 2)
    ret = pow(pi/(16*pow(alfa,5)),0.5)*(3+2*alfa*(6*c*c+x1*x1+4*x1*x2+x2*x2-6*c*(x1+x2))+
                                          4*pow(alfa*(c-x1)*(c-x2),2));

    if(a == 2 && b == 3)
    ret = pow(pi/(16*pow(alfa,5)),0.5)*(15*c-6*x1-9*x2+4*pow(alfa*(c-x1),2)*pow(c-x2,3)+
            2*alfa*(c-x2)*(10*c*c+3*x1*x1+6*x1*x2+x2*x2-4*c*(3*x1+2*x2)));

    if(a == 3 && b == 3)
    ret = pow(pi/(64*pow(alfa,7)),0.5)*(15+18*alfa*(5*c*c+x1*x1+3*x1*x2+x2*x2-5*c*(x1+x2))+
                                        12*alfa*alfa*(c-x1)*(c-x2)*(5*c*c+x1*x1+3*x1*x2+x2*x2-5*c*(x1+x2))+
                                        pow(2*alfa*(c-x1)*(c-x2),3));

    return ret*mult;
}

double WFNData::normorb(int orb)
{
    double ret = 0;
    double ints = 0;
    double occ[3]; occ[0] = 1; occ[1] = 1; occ[2] = 2;

    for(int mu = 0; mu < nprim; mu++)
        for(int nu = mu; nu < nprim; nu++)
        {
            ints = 2*occ[orspin[orb]-1]*c[orb][mu]*c[orb][nu]*intinf(0,mu,nu)*intinf(1,mu,nu)*intinf(2,mu,nu);
            if(mu == nu) ints = ints/2;
            ret += ints;
        }

    return ret;
}

double WFNData::df(int icoord, int mu, double val)
{
    double xa = val - atcoords[icent[mu]][icoord];
    double xa2 = xa*xa;
    double arg = -alpha[mu]*xa2;
    double expo = exp(arg);
    int    type = ir[icoord*nprim+mu];

    if(type == 0)
      return -2*xa*alpha[mu]*expo;

    if(type == 1)
      return (1+2*arg)*expo;

    if(type == 2)
      return 2*(arg+1)*xa*expo;

    if(type == 3)
      return (3+2*arg)*xa2*expo;

    return 0;
}

double WFNData::d2f(int icoord, int mu, double val)
{
    double xa = val - atcoords[icent[mu]][icoord];
    double xa2 = xa*xa;
    double arg = -alpha[mu]*xa2;
    double expo = exp(arg);
    int    type = ir[icoord*nprim+mu];

    if(type == 0)
      return -alpha[mu]*(4*arg+2)*expo;

    if(type == 1)
      return -xa*alpha[mu]*(4*arg+6)*expo;

    if(type == 2)
      return (2+arg*(10+4*arg))*expo;

    if(type == 3)
      return xa*(6 + arg*(14 + 4*arg))*expo;

    return 0;
}


void WFNData::set_nz(double x, double y, double z)
{
    if(nz == NULL) return;

    int i,j = 1;
    double * atdist2 = new double[nat];

    for(i = 0; i < nat; i++)
        atdist2[i] = pow(atcoords[i][0] - x,2) +
                     pow(atcoords[i][1] - y,2) +
                     pow(atcoords[i][2] - z,2);

    for(i = 0; i < nprim; i++)
        if(atdist2[icent[i]] < ts[itype[i]-1]/alpha[i])
            nz[j] = i, j++;

    nz[0] = j-1;
    delete [] atdist2;
}
//Calculating of rho without cutoffs
/*double WFNData::calc_rho(double x,double y,double z)
{
    double rh = 0;
    int munu = 0;

    for(int i = 0; i < nprim; i++)
         d[i] =  f(0,i,x)* f(1,i,y)* f(2,i,z);

    for(int mu = 0; mu < nprim; mu++)
        for(int nu = mu; nu < nprim; nu++)
        {
            rh += p[munu]*d[mu]*d[nu];
            munu++;
        }

    return rh;
}*/

//This Function calculates elf without taking in account any cutoffs
/*double WFNData::calc_elf(double x, double y, double z)
{
    double gradx = 0, grady = 0, gradz = 0, gradf = 0;
    double rh = 0;
    int munu = 0;

    for(int i = 0; i < nprim; i++)
    {
         d[i] =  f(0,i,x)* f(1,i,y)* f(2,i,z);
        dx[i] = df(0,i,x)* f(1,i,y)* f(2,i,z);
        dy[i] =  f(0,i,x)*df(1,i,y)* f(2,i,z);
        dz[i] =  f(0,i,x)* f(1,i,y)*df(2,i,z);
    }

    for(int mu = 0; mu < nprim; mu++)
        for(int nu = mu; nu < nprim; nu++)
        {
            rh += p[munu]*d[mu]*d[nu];

            gradx += p[munu]*(d[mu]*dx[nu]+d[nu]*dx[mu]);
            grady += p[munu]*(d[mu]*dy[nu]+d[nu]*dy[mu]);
            gradz += p[munu]*(d[mu]*dz[nu]+d[nu]*dz[mu]);
            gradf += p[munu]*(dx[mu]*dx[nu]+dy[mu]*dy[nu]+dz[mu]*dz[nu]);
            munu++;
        }

    if(rh == 0) return 0;

    double fermi = 0.121300565;
    double weiszacker = (gradx*gradx + grady*grady + gradz*gradz)/rh;
    double ratio = 0.5*gradf - 0.125*weiszacker + 2.871*pow(10,-5);
           ratio = ratio*ratio*fermi*pow(rh,-10/3);

           return 1/(1+ratio);
}*/

//using cutoffs
double WFNData::calc_rho(double x,double y,double z)
{
    double rh = 0;// 1+cos(x)*cos(y)*cos(z);
    int munu = 0;

    set_nz(x,y,z);

    for(int i = 0; i < nz[0]; i++)
         d[i] =  f(0,nz[i+1],x)* f(1,nz[i+1],y)* f(2,nz[i+1],z);

    for(int mu = 0; mu < nz[0]; mu++)
        for(int nu = mu; nu < nz[0]; nu++)
        {
            munu = (2*nprim - nz[mu+1] + 1)*nz[mu+1]/2 + nz[nu+1] - nz[mu+1];
            rh += p[munu]*d[mu]*d[nu];
        }

    return rh;
}

//using cutoffs
double WFNData::calc_grad_rho(double x, double y, double z, double* grad, bool calc_rho)
{
    if(!grad) return 0;
    memset(grad,0,3*sizeof(double));

    int munu = 0;
    double rh = 0;
    double fx, fy, fz;

    set_nz(x,y,z);

    for(int i = 0; i < nz[0]; i++)
    {
        fx = f(0,nz[i+1],x);
        fy = f(1,nz[i+1],y);
        fz = f(2,nz[i+1],z);

         d[i] =  fx       *fy       *fz;

        dx[i] =  df(0,nz[i+1],x)*fy       *fz;
        dy[i] =  fx       *df(1,nz[i+1],y)*fz;
        dz[i] =  fx       *fy       *df(2,nz[i+1],z);
    }

    for(int mu = 0; mu < nz[0]; mu++)
        for(int nu = mu; nu < nz[0]; nu++)
        {
            munu = (2*nprim - nz[mu+1] + 1)*nz[mu+1]/2 + nz[nu+1] - nz[mu+1];

            if(calc_rho){
                rh += p[munu]*d[mu]*d[nu];
            }

            grad[0] += p[munu]*(d[mu]*dx[nu]+d[nu]*dx[mu]);
            grad[1] += p[munu]*(d[mu]*dy[nu]+d[nu]*dy[mu]);
            grad[2] += p[munu]*(d[mu]*dz[nu]+d[nu]*dz[mu]);
        }

    return rh;
}

//using cutoffs
double WFNData::calc_hessian_rho(double x, double y, double z, bool cutoff, double* hes, double* grad, bool calc_rho){
    if(!hes) return 0;
    memset(hes,0,9*sizeof(double));

    if(grad){
        memset(grad,0,3*sizeof(double));
    }

    double rh = 0;
    int munu = 0;
    double d2rdx2  = 0, d2rdy2  = 0, d2rdz2  = 0; // second derivatives of rho
    double d2rdxdy = 0, d2rdxdz = 0, d2rdydz = 0; // mixed second derivatives of rho

    double fx, fy, fz;
    double dfx, dfy, dfz;

    if(cutoff){
        set_nz(x,y,z);
    }else{
        for(int i = 0; i < nprim; i++)
            nz[i+1] = i;
        nz[0] = nprim;
    }

    for(int i = 0; i < nz[0]; i++)
    {
        fx = f(0,nz[i+1],x);
        fy = f(1,nz[i+1],y);
        fz = f(2,nz[i+1],z);

        dfx = df(0,nz[i+1],x);
        dfy = df(1,nz[i+1],y);
        dfz = df(2,nz[i+1],z);

        d[i]    =  fx*fy*fz;

        dx[i] =  dfx*fy *fz;
        dy[i] =  fx *dfy*fz;
        dz[i] =  fx *fy *dfz;

        d2x[i]  = d2f(0,nz[i+1],x)*fy        *fz;
        d2y[i]  = fx        *d2f(1,nz[i+1],y)*fz;
        d2z[i]  = fx        *fy        *d2f(2,nz[i+1],z);

        d2xy[i] =  dfx * dfy * fz;
        d2xz[i] =  dfx *  fy * dfz;
        d2yz[i] =  fx  * dfy * dfz;
    }

    for(int mu = 0; mu < nz[0]; mu++)
        for(int nu = mu; nu < nz[0]; nu++)
        {
            munu = (2*nprim - nz[mu+1] + 1)*nz[mu+1]/2 + nz[nu+1] - nz[mu+1];

            d2rdx2 += p[munu]*(d2x[mu]*d[nu]+2*dx[mu]*dx[nu]+d2x[nu]*d[mu]);
            d2rdy2 += p[munu]*(d2y[mu]*d[nu]+2*dy[mu]*dy[nu]+d2y[nu]*d[mu]);
            d2rdz2 += p[munu]*(d2z[mu]*d[nu]+2*dz[mu]*dz[nu]+d2z[nu]*d[mu]);

            d2rdxdy += p[munu]*(d2xy[mu]*d[nu]+dy[mu]*dx[nu]+dy[nu]*dx[mu]+d2xy[nu]*d[mu]);
            d2rdxdz += p[munu]*(d2xz[mu]*d[nu]+dz[mu]*dx[nu]+dz[nu]*dx[mu]+d2xz[nu]*d[mu]);
            d2rdydz += p[munu]*(d2yz[mu]*d[nu]+dz[mu]*dy[nu]+dz[nu]*dy[mu]+d2yz[nu]*d[mu]);

            if(calc_rho){
                rh += p[munu]*d[mu]*d[nu];
            }

            if(grad){
                grad[0] += p[munu]*(d[mu]*dx[nu]+d[nu]*dx[mu]);
                grad[1] += p[munu]*(d[mu]*dy[nu]+d[nu]*dy[mu]);
                grad[2] += p[munu]*(d[mu]*dz[nu]+d[nu]*dz[mu]);
            }
        }

    hes[0] = d2rdx2;  hes[1] = d2rdxdy; hes[2] = d2rdxdz;
    hes[3] = d2rdxdy; hes[4] = d2rdy2;  hes[5] = d2rdydz;
    hes[6] = d2rdxdz; hes[7] = d2rdydz; hes[8] = d2rdz2;

    return rh;
}

//without taking into account any cutoffs
/*double WFNData::calc_grad_rho(double x, double y, double z, double* grad, bool calc_rho)
{
    if(!grad) return 0;
    memset(grad,0,3*sizeof(double));

    int munu = 0;
    double rh = 0;
    double fx, fy, fz;

    for(int i = 0; i < nprim; i++)
    {
        fx = f(0,i,x);
        fy = f(1,i,y);
        fz = f(2,i,z);


         d[i] =  fx       *fy       *fz;

        dx[i] =  df(0,i,x)*fy       *fz;
        dy[i] =  fx       *df(1,i,y)*fz;
        dz[i] =  fx       *fy       *df(2,i,z);
    }

    for(int mu = 0; mu < nprim; mu++)
        for(int nu = mu; nu < nprim; nu++)
        {
            if(calc_rho){
                rh += p[munu]*d[mu]*d[nu];
            }

            grad[0] += p[munu]*(d[mu]*dx[nu]+d[nu]*dx[mu]);
            grad[1] += p[munu]*(d[mu]*dy[nu]+d[nu]*dy[mu]);
            grad[2] += p[munu]*(d[mu]*dz[nu]+d[nu]*dz[mu]);
            munu++;
        }

    return rh;
}*/

//without taking into account any cutoffs
/*double WFNData::calc_hessian_rho(double x, double y, double z, double* hes, double* grad, bool calc_rho){
    if(!hes) return 0;
    memset(hes,0,9*sizeof(double));

    if(grad){
        memset(grad,0,3*sizeof(double));
    }

    double rh = 0;
    int munu = 0;
    double d2rdx2  = 0, d2rdy2  = 0, d2rdz2  = 0; // second derivatives of rho
    double d2rdxdy = 0, d2rdxdz = 0, d2rdydz = 0; // mixed second derivatives of rho

    double fx, fy, fz;
    double dfx, dfy, dfz;

    for(int i = 0; i < nprim; i++)
    {
        fx = f(0,i,x);
        fy = f(1,i,y);
        fz = f(2,i,z);

        dfx = df(0,i,x);
        dfy = df(1,i,y);
        dfz = df(2,i,z);

        d[i]    =  fx*fy*fz;

        dx[i] =  dfx*fy *fz;
        dy[i] =  fx *dfy*fz;
        dz[i] =  fx *fy *dfz;

        d2x[i]  = d2f(0,i,x)*fy        *fz;
        d2y[i]  = fx        *d2f(1,i,y)*fz;
        d2z[i]  = fx        *fy        *d2f(2,i,z);

        d2xy[i] =  dfx * dfy * fz;
        d2xz[i] =  dfx *  fy * dfz;
        d2yz[i] =  fx  * dfy * dfz;
    }

    for(int mu = 0; mu < nprim; mu++)
        for(int nu = mu; nu < nprim; nu++)
        {

            d2rdx2 += p[munu]*(d2x[mu]*d[nu]+2*dx[mu]*dx[nu]+d2x[nu]*d[mu]);
            d2rdy2 += p[munu]*(d2y[mu]*d[nu]+2*dy[mu]*dy[nu]+d2y[nu]*d[mu]);
            d2rdz2 += p[munu]*(d2z[mu]*d[nu]+2*dz[mu]*dz[nu]+d2z[nu]*d[mu]);

            d2rdxdy += p[munu]*(d2xy[mu]*d[nu]+dy[mu]*dx[nu]+dy[nu]*dx[mu]+d2xy[nu]*d[mu]);
            d2rdxdz += p[munu]*(d2xz[mu]*d[nu]+dz[mu]*dx[nu]+dz[nu]*dx[mu]+d2xz[nu]*d[mu]);
            d2rdydz += p[munu]*(d2yz[mu]*d[nu]+dz[mu]*dy[nu]+dz[nu]*dy[mu]+d2yz[nu]*d[mu]);

            if(calc_rho){
                rh += p[munu]*d[mu]*d[nu];
            }

            if(grad){
                grad[0] += p[munu]*(d[mu]*dx[nu]+d[nu]*dx[mu]);
                grad[1] += p[munu]*(d[mu]*dy[nu]+d[nu]*dy[mu]);
                grad[2] += p[munu]*(d[mu]*dz[nu]+d[nu]*dz[mu]);
            }

            munu++;
        }

    hes[0] = d2rdx2;  hes[1] = d2rdxdy; hes[2] = d2rdxdz;
    hes[3] = d2rdxdy; hes[4] = d2rdy2;  hes[5] = d2rdydz;
    hes[6] = d2rdxdz; hes[7] = d2rdydz; hes[8] = d2rdz2;

    return rh;
}*/

double WFNData::calc_elf(double x, double y, double z)
{
    double gradx = 0, grady = 0, gradz = 0, gradf = 0;
    double rh = 0;
    int munu = 0;
    int j;

    set_nz(x,y,z);

    for(int i = 0; i < nz[0]; i++)
    {
        j = i+1; //???
         d[i] =  f(0,nz[j],x)* f(1,nz[j],y)* f(2,nz[j],z);
        dx[i] = df(0,nz[j],x)* f(1,nz[j],y)* f(2,nz[j],z);
        dy[i] =  f(0,nz[j],x)*df(1,nz[j],y)* f(2,nz[j],z);
        dz[i] =  f(0,nz[j],x)* f(1,nz[j],y)*df(2,nz[j],z);
    }

    for(int mu = 0; mu < nz[0]; mu++)
        for(int nu = mu; nu < nz[0]; nu++)
        {
            munu = (2*nprim - nz[mu+1] + 1)*nz[mu+1]/2 + nz[nu+1] - nz[mu+1];

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

double WFNData::calc_grad_elf(double x, double y, double z, double* gx, double* gy, double* gz)
{
    double gradx   = 0, grady   = 0, gradz   = 0; // grad(rho) components
    double gradf = 0;                             // sum[ grad(psi_i)*grad(psi_i) ]
    double d2rdx2  = 0, d2rdy2  = 0, d2rdz2  = 0; // second derivatives of rho
    double d2rdxdy = 0, d2rdxdz = 0, d2rdydz = 0; // mixed second derivatives of rho
    double dkdx    = 0, dkdy    = 0, dkdz    = 0; // proportional to the grdient of kinetic energy density: grad( sum( grad(psi_i)^2 ) )
    double rh = 0;
    int munu = 0;
    int j;

    set_nz(x,y,z);

    for(int i = 0; i < nz[0]; i++)
    {
        j = i+1;
          d[i]  =   f(0,nz[j],x)*  f(1,nz[j],y)*  f(2,nz[j],z);
         dx[i]  =  df(0,nz[j],x)*  f(1,nz[j],y)*  f(2,nz[j],z);
         dy[i]  =   f(0,nz[j],x)* df(1,nz[j],y)*  f(2,nz[j],z);
         dz[i]  =   f(0,nz[j],x)*  f(1,nz[j],y)* df(2,nz[j],z);
        d2x[i]  = d2f(0,nz[j],x)*  f(1,nz[j],y)*  f(2,nz[j],z);
        d2y[i]  =   f(0,nz[j],x)*d2f(1,nz[j],y)*  f(2,nz[j],z);
        d2z[i]  =   f(0,nz[j],x)*  f(1,nz[j],y)*d2f(2,nz[j],z);
        d2xy[i] =  df(0,nz[j],x)* df(1,nz[j],y)*  f(2,nz[j],z);
        d2xz[i] =  df(0,nz[j],x)*  f(1,nz[j],y)* df(2,nz[j],z);
        d2yz[i] =   f(0,nz[j],x)* df(1,nz[j],y)* df(2,nz[j],z);
    }

    for(int mu = 0; mu < nz[0]; mu++)
        for(int nu = mu; nu < nz[0]; nu++)
        {
            munu = (2*nprim - nz[mu+1] + 1)*nz[mu+1]/2 + nz[nu+1] - nz[mu+1];

            rh += p[munu]*d[mu]*d[nu];

            gradx += p[munu]*(d[mu]*dx[nu]+d[nu]*dx[mu]);
            grady += p[munu]*(d[mu]*dy[nu]+d[nu]*dy[mu]);
            gradz += p[munu]*(d[mu]*dz[nu]+d[nu]*dz[mu]);

            gradf += p[munu]*(dx[mu]*dx[nu]+dy[mu]*dy[nu]+dz[mu]*dz[nu]);

            d2rdx2 += p[munu]*(d2x[mu]*d[nu]+2*dx[mu]*dx[nu]+d2x[nu]*d[mu]);
            d2rdy2 += p[munu]*(d2y[mu]*d[nu]+2*dy[mu]*dy[nu]+d2y[nu]*d[mu]);
            d2rdz2 += p[munu]*(d2z[mu]*d[nu]+2*dz[mu]*dz[nu]+d2z[nu]*d[mu]);

            d2rdxdy += p[munu]*(d2xy[mu]*d[nu]+dy[mu]*dx[nu]+dy[nu]*dx[mu]+d2xy[nu]*d[mu]);
            d2rdxdz += p[munu]*(d2xz[mu]*d[nu]+dz[mu]*dx[nu]+dz[nu]*dx[mu]+d2xz[nu]*d[mu]);
            d2rdydz += p[munu]*(d2yz[mu]*d[nu]+dz[mu]*dy[nu]+dz[nu]*dy[mu]+d2yz[nu]*d[mu]);

            dkdx += p[munu]*(d2x[mu]*dx[nu]+dx[mu]*d2x[nu]+d2xy[mu]*dy[nu]+dy[mu]*d2xy[nu]+d2xz[mu]*dz[nu]+dz[mu]*d2xz[nu]);
            dkdy += p[munu]*(d2xy[mu]*dx[nu]+dx[mu]*d2xy[nu]+d2y[mu]*dy[nu]+dy[mu]*d2y[nu]+d2yz[mu]*dz[nu]+dz[mu]*d2yz[nu]);
            dkdz += p[munu]*(d2xz[mu]*dx[nu]+dx[mu]*d2xz[nu]+d2yz[mu]*dy[nu]+dy[mu]*d2yz[nu]+d2z[mu]*dz[nu]+dz[mu]*d2z[nu]);

        }

    if(rh == 0) return 0;

    double fermi = 0.121300565;
    double weiszacker = (gradx*gradx + grady*grady + gradz*gradz)/rh;
    double rr    = 0.5*gradf - 0.125*weiszacker + 2.871*pow(10,-5);
    double ratio = rr*rr*fermi*pow(rh,-10/3);

    double mult = -rr*fermi*pow(rh,-13/3)*pow(1+ratio,-2);

    *gx = mult*(rh*dkdx - 0.5*(gradx*d2rdx2  + grady*d2rdxdy + gradz*d2rdxdz) + 0.25*weiszacker*gradx - 10*rr*gradx/3);
    *gy = mult*(rh*dkdy - 0.5*(gradx*d2rdxdy + grady*d2rdy2  + gradz*d2rdydz) + 0.25*weiszacker*grady - 10*rr*grady/3);
    *gz = mult*(rh*dkdz - 0.5*(gradx*d2rdxdz + grady*d2rdydz + gradz*d2rdz2 ) + 0.25*weiszacker*gradz - 10*rr*gradz/3);

    return 1/(1+ratio);
}

bool WFNData::getMolBox(Volumetrics* p_box){

    //This function estimates the box of a molecule by orbital cutoffs
    //The box is not optimal, and should be rotated to reach the minimal volume

    if(nat <= 0 || p_box == NULL) return false;

    double *atdist2 = new double[nat];
    memset(atdist2,0,nat*sizeof(double));

    for(int i = 0; i < nprim; i++)
        if(atdist2[icent[i]] < ts[itype[i]-1]/(9*alpha[i])) //divide by 9 to reduce the number
            atdist2[icent[i]] = ts[itype[i]-1]/(9*alpha[i]);

    p_box->x_min = p_box->x_max = atcoords[0][0];
    p_box->y_min = p_box->y_max = atcoords[0][1];
    p_box->z_min = p_box->z_max = atcoords[0][2];

    for(int i = 0; i < nat; i++){
        if(p_box->x_min > (atcoords[i][0] - pow(atdist2[i],0.5))) p_box->x_min = atcoords[i][0] - pow(atdist2[i],0.5);
        if(p_box->x_max < (atcoords[i][0] + pow(atdist2[i],0.5))) p_box->x_max = atcoords[i][0] + pow(atdist2[i],0.5);

        if(p_box->y_min > (atcoords[i][1] - pow(atdist2[i],0.5))) p_box->y_min = atcoords[i][1] - pow(atdist2[i],0.5);
        if(p_box->y_max < (atcoords[i][1] + pow(atdist2[i],0.5))) p_box->y_max = atcoords[i][1] + pow(atdist2[i],0.5);

        if(p_box->z_min > (atcoords[i][2] - pow(atdist2[i],0.5))) p_box->z_min = atcoords[i][2] - pow(atdist2[i],0.5);
        if(p_box->z_max < (atcoords[i][2] + pow(atdist2[i],0.5))) p_box->z_max = atcoords[i][2] + pow(atdist2[i],0.5);
    }

    return true;
}

bool WFNData::getMolBoxA(Volumetrics* p_box, int nAt){
    //This is a stub function to create a box around given atom number
    double SZ = 5;

    //p_box->x_min = atcoords[nAt][0] - SZ; p_box->x_max = atcoords[nAt][0] + SZ;
    //p_box->y_min = atcoords[nAt][1] - SZ; p_box->y_max = atcoords[nAt][1] + SZ;
    //p_box->z_min = atcoords[nAt][2] - SZ; p_box->z_max = atcoords[nAt][2] + SZ;

    //Only for H2P
    p_box->x_min = -SZ; p_box->x_max = SZ;
    p_box->y_min = -SZ; p_box->y_max = SZ;
    p_box->z_min = -SZ; p_box->z_max = SZ;

    return true;
}
