#ifndef GLWIDGET_H
#define GLWIDGET_H

//#include <QGLWidget>
#include <QtOpenGL/QGLWidget>
#include "mainwindow.h"
#include <QVector3D>
#include <QtWidgets/QOpenGLWidget>

class MainWindow;

class GLWidget : public QGLWidget
{

public:
    GLWidget(MainWindow* parent = NULL);

public:
    void update();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    void mousePress_Rotate(QMouseEvent* event);
    void mousePress_Select(QMouseEvent* event);
    void mouseMove_Rotate(QMouseEvent* event);

    void renderSurface();
    void renderBondPaths();
    void renderZfxGradLines();
    void renderCGALSurfaces();
    void renderBCPs();
    void renderCamera();

    QVector3D getSelection(int cx, int cy);

private:
    QPoint lastPos;      //The last position of the cursor in the window
    MainWindow* papa;
};

#endif // GLWIDGET_H
