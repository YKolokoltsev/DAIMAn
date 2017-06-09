#include "cslwl.h"

#include "commons.h"
#include "functions.h"

bool GetPoint(Document* doc, double x, double y, QVector3D* out)
{
    // x,y - this is a vector in the camera plane, that contains (0,0,0) point
    // out - output point, that sites on the higest triangle

    if(out == NULL) return false;
    if(!doc) return false;
    Surface * surf = doc->get_surface();
    if(!surf) return false;

    QVector3D Nt, r;
    QList<QVector3D> pts;
    double minimum, Ct;
    int l, k;
    QVector3D V;
    QQuaternion Rot = QQuaternion::fromAxisAndAngle(surf->camera.norm,surf->camera.fi*180/3.141592654);

    V = surf->camera.Ex*x + surf->camera.Ey*y;
    V = Rot.rotatedVector(V);
    V += surf->camera.center;

    for(k = 0; k < surf->triangles.count(); k++)
    {
        Nt = QVector3D::normal(surf->triangles.at(k).x1,
                               surf->triangles.at(k).x2,
                               surf->triangles.at(k).x3);

        Ct = -QVector3D::dotProduct(surf->triangles.at(k).x1,Nt);

        if(QVector3D::dotProduct(Nt,surf->camera.norm) != 0)
        {
            r = V - surf->camera.norm*(Ct + QVector3D::dotProduct(Nt,V))/
                         QVector3D::dotProduct(Nt,surf->camera.norm);

            if( fabs((QVector3D::crossProduct(surf->triangles.at(k).x1 - r,surf->triangles.at(k).x2 - r)).length() +
                     (QVector3D::crossProduct(surf->triangles.at(k).x3 - r,surf->triangles.at(k).x2 - r)).length() +
                     (QVector3D::crossProduct(surf->triangles.at(k).x1 - r,surf->triangles.at(k).x3 - r)).length() -
                     (QVector3D::crossProduct(surf->triangles.at(k).x2 - surf->triangles.at(k).x1,
                                              surf->triangles.at(k).x3 - surf->triangles.at(k).x1)).length()) < 0.01)
                pts.append(r);
        }
    }

    minimum = 99999;
    l = -1;

    for(k = 0; k < pts.count(); k++)
    {
        if(minimum - QVector3D::dotProduct(surf->camera.norm,pts.at(k)) > 0.001)
        {
            minimum = QVector3D::dotProduct(surf->camera.norm,pts.at(k));
            l = k;
        }
    }

    if (l != -1)
    {
        *out = pts.at(l);
        return true;
    }

    return false;
}

bool GetPointInPlane(Document* doc, double x, double y, QVector3D* out)
{
    // x,y - this is a vector in the camera plane, that contains (0,0,0) point
    // out - output point, that sites on the higest triangle

    if(out == NULL) return false;
    if(!doc) return false;
    Surface * surf = doc->get_surface();
    if(!surf) return false;

    QVector3D V;
    QQuaternion Rot = QQuaternion::fromAxisAndAngle(surf->camera.norm,surf->camera.fi*180/3.141592654);

    V = surf->camera.Ex*x + surf->camera.Ey*y;
    V = Rot.rotatedVector(V);
    V += surf->camera.center;

    *out = V;

    return true;
}

//Now this function is running!
void TakeVolume(QString path, Document* doc, Volumetrics box){
    ThreadController threadController(path, doc,box);
    threadController.start();

    while(threadController.isRunning()){
        sleep(1);
    }
}

//this is the main function to calculate ELF intersections
void TakePhoto(QString path, Document* doc)
{
    ThreadController threadController(path, doc,2);
    threadController.start();

    while(threadController.isRunning()){
        sleep(1);
    }
}

