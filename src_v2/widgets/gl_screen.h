//
// Created by morrigan on 20/06/17.
//

#ifndef DAIMAN_DGLSCREEN_H
#define DAIMAN_DGLSCREEN_H

//https://doc.qt.io/qt-5/qt3d-basicshapes-cpp-example.html

//#include <QtOpenGL/QGLWidget> (old base)
#include <QWidget>
//#include <Qt3DExtras/qt3dwindow.h>

#include "base_vertex.hpp"
#include "graph.h"
#include "ref_decorator.hpp"
#include "client_splitter.h"

class DGlScreen : public QWidget, public BaseObj{
Q_OBJECT
public:
    DGlScreen(node_desc_t);

protected:
    void initializeGL();

private:
    ext_weak_ptr_t<DClientSplitter> client_edge;
};


#endif //DAIMAN_DGLSCREEN_H
