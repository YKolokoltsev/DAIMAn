//
// Created by morrigan on 20/06/17.
//

#include "gl_screen.h"

DGlScreen::DGlScreen(node_desc_t client_edge_idx) {
    reg(this, true);
    client_edge = std::move(get_weak_obj_ptr<DClientSplitter>(this,client_edge_idx));
    client_edge->ptr.lock()->addWidget(this);
}

void DGlScreen::initializeGL()
{
    //qglClearColor(QColor(200,200,200));

    //glPointSize(10);
    //glEnable(GL_POINT_SMOOTH);
}