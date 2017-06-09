#include <fstream>

#include <math.h>

#include "cgal_surface.h"
#include "gui/glwidget.h"

CGALSurface::CGALSurface(){
    UVx = QVector2D(1,0);
    UVy = QVector2D(0,1);

    parameterization = NULL;
    parameterization_metric = 1;
}

CGALSurface::~CGALSurface(){
    clear_parameterization();
}

void CGALSurface::clear_parameterization(){
    if(parameterization){
        delete parameterization;
        parameterization = NULL;
    }

    for(int i = 0; i < mesh_lines.size(); i++){
        mesh_lines.at(i)->clear();
        delete mesh_lines.at(i);
    }
    mesh_lines.clear();
}

void CGALSurface::draw_polyhedron(QVector3D dv){

    for(Halfedge_iterator hi = polyhedron.halfedges_begin(); hi != polyhedron.halfedges_end(); ++hi){
        if(!hi->is_border_edge()){
            glBegin(GL_LINE_STRIP);
            glColor3f (0.8, 0.8, 1);

            glVertex3f(hi->vertex()->point().x() - dv.x(),
                       hi->vertex()->point().y() - dv.y(),
                       hi->vertex()->point().z() - dv.z());

            glVertex3f(hi->opposite()->vertex()->point().x() - dv.x(),
                       hi->opposite()->vertex()->point().y() - dv.y(),
                       hi->opposite()->vertex()->point().z() - dv.z());
            glEnd();
        }
    }
}

void CGALSurface::draw_border(QVector3D dv){
    if(!polyhedron.is_valid()) return;
    if(!polyhedron.normalized_border_is_valid())
        polyhedron.normalize_border();

    Halfedge_handle a, b = NULL;
    Halfedge_iterator hi;

    //find any border halfedge
    for(hi = polyhedron.border_halfedges_begin(); hi !=  polyhedron.halfedges_end(); ++hi)
        if(hi->is_border()) {b = hi; a = hi; break;}

    if(b == NULL) return;

    glBegin(GL_LINE_LOOP);
    glColor3f (1.0, 0.3, 0.3);
    do{
        glVertex3f(b->vertex()->point().x() - dv.x(),
                   b->vertex()->point().y() - dv.y(),
                   b->vertex()->point().z() - dv.z());
        b = b->next();
    }while(b != a);
    glEnd();
}


void CGALSurface::draw_mesh(QVector3D dv){

    draw_polyhedron(dv);
    for(int i = 0; i <  mesh_lines.size(); i++){
        glBegin(GL_LINE_STRIP);
        glColor3f (0, 0, 0);

        for(int j = 0; j < mesh_lines.at(i)->size(); j++){
            glVertex3f(mesh_lines.at(i)->at(j).x() - dv.x(),
                       mesh_lines.at(i)->at(j).y() - dv.y(),
                       mesh_lines.at(i)->at(j).z() - dv.z());
        }

        glEnd();
    }

    draw_border(dv);
}

//Len(x,y,z) = metric*Len(U.V) (this concept imply the area distorsion minimization surface parameterization)
void CGALSurface::calc_parameterization_metric(){
    Halfedge_const_handle ha;
    const Polyhedron& mesh = parameterization->get_adapted_mesh();

    QVector3D A, B, C;
    QVector3D a, b, c;
    double area = 0, Area = 0;

    for(Facet_const_iterator fi = mesh.facets_begin(); fi != mesh.facets_end(); ++fi){
        ha = fi->facet_begin();

        a = QVector3D(parameterization->info(ha)->uv().x(), parameterization->info(ha)->uv().y(),0);
        b = QVector3D(parameterization->info(ha->next())->uv().x(), parameterization->info(ha->next())->uv().y(),0);
        c = QVector3D(parameterization->info(ha->prev())->uv().x(), parameterization->info(ha->prev())->uv().y(),0);

        A = halfedgePoint(ha);
        B = halfedgePoint(ha->next());
        C = halfedgePoint(ha->prev());

        area += QVector3D::crossProduct(b - a, c - a).length();
        Area += QVector3D::crossProduct(B - A, C - A).length();
    }

    /**
     *
     * L = metric * l;
     * Area = a*L^2 = a*metric^2 * l^2 = area;
     * => metric = sqrt(L^2/l^2);
     *
     **/

    parameterization_metric = sqrt(Area/area);
}

