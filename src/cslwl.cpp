#include "cslwl.h"
#include "commons.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>

#include "cgal_routines.h"

#ifdef __cplusplus
extern "C"
{
#endif
   #include <cblas.h>
#ifdef __cplusplus
}
#endif

#include <lapacke.h>

Cslwl::Cslwl(QVector3D Ra, QVector3D Rb, WFNData* wfn){
    init( Ra,Rb,wfn);
}

Cslwl::Cslwl(QVector3D Ra, double x, double y, double z, WFNData* wfn){

    QVector3D Rb = QVector3D(x,y,z);

    init( Ra,Rb,wfn);
    this->Rc = Rb;
}

void Cslwl::init(QVector3D Ra, QVector3D Rb, WFNData* wfn){
    this->Ra = Ra;
    this->Rb = Rb;
    this->Rba = Rb - Ra;

    this->wfn = wfn;

    C = new double[2*N];
    memset(C,0,2*N*sizeof(double));

    buildOrtsToN(Rba,R1,R2);
}

Cslwl::~Cslwl()
{
    if(C) delete [] C;
    wfn = NULL;
}

//point on attractor given by "s", where ( 0 <= s <= 1 )
QVector3D Cslwl::R(double s){
    QVector3D r(0,0,0);
    r = (1 - s)*Ra + s*Rb;
    for(int k = 0; k < N; k++){
        r += (C[2*k]*R1 + C[2*k+1]*R2)*sin(M_PI*(k+1)*s);
    }
    return r;
}

//dR/ds at point s
QVector3D Cslwl::dR(double s){
    QVector3D r(0,0,0);
    r = Rb - Ra;
    for(int k = 0; k < N; k++){
        r += (C[2*k]*R1 + C[2*k+1]*R2)*M_PI*(k+1)*cos(M_PI*(k+1)*s);
    }
    return r;
}

//gradient of rho on attractor given by "s"
QVector3D Cslwl::g(double s){
    QVector3D r = R(s);
    double* grad = new double[3];

    wfn->calc_grad_rho(r.x(),r.y(),r.z(),grad);

    QVector3D g(grad[0],grad[1],grad[2]);
    delete [] grad;

    return g;
}

//hessian of rho on attractor given by "s"
QVector3D Cslwl::hes(double s, double* hes, bool calc_grad){
    double* grad;
    QVector3D G(0,0,0);

    if(!hes){
        if(calc_grad){
            return g(s);
        }else{
            return G;
        }
    }

    QVector3D r = R(s);

    if(calc_grad){
        grad = new double[3];
        wfn->calc_hessian_rho(r.x(),r.y(),r.z(),true,hes,grad);
        G = QVector3D(grad[0],grad[1],grad[2]);
        delete [] grad;
    }else{
        wfn->calc_hessian_rho(r.x(),r.y(),r.z(),true,hes);
    }

    return G;
}

//point on an attractor given by "j", (1 <= j <= N), so it never returns 0 or 1
double Cslwl::sj(int j){
    return (double)j/((double)(N+1));
}

//get 2N*2N matrix "a(C)", and 2N vector "b(C)" for the current vector C which form a system of equations
//a(C)C = b(C).
void Cslwl::calc_ab(double *a, double *b){
    if(!a || !b) return;
    memset(a,0,2*N*2*N*sizeof(double));
    memset(b,0,2*N*sizeof(double));

    double Sj, G1j, G2j, Gbaj, Gj2, KCos;
    QVector3D Gj;

    for(int j = 0; j < N; j++){
        Sj = sj(j+1); //j-th position on the line
        Gj = g(Sj); //grad(rho(r(Sj))) - gradient of rho at the point given by Sj and a current line

        G1j = QVector3D::dotProduct(Gj,R1);   //projection of gradient on R1
        G2j = QVector3D::dotProduct(Gj,R2);   //projection of gradient on R2
        Gbaj = QVector3D::dotProduct(Gj,Rba); //dot product of with Rab (Rab is not a unit vector)
        Gj2 = QVector3D::dotProduct(Gj,Gj);   //grad(rho)^2


        b[j] = G1j*Gbaj;   //U1, see article
        b[N+j] = G2j*Gbaj; //U2, see article

        for(int k = 0; k < N; k++){
            KCos = M_PI*(k+1)*cos(M_PI*(k+1)*Sj);

            //U1
            a[2*N*j + 2*k] = (Gj2 - G1j*G1j)*KCos;
            a[2*N*j + 2*k+1] = -G1j*G2j*KCos;

            //U2
            a[2*N*(N+j) + 2*k] = -G1j*G2j*KCos;
            a[2*N*(N+j) + 2*k+1] = (Gj2 - G2j*G2j)*KCos;
        }
    }
}


