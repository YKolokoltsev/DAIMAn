//
// Created by morrigan on 5/07/17.
//

#include "wfx_item.h"
#include "loaded_atoms_item.h"

DWfxItem::DWfxItem(QString path, node_desc_t tree_ctrl_idx, node_desc_t wfn_idx): path{path} {
    reg(this, true);
    setText(0, QFileInfo(path).fileName());

    tree_ctrl = std::move(get_weak_obj_ptr<DTreeCtrl>(this,tree_ctrl_idx));
    wfn = std::move(get_weak_obj_ptr<WFNData>(this,wfn_idx));

    tree_ctrl->ptr.lock()->insertTopLevelItem(0, this);
}

void DWfxItem::reset(){
    tree_ctrl.reset();
    auto idx_wfx = wfn->ptr.lock()->get_idx();
    wfn.reset();
    remove_recursive(idx_wfx);
}

QMenu* DWfxItem::context_menu(){
    QMenu* menu = new QMenu(dynamic_cast<QTreeWidget*>(tree_ctrl->ptr.lock().get()));

    auto load_atoms_act = menu->addAction(tr("Load atoms"), this, SLOT(sl_load_atoms()) );
    auto loaded_atoms_idx = doc_first_nearest_child<DLoadedAtomsItem*>(get_idx());
    if(loaded_atoms_idx == DocTree::inst()->null_vertex){
        load_atoms_act->setEnabled(true);
    }else{
        load_atoms_act->setEnabled(false);
    }

    menu->addAction(tr("Delete"), tree_ctrl->ptr.lock().get(), SLOT(sl_del_item()) );
    return menu;
}

void DWfxItem::sl_load_atoms(){
    new DLoadedAtomsItem(get_idx(),wfn->ptr.lock()->get_idx());
};

