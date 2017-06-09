#include "eps_writer.h"
#include <fstream>

EpsWriter::EpsWriter(){

}

EpsWriter::~EpsWriter(){
    for(int i = 0; i < points.size(); i++){
        delete points.at(i);
    }
    points.clear();

    for(int i = 0; i < lines.size(); i++){
        delete lines.at(i);
    }
    lines.clear();
}

void EpsWriter::appendLines(QList<QList<QVector3D>*>* lines, double width, QColor color, double dash){
    EpsLines *epsLines = new EpsLines(width,color,dash);
    for(int i = 0; i < lines->size(); i++){
        epsLines->append(*lines->at(i));
    }
    this->lines.append(epsLines);
}

void EpsWriter::appendPoints(QList<QVector3D>* points, double width, QColor color){
    this->points.append(new EpsPoints(*points,width,color));
}

void EpsWriter::insertBox(bool isEmpty, QVector2D P){
    double xmin, xmax, ymin, ymax;

    xmin = bbox.xmin();
    xmax = bbox.xmax();
    ymin = bbox.ymin();
    ymax = bbox.ymax();

    if(isEmpty || bbox.xmin() > P.x()) xmin = P.x();
    if(isEmpty || bbox.ymin() > P.y()) ymin = P.y();
    if(isEmpty || bbox.xmax() < P.x()) xmax = P.x();
    if(isEmpty || bbox.ymax() < P.y()) ymax = P.y();

    bbox = Bbox_2(xmin,ymin,xmax,ymax);
}

void EpsWriter::update2DInfo(QVector3D camera){
    bbox = Bbox_2(0,0,0,0);

    QVector3D Nx, Ny;
    QVector2D P;
    QList<QVector2D> prj;
    bool isEmpty = true;

    buildOrtsToN(camera, Nx, Ny);

    for(int i = 0; i < points.size(); i++){
        points.at(i)->prj.clear();
        for(int j = 0; j < points.at(i)->points.size(); j++){
            P = projectPoint(points.at(i)->points.at(j),Nx,Ny);

            insertBox(isEmpty,P);
            points.at(i)->prj.append(P);

            isEmpty = false;
        }
    }

    for(int i = 0; i < lines.size(); i++){
        for(int j = 0; j < lines.at(i)->lines.size(); j++){
            prj.clear();
            for(int k = 0; k < lines.at(i)->lines.at(j).size(); k++){
                P = projectPoint(lines.at(i)->lines.at(j).at(k),Nx,Ny);
                insertBox(isEmpty,P);
                prj.append(P);
                isEmpty = false;
            }
            lines.at(i)->prj.append(prj);
        }
    }
}

void EpsWriter::saveEps(QString path, QVector3D camera, double scale){
    QVector2D P;

    update2DInfo(camera);

    std::ofstream out(path.toStdString() /*path.toAscii().data()*/);
    if(!out) return;

    double dx = std::abs(bbox.xmax() - bbox.xmin());
    double dy = std::abs(bbox.ymax() - bbox.ymin());
    double m = 0.05;

    out << "%!PS-Adobe-2.0 EPSF-2.0" << std::endl;
    out << "%%BoundingBox: " << int(scale*(bbox.xmin() - m*dx)) << " "
                                << int(scale*(bbox.ymin() - m*dy)) << " "
                                << int(scale*(bbox.xmax() + m*dx)) << " "
                                << int(scale*(bbox.ymax() + m*dy)) << std::endl;
    out << "%%HiResBoundingBox: " << scale*bbox.xmin() << " "
                                    << scale*bbox.ymin() << " "
                                    << scale*bbox.xmax() << " "
                                    << scale*bbox.ymax() << std::endl;
    out << "%%EndComments" << std::endl;
    out << "gsave" << std::endl;
    out << std::endl;
    out << "/L {lineto} bind def" << std::endl;
    out << "/A {0 360 arc closepath} bind def" << std::endl;
    out << "save" << std::endl;
    out << std::endl;

    for(int i = 0; i < lines.size(); i++){

        out << std::endl;
        out << "restore save" << std::endl;
        out << lines.at(i)->width << " setlinewidth" << std::endl;
        out << "newpath" << std::endl;
        out << lines.at(i)->color.redF() << " "
            << lines.at(i)->color.greenF() << " "
            << lines.at(i)->color.blueF() << " setrgbcolor" << std::endl;
        if(lines.at(i)->dash != 0){
            out << "[" << lines.at(i)->dash << " " << lines.at(i)->dash << "] 0 setdash" << std::endl;
        }

        for(int j = 0; j < lines.at(i)->prj.size(); j++){

            for(int k = 0; k < lines.at(i)->prj.at(j).size(); k++){
                P = lines.at(i)->prj.at(j).at(k);
                out << scale*P.x() << " " << scale*P.y();

                if(k == 0){
                    out << " moveto" << std::endl;
                }else{
                    out << " L" << std::endl;
                }
            }
        }
        out << "stroke" << std::endl;
        out << "gsave" << std::endl;
    }

    for(int i = 0; i < points.size(); i++){
        out << std::endl;
        out << "restore save" << std::endl;
        for(int j = 0; j < points.at(i)->prj.size(); j++){
            P = points.at(i)->prj.at(j);
            out << scale*P.x() << " " << scale*P.y() << " moveto "
                << scale*P.x() << " " << scale*P.y() << " " << points.at(i)->width << " A" << std::endl;
        }
        out << points.at(i)->color.redF() << " "
            << points.at(i)->color.greenF() << " "
            << points.at(i)->color.blueF() << " setrgbcolor fill" << std::endl;
        out << "stroke" << std::endl;
        out << "gsave" << std::endl;
    }

    /* Emit EPS trailer. */
    out << std::endl << std::endl;
    out << "grestore" << std::endl;
    out << std::endl;
    out << "showpage" << std::endl;
}