void TakePhoto2(QString path, Document* doc)
{
    if(!doc) return;
    Surface * surf = doc->get_surface();
    if(!surf) return;

    int i,j;
    QVector3D point;
    int nx = surf->camera.resolution*surf->camera.cx,
        ny = surf->camera.resolution*surf->camera.cy;

    int** bas = NULL; bas = AllocMat(nx,ny, bas); if(!bas) return;
    QVector3D** vecs = NULL; vecs = AllocMat(nx,ny,vecs); if(!vecs) return;
    double** fld = NULL; fld = AllocMat(nx,ny,fld); if(!fld) return;


    //Filling Matrices Accordingly to points projections*************************
    int b; double f; QVector3D v;
    for(i = 0; i < nx; i++)
    for(j = 0; j < ny; j++)
    {
           bas[i][j] = 0; fld[i][j] = 0; vecs[i][j] = QVector3D(0,0,0);

           if(GetPoint(doc,i/surf->camera.resolution - 0.5*surf->camera.cx,
                           j/surf->camera.resolution - 0.5*surf->camera.cy,&point))
           {

              GradientPoint(point, surf->camera.resolution, surf->camera.Ex, surf->camera.Ey,
                             &b, &v, &f, doc);
              bas[i][j] = b; fld[i][j] = f; vecs[i][j] = v;
           }

    }

    //Call for the basic algorithm function************************************
    QList<QPoint> bases, lost;
    GradientBasins(nx, ny, bas, vecs, &bases, &lost);
    //Saving results************************************
    SaveRect(path, fld, nx, ny, surf->camera.resolution);
    SavePoints(path, bases, nx, ny, surf->camera.resolution);
    SavePoints(path+QString("_lost"), lost, nx, ny, surf->camera.resolution);
    //Freing memory ****************************************************************************
    FreeMat(nx,bas);
    FreeMat(nx,vecs);
    FreeMat(nx,fld);
}

bool findpoint(QList<QPoint> list, QPoint point)
{
    for(int i = list.count()-1; i >= 0 ; i--)
        if(list.at(i) == point)
            return true;

    return false;
}

double calcTotElectrons(Document* doc)
{
    if(!doc) return -1;
    WFNData *wfn = doc->get_wfn();
    if(!wfn) return -1;

    double res = 0;
    for(int i = 0; i < wfn->nat; i++)
        res += wfn->atcharges[i];

    return res;
}

void CalcBasins(Document* doc)
{
    if(!doc) return;
    Surface* surf = doc->get_surface();
    if(!surf) return;
    Triangle tri;

    for(int i = 0; i < surf->triangles.count(); i++)
    {
        tri = surf->triangles.at(i);
        FindBasin(doc,&tri);
        surf->triangles.removeAt(i);
        surf->triangles.insert(i,tri);
    }
}

void FindBasin(Document* doc, Triangle* tri)
{
    if(!doc || ! tri) return;
    double resolution = 100;

    //Looking for ex, ey, nx, ny, x0 ****************************************************************************
    QVector3D ex, ey, x0, norm;
    int nx = 0, ny = 0;
    double proj = 0;

    ex = QVector3D(tri->x2 - tri->x1);
    ey = QVector3D(tri->x3 - tri->x1);

    x0 = tri->x1;
    nx = ex.length()*resolution;
    proj = QVector3D::dotProduct(ex,ey)/ex.length();

    if(proj < 0) nx = (ex.length() - proj)*resolution,
                 x0 = tri->x1 + ex*proj/ex.length();
    if(proj > ex.length()) nx = proj*resolution;

    ex.normalize();
    ey = ey - ex*proj;
    ny = ey.length()*resolution;
    ey.normalize();
    proj = QVector3D::dotProduct(ex,ey);

    norm = QVector3D::crossProduct(ex,ey);

    //Creating regions and bisectrices ****************************************************************************
    int** bas = NULL; bas = AllocMat(nx,ny, bas); if(!bas) return;
    QVector3D** vecs = NULL; vecs = AllocMat(nx,ny,vecs); if(!vecs) return;
    QVector3D point;
    double fld = 0;
    int i, j;

    for(i = 0; i < nx; i++)
    for(j = 0; j < ny; j++)
    {
        point = x0 + (ex*i + ey*j)/resolution;

        if(PtInTri(*tri,point))
        GradientPoint(point, resolution, ex, ey,
                      &bas[i][j], &vecs[i][j], &fld, doc);
        else bas[i][j] = 0;
    }

    //Call for the basic algorithm function...
    QList<QPoint> bases, lost;
    GradientBasins(nx, ny, bas, vecs, &bases, &lost);
    while(bases.count() != 0)
    {
        point = x0 + (ex*bases.first().x()+ ey*bases.first().y())/resolution;
        tri->rhobas.append(point);
        bases.removeFirst();
    }

    while(lost.count() != 0)
    {
        point = x0 + (ex*lost.first().x()+ ey*lost.first().y())/resolution;
        tri->rhobas.append(point);
        lost.removeFirst();
    }
    //Freing memory ****************************************************************************
    FreeMat(nx,bas);
    FreeMat(nx,vecs);

}

