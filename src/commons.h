#ifndef COMMONS_H
#define COMMONS_H

#include <stdlib.h>
#include <QVector3D>

template <class Type> Type** AllocMatT(int nx, int ny, Type** type);
int** AllocMat(int a, int b, int** c);
double** AllocMat(int a, int b, double** c);
QVector3D** AllocMat(int a, int b, QVector3D** c);

template <class Type> void FreeMatT(int nx, Type** mat);
void FreeMat(int a, int** b);
void FreeMat(int a, double** b);
void FreeMat(int a, QVector3D** b);

template <class Type> void CopyMatT(int nx, int ny, Type** dest, Type** src);
void CopyMat(int a, int b, double** c, double** d);

#endif // COMMONS_H
