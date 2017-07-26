//
// Created by morrigan on 19/07/17.
//

#include "nacp_items.h"

DFolderNACPItem::DFolderNACPItem(node_desc_t cp_folder_item_idx){
    reg(this, true);
    setText(0, QString("Nuclear Attractors"));

    cp_folder_item = std::move(get_weak_obj_ptr<DCriticalPointsItem>(this,cp_folder_item_idx));
    cp_folder_item->ptr.lock()->insertChild(0,this);
}


std::unique_ptr<QMenu> DFolderNACPItem::context_menu(QWidget*){
    return std::unique_ptr<QMenu>(nullptr);
}