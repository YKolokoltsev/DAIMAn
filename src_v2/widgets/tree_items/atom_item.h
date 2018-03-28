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
class DAtomItem : public IItem{
public:
    DAtomItem(node_desc_t, node_desc_t, int);
    void add3DEntity();
    void remove3DEntity();
    bool check_entity_exist();

    virtual void setData(int column, int role, const QVariant &value);
    virtual std::unique_ptr<QMenu> context_menu(QWidget* menu_parent){
        return std::unique_ptr<QMenu>(nullptr);
    };
    virtual void setChecked(bool checked);
    bool isChecked();
    int getAtomNo(){return atom_no;}
private:
    ext_weak_ptr_t<DLoadedAtomsItem> loaded_atoms_item;
    ext_shared_ptr_t<WFNData> wfn;
    const int atom_no;
    Qt3DCore::QEntity* boundingBoxEntity;
};


#endif //DAIMAN_ATOM_ITEM_H