//cgrad is 2N*3 preallocated vector
QVector3D Cslwl::calc_cgradg(double* cgrad, int j){
    QVector3D g(0,0,0); //grad(rho)
    if(!cgrad) return g;

    memset(cgrad,0,2*N*3*sizeof(double));

    double* H = new double[9];    //hessian(rho) respectively
    QVector3D DkR;    //partial of R(C) by Ck;
    double s = sj(j); //point on attractor (0 < s < 1)

    g = hes(s,H,true);

    //partials of grad(rho) by Ck
    for(int k = 0; k < N; k++){
        //Ck1
        DkR = R1*sin(M_PI*(k+1)*s);
        cgrad[2*k*3 + 0] = H[0]*DkR.x() + H[1]*DkR.y() + H[2]*DkR.z();
        cgrad[2*k*3 + 1] = H[3]*DkR.x() + H[4]*DkR.y() + H[5]*DkR.z();
        cgrad[2*k*3 + 2] = H[6]*DkR.x() + H[7]*DkR.y() + H[8]*DkR.z();

        //Ck2
        DkR = R2*sin(M_PI*(k+1)*s);
        cgrad[(2*k+1)*3 + 0] = H[0]*DkR.x() + H[1]*DkR.y() + H[2]*DkR.z();
        cgrad[(2*k+1)*3 + 1] = H[3]*DkR.x() + H[4]*DkR.y() + H[5]*DkR.z();
        cgrad[(2*k+1)*3 + 2] = H[6]*DkR.x() + H[7]*DkR.y() + H[8]*DkR.z();
    }

    delete [] H;

    return g;
}

