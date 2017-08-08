/*
 * Author: Dr. Yevgeniy Kolokoltsev
 * Created: 12 Jun 2017
 *
 * Remarks: This code shell be compilable without any additional
 * libraries (for portability reasons), so we use a pure C++11 standard
 */

#ifndef DAIMAN_CRITICALPOINT_H
#define DAIMAN_CRITICALPOINT_H

#include <iostream>
#include <vector>
#include <array>
#include <memory>

#include "base_vertex.hpp"
#include "math_property_map.hpp"

using namespace std;

/*
 * Groups standard properties that can be calculated for
 * any point in space, including a critical point.
 */
class PointPropertyMap : public MathPropertyMap{
public:

    struct Rho : public MathProperty<double>{
        Rho(Rho::valtype val) : MathProperty<double>(val){};
        virtual string name(){return "Rho";}
        virtual string description(){return "Electron Density";}
    };

    struct GradRho : public MathProperty<std::array<double, 3>>{
        GradRho(GradRho::valtype val) : MathProperty<std::array<double, 3>>(val){};
        virtual string name(){return "GradRho";}
        virtual string description(){return "Gradient of Electron Density";}
    };

    struct HessRhoEigVals : public MathProperty<std::array<double, 3>>{
        HessRhoEigVals(HessRhoEigVals::valtype val) : MathProperty<std::array<double, 3>>(val){};
        virtual string name(){return "HessRhoEigVals";}
        virtual string description(){return "Eigenvalues of the Hessian of Rho, Ascending Order";}
    };

    struct HessRhoEigVec1 : public MathProperty<std::array<double, 3>>{
        HessRhoEigVec1(HessRhoEigVec1::valtype val) : MathProperty<std::array<double, 3>>(val){};
        virtual string name(){return "HessRhoEigVec1";}
        virtual string description(){return "1-st Eigenvector of the Hessian of Rho";}
    };

    struct HessRhoEigVec2 : public MathProperty<std::array<double, 3>>{
        HessRhoEigVec2(HessRhoEigVec2::valtype val) : MathProperty<std::array<double, 3>>(val){};
        virtual string name(){return "HessRhoEigVec2";}
        virtual string description(){return "2-nd Eigenvector of the Hessian of Rho";}
    };

    struct HessRhoEigVec3 : public MathProperty<std::array<double, 3>>{
        HessRhoEigVec3(HessRhoEigVec3::valtype val) : MathProperty<std::array<double, 3>>(val){};
        virtual string name(){return "HessRhoEigVec3";}
        virtual string description(){return "3-rd Eigenvector of the Hessian of Rho";}
    };

    struct DelSqRho : public MathProperty<double>{
        DelSqRho(DelSqRho::valtype val) : MathProperty<double>(val){};
        virtual string name(){return "DelSqRho";}
        virtual string description(){return "Laplacian of Rho (Trace of Hessian of Rho)";}
    };

    struct V : public MathProperty<double>{
        V(V::valtype val) : MathProperty<double>(val){};
        virtual string name(){return "V";}
        virtual string description(){return "Virial Field = Potential Energy Density = Trace of Stress Tensor";}
    };

    struct G : public MathProperty<double>{
        G(G::valtype val) : MathProperty<double>(val){};
        virtual string name(){return "G";}
        virtual string description(){return "Lagrangian Form of Kinetic Energy Density";}
    };

    struct K : public MathProperty<double>{
        K(K::valtype val) : MathProperty<double>(val){};
        virtual string name(){return "K";}
        virtual string description(){return "Hamiltonian Form of Kinetic Energy Density";}
    };

    struct L : public MathProperty<double>{
        L(L::valtype val) : MathProperty<double>(val){};
        virtual string name(){return "L";}
        virtual string description(){return "Lagrangian Density = K - G = (-1/4)DelSqRho";}
    };

    struct DelSqV : public MathProperty<double>{
        DelSqV(DelSqV::valtype val) : MathProperty<double>(val){};
        virtual string name(){return "DelSqV";}
        virtual string description(){return "Laplacian of V";}
    };

    struct DelSqVen : public MathProperty<double>{
        DelSqVen(DelSqVen::valtype val) : MathProperty<double>(val){};
        virtual string name(){return "DelSqVen";}
        virtual string description(){return "Laplacian of Ven";}
    };

    struct DelSqVrep : public MathProperty<double>{
        DelSqVrep(DelSqVrep::valtype val) : MathProperty<double>(val){};
        virtual string name(){return "DelSqVrep";}
        virtual string description(){return "Laplacian of Vrep";}
    };

    struct DelSqG : public MathProperty<double>{
        DelSqG(DelSqG::valtype val) : MathProperty<double>(val){};
        virtual string name(){return "DelSqG";}
        virtual string description(){return "Laplacian of G";}
    };

    struct DelSqK : public MathProperty<double>{
        DelSqK(DelSqK::valtype val) : MathProperty<double>(val){};
        virtual string name(){return "DelSqK";}
        virtual string description(){return "Laplacian of K";}
    };
};

/*
 * A critical point is strictly defined by it's type and coordinates,
 * if these are changed - we just get another critical point.
 */
struct CriticalPoint : public ThreadSafeBaseObject, public MathBase{

    enum CP_TYPE{NACP, NNACP, BCP, RCP, CCP, UNDEF};

    CriticalPoint(const std::array<double, 3>& xyz, CP_TYPE type): xyz(xyz), type{type} {};

    virtual string name(){
        switch(type){
            case NACP: return "NACP";
            case NNACP: return "NNACP";
            case BCP: return "BCP";
            case RCP: return "RCP";
            case CCP: return "CCP";
            case UNDEF: return "UNDEF";
        }
    }

    virtual string description(){
        switch(type){
            case NACP: return "Nuclear Attractor Critical Point";
            case NNACP: return "Non-Nuclear Attractor Critical Point";
            case BCP: return "Bond Critical Point";
            case RCP: return "Ring Critical Point";
            case CCP: return "Cage Critical Point";
            case UNDEF: return "Undefined Critical Point Type";
        }
    }

    const std::array<double, 3> xyz;
    const CP_TYPE type;
    PointPropertyMap properties;
};

#endif //DAIMAN_CRITICALPOINT_H
