#ifndef WFNDATA_H
#define WFNDATA_H

#include "structs.h"
#include "base_vertex.hpp"

class WFNData : public BaseObj {
public:
    WFNData();
    ~WFNData();

public:
    char*    title{nullptr};
    char*    keywords{nullptr};    //Gaussian or Slater type of orbitals
    int      nat{0};         //number of atoms
    int      nprim{0};       //number of primitives (ngto)
    int      norb{0};        //number of molecular orbitals
    int      npert{0};       //number of perturbations
    char**   atnames{nullptr};     //an array of original atomic names;
    int*     atnumbers{nullptr};   //nuclear charge of each atom
    double*  atcharges{nullptr};   //atomic charges
    double** atcoords{nullptr};    //coordinates of the atoms
    double   netcharge{0};   //don't know what is that
    int      nelectrons{0};  //total number of electrons
    int      nalpha{0};      //number of alpha electrons
    int      nbeta{0};       //number of beta electrons
    int      multi{0};       //multiplicity
    char*    model{nullptr};       //RHF or UHF
    int*     icent{nullptr};       //primitive centers, starting from 0
    int*     itype{nullptr};       //type of the angular function of the primitive (cartesian basis)
    double*  alpha{nullptr};       //~exp(-alpha*|distance|^2) << that alpha $)
    double*  occup{nullptr};       //orbital occupation numbers
    double*  orben{nullptr};       //molecular orbital energies
    int*     orspin{nullptr};      //molecular orbital spin types (1 = Alpha, 2 = Beta, 3 = Alpha And Beta)
    double** c{nullptr};           //molecular orbital primitive coefficients
    double   etotal{0};      //Total molecular energy: T + Vne + Vee + Vnn
    double   rvirial{0};     //Virial Ratio (-V/T)
    double*  p{nullptr};           //sum[ i // ci,mu ci,nu ] - simplification (like a density matrix)
    int*     ir{nullptr};          //type of the orbitals(like s,p,d,f...)
    double*  ts{nullptr};          //Magic numbers used to estimate cutoff radii of the different orbital types, r^2 = ts[orb]/alpha[orb];

 private:
    double* d2x{nullptr};
    double* d2y{nullptr};
    double* d2z{nullptr};
    double* d2xy{nullptr};
    double* d2xz{nullptr};
    double* d2yz{nullptr};

    double* d{nullptr};
    double* dx{nullptr};
    double* dy{nullptr};
    double* dz{nullptr};
    int*    nz{nullptr};           //Overestimated array of the nonezero primitives for current point. It's size is nprim+1. nz[0] - number of nonezero primitives it contain
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