void RecBas(int nx, int ny, int i, int j, int NBas, int** mat)
{
    if(!mat) return;

    QList<QPoint> unass; //Unassigned points that are TO BE marked with an NBas number
    unass.append(QPoint(i,j));
    int a,b,lvl = 0;

    while(unass.count() != 0)
    {
        a = unass.last().x();
        b = unass.last().y();

        if(lvl > 0) mat[a][b] = NBas;
        if(a > 0)    if(mat[a-1][b  ] >= 1)
        if(b > 0)    if(mat[a  ][b-1] >= 1)
        if(a < nx-1) if(mat[a+1][b  ] >= 1)
        if(b < ny-1) if(mat[a  ][b+1] >= 1)
        {                                    //Only the point with neighbors can be a source of
                                             //concentration region.
         mat[a][b] = NBas;
         lvl++;
         if(a > 0    ) if(mat[a-1][b  ] == 1) unass.insert(0,QPoint(a-1,b  )), mat[a-1][b  ]=2;
         if(b > 0    ) if(mat[a  ][b-1] == 1) unass.insert(0,QPoint(a  ,b-1)), mat[a  ][b-1]=2;
         if(a < nx-1 ) if(mat[a+1][b  ] == 1) unass.insert(0,QPoint(a+1,b  )), mat[a+1][b  ]=2;
         if(b < ny-1 ) if(mat[a  ][b+1] == 1) unass.insert(0,QPoint(a  ,b+1)), mat[a  ][b+1]=2;
        }

        unass.removeLast();
    }

}

bool PtInTri(Triangle tri, QVector3D pt)
{
    QVector3D x1 = tri.x1, x2 = tri.x2, x3=tri.x3;

    bool ret =(QVector3D::crossProduct(x2 -x1,x3-x1).length() -
               QVector3D::crossProduct(x1 - pt, x2-pt).length() -
               QVector3D::crossProduct(x3 - pt, x2-pt).length() -
               QVector3D::crossProduct(x1 - pt, x3-pt).length() > -0.001 );
    return ret;
}



/*static int cmpsec(const void* p1, const void* p2)
{
    return ((**(Sector**)p1).dfi > (**(Sector**)p2).dfi);
}*/

