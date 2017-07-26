//
// Created by morrigan on 19/07/17.
//

#ifndef DAIMAN_DCRITICALPOINTSITEM_H
#define DAIMAN_DCRITICALPOINTSITEM_H

#include "iitem.h"
#include "wfx_item.h"
#include "ref_decorator.hpp"

class DWfxItem;
class DCriticalPointsItem : public IItem{
Q_OBJECT
public:
    DCriticalPointsItem(node_desc_t wfx_item_idx);
    std::unique_ptr<QMenu> context_menu(QWidget*);

private:
    ext_weak_ptr_t<DWfxItem> wfx_item;
};


#endif //DAIMAN_DCRITICALPOINTSITEM_H
