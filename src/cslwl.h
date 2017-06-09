#ifndef CSLWL_H
#define CSLWL_H

#include <QVector3D>

#include "wavefunction/wfndata.h"

class Cslwl
{

public:
    Cslwl(QVector3D Ra, QVector3D Rb, WFNData* wfn);
    Cslwl(QVector3D Ra, double x, double y, double z, WFNData* wfn); //setup a BCP
    ~Cslwl();

    bool calc_bond_path(bool useBCP); //runs main calculation
    QVector3D R(double s); //attractor interaction line coordinates (0 <= s <= 1)

    QVector3D Rc;     //Bond critical point
    QVector3D Ecz;    //critical point axis along bond line in direction of positive hessian eigenvector
    QVector3D Ecx;    //first negative hessian eigenvector
    QVector3D Ecy;    //second negative hessian eigenvector
    double rhoRc;     //electron density at BCP

private:
    static const int N = 40;  //Length of the expansion, should be even

    QVector3D Ra;     //Atom A center
    QVector3D Rb;     //Atom B center
    QVector3D Rba;    //Vector that interconnects two atoms A and B (not a unit verctor) = Rb - Ra
    QVector3D R1;     //First perpendicular to Rba (unit vector)
    QVector3D R2;     //Second perpendicular to Rba (unit vector)

    double* C;        //Expansion coefficients Ck1 = C[2k], Ck2 = C[2k + 1], total length of 2k
                      //These coefficients are to be found during Cioslowski procedure
    WFNData* wfn;     //Wavefunction data and routines



private:

    void init(QVector3D Ra, QVector3D Rb, WFNData* wfn);

    QVector3D dR(double s);       //a vector dR/ds at point s;

    double sj(int j);      //grid points

    QVector3D g(double s); //returns rho gradient for the given point on the attractor

    QVector3D hes(double s, double* hes, bool calc_grad = false); //gets hessian of rho and
                           //optionally its gradient as a coproduct

    void calc_ab(double *a, double *b); //calcs a 2N*2N matrix "a(C)", and 2N vector "b(C)"
                           //for the current vector C which form a system of equations to be solved a(C)C = b(C).

    QVector3D calc_cgradg(double* cgrad, int j); //Returns an 2Nx3 matrix of the first derivatives of grad(rho) over
                           //the N expansion coefficients. This matrix consists of N partials of g by C[i].
                           //This function also returns a gred(rho): "g". "j" is the number of a grid point on the
                           //attractor for which the cgradg is calculated.

    void calc_jacobian(double* jac); //This returns a jacobian of the a(C)C - b(C) = 0
                           //system of nonlinear equations. For this jacobian current vector C is used

    double iterate_linear(); //the linear iteration, where grad(rho) is considered independent from "C".
                             //In this assumption a(C)C = b(C) is reduced to aC = b. This iterations
                             //are saved for history because of bad convergence

    double iterate_newton(); //the Newton-Raphson iteration as it should be for the system of a nonelinear equetions
                             //it returns a step length L in the case of success, or a -1 in the case of fail

    bool findBCPOnLine();    //Finds a critical point on the bond with the help of division by two
    bool findBCPNewton(QVector3D r0);  //Finds a BCP with the help of newton method starting fromgiven point on line
    bool calcBPCProps();     //Calculates eigenvalues and eigenvectors. Checks a BCP type.



//--MATH UTILS--
private:
    void minor_mat(int col, int n, const double* mat, double* minor); //fills a minor array with a minor of mat at "a"
    double det_mat(int n, const double* mat); //determinant
    double length_vec(int n, const double* vec);// returns ||v||
    void sum_vec(int n, double alpha, double* a, double beta, const double* b); //a = alpha*a + beta*b;
    void mult_mat_vec(int n, double* mat, double *vec); //vec = mat*vec
    bool check_inverse_mat(const int n, double* m_dir, double* m_inv); //checks an inverse matrix det||m_dir*m_inv|| == 1
    double check_linear_solution(const int n, const double* a, const double *b, const double *x); //returns ||ax - b||
    void print_num(double val); //prints a number in scientific form


    void print_mat(int m, int n, const double* mat); //prints an m*n matrix

};

#endif // CSLWL_H
