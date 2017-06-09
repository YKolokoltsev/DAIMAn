#ifndef ZFXSURFGRAD_H
#define ZFXSURFGRAD_H

#include <QVector3D>
#include <QVector2D>
#include <QList>
#include "wavefunction/wfndata.h"

#include "cgal_routines.h"


typedef struct TriVert{
    int v1;
    int v2;
    int v3;
    TriVert(int v1, int v2, int v3){
        this->v1 = v1;
        this->v2 = v2;
        this->v3 = v3;
    }
} TriVert;

typedef struct Vertex
{
    Vertex(QVector3D r)
    {
        this->r = r;
        closed = false;
    }

    Vertex& operator= (const Vertex& v)
    {
     if(this == &v) return *this;
     this->adj = v.adj;
     this->r = v.r;
     this->closed = v.closed;
     return *this;
    }

public:
    QList<int> adj;
    QVector3D r;
    bool closed;

} Vertex;

class ZfxSurfGrad{
public:
    ZfxSurfGrad(QVector3D Rc, QVector3D Ecx, QVector3D Ecy, QVector3D Ecz, WFNData* wfn);
    ~ZfxSurfGrad();

public:
    void draw(QVector3D dv);

private:
    void draw_lines(QVector3D dv); //opengl drawing gradients
    void draw_surface_grad(QVector3D dv); //opengl drawing surface
    void draw_points(QVector3D dv);

private:
    constexpr static const int N = 20;
    constexpr static const double R = 5;
    constexpr static const double eps = 1e-1;

    QVector3D Rc;     //Bond critical point
    QVector3D Ecz;    //critical point axis along bond line in direction of positive hessian eigenvector
    QVector3D Ecx;    //first negative hessian eigenvector
    QVector3D Ecy;    //second negative hessian eigenvector

    WFNData* wfn;

private:
    QList<QList<QVector3D>* > grad_lines; //set of gradient lines
    QList<TriVert*> triangles; //triangles are described with indices of a vertices
    QList<Vertex*> gradverts; //vertices on the surface obtained by the gradient lines
    Point_with_normal_3_list rand_points;

public:
    void calcGradLines(); //fill out a set of gradient lines
    bool compRc(QVector3D pt); //compares an argument point with a stored Rc
    void calcGradSurface(); //Create a surface of triangles
    void fillPolyhedron(Polyhedron* polyhedron); //Fill the given polyhedron with tryangles
    void fillPoints(double density, Point_with_normal_3_list* pt_list = NULL); //Fill the given list with on surface points and their normals

private:
    void clear();
    void calcGradLine(QVector3D st, double step, double len, QList<QVector3D>* dst); //create a gradient line of a given length
};

#endif // ZFXSURFGRAD_H
