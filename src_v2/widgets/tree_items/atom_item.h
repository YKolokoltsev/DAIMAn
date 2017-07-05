//
// Created by morrigan on 5/07/17.
//

#ifndef DAIMAN_ATOM_ITEM_H
#define DAIMAN_ATOM_ITEM_H

#include <QTreeWidgetItem>

#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>

#include "base_vertex.hpp"
#include "ref_decorator.hpp"
#include "wfndata.h"
#include "loaded_atoms_item.h"

class DLoadedAtomsItem;
class DAtomItem : public QTreeWidgetItem, public BaseObj{
public:
    DAtomItem(node_desc_t, node_desc_t, int);

private:
    ext_weak_ptr_t<DLoadedAtomsItem> loaded_atoms_item;
    ext_weak_ptr_t<WFNData> wfn;
    const int atom_no;

//graphics
    Qt3D::QEntity *m_sphereEntity;
};


#endif //DAIMAN_ATOM_ITEM_H
