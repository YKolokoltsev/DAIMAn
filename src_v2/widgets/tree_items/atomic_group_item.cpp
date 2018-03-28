//
// Created by morrigan on 3/28/18.
//

#include <string>

#include "atomic_group_item.h"

DAtomicGroupItem::DAtomicGroupItem(node_desc_t at_groups_idx, QString name){
    reg(this, true);

    setText(0, name);

    atomic_groups_item = std::move(get_weak_obj_ptr<DAtomicGroupsItem>(this,at_groups_idx));
    auto sz = atomic_groups_item->ptr.lock()->childCount();
    atomic_groups_item->ptr.lock()->insertChild(sz, this);

    //always lock DocTree when storing indexes
    auto lock_inst = DocTree::inst();
    auto wfn_idx = doc_first_nearest_father<WFNData>(this->get_idx());
    auto gr = new AtomicGroup(wfn_idx, name.toStdString());
    atomic_group = get_shared_obj_ptr<AtomicGroup>(this, gr->get_idx());
}

std::unique_ptr<QMenu> DAtomicGroupItem::context_menu(QWidget*){
    return std::unique_ptr<QMenu>(nullptr);
}