void GradientBasins(int nx, int ny, int** bas, QVector3D** vecs, QList<QPoint>* bases, QList<QPoint>* lost)
{
    if(nx <= 0 || ny <= 0 || !bas || !vecs || !bases || !lost) return;

    //Naming of concentration regions **********************************************************
    int i,j;
    int NBas = 3;
    for(i = 0; i < nx; i++)
    for(j = 0; j < ny; j++)
    if(bas[i][j] == 1)
    {
        RecBas(nx, ny, i, j, NBas, bas);
        NBas++;
    }

    //Connecting discharged points to the concentrated regions**********************************
    int k,l,m;
    bool ok;
    for(i = 0; i < nx; i++)
    for(j = 0; j < ny; j++)
    if(bas[i][j] == -1)
    {
        ok = true; k = 1;
        while(ok)
        {
            if(k > 1000)
            {
               bas[i][j] = 0;
               ok = false;
               printf("Zero length of vector!!! \n");
            }

            l = i + floor((double)vecs[i][j].x()*k);
            m = j + floor((double)vecs[i][j].y()*k);
            k++;

            if(l >= 0 && l < nx && m >=0 && m < ny)
            {
                if(bas[l][m] > 2) bas[i][j] = -bas[l][m], ok = false; //change it to see concatenation!
                if(bas[l][m] == 0) ok = false;
            }
            else ok = false;
        }
    }
    //And finally: zero flux surfaces***********************************************************
    QPoint next; bases->clear(); lost->clear();
    for(i = 0; i < nx; i++)
    for(j = 0; j < ny; j++)
    if(fabs(bas[i][j]) > 1)
    {
      next = QPoint(10,10);
      if(i != 0 &&                 fabs(bas[i][j]) != fabs(bas[i-1][j  ]) && fabs(bas[i-1][j  ]) > 1) next = QPoint(-1, 0);
      if(i != 0 && j != 0 &&       fabs(bas[i][j]) != fabs(bas[i-1][j-1]) && fabs(bas[i-1][j-1]) > 1) next = QPoint(-1,-1);
      if(i != 0 && j != ny-1 &&    fabs(bas[i][j]) != fabs(bas[i-1][j+1]) && fabs(bas[i-1][j+1]) > 1) next = QPoint(-1, 1);
      if(j != ny-1 &&              fabs(bas[i][j]) != fabs(bas[i  ][j+1]) && fabs(bas[i  ][j+1]) > 1) next = QPoint(0 , 1);
      if(j != 0 &&                 fabs(bas[i][j]) != fabs(bas[i  ][j-1]) && fabs(bas[i  ][j-1]) > 1) next = QPoint(0 ,-1);
      if(i != nx-1 && j != ny-1 && fabs(bas[i][j]) != fabs(bas[i+1][j+1]) && fabs(bas[i+1][j+1]) > 1) next = QPoint(+1, 1);
      if(i != nx-1 &&              fabs(bas[i][j]) != fabs(bas[i+1][j  ]) && fabs(bas[i+1][j  ]) > 1) next = QPoint(+1, 0);
      if(i != nx-1 && j != 0 &&    fabs(bas[i][j]) != fabs(bas[i+1][j-1]) && fabs(bas[i+1][j-1]) > 1) next = QPoint(+1,-1);


      if(next != QPoint(10,10))
          bases->append(QPoint(i,j));
    }
    else if(fabs(bas[i][j]) == 1)
          lost->append(QPoint(i,j));
}

void GradientPoint(QVector3D point, double resolution, QVector3D ex, QVector3D ey,
                   int* bas, QVector3D* vec, double* fld, Document* doc)
{
    if(resolution <= 0 || ex.length() < 0.5 || ey.length() < 0.5 ||
       !bas || !vec || !fld) return;

    QVector3D grad1, grad2, nv;
    QVector3D norm = QVector3D::crossProduct(ex,ey);
    bool ok = true;
    double gx = 0, gy = 0, gz = 0;
    *bas = 0;

    if(ok) (*fld) = doc->get_wfn()->calc_grad_elf(point.x(),point.y(),point.z(),
                            &gx, &gy, &gz);

    if(ok) grad1 = QVector3D(gx,gy,gz);
    if(ok) grad1.normalize();
    if(ok) ok = grad1.length() > 0.5;

    if(ok) nv = QVector3D::crossProduct(norm,grad1);
    if(ok) nv.normalize();
    if(ok) ok = nv.length() > 0.5;

    if(ok) point += nv/resolution;

    if(ok) doc->get_wfn()->calc_grad_elf(point.x(),point.y(),point.z(), &gx, &gy, &gz);
    if(ok) grad2 = QVector3D(gx,gy,gz);
    if(ok) grad2.normalize();
    if(ok) ok = grad2.length() > 0.5;


    if(ok)
    {
        if(QVector3D::dotProduct(nv,grad2) >= -0.00001) // discharged region;
           (*bas) = -1;
        else                                     // region of concentration;
           (*bas) = 1;

        nv.setX(QVector3D::dotProduct(ex,grad1));
        nv.setY(QVector3D::dotProduct(ey,grad1));
        nv.setZ(0);
        nv.normalize();

        if(nv.length() > 0.5) (*vec) = nv;
        else (*bas) = 0;
    }

    if(!ok)
    {
        (*bas) = 0;
        (*vec) = QVector3D(0,0,0);
    }
}