//This Jacobian is a 2N*2N matrix of partials of the nonlinear equations by Ck
void Cslwl::calc_jacobian(double* jac){
    if(!jac) return;
    memset(jac,0,4*N*N*sizeof(double));

    double *ptr, *KCos, *cgrad;
    double sum;
    double   G1j,   G2j,   Gabj,   Gj2; //G1j = grad(rho(sj(j)))*R1, G2j = g*R2, Gab = g*Rba, Gj2 = g^2
    double DkG1j, DkG2j, DkGabj, DkGj2; // ... partials by Ck
    QVector3D Gj, Dk1Gj, Dk2Gj;

    ptr = new double[N + 2*N*3];
    KCos = ptr;
    cgrad = &ptr[N];

    for(int j = 0; j < N; j++){
        Gj = calc_cgradg(cgrad,j+1); //partials of grad(rho) by Ck

        G1j = QVector3D::dotProduct(Gj,R1);   //projection of gradient on R1
        G2j = QVector3D::dotProduct(Gj,R2);   //projection of gradient on R2
        Gabj = QVector3D::dotProduct(Gj,Rba); //dot product of with Rab (Rab is not a unit vector)
        Gj2 = QVector3D::dotProduct(Gj,Gj);   //grad(rho)^2

        for(int k = 0; k < N; k++)
            KCos[k] = M_PI*(k+1)*cos(M_PI*(k+1)*sj(j+1));

        for(int k = 0; k < N; k++){

            Dk1Gj = QVector3D(cgrad[2*k*3 + 0],cgrad[2*k*3 + 1],cgrad[2*k*3 + 2]);
            Dk2Gj = QVector3D(cgrad[(2*k+1)*3 + 0],cgrad[(2*k+1)*3 + 1],cgrad[(2*k+1)*3 + 2]);

            //U1 ---------------------

                //Ck1
                DkG1j = QVector3D::dotProduct(Dk1Gj,R1);
                DkG2j = QVector3D::dotProduct(Dk1Gj,R2);
                DkGabj = QVector3D::dotProduct(Dk1Gj,Rba);
                DkGj2 = 2*QVector3D::dotProduct(Gj,Dk1Gj);

                sum = 0;
                for(int m = 0; m < N; m++){
                    sum += (C[2*m]*(DkGj2 - 2*G1j*DkG1j) - C[2*m+1]*(G1j*DkG2j + G2j*DkG1j))*KCos[m];
                }

                jac[2*N*j + 2*k]   = -(Gabj*DkG1j + G1j*DkGabj) + sum + (Gj2 - G1j*G1j)*KCos[k];

                //Ck2
                DkG1j = QVector3D::dotProduct(Dk2Gj,R1);
                DkG2j = QVector3D::dotProduct(Dk2Gj,R2);
                DkGabj = QVector3D::dotProduct(Dk2Gj,Rba);
                DkGj2 = 2*QVector3D::dotProduct(Gj,Dk2Gj);

                sum = 0;
                for(int m = 0; m < N; m++){
                    sum += (C[2*m]*(DkGj2 - 2*G1j*DkG1j) - C[2*m+1]*(G1j*DkG2j + G2j*DkG1j))*KCos[m];
                }

                jac[2*N*j + 2*k+1] = -(Gabj*DkG1j + G1j*DkGabj) + sum - G1j*G2j*KCos[k];

            //U2 ---------------------

                //Ck1
                DkG1j = QVector3D::dotProduct(Dk1Gj,R1);
                DkG2j = QVector3D::dotProduct(Dk1Gj,R2);
                DkGabj = QVector3D::dotProduct(Dk1Gj,Rba);
                DkGj2 = 2*QVector3D::dotProduct(Gj,Dk1Gj);

                sum = 0;
                for(int m = 0; m < N; m++){
                    sum += (-C[2*m]*(G1j*DkG2j + G2j*DkG1j) + C[2*m+1]*(DkGj2 - 2*G2j*DkG2j))*KCos[m];
                }

                jac[2*N*(N+j) + 2*k]   = -(Gabj*DkG2j + G2j*DkGabj) + sum - G1j*G2j*KCos[k];

                //Ck2
                DkG1j = QVector3D::dotProduct(Dk2Gj,R1);
                DkG2j = QVector3D::dotProduct(Dk2Gj,R2);
                DkGabj = QVector3D::dotProduct(Dk2Gj,Rba);
                DkGj2 = 2*QVector3D::dotProduct(Gj,Dk2Gj);

                sum = 0;
                for(int m = 0; m < N; m++){
                    sum += (-C[2*m]*(G1j*DkG2j + G2j*DkG1j) + C[2*m+1]*(DkGj2 - 2*G2j*DkG2j))*KCos[m];
                }

                jac[2*N*(N+j) + 2*k+1] = -(Gabj*DkG2j + G2j*DkGabj) + sum + (Gj2 - G2j*G2j)*KCos[k];
        }
    }

    delete [] ptr;
}

//a simple linear iteration which solves just a system of linear equations (bad convergence, saved for history)
double Cslwl::iterate_linear(){

    double L;
    double *ptr, *a, *b, *A, *B;
    lapack_int* ipiv;
    lapack_int info;

    ptr = new double[4*N*N + 2*N + 4*N*N + 2*N];
    ipiv = new lapack_int[2*N];

    if(!ptr || !ipiv){
        L = -1;
        goto return_iterate_linear;
    }

    a = ptr;
    b = &ptr[4*N*N];
    A = &ptr[4*N*N + 2*N];
    B = &ptr[4*N*N + 2*N + 4*N*N];

    //get "a" and "b" on the basis of current expansion coefficients "C"
    calc_ab(a,b);

    //solve the linear system of equations using LAPACKe
    memcpy(A,a,4*N*N*sizeof(double));
    memcpy(B,b,2*N*sizeof(double));

    info = LAPACKE_dgels(LAPACK_ROW_MAJOR,'N',2*N,2*N,1,a,2*N,b,1);

    //info = LAPACKE_dgesv( LAPACK_ROW_MAJOR, 2*N, 1, a, 2*N, ipiv, b, 1);

    if(info != 0){
        printf("LAPACKE_dgele was unable to find a min||aX - b||, bond aborted\n");
        L = -1;
        goto return_iterate_linear;
    }

    check_linear_solution(2*N, A, B, b);

    //find a coefficient difference came with this iteration
    for(int k = 0; k < N; k++){
        L = pow(C[2*k] - b[2*k],2) + pow(C[2*k+1] - b[2*k+1],2);
    }
    L = sqrt(L);
    printf("L: %lf \n",L);

    //store new expansion coefficients
    for(int k = 0; k < N; k++){
        C[2*k] = b[2*k];
        C[2*k+1] = b[2*k+1];
    }

    return_iterate_linear:
    if(ptr) delete [] ptr;
    if(ipiv) delete [] ipiv;

    return L;
}

