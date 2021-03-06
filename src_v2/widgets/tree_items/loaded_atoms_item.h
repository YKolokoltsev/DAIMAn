//
// Created by morrigan on 5/07/17.
//

#ifndef DAIMAN_LOADED_ATOMS_ITEM_H
#define DAIMAN_LOADED_ATOMS_ITEM_H

#include <list>

#include <QTreeWidgetItem>

#include "wfx_item.h"
#include "base_vertex.hpp"
#include "ref_decorator.hpp"

#include "iitem.h"

/*
 * this is a folder-like item, it does not make any computations
 */
class DWfxItem;
class DLoadedAtomsItem : public IItem{
Q_OBJECT
public:
    DLoadedAtomsItem(node_desc_t, node_desc_t);

    std::unique_ptr<QMenu> context_menu(QWidget*);
    std::list<int> list_selected_atoms();


public slots:
    void sl_show_all_atoms();
    void sl_hide_all_atoms();

private:
    ext_weak_ptr_t<DWfxItem> wfx_item;
};

#endif //DAIMAN_LOADED_ATOMS_ITEM_H
