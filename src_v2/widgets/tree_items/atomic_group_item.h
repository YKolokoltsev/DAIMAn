//
// Created by morrigan on 3/28/18.
//

#ifndef DAIMAN_ATOMIC_GROUP_H
#define DAIMAN_ATOMIC_GROUP_H

#include <string>

#include <QTreeWidgetItem>
#include <QString>

//#include "ref_decorator.hpp"
#include "atomic_group.hpp"

#include "atomic_groups_item.h"
#include "iitem.h"

/*
 * this is a folder-like item, it does not make any computations
 */

class DAtomicGroupItem : public IItem{
Q_OBJECT
public:
    DAtomicGroupItem(node_desc_t, QString);
    std::unique_ptr<QMenu> context_menu(QWidget*);

private:
    ext_weak_ptr_t<DAtomicGroupsItem> atomic_groups_item;
    ext_shared_ptr_t<AtomicGroup> atomic_group;
};


#endif //DAIMAN_ATOMIC_GROUP_H
