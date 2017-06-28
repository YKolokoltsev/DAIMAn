//
// Created by morrigan on 20/06/17.
//

#include <QContextMenuEvent>
#include <QApplication>
#include <QObject>

#include "tree_ctrl.h"
#include "wfx_item.hpp"

DTreeCtrl::DTreeCtrl(DocTree::node_desc_t client_edge_idx) {
    reg_weak(this);
    client_edge = std::move(doc_get_weak_obj_ptr<DClientSplitter>(this,client_edge_idx));

    setColumnCount(1);
    headerItem()->setText(0,"Document");

    client_edge->ptr->addWidget(this);
}

void DTreeCtrl::contextMenuEvent(QContextMenuEvent* p_Context){
    QObject* p_Item = dynamic_cast<QObject*>(itemAt(p_Context->x(),p_Context->y()));
    if(!p_Item) return;
    QApplication::postEvent(p_Item, new WfxItemEvent(WfxItemEvent::Context, p_Context->globalPos()));
}

void DTreeCtrl::add_wfx_item(QString path,  DocTree::node_desc_t wfx_idx){
    new DWfxItem(path, get_idx(), wfx_idx);
}