#ifndef CGAL_SURFACE_H
#define CGAL_SURFACE_H

#include <QVector2D>
#include <QList>

#include "cgal_routines.h"

class CGALSurface{
public:
    CGALSurface();
    ~CGALSurface();

public:
    void removeZeroTriangles();
    void collapse(double ratio);
    bool parameterize();
    void clear_parameterization();
    bool setMeshVectors(QVector3D Pt, QVector3D Ex, QVector3D Ey);
    bool buildMesh(double L, int steps);
    bool save_parameterization_eps(const char *pFilename, double scale);
    bool createBasingOnPoints(Point_with_normal_3_list* points);

public:
    void draw_polyhedron(QVector3D dv);
    void draw_border(QVector3D dv);
    void draw_mesh(QVector3D dv);

public:
    Polyhedron* getSurface(){return &polyhedron;} //to fill it with data
    void print_polyhedron_info();

private:
    Polyhedron polyhedron;

    Parameterization_polyhedron_adaptor *parameterization;
    Bbox_2 uv_box;

    QList<QList<QVector3D>* > mesh_lines;

    QVector2D UVPt;
    QVector2D UVx;
    QVector2D UVy;

    double parameterization_metric;

private:
    //internal subroutines
    bool collapseEdge(Halfedge_handle hi); //sometimes fails
    bool eraseLoneFacet(Halfedge_handle a);
    bool uv2Point3(QVector3D& dest, QVector2D uv);
    bool uvNormal3(QVector3D& dest, QVector2D uv);

    void calc_uv_box();
    void calc_parameterization_metric();

    void ortMeshLines(QVector2D UVo, QVector2D Ort, double L, int steps);
};

#endif // CGAL_SURFACE_H
