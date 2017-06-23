//
// Created by morrigan on 20/06/17.
//

#ifndef DAIMAN_DGLSCREEN_H
#define DAIMAN_DGLSCREEN_H

#include <QtOpenGL/QGLWidget>

#include "doc_tree.hpp"
#include "client_splitter.h"

class DGlScreen : public QGLWidget , public DocTree::BaseObj{
Q_OBJECT
public:
    DGlScreen(DocTree::node_desc_t);
private:
    DocTree::ext_node_ptr_t<DClientSplitter> client_edge;
};


#endif //DAIMAN_DGLSCREEN_H
