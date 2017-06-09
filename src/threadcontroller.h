#ifndef THREADCONTROLLER_H
#define THREADCONTROLLER_H

#include "old_misc/structs.h"
#include "document.h"
#include <QString>
#include <QThread>
#include <QFile>

class ThreadController : public QThread
{
    Q_OBJECT

public:
    ThreadController(QString path, Document* doc, int runType);
    ThreadController(QString path, Document* doc, Volumetrics box);
protected:
    void run(){

        if(runType == 1){
            //Volume
            volThread();
        }else if(runType == 2){
            //Plane
            planeThread();
        }
    }
private:

    void planeThread();
    void volThread();
    bool openFilePlane();
    bool openFileVol(int i_max, int j_max, int k_max);
    bool writeFileVol(int nPoints, int* coords, double* data);
    bool writeFilePlane(int nPoints, double* coords, double* data);
    void closeFile();


private:
    QString path;
    Document* doc;
    Volumetrics vol;
    QFile* file;
    int runType; //1 = volume; 2 = plane;
};

#endif // THREADCONTROLLER_H
