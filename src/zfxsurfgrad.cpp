#include "zfxsurfgrad.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <random>

#include "gui/glwidget.h"

ZfxSurfGrad::ZfxSurfGrad(QVector3D Rc, QVector3D Ecx, QVector3D Ecy, QVector3D Ecz, WFNData* wfn){
    this->Rc = Rc;
    this->Ecx = Ecx;
    this->Ecy = Ecy;
    this->Ecz = Ecz;

    this->wfn = wfn;
}

ZfxSurfGrad::~ZfxSurfGrad()
{
    clear();
    wfn = NULL;
}

void ZfxSurfGrad::clear(){
    for(int i = 0; i < grad_lines.size(); i++){
        grad_lines.at(i)->clear();
        delete grad_lines.at(i);
    }
    grad_lines.clear();

    for(int i = 0; i < gradverts.size(); i++){
        delete [] gradverts.at(i);
    }
    gradverts.clear();

    for(int i = 0; i < triangles.size(); i++){
        delete [] triangles.at(i);
    }
    triangles.clear();
}

void ZfxSurfGrad::draw(QVector3D dv){
    draw_points(dv);
}

void ZfxSurfGrad::draw_lines(QVector3D dv){
    for(int i = 0; i < grad_lines.size(); i++){
        glBegin(GL_LINE_STRIP);
        glColor3f (0.3, 0.3, 0.3);

        for(int j = 0; j < grad_lines.at(i)->size(); j++){
            glVertex3f(grad_lines.at(i)->at(j).x() - dv.x(),
                       grad_lines.at(i)->at(j).y() - dv.y(),
                       grad_lines.at(i)->at(j).z() - dv.z());
        }

        glEnd();
    }
}

void ZfxSurfGrad::draw_surface_grad(QVector3D dv){
    for(int i = 0; i < gradverts.size(); i++){
        for(int j = 0; j < gradverts.at(i)->adj.size(); j++){
            if(i < gradverts.at(i)->adj.at(j)){
                glBegin(GL_LINE_STRIP);
                glColor3f (0.3, 0.3, 0.3);

                glVertex3f(gradverts.at(i)->r.x() - dv.x(),
                           gradverts.at(i)->r.y() - dv.y(),
                           gradverts.at(i)->r.z() - dv.z());

                glVertex3f(gradverts.at(gradverts.at(i)->adj.at(j))->r.x() - dv.x(),
                           gradverts.at(gradverts.at(i)->adj.at(j))->r.y() - dv.y(),
                           gradverts.at(gradverts.at(i)->adj.at(j))->r.z() - dv.z());
                glEnd();
            }
        }
    }
}

void ZfxSurfGrad::draw_points(QVector3D dv){
    //glPointSize(1);

    for(Point_with_normal_3_list_iterator it = rand_points.begin(); it != rand_points.end(); ++it){
        glBegin(GL_LINE_STRIP);
        glColor3f (0.3, 0.3, 0.7);

        glVertex3f(it->first.x() - dv.x(),
                   it->first.y() - dv.y(),
                   it->first.z() - dv.z());
        glVertex3f(it->first.x() + it->second.x() - dv.x(),
                   it->first.y() + it->second.y() - dv.y(),
                   it->first.z() + it->second.z() - dv.z());

        glEnd();
    }
    //glPointSize(10);
}



void ZfxSurfGrad::calcGradLines(){
    clear();

    QVector3D st;
    QList<QVector3D>* line;

    for(int i = 0; i < N; i++){
        st = Rc + eps*(Ecx*cos(2*M_PI*i/N) + Ecy*sin(2*M_PI*i/N));
        line = new QList<QVector3D>;
        calcGradLine(st, 0.05, 10, line);
        grad_lines.append(line);
    }
}

void ZfxSurfGrad::calcGradLine(QVector3D st, double step, double len, QList<QVector3D>* dst){

    bool iterate = true;
    int counter = 1;
    double *grad = new double[3], rho;
    QVector3D G;

    while(iterate && (counter*step < len)){

        dst->append(st);

        rho = wfn->calc_grad_rho(st.x(),st.y(),st.z(),grad,true);
        G = QVector3D(grad[0],grad[1],grad[2]);

        if(rho < 1e-5 || G.length() < 1e-5){
            iterate = false;
            continue;
        }

        G.normalize();
        st += -step*G;
        counter++;
    }

    delete [] grad;
}

bool ZfxSurfGrad::compRc(QVector3D pt){
    pt -= Rc;
    return (pt.length() < 1e-6);
}

