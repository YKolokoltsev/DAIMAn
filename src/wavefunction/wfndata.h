#ifndef WFNDATA_H
#define WFNDATA_H

#include "old_misc/structs.h"

class WFNData
{
public:
    WFNData();
    ~WFNData();

public:
    char*    title;
    char*    keywords;    //Gaussian or Slater type of orbitals
    int      nat;         //number of atoms
    int      nprim;       //number of primitives (ngto)
    int      norb;        //number of molecular orbitals
    int      npert;       //number of perturbations
    char**   atnames;     //an array of original atomic names;
    int*     atnumbers;   //nuclear charge of each atom
    double*  atcharges;   //atomic charges
    double** atcoords;    //coordinates of the atoms
    double   netcharge;   //don't know what is that
    int      nelectrons;  //total number of electrons
    int      nalpha;      //number of alpha electrons
    int      nbeta;       //number of beta electrons
    int      multi;       //multiplicity
    char*    model;       //RHF or UHF
    int*     icent;       //primitive centers, starting from 0
    int*     itype;       //type of the angular function of the primitive (cartesian basis)
    double*  alpha;       //~exp(-alpha*|distance|^2) << that alpha $)
    double*  occup;       //orbital occupation numbers
    double*  orben;       //molecular orbital energies
    int*     orspin;      //molecular orbital spin types (1 = Alpha, 2 = Beta, 3 = Alpha And Beta)
    double** c;           //molecular orbital primitive coefficients
    double   etotal;      //Total molecular energy: T + Vne + Vee + Vnn
    double   rvirial;     //Virial Ratio (-V/T)
    double*  p;           //sum[ i // ci,mu ci,nu ] - simplification (like a density matrix)
    int*     ir;          //type of the orbitals(like s,p,d,f...)
    double*  ts;          //Magic numbers used to estimate cutoff radii of the different orbital types, r^2 = ts[orb]/alpha[orb];

 private:
    double* d2x;
    double* d2y;
    double* d2z;
    double* d2xy;
    double* d2xz;
    double* d2yz;

    double* d;
    double* dx;
    double* dy;
    double* dz;
    int*    nz;           //Overestimated array of the nonezero primitives for current point. It's size is nprim+1. nz[0] - number of nonezero primitives it contain
                          //and from nz[1] to nz[nz[0]] indices of nonezero primitives are stored.

 public:
    double getMinMaxXYZ(int coord, bool min); //coord = 0,1,2
    double getMidXYZ(int coord);
    void   clear();
    bool   calc_helpers(void);
    double calc_rho(double x, double y, double z);
    double calc_grad_rho(double x, double y, double z, double* grad, bool calc_rho = false);
                          // x,y,z = is the point where to calculate grad(elf), and grad - is a preallocated memory where to write an answer;
                          // if the last argument is true, this function will also return rho as a coproduct, otherwise it return zero
    double calc_hessian_rho(double x, double y, double z, bool cutoff, double* hes, double* grad = 0, bool calc_rho = false);
                          // x,y,z = is the point where to calculate a hessian, which is stored into the preallocated memory "hes"
                          // if "grad" is not null, this function will also calculate a grad(rho) as a coproduct
                          // if also rho is set to true, the function will return rho, otherwise it returns 0
    double calc_elf(double x, double y, double z);
    double calc_grad_elf(double x, double y, double z, double* gx, double* gy, double* gz);
                          // x,y,z = is the point where to calculate grad(elf), and gx,gy,gz - is a preallocated memory where to write an answer;
                          // this function also returns elf, as a co-product
    double normorb(int orb);                     //this calculates <orb|orb>

    bool getMolBox(Volumetrics* p_box);
    bool getMolBoxA(Volumetrics* p_box, int nAt);

public:
    double  f(int icoord, int mu, double val);   //icoord = 1,2,3; val - value of the icoord (x,y,z); f = Psi
    double df(int icoord, int mu, double val);   //first derivatives of the basis functions

private:
    double d2f(int icoord, int mu, double val);  //second derivatives of the basis functions
    void   set_nz(double x, double y, double z); //this function partially fills "nz" array
    double intinf(int icoord, int mu, int nu);  //this calculates an integral from -inf to +inf of two basis functions mu and nu

};

#endif // WFNDATA_H