bool CGALSurface::save_parameterization_eps(const char *pFilename, double scale)
{
    if(!parameterization) return false;

    Polyhedron::Halfedge_const_iterator pHalfedge;
    const Polyhedron& mesh = parameterization->get_adapted_mesh();

    std::ofstream out(pFilename);
    if(!out) return false;

    CGAL::set_ascii_mode(out);


    out << "%!PS-Adobe-2.0 EPSF-2.0" << std::endl;
    out << "%%BoundingBox: " << int(scale*uv_box.xmin()+0.5) << " "
                                << int(scale*uv_box.ymin()+0.5) << " "
                                << int(scale*uv_box.xmax()+0.5) << " "
                                << int(scale*uv_box.ymax()+0.5) << std::endl;
    out << "%%HiResBoundingBox: " << scale*uv_box.xmin() << " "
                                    << scale*uv_box.ymin() << " "
                                    << scale*uv_box.xmax() << " "
                                    << scale*uv_box.ymax() << std::endl;
    out << "%%EndComments" << std::endl;
    out << "gsave" << std::endl;
    out << "0.1 setlinewidth" << std::endl;
    // color macros
    out << std::endl;
    out << "% RGB color command - r g b C" << std::endl;
    out << "/C { setrgbcolor } bind def" << std::endl;
    out << "/white { 1 1 1 C } bind def" << std::endl;
    out << "/black { 0 0 0 C } bind def" << std::endl;
    // edge macro -> E
    out << std::endl;
    out << "% Black stroke - x1 y1 x2 y2 E" << std::endl;
    out << "/E {moveto lineto stroke} bind def" << std::endl;
    out << "black" << std::endl << std::endl;
    // for each halfedge
    for (pHalfedge = mesh.halfedges_begin();
         pHalfedge != mesh.halfedges_end();
         pHalfedge++)
    {
        double x1 = scale * parameterization->info(pHalfedge->prev())->uv().x();
        double y1 = scale * parameterization->info(pHalfedge->prev())->uv().y();
        double x2 = scale * parameterization->info(pHalfedge)->uv().x();
        double y2 = scale * parameterization->info(pHalfedge)->uv().y();
        out << x1 << " " << y1 << " " << x2 << " " << y2 << " E" << std::endl;
    }
    /* Emit EPS trailer. */
    out << "grestore" << std::endl;
    out << std::endl;
    out << "showpage" << std::endl;
    return true;
}

void CGALSurface::print_polyhedron_info(){
    int za_facets = 0;
    Halfedge_handle a;

    for(Facet_iterator fi = polyhedron.facets_begin(); fi != polyhedron.facets_end(); ++fi){
        a = fi->facet_begin();

        if(CGAL::to_double(CGAL::squared_area(a->vertex()->point(),
                                              a->next()->vertex()->point(),
                                              a->prev()->vertex()->point())) < 1e-6){
            za_facets++;
        }
    }

    printf("Total facets: %d; "
           "Zero area facets: %d; "
           "is_pure_triangle: %s; "
           "is_valid: %s\n",
           (int)polyhedron.size_of_facets(),
           za_facets,
           polyhedron.is_pure_triangle() ? "true" : "false",
           polyhedron.is_valid(false) ? "true" : "false");
}

bool CGALSurface::eraseLoneFacet(Halfedge_handle hi){

    Halfedge_handle a = hi->is_border() ? hi->opposite() : hi;
    if(a->vertex()->vertex_degree() == 2){
        polyhedron.erase_facet(a);
        return true;
    }else if(a->prev()->vertex()->vertex_degree() == 2){
        polyhedron.erase_facet(a->prev());
        return true;
    }else if(a->next()->vertex()->vertex_degree() == 2){
        polyhedron.erase_facet(a->next());
        return true;
    }

    return false;
}

