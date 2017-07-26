//
// Created by morrigan on 19/07/17.
//

#ifndef DAIMAN_DNACPITEM_H
#define DAIMAN_DNACPITEM_H

#include "base_vertex.hpp"
#include "ref_decorator.hpp"
#include "wfndata.h"
#include "critical_points_item.h"

class DCriticalPointsItem;
class DFolderNACPItem : public IItem{
Q_OBJECT
public:
    DFolderNACPItem(node_desc_t cp_folder_item_idx);
    std::unique_ptr<QMenu> context_menu(QWidget*);

private:
    ext_weak_ptr_t<DCriticalPointsItem> cp_folder_item;
};


#endif //DAIMAN_DNACPITEM_H
