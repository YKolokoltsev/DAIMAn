//
// Created by morrigan on 19/07/17.
//

#include "critical_points_item.h"
#include "nacp_items.h"
#include "nnacp_items.h"

DCriticalPointsItem::DCriticalPointsItem(node_desc_t wfx_item_idx){
    reg(this, true);
    setText(0, QString("Critical Points"));

    wfx_item = std::move(get_weak_obj_ptr<DWfxItem>(this,wfx_item_idx));
    wfx_item->ptr.lock()->insertChild(0,this);

    new DFolderNACPItem(get_idx());
    new DFolderNNACPItem(get_idx());
}

std::unique_ptr<QMenu> DCriticalPointsItem::context_menu(QWidget* menu_parent){
    std::unique_ptr<QMenu> menu(new QMenu(menu_parent));
    menu->addAction(tr("Read from *.mgp"), this, SLOT(sl_open_mgp()) );
    return menu;
}

void DCriticalPointsItem::sl_open_mgp(){
    auto v_main_window = doc_first_nearest_father<DMainWindow>(get_idx());
    auto main_window = get_weak_obj_ptr<DMainWindow>(this, v_main_window);
    auto mw = main_window->ptr.lock();

    QStringList file_list = mw->open_file_dlg(tr("AIM molecular graph points (*.mgp)"));

    //open mgp document
    if(file_list.count() != 0)
        openMgp(file_list.at(0));
}

void DCriticalPointsItem::openMgp(QString path){
    cout << "todo: run parser task" << endl;
}