//The newton iteration is: C{n+1} = C{n} - Jac^-1 * (A*C{n} - B)
double Cslwl::iterate_newton(){

    double var, L = -1;
    lapack_int info;
    double *ptr, *a, *f, *jac, *jac_cpy, *f_cpy, *b;
    int *ipiv;

    ptr = new double[4*N*N + 2*N + 4*N*N + 4*N*N + 2*N + 2*N];
    ipiv = new int[2*N];
    if(!ptr || !ipiv){
        goto return_iterate_newton;
    }

    a = ptr;
    f = &ptr[4*N*N];
    jac = &ptr[4*N*N + 2*N];
    jac_cpy = &ptr[4*N*N + 2*N + 4*N*N];
    f_cpy = &ptr[4*N*N + 2*N + 4*N*N + 4*N*N];
    b = &ptr[4*N*N + 2*N + 4*N*N + 4*N*N + 2*N];

    //calc jacobian matrix ---------------------------
    calc_jacobian(jac);
    memcpy(jac_cpy,jac,4*N*N*sizeof(double));
    //printf("Det(jac) = % .3e\n",det_mat(2*N,jac));

    //calc dC as a solution of a system of linear equations: J dC = -aC + b -------
    calc_ab(a,b);
    //printf("Det(a) = % .3e\n",det_mat(2*N,a));
    //var = check_linear_solution(2*N,a,b,C);
    //printf("||ac - b|| = % .3e; ", var);

    //set f = -aC + b
    memcpy(f,C,2*N*sizeof(double));
    mult_mat_vec(2*N,a,f);
    sum_vec(2*N, -1, f, 1, b);
    memcpy(f_cpy,f,2*N*sizeof(double));

    //solve a system of linear equations J dC = f, and write the result (dC) to f
    info = LAPACKE_dgels(LAPACK_ROW_MAJOR,'N',2*N,2*N,1,jac,2*N,f,1);
    //info = LAPACKE_dgesv( LAPACK_ROW_MAJOR, 2*N, 1, jac, 2*N, ipiv, f, 1);

    if(info != 0){
        printf("Unable to solve J dC = f, bond aborted\n");
        L = -1;
        goto return_iterate_newton;
    }

    //calc L = ||dC|| and make an iteration C{n+1} = C{n} + dC ---------------
    L = length_vec(2*N,f);

    sum_vec(2*N, 1, C, 1, f);

    return_iterate_newton:
    if(ptr) delete [] ptr;
    if(ipiv) delete [] ipiv;

    return L;
}

bool Cslwl::findBCPNewton(QVector3D r0){
    double *ptr, *r, *hes, *grad, *dr;
    lapack_int info;
    bool res = false;
    int count = 0;

    ptr = new double[3 + 9 + 3 + 3];
    if(!ptr) return false;

    r = ptr;
    hes = &ptr[3];
    grad = &ptr[3 + 9];
    dr = &ptr[3 + 9 + 3];

    r[0] = r0.x(); r[1] = r0.y(); r[2] = r0.z();

    do{
        wfn->calc_hessian_rho(r[0],r[1],r[2],false,hes,grad);

        if(length_vec(3,grad) < 1e-6){
            Rc = QVector3D(r[0],r[1],r[2]);
            res = true;
            goto return_findBCPNewton;
        }

        sum_vec(3, 0, dr, -1, grad);
        info = LAPACKE_dgels(LAPACK_ROW_MAJOR,'N',3,3,1,hes,3,dr,1);
        if(info != 0){
            printf("findBCPNewton failed to solve equations, abotring...\n");
            goto return_findBCPNewton;
        }

        sum_vec(3,1,r,1,dr);
        count++;
    }while(count < 20 || length_vec(3,dr) < 1e-10);

    return_findBCPNewton:
    if(!res){
        printf("INFO: count=%d, ||grad||=% .3e, ||dr||=% .3e\n",count, length_vec(3,grad),length_vec(3,dr));
    }
    delete [] ptr;
    return res;
}