bool CGALSurface::collapseEdge(Halfedge_handle hi){
    Halfedge_handle a;

    a = hi->is_border() ? hi->opposite() : hi;

    if(is_border_edge(a)){

        if(is_border_edge(a->next()) || is_border_edge(a->prev())){
            return eraseLoneFacet(a);
        }

        if(a->next()->vertex()->vertex_degree() == 3){
            polyhedron.erase_center_vertex(a->next());
            return true;
        }

        is_valid_join(a->next());
        a = polyhedron.join_facet(a->next());
        polyhedron.join_vertex(a);

    }else{

        if(is_border_edge(a->next()) && is_border_edge(a->prev())){
            return eraseLoneFacet(a);
        }

        if(is_border_edge(a->opposite()->next()) && is_border_edge(a->opposite()->prev())){
            return eraseLoneFacet(a->opposite());
        }


        if(!is_border_edge(a->next())){

            if(a->vertex()->vertex_degree() == 3){
                polyhedron.erase_center_vertex(a);
                return true;
            }

            is_valid_join(a->next());
            a = polyhedron.join_facet(a->next());

            if(!is_border_edge(a->opposite()->next())){
                is_valid_join(a->opposite()->next());
                a = polyhedron.join_facet(a->opposite()->next());
            }else{
                is_valid_join(a->opposite()->prev());
                a = polyhedron.join_facet(a->opposite()->prev())->prev();
            }

            polyhedron.join_vertex(a);

        }else{

            if(a->prev()->vertex()->vertex_degree() == 3){
                polyhedron.erase_center_vertex(a->prev());
                return true;
            }

            is_valid_join(a->prev());
            a = polyhedron.join_facet(a->prev())->prev();

            if(!is_border_edge(a->opposite()->next())){
                is_valid_join(a->opposite()->next());
                a = polyhedron.join_facet(a->opposite()->next());
            }else{
                is_valid_join(a->opposite()->prev());
                a = polyhedron.join_facet(a->opposite()->prev())->prev();
            }

            polyhedron.join_vertex(a);
        }
    }

    return true;
}

bool CGALSurface::parameterize(){

    if(parameterization){
        delete parameterization;
    }

    parameterization = new Parameterization_polyhedron_adaptor(polyhedron);

    Parameterizer::Error_code err = CGAL::parameterize(*parameterization,Parameterizer());

    if(err != Parameterizer::OK){
        printf("parameterization failed with code %d\n", err);
        delete parameterization;
        parameterization = NULL;
        return false;
    }

    calc_uv_box();
    UVPt = QVector2D((uv_box.xmax() - uv_box.xmin())/2,(uv_box.ymax() - uv_box.ymin())/2);

    //if a middle point is on the mesh than we can also define standard UVx and UVy
    QVector3D XYZPt,XYZn, XYZx, XYZy;
    if(uvNormal3(XYZn, UVPt) && uv2Point3(XYZPt,UVPt)){
        buildOrtsToN(XYZn, XYZx, XYZy);
        setMeshVectors(XYZPt, XYZx, XYZy);
    };

    calc_parameterization_metric();

    return true;
}

void CGALSurface::removeZeroTriangles(){
    bool cont;
    Halfedge_handle a, b;
    do{
        cont = false;
        polyhedron.normalize_border();
        for(Facet_iterator fi = polyhedron.facets_begin(); fi != polyhedron.facets_end() && !cont; ++fi){

            a = fi->facet_begin();

            if(CGAL::to_double(CGAL::squared_area(a->vertex()->point(),
                                                  a->next()->vertex()->point(),
                                                  a->prev()->vertex()->point())) < 1e-3){

                if(getEdgeLength(a) < getEdgeLength(a->next())){
                    if(getEdgeLength(a) < getEdgeLength(a->prev())){
                        b = a;
                    }else{
                        b = a->prev();
                    }
                }else{
                    if(getEdgeLength(a->next()) < getEdgeLength(a->prev())){
                        b = a->next();
                    }else{
                        b = a->prev();
                    }
                }

                cont = collapseEdge(b);
            }

        }
    }while(cont);
}

