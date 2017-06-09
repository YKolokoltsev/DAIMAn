#include "threadcontroller.h"

#include "functions.h"
#include <math.h>
#include "volumetricelfthread.h"

//console output
#include <stdio.h>

ThreadController::ThreadController(QString path, Document* doc, int runType){
    this->doc = doc;
    this->path = path;
    this-> runType = runType;

    //Volume
    if(runType == 1){
        double resolution = 25; //points per angstrum

        //TODO: Read this data from input
        vol.x_min = -5;
        vol.y_min = -5;
        vol.z_min = -5;

        vol.x_max = 5;
        vol.y_max = 5;
        vol.z_max = 5;

        vol.dx = 1/resolution;
        vol.dy = 1/resolution;
        vol.dz = 1/resolution;
    }
}

ThreadController::ThreadController(QString path, Document* doc, Volumetrics box){
    this->doc = doc;
    this->path = path;
    this-> runType = 1;

    //Volume
    if(runType == 1){
        double resolution = 25; //points per angstrum

        //TODO: Read this data from input
        vol.x_min = box.x_min;
        vol.y_min = box.y_min;
        vol.z_min = box.z_min;

        vol.x_max = box.x_max;
        vol.y_max = box.y_max;
        vol.z_max = box.z_max;

        vol.dx = 1/resolution;
        vol.dy = 1/resolution;
        vol.dz = 1/resolution;
    }
}

