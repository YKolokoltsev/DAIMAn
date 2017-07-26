//
// Created by morrigan on 19/07/17.
//

#ifndef DAIMAN_DFOLDERNNACPITEM_H
#define DAIMAN_DFOLDERNNACPITEM_H

#include "base_vertex.hpp"
#include "ref_decorator.hpp"
#include "wfndata.h"
#include "critical_points_item.h"

class DFolderNNACPItem  : public IItem{
Q_OBJECT
public:
    DFolderNNACPItem(node_desc_t cp_folder_item_idx);
    std::unique_ptr<QMenu> context_menu(QWidget*);

private:
    ext_weak_ptr_t<DCriticalPointsItem> cp_folder_item;
};


#endif //DAIMAN_DFOLDERNNACPITEM_H