bool  Cslwl::calcBPCProps(){
    bool ret = false;
    double *ptr, *hes, *U, *alphar, *alphai, *beta, *vr;
    lapack_int info;
    int positives = 0, negatives = 0;

    ptr = new double[9 + 9 + 3 + 3 + 3 + 9];
    if(!ptr){
        goto return_calcBPCProps;
    }
    memset(ptr,0,(9 + 9 + 3 + 3 + 3 + 9)*sizeof(double));

    hes = ptr;
    U =      &ptr[9];
    alphar = &ptr[9 + 9];
    alphai = &ptr[9 + 9 + 3];
    beta =   &ptr[9 + 9 + 3 + 3];
    vr =     &ptr[9 + 9 + 3 + 3 + 3];

    rhoRc = wfn->calc_hessian_rho(Rc.x(),Rc.y(),Rc.z(),false,hes, NULL, true);
    U[0] = 1; U[4] = 1; U[8] = 1;

    info = LAPACKE_dggev(LAPACK_ROW_MAJOR, 'N', 'V',
                         3, hes, 3, U, 3, alphar, alphai, beta,
                         NULL, 3, vr, 3);
    if(info != 0){
        printf("Failed to calculate eigenvectors for hessian at BCP\n");
        goto return_calcBPCProps;
    }

    //this check should be always passed because Hessian is a symmetric
    if(length_vec(3,alphai) > 1e-8){
        printf("Hessian at BCP contains complex eigenvalues, aborted\n");
        goto return_calcBPCProps;
    }


    for(int i = 0; i < 3; i++){
        if(alphar[i] > 0){
            if(positives > 0){
                printf("Hessian at BCP contains more than one positive eigenvalue, it is not a BCP\n");
                goto return_calcBPCProps;
            }
            Ecz = QVector3D(vr[i],vr[i + 3], vr[i+6]);
            if(Ecz.length() < 1e-6){
                printf("Ecz of Hessian at BCP is degenerate\n");
                goto return_calcBPCProps;
            }
            Ecz.normalize();
            positives++;
        }else if(alphar[i] == 0){
            printf("Hessian at BCP contains degenerate eigenvalue, not a point\n");
            goto return_calcBPCProps;
        }else if(alphar[i] < 0){
            if(negatives == 0){
                Ecx = QVector3D(vr[i],vr[i + 3], vr[i+6]);
                if(Ecx.length() < 1e-6){
                    printf("Ecx of Hessian at BCP is degenerate\n");
                    goto return_calcBPCProps;
                }
                Ecx.normalize();
            }else if(negatives == 1){
                Ecy = QVector3D(vr[i],vr[i + 3], vr[i+6]);
                if(Ecy.length() < 1.e-6){
                    printf("Ecy of Hessian at BCP is degenerate\n");
                    goto return_calcBPCProps;
                }
                Ecy.normalize();
            }else if(negatives == 2){
                printf("Hessian at BCP contains more than two negative eigenvalues, it is not a BCP\n");
                goto return_calcBPCProps;
            }
            negatives++;
        }

    }

    ret = true;

    return_calcBPCProps:
    if(ptr) delete [] ptr;
    return ret;
}

bool Cslwl::findBCPOnLine(){
    double a = 0, b = 1, s, val, ds;
    QVector3D grad, NdR;

    while(1){
        ds = (b - a)/2;

        if(ds < 1e-10){
            goto newton_iterations;
        }
        s = a + ds;

        NdR = dR(s);
        NdR.normalize();

        grad = g(s);
        if(grad.length() < 1e-6){
            Rc = R(s);
            goto cp_found;
        }
        grad.normalize();

        val = QVector3D::dotProduct(NdR,grad);
        if(abs(val) < 1e-6){
            goto newton_iterations;
        }
        if(val < 0){
            a += ds;
        }else{
            b -= ds;
        }
    }

    newton_iterations:
    if(!findBCPNewton(R(s))){
        printf("Failed to find CP with Newton...\n");
        return false;
    }

    cp_found:
    printf("Found CP(% .3e,% .3e,% .3e) for s = % .3e\n", Rc.x(), Rc.y(), Rc.z(),s);
    val = (Rc - R(s)).length();
    printf("||R(s) - Rc|| = % .3e\n\n",val);

    if(!calcBPCProps()){
        return false;
    }

    return true;
}