bool CGALSurface::setMeshVectors(QVector3D Pt, QVector3D Ex, QVector3D Ey){
    if(!parameterization) return false;

    Halfedge_const_handle ha, ha_save;
    Halfedge_around_vertex_const_circulator hcirc ;
    Vertex_const_handle hv;
    const Polyhedron& mesh = parameterization->get_adapted_mesh();
    double norm = 1e10, cur_norm;

    QVector3D P = Pt;
    QVector3D A, B, C, N, Ps, Ps_save;
    QVector3D a,b,c, ps;
    QVector3D AB, AC, BC, APs;
    QVector3D dPsAB, dPsAC, dPsBC, Ntri;
    QVector3D Pnc, UV1, UV2;
    double alpha, M;


    //find the nearest to P surface vertex
    for(Vertex_const_iterator vi = mesh.vertices_begin(); vi != mesh.vertices_end(); ++vi){
        cur_norm = sqrt(pow(P.x()-vi->point().x(),2) +
                            pow(P.y()-vi->point().y(),2) +
                            pow(P.z()-vi->point().z(),2));
        if(norm > cur_norm)
        {
            norm = cur_norm;
             hv = vi;
        }
    }

    /**
     *  Each triangle corresponds to surface:
     *  N * r = M; => M = N*A = N*B = N*C
     *
     *  The point pulled down on this surface from point P is:
     *  Ps = P + alpha * N, where
     *  alpha = (M - N*P)/N^2
     **/

    //find a 3D projection on the surface
    norm = 1e10;
    hcirc = hv->vertex_begin();
    do{
        ha = hcirc;

        A = halfedgePoint(ha);
        B = halfedgePoint(ha->next());
        C = halfedgePoint(ha->prev());

        AB = B - A;
        AC = C - A;
        BC = C - B;

        N = QVector3D::crossProduct(AB,AC);
        M = QVector3D::dotProduct(N,A);
        alpha = (M - QVector3D::dotProduct(N,P)/N.lengthSquared());

        Ps = P + alpha*N;

        APs = Ps - A;

        //finally, lets see if the point Ps lies inside a triangle ABC:
        cur_norm = QVector3D::crossProduct(AB,APs).length() +
                   QVector3D::crossProduct(AC,APs).length() +
                   QVector3D::crossProduct(BC,Ps-B).length() -
                   QVector3D::crossProduct(AB,AC).length();

        if(norm > cur_norm){
            norm = cur_norm;
            Ps_save = Ps;
            ha_save = ha;
        }
    }while(++hcirc != hv->vertex_begin());

    Ps = Ps_save;
    ha = ha_save;

    A = halfedgePoint(ha);
    B = halfedgePoint(ha->next());
    C = halfedgePoint(ha->prev());

    AB = B - A;
    AC = C - A;
    BC = C - B;

    Ntri = QVector3D::crossProduct(AB,AC);


    /**
     *  Check that a point is outside the nearest triangle, and find a projection of the point PS onto
     *  the nearest edge of triangle. For this we calculate all three projections and find the shortest.
     *
     *  N - normal to the edge, for AB is:
     *  N = Ntri x AB, where Ntri - is normal to triangle = AB x AC
     *
     *  an equation is:
     *
     *  Ps + dPs = AB*t + A; where A - is any point at line of the edge
     *  and dPs = alpha * N
     *
     *  therefore by multiplying on N we have:
     *  alpha = (A - Ps)/N^2, and doues not depend from A on the line (can be B for example)
     *
    **/
   if(norm > 1e-6) {

        //AB
        N = QVector3D::crossProduct(Ntri,AB);
        dPsAB = N*QVector3D::dotProduct(A - Ps,N)/N.lengthSquared();

        //AC
        N = QVector3D::crossProduct(Ntri,AC);
        dPsAC = N*QVector3D::dotProduct(A - Ps,N)/N.lengthSquared();

        //BC
        N = QVector3D::crossProduct(Ntri,BC);
        dPsBC = N*QVector3D::dotProduct(B - Ps,N)/N.lengthSquared();

        //and find shortest
        if(dPsAB.length() < dPsAC.length()){
            if(dPsAB.length() < dPsBC.length()){
                Ps = Ps + dPsAB;
            }else{
                Ps = Ps + dPsBC;
            }
        }else{
            if(dPsAC.length() < dPsBC.length()){
                Ps = Ps + dPsAC;
            }else{
                Ps = Ps + dPsBC;
            }
        }
    }

    a = QVector3D(parameterization->info(ha)->uv().x(),parameterization->info(ha)->uv().y(),0);
    b = QVector3D(parameterization->info(ha->next())->uv().x(),parameterization->info(ha->next())->uv().y(),0);
    c = QVector3D(parameterization->info(ha->prev())->uv().x(),parameterization->info(ha->prev())->uv().y(),0);

    //find UV coordinates of the critical point
    ps = uv_transform_tensorial(A,B,C,Ps,a,b,c);
    UVPt = QVector2D(ps.x(),ps.y());

    //find hessian eigenvectors in UV coordinates (UVcx)
    Pnc = Ex - Ntri*QVector3D::dotProduct(Ntri,Ex)/Ntri.lengthSquared();
    Pnc.normalize();

    if(findVecBetween(B-A,C-A,Pnc,&alpha)){
        UV1 = uv_transform_tensorial(A,B,C,A,a,b,c);
        UV2 = uv_transform_tensorial(A,B,C,A + alpha*Pnc,a,b,c);
    }else if(findVecBetween(A-B,C-B,Pnc,&alpha)){
        UV1 = uv_transform_tensorial(A,B,C,B,a,b,c);
        UV2 = uv_transform_tensorial(A,B,C,B + alpha*Pnc,a,b,c);
    }else if(findVecBetween(A-C,B-C,Pnc,&alpha)){
        UV1 = uv_transform_tensorial(A,B,C,C,a,b,c);
        UV2 = uv_transform_tensorial(A,B,C,C + alpha*Pnc,a,b,c);
    }else{
        UV1 = QVector3D(0,0,0);
        UV2 = QVector3D(1,0,0);
        printf("ERROR FINDING UVcx\n");
    }

    UVx = QVector2D(UV2.x() - UV1.x(),UV2.y() - UV1.y());

    //find hessian eigenvectors in UV coordinates (UVcy)
    Pnc = Ey - Ntri*QVector3D::dotProduct(Ntri,Ey)/Ntri.lengthSquared();
    Pnc.normalize();

    if(findVecBetween(B-A,C-A,Pnc,&alpha)){
        UV1 = uv_transform_tensorial(A,B,C,A,a,b,c);
        UV2 = uv_transform_tensorial(A,B,C,A + alpha*Pnc,a,b,c);
    }else if(findVecBetween(A-B,C-B,Pnc,&alpha)){
        UV1 = uv_transform_tensorial(A,B,C,B,a,b,c);
        UV2 = uv_transform_tensorial(A,B,C,B + alpha*Pnc,a,b,c);
    }else if(findVecBetween(A-C,B-C,Pnc,&alpha)){
        UV1 = uv_transform_tensorial(A,B,C,C,a,b,c);
        UV2 = uv_transform_tensorial(A,B,C,C + alpha*Pnc,a,b,c);
    }else{
        UV1 = QVector3D(0,0,0);
        UV2 = QVector3D(1,0,0);
        printf("ERROR FINDING UVcy\n");
    }

    UVy = QVector2D(UV2.x() - UV1.x(),UV2.y() - UV1.y());

    return true;
}

