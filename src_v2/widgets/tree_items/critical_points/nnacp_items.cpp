//
// Created by morrigan on 19/07/17.
//

#include "nnacp_items.h"

DFolderNNACPItem::DFolderNNACPItem(node_desc_t cp_folder_item_idx){
    reg(this, true);
    setText(0, QString("Not Nuclear Attr."));

    cp_folder_item = std::move(get_weak_obj_ptr<DCriticalPointsItem>(this,cp_folder_item_idx));
    cp_folder_item->ptr.lock()->insertChild(0,this);
}


std::unique_ptr<QMenu> DFolderNNACPItem::context_menu(QWidget*){
    return std::unique_ptr<QMenu>(nullptr);
}