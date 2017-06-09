//static routines of cgal_surface
#include "cgal_routines.h"
#include <QList>

double getEdgeLength(Halfedge_handle a){
    return CGAL::to_double(CGAL::squared_distance(a->vertex()->point(),
                                                  a->opposite()->vertex()->point()));
}

bool is_valid_join(Halfedge_handle a){

    if(circulator_size(a->vertex_begin()) < 3 ||
    circulator_size(a->opposite()->vertex_begin()) < 3){
        return false;
    }

    return true;
}

bool is_border_edge(Halfedge_handle a){
    if(a->vertex()->vertex_degree() < 3) return true;
    if(a->opposite()->vertex()->vertex_degree() < 3) return true;
    return (a->is_border() || a->opposite()->is_border());
}

QVector3D halfedgePoint(Halfedge_const_handle h){
    return QVector3D(h->vertex()->point().x(), h->vertex()->point().y(), h->vertex()->point().z());
}

QVector3D uv_transform_linear(QVector3D a, QVector3D b, QVector3D c, QVector3D p, QVector3D A, QVector3D B, QVector3D C){

    QVector3D P, Vb, Vc, Nrm;
    QVector3D vb, vc;
    double alpha, beta, t;

    vb = b - a;
    vc = c - a;

    Vb = B - A;
    Vc = C - A;

    Nrm = QVector3D::crossProduct(Vb, Vc);
    Nrm.normalize();

    /**
     * System of equations:
     *
     * 1) P = A + alpha * Vb + t1*(Nrm x Vb)
     * 2) P = A + beta * Vc + t2*(Nrm x Vc)
     *
     * Solution:
     * t1 = (beta * Vc^2 - alpha * Vb * Vc) / [ (Nrm x Vb) * Vc ]
     * t2 = (alpha * Vb^2 - beta * Vb * Vc) / [ (Nrm x Vc) * Vb ]
     **/

    alpha = QVector3D::dotProduct(vb,p - a)/vb.lengthSquared();
    beta = QVector3D::dotProduct(vc,p - a)/vc.lengthSquared();


    t = beta * QVector3D::dotProduct(Vc, Vc) - alpha *  QVector3D::dotProduct(Vb, Vc);
    t = t/(QVector3D::dotProduct(QVector3D::crossProduct(Nrm,Vb),Vc));

    P = A + alpha*Vb + t*QVector3D::crossProduct(Nrm,Vb);

    return P;
}

QVector3D uv_transform_tensorial(QVector3D a, QVector3D b, QVector3D c, QVector3D p,
                                     QVector3D A, QVector3D B, QVector3D C){
    QVector3D P, AB, AC, HB;
    QVector3D ab, ac, hb, ap;

    double a11, a12, a21, a22;
    double Uc, Ub, Vb, Up, Vp;
    double Xc, Xb, Yb, Xp, Yp;

    //(U,V) triangle
    ab = b - a;
    ac = c - a;
    ap = p - a;

    Uc = ac.length();
    Ub = QVector3D::dotProduct(ab,ac)/Uc;

    hb = ab - Ub*ac/Uc;
    Vb = hb.length();

    //(X,Y,Z) triangle
    AB = B - A;
    AC = C - A;

    Xc = AC.length();
    Xb = QVector3D::dotProduct(AB,AC)/Xc;

    HB = AB - Xb*AC/Xc;
    Yb = HB.length();

    //Matrix coefficients
    a11 = Xc/Uc;
    a12 = Xb/Vb - (Ub/Uc)*(Xc/Vb);
    a21 = 0;
    a22 = Yb/Vb;


    //get "p" in new UV coords
    Up = QVector3D::dotProduct(ap,ac)/Uc;
    Vp = QVector3D::dotProduct(ap,hb)/Vb;

    //get "P" in new XY coords
    Xp = a11*Up + a12*Vp;
    Yp = a21*Up + a22*Vp;

    //Translate XY -> XYZ
    P = A + Xp*AC/Xc + Yp*HB/Yb;

    return P;
}

bool findVecBetween(QVector3D v1, QVector3D v2, QVector3D btw, double* l){
    QVector3D Ex, Ey, N;
    double x1, x2, y1, y2;
    double x;

    N = QVector3D::crossProduct(v1,v2);
    N.normalize();

    Ex =  btw - N*QVector3D::dotProduct(N,btw);
    Ex.normalize();

    Ey = QVector3D::crossProduct(N,Ex);
    Ey.normalize();

    x1 = QVector3D::dotProduct(Ex,v1);
    x2 = QVector3D::dotProduct(Ex,v2);
    y1 = QVector3D::dotProduct(Ey,v1);
    y2 = QVector3D::dotProduct(Ey,v2);

    //special cases
    if(y1 == y2) return false;

    if(x1 == x2){
        if(y1*y2 <= 0){
            *l = x1;
            return true;
        }else{
            return false;
        }
    }

    //general case
    x = (y1*x2 - y2*x1)/(y1 - y2);
    if(std::abs(std::abs(x-x1) + std::abs(x-x2) - std::abs(x1-x2)) < 1e-6){
        *l = x;
        return true;
    }else{
        return false;
    }
}

void buildOrtsToN(QVector3D N, QVector3D& v1, QVector3D& v2){
    //Create two arbitrary perpendicular orts to Rba
    QVector3D ort(0,0,0);

    double lx = std::abs(N.x());
    double ly = std::abs(N.y());
    double lz = std::abs(N.z());

    if(lx <= ly && ly <= lz){
        ort.setX(1);
    }else if(ly <= lx && lx <= lz){
        ort.setY(1);
    }else{
        ort.setZ(1);
    }

    v1 = QVector3D::crossProduct(N,ort);
    v1.normalize();

    v2 = QVector3D::crossProduct(N,v1);
    v2.normalize();
}

QVector2D projectPoint(QVector3D P, QVector3D Nx, QVector3D Ny){

    double x = QVector3D::dotProduct(Nx, P);
    double y = QVector3D::dotProduct(Ny, P);

    return QVector2D(x,y);
}

void clearLines(QList<QList<QVector3D>*>* lines){
    if(!lines) return;
    for(int i = 0; i < lines->size(); i++){
        lines->at(i)->clear();
        delete lines->at(i);
    }
    lines->clear();
}