bool CGALSurface::uvNormal3(QVector3D& dest, QVector2D uv){
    const Polyhedron& mesh = parameterization->get_adapted_mesh();
    Halfedge_const_handle ha, ht;

    QVector3D A, B, C;
    QVector3D a, b, c, p, vb, vc;
    double delta = 10, curr;
    p = QVector3D(uv.x(),uv.y(),0);

    //find a triangle (ht of its edge) which contains a point uv
    for(Facet_const_iterator fi = mesh.facets_begin(); fi != mesh.facets_end(); ++fi){
        ha = fi->facet_begin();

        a = QVector3D(parameterization->info(ha)->uv().x(),parameterization->info(ha)->uv().y(),0);
        b = QVector3D(parameterization->info(ha->next())->uv().x(),parameterization->info(ha->next())->uv().y(),0);
        c = QVector3D(parameterization->info(ha->prev())->uv().x(),parameterization->info(ha->prev())->uv().y(),0);

        vb = b - a;
        vc = c - a;

        curr = std::abs(QVector3D::crossProduct(a-p,b-p).length() +
                    QVector3D::crossProduct(a-p,c-p).length() +
                    QVector3D::crossProduct(b-p,c-p).length() -

                    QVector3D::crossProduct(vb,vc).length());

        if(curr < delta){
            delta = curr;
            ht = ha;
        }

    }

    if(delta > 1e-5) return false;

    A = halfedgePoint(ht);
    B = halfedgePoint(ht->next());
    C = halfedgePoint(ht->prev());

    dest = QVector3D::crossProduct(B-A,C-A);
    dest.normalize();

    return true;
}