bool Cslwl::calc_bond_path(bool useBCP){

    double L = 1;
    int count = 0;
    bool ret = false;

    while(L > 1e-6){
        L = iterate_newton();
        if(L < 0){
            //an iteration rocess failed
            goto return_calc_bond_path;
        }else if(L > 100){
            //an iteration  rpocss has diverged
            goto return_calc_bond_path;
        }else if(count > 10){
            //did not converge
            goto return_calc_bond_path;
        }

    count++;
    }

    ret = (L >= 0 && L < 1e-6);

    return_calc_bond_path:
    if(!ret) return false;

    if(useBCP){
        ret = calcBPCProps();
    }else{
        //calls calcBPCProps automatically
        ret = findBCPOnLine();
    }

    return ret;
}

//--MATH ROUTINES -----------------------------------------------------------

void Cslwl::minor_mat(int col, int n, const double* mat, double* minor){
    int k = 0, l;
    for(int i = 1; i < n; i++){
        l=0;
        for(int j = 0; j < n; j++){
            if(j != col){
                minor[k*(n-1) + l] = mat[i*n + j];
                l++;
            }

        }
        k++;
    }
}

double Cslwl::det_mat(int n, const double* mat){

    if(n == 1) return mat[0];
    if(n == 2) return mat[0]*mat[3] - mat[1]*mat[2];

    double det = 0;
    double *minor;

    minor = new double[(n-1)*(n-1)];
    if(!minor){
        goto return_det_mat;
    }

    for(int i = 0; i < n; i++){
        minor_mat(i,n,mat,minor);
        det += pow(-1,i)*mat[i]*det_mat(n-1,minor);
    }

    return_det_mat:
    if(minor) delete [] minor;
    return det;
}

void Cslwl::print_mat(int m, int n, const double* mat){
    for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            print_num(mat[m*i + j]);
            printf(" ");
        }
        printf("\n");
    }
    printf("\n");
}

void Cslwl::print_num(double val){
    if(val < 10e-10){
        printf("         0");
    }else{
        printf("% .3e",val);
    }

}

// checks ||m_dir*m_inv|| == 1
bool Cslwl::check_inverse_mat(const int n, double* m_dir, double* m_inv){

    double *C = new double[n*n];
    if(!C){
        goto return_check_inverse_mat;
    }

    memset(C,0,n*n*sizeof(double));

    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, n, n, n, 1, m_dir, n, m_inv, n, 0, C, n);
    print_mat(n,n,C);

    return_check_inverse_mat:
    if(C) delete [] C;

    return true;
}

//vec = mat*vec
void Cslwl::mult_mat_vec(int n, double* mat, double* vec){
    double* res = new double[n];
    memset(res,0,n*sizeof(double));

    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            res[i] += mat[i*n + j]*vec[j];
        }
    }

    memcpy(vec,res,n*sizeof(double));
    delete [] res;
}

double Cslwl::check_linear_solution(const int n, const double* a, const double *b, const double *x){
    double sum, diff = 0;

    for(int i = 0; i < n; i++){
        sum = 0;
        for(int j = 0; j < n; j++){
            sum += a[n*i + j]*x[j];
        }
        diff += pow(sum - b[i],2);
    }
    diff = sqrt(diff);

    return diff;
}

double Cslwl::length_vec(int n, const double* vec){
    double sum = 0;
    for(int i = 0; i < n; i++){
        sum += vec[i]*vec[i];
    }
    sum = pow(sum,0.5);
    return sum;
}

//a = alpha*a + beta*b;
void Cslwl::sum_vec(int n, double alpha, double* a, double beta, const double* b){
    for(int i = 0; i < n; i++){
        a[i] = alpha*a[i] + beta*b[i];
    }
}
