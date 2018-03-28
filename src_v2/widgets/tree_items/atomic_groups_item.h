//
// Created by morrigan on 3/27/18.
//

#ifndef DAIMAN_ATOMIC_GROUPS_ITEM_H
#define DAIMAN_ATOMIC_GROUPS_ITEM_H

#include <QTreeWidgetItem>

#include "wfx_item.h"
#include "loaded_atoms_item.h"

/*
 * this is a folder-like item, it does not make any computations
 */
class DWfxItem;
class DAtomicGroupsItem : public IItem{
Q_OBJECT
public:
    DAtomicGroupsItem(node_desc_t, node_desc_t);

    std::unique_ptr<QMenu> context_menu(QWidget*);

public slots:
    void sl_new_atomic_group();

private:
    ext_weak_ptr_t<DWfxItem> wfx_item;
    ext_weak_ptr_t<DLoadedAtomsItem> atoms_item;
};

#endif //DAIMAN_ATOMIC_GROUPS_ITEM_H
