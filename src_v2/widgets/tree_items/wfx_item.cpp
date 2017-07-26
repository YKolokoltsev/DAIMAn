//
// Created by morrigan on 5/07/17.
//

#include "wfx_item.h"
#include "loaded_atoms_item.h"
#include "gl_screen.h"
#include "widgets/tree_items/critical_points/critical_points_item.h"

DWfxItem::DWfxItem(QString path, node_desc_t tree_ctrl_idx, node_desc_t wfn_idx): path{path} {
    reg(this, true);
    setText(0, QFileInfo(path).fileName());

    tree_ctrl = std::move(get_weak_obj_ptr<DTreeCtrl>(this,tree_ctrl_idx));
    wfn = std::move(get_shared_obj_ptr<WFNData>(this,wfn_idx));

    tree_ctrl->ptr.lock()->insertTopLevelItem(0, this);

    //add standard folders
    new DCriticalPointsItem(get_idx());
    new DLoadedAtomsItem(get_idx(),wfn->get_idx());
}

std::unique_ptr<QMenu> DWfxItem::context_menu(QWidget* menu_parent){
    std::unique_ptr<QMenu> menu(new QMenu(menu_parent));

    //todo: change it to eye icon somewhere in toolbar for example
    menu->addAction("Activate Scene", this, SLOT(sl_activate_scene()) );
    menu->addAction(tr("Delete"), tree_ctrl->ptr.lock().get(), SLOT(sl_del_item()) );
    return menu;
}

void DWfxItem::sl_activate_scene(){
    auto v_client_splitter = doc_first_nearest_father<DClientSplitter>(get_idx());
    auto v_gl_screen = doc_first_nearest_child<DGlScreen>(v_client_splitter);
    auto gl_screen = std::move(get_weak_obj_ptr<DGlScreen>(this,v_gl_screen));
    gl_screen->ptr.lock()->switch_scene(wfn->get_idx());
}

