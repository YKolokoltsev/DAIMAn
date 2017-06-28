//
// Created by morrigan on 20/06/17.
//

#ifndef DAIMAN_DGLSCREEN_H
#define DAIMAN_DGLSCREEN_H

#include <QtOpenGL/QGLWidget>

#include "base_vertex.hpp"
#include "graph.h"
#include "ref_decorator.hpp"
#include "client_splitter.h"

class DGlScreen : public QGLWidget , public BaseObj{
Q_OBJECT
public:
    DGlScreen(node_desc_t);
private:
    ext_weak_ptr_t<DClientSplitter> client_edge;
};


#endif //DAIMAN_DGLSCREEN_H