void ZfxSurfGrad::calcGradSurface(){
    clear();

    const double L = 0.5; //max edge

    QVector3D *curr = new QVector3D[N];
    QList<QVector3D> gradline;

    gradverts.append(new Vertex(Rc));

    //create surface
    for(int j = 0; j < 20; j++)
    for(int i = 0; i < N; i++){
        if(j == 0){
            //concentric triangles
            calcGradLine(Rc + eps*(Ecx*cos(2*M_PI*i/N) + Ecy*sin(2*M_PI*i/N)),0.01,L-eps,&gradline);

            if(i > 0){
                triangles.append(new TriVert(0,i,i+1));
            }else{
                triangles.append(new TriVert(0,N,1));
            }
        }else{
            //triangle stripes
            calcGradLine(curr[i],0.01,L,&gradline);

            if(i > 0){
                triangles.append(new TriVert((j-1)*N + i,j*N+i,j*N+i+1));
                triangles.append(new TriVert((j-1)*N + i,j*N+i+1,(j-1)*N+i+1));
            }else{
                triangles.append(new TriVert(j*N,(j+1)*N,j*N+1));
                triangles.append(new TriVert(j*N,j*N+1,(j-1)*N+1));
            }
        }

        gradverts.append(new Vertex(gradline.last()));
        curr[i] = gradline.last();
        gradline.clear();
    }

    //estimate point connectivity
    for(int i = 0; i < triangles.size(); i++){
        //TODO: here can appear a dublications
        gradverts.at(triangles.at(i)->v1)->adj.append(triangles.at(i)->v2);
        gradverts.at(triangles.at(i)->v1)->adj.append(triangles.at(i)->v3);

        gradverts.at(triangles.at(i)->v2)->adj.append(triangles.at(i)->v1);
        gradverts.at(triangles.at(i)->v2)->adj.append(triangles.at(i)->v3);

        gradverts.at(triangles.at(i)->v3)->adj.append(triangles.at(i)->v1);
        gradverts.at(triangles.at(i)->v3)->adj.append(triangles.at(i)->v2);
    }

    delete [] curr;
}


void ZfxSurfGrad::fillPolyhedron(Polyhedron* polyhedron){
    const double L = 0.3; //max edge
    QList<QVector3D> gradline;
    Halfedge_handle a = polyhedron->make_triangle(), b;

    polyhedron->clear();
    polyhedron->reserve(1 + N,3*N + N,N);

    //concentric triangles
    a = polyhedron->make_triangle();

    for(int i = 0; i < N; i++){
       calcGradLine(Rc + eps*(Ecx*cos(2*M_PI*i/N) + Ecy*sin(2*M_PI*i/N)),0.01,L-eps,&gradline);

       if(i == 0){
           b = a;
       }else if( i > 0 && i < 3){
           b = a->prev();
           a = b;
       }else{
           b = polyhedron->split_edge(a);
           a = b;
       }

       b->vertex()->point() = Point_3(gradline.last().x(),gradline.last().y(),gradline.last().z());
       gradline.clear();
    }

    b = polyhedron->create_center_vertex(a);
    b->vertex()->point() = Point_3(Rc.x(),Rc.y(),Rc.z());

    //stripes of surface
    a = a->opposite();
    for(int j = 0; j < R/L; j++){
        //make saw
        for(int i = 0; i < N; i++){
            calcGradLine(QVector3D(a->vertex()->point().x(),a->vertex()->point().y(),a->vertex()->point().z()),0.01,L,&gradline);
            b = polyhedron->add_vertex_and_facet_to_border(a,a->next());
            b->vertex()->point() = Point_3(gradline.last().x(),gradline.last().y(),gradline.last().z());
            gradline.clear();
            a = b->opposite();
        }
        //close each cog
        for(int i = 0; i < N; i++){
            b = polyhedron->add_facet_to_border(a->prev(),a->next());
            a = b->opposite()->next();
        }
    }
}

void ZfxSurfGrad::fillPoints(double density, Point_with_normal_3_list* pt_list){

    Point_with_normal_3_list* dest;
    Polyhedron polyhedron;
    Halfedge_handle h;
    QVector3D A, B, C, N, P;

    if(pt_list){
        dest = pt_list;
    }else{
        dest = &rand_points;
    }

    fillPolyhedron(&polyhedron);
    std::default_random_engine generator;
    std::poisson_distribution<int> distribution;

    double a, x, y;
    int k, r;

    for(Facet_iterator fi = polyhedron.facets_begin(); fi != polyhedron.facets_end(); ++fi){
        h = fi->facet_begin();

        A = halfedgePoint(h);
        B = halfedgePoint(h->next());
        C = halfedgePoint(h->prev());
        N = QVector3D::crossProduct(B-A,C-A);

        a = N.length()*density/2;
        k = 0;

        distribution = std::poisson_distribution<int>(a);
        r = distribution(generator);

        while(k < r){
            x = (double)rand()/RAND_MAX;
            y = (double)rand()/RAND_MAX;

            P = uv_transform_tensorial(QVector3D(0,0,0),QVector3D(1,0,0),QVector3D(0,1,0),
                                       QVector3D(x,y*(1-x),0),A,B,C);

            dest->insert(dest->end(),
                               Point_with_normal_3_pair(Point_3(P.x(),P.y(),P.z()),Vector_3(0,0,0)));

            k++;
        }
    }

    CGAL::pca_estimate_normals(dest->begin(), dest->end(),
                               CGAL::First_of_pair_property_map<Point_with_normal_3_pair>(),
                               CGAL::Second_of_pair_property_map<Point_with_normal_3_pair>(),6);

    //arrange normals manually(mst does not compile)
    for(Point_with_normal_3_list_iterator it = dest->begin(); it != dest->end(); ++it){
        if(QVector3D::dotProduct(QVector3D(it->second.x(),it->second.y(),it->second.z()),Ecz) < 0){
            it->second = Vector_3(-it->second.x(),-it->second.y(),-it->second.z());
        }
    }
}
