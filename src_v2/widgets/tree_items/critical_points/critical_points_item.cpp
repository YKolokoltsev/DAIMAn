//
// Created by morrigan on 19/07/17.
//

#include "critical_points_item.h"
#include "nacp_items.h"
#include "nnacp_items.h"

DCriticalPointsItem::DCriticalPointsItem(node_desc_t wfx_item_idx){
    reg(this, true);
    setText(0, QString("Critical Points"));

    wfx_item = std::move(get_weak_obj_ptr<DWfxItem>(this,wfx_item_idx));
    wfx_item->ptr.lock()->insertChild(0,this);

    new DFolderNACPItem(get_idx());
    new DFolderNNACPItem(get_idx());
}

std::unique_ptr<QMenu> DCriticalPointsItem::context_menu(QWidget*){
    return std::unique_ptr<QMenu>(nullptr);
}