//
// Created by morrigan on 5/07/17.
//

#include "loaded_atoms_item.h"
#include "atom_item.h"
#include "algorithm.hpp"

DLoadedAtomsItem::DLoadedAtomsItem(node_desc_t wfx_item_idx, node_desc_t wfx_idx){
    reg(this, true);
    setText(0, QString("Atoms"));

    wfx_item = std::move(get_weak_obj_ptr<DWfxItem>(this,wfx_item_idx));
    wfx_item->ptr.lock()->insertChild(0,this);

    //auto-load all atoms
    auto wfn = std::move(get_weak_obj_ptr<WFNData>(this, wfx_idx));
    auto p_wfn = wfn->ptr.lock();
    auto own_idx = get_idx();
    for(int atom_no = 0; atom_no < p_wfn->nat; atom_no++){
        new DAtomItem(own_idx, wfx_idx, atom_no);
    }
}

std::unique_ptr<QMenu> DLoadedAtomsItem::context_menu(QWidget* menu_parent){
    std::unique_ptr<QMenu> menu(new QMenu(menu_parent));

    bool can_show = false;
    bool can_hide = false;
    for_each_child<DAtomItem>(get_idx(),[&](DAtomItem* atom_item){
        bool entity_exist = atom_item->check_entity_exist();
        if(!entity_exist) can_show = true;
        if(entity_exist) can_hide = true;
    });

    auto show_atoms_act = menu->addAction(tr("Show atoms"), this, SLOT(sl_show_all_atoms()) );
    show_atoms_act->setEnabled(can_show);
    auto hide_atoms_act = menu->addAction(tr("Hide atoms"), this, SLOT(sl_hide_all_atoms()) );
    hide_atoms_act->setEnabled(can_hide);

    //menu->addAction(tr("Remove"), tree_ctrl->ptr.lock().get(), SLOT(sl_del_item()) );
    return menu;
}

void DLoadedAtomsItem::sl_show_all_atoms(){
    for_each_child<DAtomItem>(get_idx(),[](DAtomItem* atom_item){
        atom_item->add3DEntity();
    });
}

void DLoadedAtomsItem::sl_hide_all_atoms(){
    for_each_child<DAtomItem>(get_idx(),[](DAtomItem* atom_item){
        atom_item->remove3DEntity();
    });
}

