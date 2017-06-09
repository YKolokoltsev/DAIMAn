#include "commons.h"

template <class Type> Type** AllocMatT(int nx, int ny, Type** type)
{
    if(nx <= 0 || ny <= 0) return NULL;
    if(type != NULL) return NULL;

    Type ** ret = (Type**) malloc(nx*sizeof(Type*));
    if(!ret) return NULL;

    for(int i = 0; i < nx; i++)
    {
        ret[i] = (Type*)malloc(ny*sizeof(Type));
        if(!ret[i])
        {
            i--;
            while(i >= 0)
                free(ret[i]);
            free(ret);
            return NULL;
        }
    }

    return ret;
}

int** AllocMat(int a, int b, int** c){return AllocMatT(a,b,c);}
double** AllocMat(int a, int b, double** c){return AllocMatT(a,b,c);}
QVector3D** AllocMat(int a, int b, QVector3D** c){return AllocMatT(a,b,c);}

template <class Type> void FreeMatT(int nx, Type** mat)
{
    if(!mat) return;
    for(int i = 0; i < nx; i++)
        if(mat[i])
            free(mat[i]);
    free(mat);
}

void FreeMat(int a, int** b){FreeMatT(a,b);}
void FreeMat(int a, double** b){FreeMatT(a,b);}
void FreeMat(int a, QVector3D** b){FreeMatT(a,b);}

template <class Type> void CopyMatT(int nx, int ny, Type** dest, Type** src){
    if(!dest || !src || nx <= 0 || ny <= 0) return;
    for(int i = 0; i < nx; i++){
        memcpy(dest[i], src[i], ny*sizeof(Type));
    }
}

void CopyMat(int a, int b, double** c, double** d){CopyMatT(a,b,c,d);}