void SaveRect(QString path, double ** mat, int nx, int ny, double resolution)
{
    if(!mat || path.length() == 0 || nx <= 0 || ny <= 0 || resolution <=0) return;

    if(path.length() == 0) return;
    QFile file(path + QString("_mat.dat"));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;

    QString text;
    char    line[100];


    for(int i = 0; i < nx; i++)
    {
        for(int j = 0; j < ny; j++)
        {
            memset(line,'\0',100*sizeof(char));
            sprintf(line,"%15.5E %15.5E %15.5E\n",i/resolution,
                                                  j/resolution,
                                                  mat[i][j] == -1?0:mat[i][j]);
            text += QString(line);
        }

        if(text.length() > 1024*1024)
        {
            file.write(text.toStdString().data()/*text.toAscii()*/);
            text.clear();
        }
    }
    if(text.length() > 0) file.write(text.toStdString().data()/*text.toAscii()*/);

    file.close();
    text.clear();
}

void SavePoints(QString path, QList<QPoint> pts, int nx, int ny, double resolution)
{
    if(path.length() == 0 || pts.count() == 0|| nx <= 0 || ny <= 0 || resolution <= 0) return;

    if(path.length() == 0) return;
    QFile file(path + QString("_bas.dat"));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;

    QString text;
    char    line[100];

    for(int i = 0; i < pts.count(); i++)
    {
        memset(line,'\0',100*sizeof(char));
        sprintf(line,"%15.5E %15.5E\n",pts.at(i).x()/resolution,
                                              pts.at(i).y()/resolution);
        text += QString(line);


        if(text.length() > 1024*1024)
        {
            file.write(text.toStdString().data()/*text.toAscii()*/);
            text.clear();
        }
    }
    if(text.length() > 0) file.write(text.toStdString().data()/*text.toAscii()*/);

    file.close();
    text.clear();
}

double calcDistance(QVector3D v1, QVector3D v2){
    QVector3D diff = v2 - v1;
    return diff.length();
}

Cslwl* findBond(WFNData* wfn, int a, int b){
    if(wfn == NULL || a == b) return NULL;
    if(a >= wfn->nat || b >= wfn->nat) return NULL;

    QVector3D Ra, Rb;

    Ra = QVector3D(wfn->atcoords[a][0], wfn->atcoords[a][1], wfn->atcoords[a][2]);
    Rb = QVector3D(wfn->atcoords[b][0], wfn->atcoords[b][1], wfn->atcoords[b][2]);

    if(calcDistance(Ra,Rb) > 5) return NULL;

    Cslwl* cslwl = new Cslwl(Ra, Rb, wfn);
    if(!cslwl->calc_bond_path(false)){
        delete cslwl;
        return NULL;
    }

    return cslwl;
}

Cslwl* findBond(WFNData* wfn, int a, double x, double y, double z){
    if(wfn == NULL) return NULL;
    if(a >= wfn->nat) return NULL;

    QVector3D Ra, Rc;

    Ra = QVector3D(wfn->atcoords[a][0], wfn->atcoords[a][1], wfn->atcoords[a][2]);
    Rc = QVector3D(x,y,z); //bond critical point

    if(calcDistance(Ra,Rc) > 5) return NULL;

    Cslwl* cslwl = new Cslwl(Ra, x,y,z, wfn);

    if(!cslwl->calc_bond_path(true)){
        delete cslwl;
        return NULL;
    }

    return cslwl;
}
