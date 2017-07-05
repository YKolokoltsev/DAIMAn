//
// Created by morrigan on 5/07/17.
//

#include "loaded_atoms_item.h"
#include "atom_item.h"

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