void ThreadController::planeThread(){

    Surface * surf = this->doc->get_surface(); if(!surf) return;
    if(!openFilePlane()) return;

    int num = 4;
    int points_per_thread = 150000;

    int i_max = (int)fabs(ceil(surf->camera.resolution*surf->camera.cx));
    int j_max = (int)fabs(ceil(surf->camera.resolution*surf->camera.cy));


    VolumetricElfThread ** threads = new VolumetricElfThread* [num];
    double ** results = new double* [num];
    double ** coords = new double* [num];
    double * buffer = new double[points_per_thread*4];
    double * coordsbuf = new double[points_per_thread*2];

    //Initializing thread objects
    for(int i = 0; i < num; i++){
        results[i] = new double[points_per_thread*4];
        coords[i] = new double[points_per_thread*2];
        threads[i] = new VolumetricElfThread(doc,results[i]);
    }



    //Main thread cycle: assign jobs to threads and write data to file
    QVector3D V;
    int pts_processed = 0;
    int i_buf = 0;
    bool proc_is_written[num];
    for(int proc = 0; proc < num; proc++) proc_is_written[proc] = true;

    for(int i = 0; i < i_max; i++){
        for(int j = 0; j < j_max; j++){
           GetPointInPlane(doc,i/surf->camera.resolution - 0.5*surf->camera.cx,
                               j/surf->camera.resolution - 0.5*surf->camera.cy,&V);

           buffer[4*i_buf+0] = V.x();
           buffer[4*i_buf+1] = V.y();
           buffer[4*i_buf+2] = V.z();
           buffer[4*i_buf+3] = 0.0;

           coordsbuf[2*i_buf+0] = (double)i/surf->camera.resolution;
           coordsbuf[2*i_buf+1] = (double)j/surf->camera.resolution;

           i_buf++;
           pts_processed++;

           if(i_buf == points_per_thread || pts_processed == i_max*j_max)
           {
               while(i_buf != 0){
                   for(int proc = 0; proc < num; proc++){
                       //write file
                       if(threads[proc]->isFinished() && !proc_is_written[proc]){
                           writeFilePlane(threads[proc]->getNumPoints(),coords[proc],results[proc]);
                           proc_is_written[proc] = true;
                           fprintf(stderr,"Completed: %2.3f \n\n", 100*(double)pts_processed/((double)i_max*j_max));
                       }
                       if(i_buf !=0 && !threads[proc]->isRunning() && proc_is_written[proc]){

                           memcpy((void*)results[proc],(void*)buffer,4*i_buf*sizeof(double));
                           memcpy((void*)coords[proc],(void*)coordsbuf,2*i_buf*sizeof(double));

                           proc_is_written[proc] = false;
                           threads[proc]->setNumPoints(i_buf);
                           threads[proc]->start();
                           i_buf = 0;
                       }
                   }
                   this->sleep(1);
               }
           }

        }
    }

    //Wait for calcs

    bool all_written = false;
    while(!all_written){
        for(int proc = 0; proc < num; proc++){
            if(threads[proc]->isFinished() && !proc_is_written[proc]){
                writeFilePlane(threads[proc]->getNumPoints(),coords[proc],results[proc]);
                proc_is_written[proc] = true;
                fprintf(stderr,"Colpleted: %2.3f \n\n", 100*(double)pts_processed/((double)i_max*j_max));
            }
        }
        all_written = true;
        for(int proc = 0; proc < num; proc++){
            if(!proc_is_written[proc]) all_written = false;
        }
    }

    closeFile();

    return;
}

 void ThreadController::volThread(){
     int num = 4;
     int points_per_thread = 150000;

     int i_max = (int)fabs(ceil((vol.x_max - vol.x_min)/vol.dx));
     int j_max = (int)fabs(ceil((vol.y_max - vol.y_min)/vol.dy));
     int k_max = (int)fabs(ceil((vol.z_max - vol.z_min)/vol.dz));

     if(!openFileVol(i_max, j_max, k_max)) return;

     vol.dx = (vol.x_max - vol.x_min)/(double)i_max;
     vol.dy = (vol.y_max - vol.y_min)/(double)j_max;
     vol.dz = (vol.z_max - vol.z_min)/(double)k_max;

     VolumetricElfThread ** threads = new VolumetricElfThread* [num];
     double ** results = new double* [num];
     int     ** coords = new int* [num];
     double   * buffer = new double[points_per_thread*4];
     int      *coordsbuf = new int[points_per_thread*3];

     //Initializing thread objects
     for(int i = 0; i < num; i++){
         results[i] = new double[points_per_thread*4];
         coords[i] =  new int[points_per_thread*3];
         threads[i] = new VolumetricElfThread(doc,results[i]);
     }

     //Main thread cycle: assign jobs to threads and write data to file

     int pts_processed = 0;
     int i_buf = 0;
     bool proc_is_written[num];
     for(int proc = 0; proc < num; proc++) proc_is_written[proc] = true;

     for(int i = 0; i < i_max; i++){
         for(int j = 0; j < j_max; j++){
             for(int k = 0; k < k_max; k++){

                 buffer[4*i_buf+0] = vol.x_min + (double)i*vol.dx;
                 buffer[4*i_buf+1] = vol.y_min + (double)j*vol.dx;
                 buffer[4*i_buf+2] = vol.z_min + (double)k*vol.dx;
                 buffer[4*i_buf+3] = 0.0;

                 coordsbuf[3*i_buf+0] = i;
                 coordsbuf[3*i_buf+1] = j;
                 coordsbuf[3*i_buf+2] = k;


                 i_buf++;
                 pts_processed++;

                 if(i_buf == points_per_thread || pts_processed == i_max*j_max*k_max)
                 {
                     while(i_buf != 0){
                         for(int proc = 0; proc < num; proc++){
                             //write file
                             if(threads[proc]->isFinished() && !proc_is_written[proc]){
                                 writeFileVol(threads[proc]->getNumPoints(),coords[proc], results[proc]);
                                 proc_is_written[proc] = true;
                                 fprintf(stderr,"Completed: %2.3f \n\n", 100*(double)pts_processed/((double)i_max*j_max*k_max));
                             }
                             if(i_buf !=0 && !threads[proc]->isRunning() && proc_is_written[proc]){
                                 memcpy((void*)results[proc],(void*)buffer,4*i_buf*sizeof(double));
                                 memcpy((void*)coords[proc],(void*)coordsbuf,3*i_buf*sizeof(int));
                                 proc_is_written[proc] = false;
                                 threads[proc]->setNumPoints(i_buf);
                                 threads[proc]->start();
                                 i_buf = 0;
                             }
                         }
                         this->sleep(1);
                     }
                 }
             }
         }
     }

     //Wait for calcs
     bool all_written = false;

     while(!all_written){
         for(int proc = 0; proc < num; proc++){
             if(threads[proc]->isFinished() && !proc_is_written[proc]){
                 writeFileVol(threads[proc]->getNumPoints(), coords[proc], results[proc]);
                 proc_is_written[proc] = true;
                 fprintf(stderr,"Colpleted: %2.3f \n\n", 100*(double)pts_processed/((double)i_max*j_max*k_max));
             }
         }
         all_written = true;
         for(int proc = 0; proc < num; proc++){
             if(!proc_is_written[proc]) all_written = false;
         }
     }

     closeFile();

     return;
 }

 //ThreadController

 bool ThreadController::openFilePlane(){

     file = new QFile(path);
     if (!file->open(QIODevice::WriteOnly | QIODevice::Text)) return false;
     return true;
 }

 bool ThreadController::openFileVol(int i_max, int j_max, int k_max){

     file = new QFile(path);
     if (!file->open(QIODevice::WriteOnly | QIODevice::Text)) return false;

     QString text = "";
     char line[100];

     memset(line,'\0',100*sizeof(char));
     sprintf(line,"%15.5E %15.5E %15.5E\n",vol.x_min,vol.y_min,vol.z_min);
     text += QString(line);

     memset(line,'\0',100*sizeof(char));
     sprintf(line,"%15.5E %15.5E %15.5E\n",vol.dx,vol.dy,vol.dz);
     text += QString(line);

     memset(line,'\0',100*sizeof(char));
     sprintf(line,"%d %d %d\n",i_max,j_max,k_max);
     text += QString(line);

     file->write(text.toStdString().data()/*text.toAscii()*/);

     return true;
 }

 bool ThreadController::writeFileVol(int nPoints, int* coords, double* data){
     QString text = "";
     char line[100];

     for(int i = 0; i < nPoints; i++){
         memset(line,'\0',100*sizeof(char));
         sprintf(line,"%d %d %d %15.5E\n", coords[3*i]+1,
                                           coords[3*i+1]+1,
                                           coords[3*i+2]+1,
                                           data[4*i+3] ==-1? 0 : data[4*i+3]);
         text += QString(line);
     }
     file->write(text.toStdString().data()/*text.toAscii()*/);

     return true;
 }

 bool ThreadController::writeFilePlane(int nPoints, double* coords, double* data){
     QString text = "";
     char line[100];

     for(int i = 0; i < nPoints; i++){
         memset(line,'\0',100*sizeof(char));
         sprintf(line,"%15.5E %15.5E %15.5E\n",coords[2*i+0],
                                               coords[2*i+1],
                                               data[4*i+3] ==-1? 0 : data[4*i+3]);
         text += QString(line);
     }
     file->write(text.toStdString().data()/*text.toAscii()*/);

     return true;
 }

 void ThreadController::closeFile(){
     file->close();
     delete file;
     file = NULL;
 }