bool CGALSurface::uv2Point3(QVector3D& dest, QVector2D uv){
    const Polyhedron& mesh = parameterization->get_adapted_mesh();
    Halfedge_const_handle ha, ht;

    QVector3D A, B, C;
    QVector3D a, b, c, p, vb, vc;
    double delta = 10, curr;
    p = QVector3D(uv.x(),uv.y(),0);

    //find a triangle (ht of its edge) which contains a point uv
    for(Facet_const_iterator fi = mesh.facets_begin(); fi != mesh.facets_end(); ++fi){
        ha = fi->facet_begin();

        a = QVector3D(parameterization->info(ha)->uv().x(),parameterization->info(ha)->uv().y(),0);
        b = QVector3D(parameterization->info(ha->next())->uv().x(),parameterization->info(ha->next())->uv().y(),0);
        c = QVector3D(parameterization->info(ha->prev())->uv().x(),parameterization->info(ha->prev())->uv().y(),0);

        vb = b - a;
        vc = c - a;

        curr = std::abs(QVector3D::crossProduct(a-p,b-p).length() +
                    QVector3D::crossProduct(a-p,c-p).length() +
                    QVector3D::crossProduct(b-p,c-p).length() -

                    QVector3D::crossProduct(vb,vc).length());

        if(curr < delta){
            delta = curr;
            ht = ha;
        }

    }

    if(delta > 1e-5) return false;

    //2D Math
    a = QVector3D(parameterization->info(ht)->uv().x(),parameterization->info(ht)->uv().y(),0);
    b = QVector3D(parameterization->info(ht->next())->uv().x(),parameterization->info(ht->next())->uv().y(),0);
    c = QVector3D(parameterization->info(ht->prev())->uv().x(),parameterization->info(ht->prev())->uv().y(),0);

    A = halfedgePoint(ht);
    B = halfedgePoint(ht->next());
    C = halfedgePoint(ht->prev());

    dest = uv_transform_tensorial(a,b,c,p,A,B,C);
    return true;
}

void CGALSurface::calc_uv_box(){
    const Polyhedron& mesh = parameterization->get_adapted_mesh();
    double xmin,xmax,ymin,ymax;
    Polyhedron::Halfedge_const_iterator pHalfedge;

    xmin = ymin = xmax = ymax = 0;
    for (pHalfedge = mesh.halfedges_begin(); pHalfedge != mesh.halfedges_end(); pHalfedge++)
    {
        double x1 = parameterization->info(pHalfedge->prev())->uv().x();
        double y1 = parameterization->info(pHalfedge->prev())->uv().y();
        double x2 = parameterization->info(pHalfedge)->uv().x();
        double y2 = parameterization->info(pHalfedge)->uv().y();
        xmin = (std::min)(xmin,x1);
        xmin = (std::min)(xmin,x2);
        xmax = (std::max)(xmax,x1);
        xmax = (std::max)(xmax,x2);
        ymax = (std::max)(ymax,y1);
        ymax = (std::max)(ymax,y2);
        ymin = (std::min)(ymin,y1);
        ymin = (std::min)(ymin,y2);
    }

    uv_box = Bbox_2(xmin,ymin,xmax,ymax);
}


