//
// Created by morrigan on 20/06/17.
//

#include "gl_screen.h"

DGlScreen::DGlScreen(DocTree::node_desc_t client_edge_idx){
    reg_weak(this);
    client_edge = std::move(doc_get_weak_obj_ptr<DClientSplitter>(this,client_edge_idx));
    client_edge->ptr->addWidget(this);
}