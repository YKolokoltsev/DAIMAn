//
// Created by morrigan on 20/06/17.
//

#include "tree_ctrl.h"

DTreeCtrl::DTreeCtrl(DocTree::node_desc_t client_edge_idx) {
    reg_weak(this);
    client_edge = std::move(doc_get_weak_obj_ptr<DClientSplitter>(this,client_edge_idx));

    setColumnCount(1);
    headerItem()->setText(0,"Document");

    client_edge->ptr->addWidget(this);
}