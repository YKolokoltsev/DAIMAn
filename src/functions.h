#ifndef FUNCTIONS_H
#define FUNCTIONS_H
//He we collect some more or less general functions that sould not appear in any of the clesses

#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>

#include <QFile>
#include <QIODevice>
#include <QVector3D>
#include <QList>
#include <QQuaternion>
#include <QPoint>
#include <QRect>

#include "document.h"
#include "threadcontroller.h"
#include "gui/progressdlg.h"
#include "sector.h"

bool GetPointInPlane(Document* doc, double x, double y, QVector3D* out);
void TakeVolume(QString path, Document* doc, Volumetrics box);
void TakePhoto(QString path, Document* doc);
void TakePhoto2(QString path, Document* doc);
bool GetPoint(Document* doc, QVector3D V, QVector3D* out);
bool findpoint(QList<QPoint> list, QPoint point);
double calcTotElectrons(Document* doc);
bool PtInTri(Triangle tri, QVector3D pt);
void FindBasin(Document* doc, Triangle* tri);
void CalcBasins(Document* doc);
void SaveRect(QString path, double ** mat, int nx, int ny, double resolution);
void SavePoints(QString path, QList<QPoint> pts, int nx, int ny, double resolution);
//static int  cmpsec(const void* p1, const void* p2);
void RecBas(int nx, int ny, int i, int j, int NBas, int** mat);
void GradientBasins(int nx, int ny, int** bas, QVector3D** vecs, QList<QPoint>* bases, QList<QPoint>* lost);
void GradientPoint(QVector3D point, double resolution, QVector3D ex, QVector3D ey,
                   int* bas, QVector3D* vec, double* fld, Document* doc);
double calcDistance(QVector3D v1, QVector3D v2);
Cslwl* findBond(WFNData* wfn, int a1, int a2);
Cslwl* findBond(WFNData* wfn, int a, double x, double y, double z);


#endif // FUNCTIONS_H
