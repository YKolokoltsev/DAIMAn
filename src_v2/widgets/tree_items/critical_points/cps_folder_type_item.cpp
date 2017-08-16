//
// Created by morrigan on 19/07/17.
//

#include "cps_folder_type_item.h"
#include "cp_item.h"


DCpsFolderTypeItem::DCpsFolderTypeItem(CriticalPoint::CP_TYPE type,
                                       node_desc_t cp_group_idx,
                                       list<node_desc_t> cp_idxs)
:type{type}
{
    reg(this, true);
    auto name = QString::fromStdString(CriticalPoint::type_name(type));
    setText(0, name);

    cp_group = std::move(get_weak_obj_ptr<DCriticalPointsGroup>(this,cp_group_idx));
    cp_group->ptr.lock()->insertChild(0,this);

    int digits = to_string(cp_idxs.size()).length();
    int i = cp_idxs.size();
    for(auto idx : cp_idxs){
        auto name = to_string(i);
        if(name.length() < digits)
            name.insert(0,digits - name.length(),'0');
        name.insert(0,1,'#');
        new DCpItem(QString::fromStdString(name), get_idx(), idx);
        i--;
    }
}


std::unique_ptr<QMenu> DCpsFolderTypeItem::context_menu(QWidget* menu_parent){
    std::unique_ptr<QMenu> menu(new QMenu(menu_parent));

    bool can_show = false;
    bool can_hide = false;
    for_each_child<DCpItem>(get_idx(),[&](DCpItem* cp_item){
        bool entity_exist = cp_item->check_entity_exist();
        if(!entity_exist) can_show = true;
        if(entity_exist) can_hide = true;
    });

    auto show_atoms_act = menu->addAction(tr("Show all"), this, SLOT(sl_show_all()) );
    show_atoms_act->setEnabled(can_show);
    auto hide_atoms_act = menu->addAction(tr("Hide all"), this, SLOT(sl_hide_all()) );
    hide_atoms_act->setEnabled(can_hide);

    return menu;
}

void DCpsFolderTypeItem::sl_show_all(){
    for_each_child<DCpItem>(get_idx(),[](DCpItem* atom_item){
        atom_item->add3DEntity();
    });
}

void DCpsFolderTypeItem::sl_hide_all(){
    for_each_child<DCpItem>(get_idx(),[](DCpItem* atom_item){
        atom_item->remove3DEntity();
    });
}

