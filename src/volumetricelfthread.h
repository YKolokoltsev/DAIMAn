#ifndef VOLUMETRICELFTHREAD_H
#define VOLUMETRICELFTHREAD_H

#include <QThread>
#include "document.h"
#include "old_misc/structs.h"

class VolumetricElfThread : public QThread
{
     Q_OBJECT

public:
    VolumetricElfThread(Document* doc, double* result);
    ~VolumetricElfThread();
    int getNumPoints(){return numPoints;}
    void setNumPoints(int numPoints){this->numPoints = numPoints;}

private:
    double calc_elf(double x, double y, double z);
    void set_nz(double x, double y, double z);
    void initializeHelpers();

protected:
    void run();

private:
    Document* doc;
    double*  p;           //this is an external density matrix
    double* result;
    int numPoints;

private:
    //helpers
    int*    nz;
    double* atdist2;

    double* d;
    double* dx;
    double* dy;
    double* dz;
};

#endif // VOLUMETRICELFTHREAD_H
