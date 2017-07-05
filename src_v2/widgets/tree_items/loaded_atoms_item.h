//
// Created by morrigan on 5/07/17.
//

#ifndef DAIMAN_LOADED_ATOMS_ITEM_H
#define DAIMAN_LOADED_ATOMS_ITEM_H

#include <QTreeWidgetItem>

#include "wfx_item.h"
#include "base_vertex.hpp"
#include "wfx_item.h"
#include "ref_decorator.hpp"

/*
 * this is a folder-like item, it does not make any computations
 */
class DWfxItem;
class DLoadedAtomsItem : public QObject, public QTreeWidgetItem, public BaseObj {
Q_OBJECT
public:
    DLoadedAtomsItem(node_desc_t, node_desc_t);

private:
    ext_weak_ptr_t<DWfxItem> wfx_item;
};

#endif //DAIMAN_LOADED_ATOMS_ITEM_H
