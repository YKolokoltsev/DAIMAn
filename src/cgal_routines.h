#ifndef CGAL_ROUTINES_H
#define CGAL_ROUTINES_H

#define CGAL_EIGEN3_ENABLED

#include <stack>
#include <vector>
#include <utility>
#include <list>
#include <fstream>
#include <QVector3D>
#include <QVector2D>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/vector.h>
#include <CGAL/Point_with_normal_3.h>


#include <CGAL/pca_estimate_normals.h>
#include <CGAL/property_map.h>

#include <CGAL/IO/output_surface_facets_to_polyhedron.h>
#include <CGAL/Eigen_solver_traits.h>
#include <CGAL/Surface_mesh_default_triangulation_3.h>
#include <CGAL/make_surface_mesh.h>
#include <CGAL/Implicit_surface_3.h>
#include <CGAL/compute_average_spacing.h>
#include <CGAL/Poisson_reconstruction_function.h>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/squared_distance_3.h>
#include <CGAL/Parameterization_polyhedron_adaptor_3.h>
#include <CGAL/parameterize.h>
#include <CGAL/Triangle_2.h>

#include <CGAL/Surface_mesh_simplification/HalfedgeGraph_Polyhedron_3.h>
#include <CGAL/Surface_mesh_simplification/edge_collapse.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Count_ratio_stop_predicate.h>

#include <CGAL/Discrete_authalic_parameterizer_3.h>
#include <CGAL/Discrete_conformal_map_parameterizer_3.h>

//typedef CGAL::Simple_cartesian<double> Kernel;
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef CGAL::Polyhedron_3<Kernel> Polyhedron;

//parameterizers
typedef CGAL::Parameterization_polyhedron_adaptor_3<Polyhedron> Parameterization_polyhedron_adaptor;
typedef CGAL::Discrete_authalic_parameterizer_3<Parameterization_polyhedron_adaptor> Parameterizer;

//Misc
typedef Polyhedron::Halfedge_handle Halfedge_handle;
typedef Polyhedron::Halfedge_const_handle Halfedge_const_handle;
typedef Polyhedron::Halfedge_around_vertex_const_circulator Halfedge_around_vertex_const_circulator;
typedef Polyhedron::Halfedge_iterator Halfedge_iterator;
typedef Polyhedron::Vertex_const_handle Vertex_const_handle;
typedef Polyhedron::Vertex_const_iterator Vertex_const_iterator;
typedef Polyhedron::Facet_handle Facet_handle;
typedef Polyhedron::Facet_iterator Facet_iterator;
typedef Polyhedron::Facet_const_iterator Facet_const_iterator;

//basic
typedef Kernel::FT FT;
typedef CGAL::Point_3<Kernel> Point_3;
typedef CGAL::Bbox_3 Bbox_3;
typedef CGAL::Sphere_3<Kernel> Sphere_3;
typedef CGAL::Vector_3<Kernel> Vector_3;
typedef CGAL::Bbox_2 Bbox_2;
typedef CGAL::Triangle_2<Kernel> Triangle_2;
typedef CGAL::Point_2<Kernel> Point_2;

//3d mesh building from pointset
typedef CGAL::Poisson_reconstruction_function<Kernel> Poisson_reconstruction_function;
typedef CGAL::Surface_mesh_default_triangulation_3 STr;
typedef CGAL::Surface_mesh_complex_2_in_triangulation_3<STr> C2t3;
typedef CGAL::Implicit_surface_3<Kernel, Poisson_reconstruction_function> Surface_3;

//spesial type for mesh building
typedef CGAL::Point_with_normal_3<Kernel> pwn;
typedef std::list<pwn> pwn_list;

//point set
typedef std::pair<Point_3, Vector_3> Point_with_normal_3_pair;
typedef std::list<Point_with_normal_3_pair> Point_with_normal_3_list;
typedef std::list<Point_with_normal_3_pair>::iterator Point_with_normal_3_list_iterator;


double getEdgeLength(Halfedge_handle a);
bool is_valid_join(Halfedge_handle a);
bool is_border_edge(Halfedge_handle a);
QVector3D halfedgePoint(Halfedge_const_handle h);

QVector3D uv_transform_linear(QVector3D a, QVector3D b, QVector3D c, QVector3D p, QVector3D A, QVector3D B, QVector3D C);
QVector3D uv_transform_tensorial(QVector3D a, QVector3D b, QVector3D c, QVector3D p, QVector3D A, QVector3D B, QVector3D C);
bool findVecBetween(QVector3D v1, QVector3D v2, QVector3D btw, double* l);
void buildOrtsToN(QVector3D N, QVector3D& v1, QVector3D& v2);
QVector2D projectPoint(QVector3D P, QVector3D Nx, QVector3D Ny);
void clearLines(QList<QList<QVector3D>*>* lines);

#endif // CGAL_ROUTINES_H