void CGALSurface::ortMeshLines(QVector2D UVo, QVector2D Ort, double L, int steps){
    double hstrt; //half interval along Ort which defines a starting point
    double t, u, v;
    bool found;
    QVector2D XOrt; //A point on the Ort axis where the orthogonal mesh line should pass;
    QVector2D Lin;  //Unit Vector orthogonal to Ort along which we draw a line
    QVector3D P;
    QList<QVector3D>* p_line = NULL;
    QList<double> tt;//t params at which the line intersects uv_bbox

    hstrt = std::max(std::max((QVector2D(uv_box.xmin(),uv_box.ymin()) - UVo).length(),
                    (QVector2D(uv_box.xmax(),uv_box.ymin()) - UVo).length()),
                std::max((QVector2D(uv_box.xmin(),uv_box.ymax()) - UVo).length(),
                    (QVector2D(uv_box.xmax(),uv_box.ymax()) - UVo).length()));


    hstrt = floor((hstrt/L))*L;

    Ort.normalize();
    Lin = QVector2D(-Ort.y(),Ort.x());

    for(double h = 0; h < 2*hstrt; h+= L){

        XOrt = UVo +(h - hstrt)*Ort;

        found = false;
        tt.clear();


        if(pow(Lin.y(),2) > 1e-12){
            //Ymin = const
            t = (uv_box.ymin() - XOrt.y())/Lin.y();
            u = t*Lin.x() + XOrt.x();
            if(std::abs(std::abs(uv_box.xmin()-u) + std::abs(uv_box.xmax()-u) -
                        std::abs(uv_box.xmax() - uv_box.xmin())) < 1e-6){
                tt.append(t);
                found = true;
            }

            //YMax = const
            t = (uv_box.ymax() - XOrt.y())/Lin.y();
            u = t*Lin.x() + XOrt.x();
            if(std::abs(std::abs(uv_box.xmin()-u) + std::abs(uv_box.xmax()-u) -
                        std::abs(uv_box.xmax() - uv_box.xmin())) < 1e-6){
                tt.append(t);
                found = true;
            }
        }

        if(pow(Lin.x(),2) > 1e-12){
            //Xmin = const
            t = (uv_box.xmin() - XOrt.x())/Lin.x();
            v = t*Lin.y() + XOrt.y();
            if(std::abs(std::abs(uv_box.ymin()-v) + std::abs(uv_box.ymax()-v) -
                        std::abs(uv_box.ymax() - uv_box.ymin())) < 1e-6){
                tt.append(t);
                found = true;
            }

            //Xmax = const
            t = (uv_box.xmax() - XOrt.x())/Lin.x();
            v = t*Lin.y() + XOrt.y();
            if(std::abs(std::abs(uv_box.ymin()-v) + std::abs(uv_box.ymax()-v) -
                        std::abs(uv_box.ymax() - uv_box.ymin())) < 1e-6){
                tt.append(t);
                found = true;
            }
        }

        if(found){
            t = tt.at(0);
            for(int i = 0; i < tt.size(); i++)
                if(t > tt.at(i)) t = tt.at(i);

            u = t*Lin.x() + XOrt.x();
            v = t*Lin.y() + XOrt.y();
        }

        //draw line
        for(; found && std::abs(std::abs(uv_box.xmin()-u) + std::abs(uv_box.xmax()-u) -
                                 std::abs(uv_box.xmax() - uv_box.xmin())) < 1e-6 &&
                        std::abs(std::abs(uv_box.ymin()-v) + std::abs(uv_box.ymax()-v) -
                                 std::abs(uv_box.ymax() - uv_box.ymin())) < 1e-6;
            t += L/steps){

            u = t*Lin.x() + XOrt.x();
            v = t*Lin.y() + XOrt.y();

            if(p_line == NULL) p_line = new QList<QVector3D>;

            if(uv2Point3(P,QVector2D(u,v))){
                p_line->append(P);
            }else{
                if(p_line->isEmpty()){
                    delete p_line;
                    p_line = NULL;
                }else{
                    mesh_lines.append(p_line);
                    p_line = NULL;
                }
            }
        }

        //store the last line drawn if necessary
        if(p_line){
            if(p_line->isEmpty()){
                delete p_line;
                p_line = NULL;
            }else{
                mesh_lines.append(p_line);
                p_line = NULL;
            }
        }
    }
}

 bool CGALSurface::buildMesh(double L, int steps){
     if(!parameterization) return false;

     double Luv = L/parameterization_metric;

     //clear mesh lines
     for(int i = 0; i < mesh_lines.size(); i++){
         mesh_lines.at(i)->clear();
         delete mesh_lines.at(i);
     }
     mesh_lines.clear();

     //draw the lines from UVc at directions UVcx and UVcy
     ortMeshLines(UVPt, UVx, Luv, steps);
     ortMeshLines(UVPt, UVy, Luv, steps);
     return true;
 }

 //void CGALSurface::collapse(double ratio){
 //    CGAL::Surface_mesh_simplification::Count_ratio_stop_predicate<Polyhedron> stop(ratio);
 //    CGAL::Surface_mesh_simplification::edge_collapse(polyhedron,stop,
 //                        /*CGAL*/boost::vertex_index_map(boost::get(/*CGAL*/boost::vertex_external_index,polyhedron))
 //                       .edge_index_map(boost::get(/*CGAL*/boost::edge_external_index,polyhedron)));
 //}

 bool CGALSurface::createBasingOnPoints(Point_with_normal_3_list* points){

     pwn_list list;

     for(Point_with_normal_3_list_iterator pit = points->begin(); pit != points->end(); ++pit){
         list.insert(list.end(),pwn(pit->first,pit->second));
     }

     Poisson_reconstruction_function function(list.begin(), list.end(),
             CGAL::make_normal_of_point_with_normal_pmap(pwn_list::value_type()) );

     if(!function.compute_implicit_function())
         return false;

     printf("Implicit function done\n");

     FT sm_angle = 20.0; // Min triangle angle in degrees.
     FT sm_radius = 30; // Max triangle size w.r.t. point set average spacing.
     FT sm_distance = 0.375; // Surface Approximation error w.r.t. point set average spacing.

     FT average_spacing = CGAL::compute_average_spacing(list.begin(),list.end(),6);
     Point_3 inner_point = function.get_inner_point();
     Sphere_3 bsphere = function.bounding_sphere();

     FT radius = std::sqrt(bsphere.squared_radius());
     FT sm_sphere_radius = 5.0*radius;
     FT sm_dichotomy_error = sm_distance*average_spacing/1000.0; // Dichotomy error must be << sm_distance

     //surface type definition
     Surface_3 surface(function,
                          Sphere_3(inner_point,2),
                                   //sm_sphere_radius*sm_sphere_radius),
                          sm_dichotomy_error/sm_sphere_radius);

     // Defines surface mesh generation criteria
     CGAL::Surface_mesh_default_criteria_3<STr> criteria(sm_angle,  // Min triangle angle (degrees)
                                                         sm_radius*average_spacing,  // Max triangle size
                                                         sm_distance*average_spacing); // Approximation error

     // Generates surface mesh with manifold option
     STr tr; // 3D Delaunay triangulation for surface mesh generation
     C2t3 c2t3(tr); // 2D complex in 3D Delaunay triangulation
     CGAL::make_surface_mesh(c2t3,                                 // reconstructed mesh
                             surface,                              // implicit surface
                             criteria,                             // meshing criteria
                             CGAL::Manifold_with_boundary_tag());  // require manifold mesh
     printf("Surface mesh done\n");

     if(tr.number_of_vertices() == 0)
         return false;

     CGAL::output_surface_facets_to_polyhedron(c2t3, polyhedron);

     return true;
 }
