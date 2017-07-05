//
// Created by morrigan on 20/06/17.
//

#include <QContextMenuEvent>
#include <QApplication>

#include "tree_ctrl.h"
#include "wfx_item.h"

DTreeCtrl::DTreeCtrl(node_desc_t client_edge_idx) {
    reg(this, true);
    client_edge = std::move(get_weak_obj_ptr<DClientSplitter>(this,client_edge_idx));

    setColumnCount(1);
    headerItem()->setText(0,"Document");

    client_edge->ptr.lock()->addWidget(this);
}

void DTreeCtrl::contextMenuEvent(QContextMenuEvent* p_Context){
    DWfxItem* item = dynamic_cast<DWfxItem*>(itemAt(p_Context->x(),p_Context->y()));
    if(item){
        auto menu = item->context_menu();
        menu->exec(p_Context->globalPos());
        menu->clear();
    }
}

void DTreeCtrl::add_wfx_item(QString path,  node_desc_t wfx_idx){
    new DWfxItem(path, get_idx(), wfx_idx);
}


void DTreeCtrl::sl_del_item(){
    auto item = dynamic_cast<DWfxItem*>(currentItem());
    if(item){
        item->reset();
        delete item;
    };
};