#ifndef EPS_WRITER_H
#define EPS_WRITER_H

#include <QList>
#include <QVector3D>
#include <QVector2D>
#include <QColor>
#include <QString>

#include "cgal_routines.h"

typedef struct EpsPoints{
    QList<QVector3D> points;
    QList<QVector2D> prj;
    double width;
    QColor color;
    EpsPoints(QList<QVector3D> points,double width, QColor color){
        this->points = points;
        this->width = width;
        this->color = color;
    }
} EpsPoints;

typedef struct EpsLines{
    QList<QList<QVector3D>> lines;
    QList<QList<QVector2D>> prj;
    double width;
    QColor color;
    double dash;

    EpsLines(double width, QColor color, double dash){
        this->width = width;
        this->color = color;
        this->dash = dash;
    }
public:
    void append(QList<QVector3D> line){
        lines.append(line);
    }
}EpsLines;

class EpsWriter{
public:
    EpsWriter();
    ~EpsWriter();

public:
    void appendLines(QList<QList<QVector3D>*>* lines, double width, QColor color, double dash = 0);
    void appendPoints(QList<QVector3D>* points, double width, QColor color);

    void saveEps(QString path, QVector3D camera, double scale);

private:
    QList<EpsLines*> lines;
    QList<EpsPoints*> points;

    Bbox_2 bbox;

private:
     void update2DInfo(QVector3D camera);
     void insertBox(bool isEmpty, QVector2D P);
};

#endif // EPS_WRITER_H
