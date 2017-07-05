//
// Created by morrigan on 5/07/17.
//

#include "atom_item.h"

DAtomItem::DAtomItem(node_desc_t loaded_atoms_item_idx, node_desc_t wfn_idx, int atom_no) :
        atom_no{atom_no} {
    reg(this, true);

    wfn = std::move(get_weak_obj_ptr<WFNData>(this, wfn_idx));
    setText(0, QString(wfn->ptr.lock()->atnames[atom_no]));

    loaded_atoms_item = std::move(get_weak_obj_ptr<DLoadedAtomsItem>(this,loaded_atoms_item_idx));
    loaded_atoms_item->ptr.lock()->insertChild(0,this);
};

//so now, each time when I need an atom (for example to show it), I can go to
//wfn and get that atom coordinates and other properties, I do not store here
//any info duplicates that is already stored in wfn