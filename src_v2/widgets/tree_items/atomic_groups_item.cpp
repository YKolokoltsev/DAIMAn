//
// Created by morrigan on 3/27/18.
//

#include <QInputDialog>
#include <QMessageBox>

#include "atomic_groups_item.h"
#include "atomic_group_item.h"

DAtomicGroupsItem::DAtomicGroupsItem(node_desc_t wfx_item_idx, node_desc_t atoms_item_idx){
    reg(this, true);
    setText(0, QString("Atomic Groups"));

    wfx_item = std::move(get_weak_obj_ptr<DWfxItem>(this,wfx_item_idx));
    auto sz = wfx_item->ptr.lock()->childCount();
    wfx_item->ptr.lock()->insertChild(sz, this);

    atoms_item = std::move(get_weak_obj_ptr<DLoadedAtomsItem>(this,atoms_item_idx));
}

std::unique_ptr<QMenu>
DAtomicGroupsItem::context_menu(QWidget* menu_parent){
    std::unique_ptr<QMenu> menu(new QMenu(menu_parent));

    auto new_atomic_group_act = menu->addAction(tr("Group selected atoms"), this, SLOT(sl_new_atomic_group()) );
    new_atomic_group_act->setEnabled(!atoms_item->ptr.lock()->list_selected_atoms().empty());

    return menu;
}

void DAtomicGroupsItem::sl_new_atomic_group(){

    auto selected = atoms_item->ptr.lock()->list_selected_atoms();
    bool ok;

    QString text = QInputDialog::getText(nullptr, tr("QInputDialog::getText()"),
                                         tr("Atomic group name:"), QLineEdit::Normal,
                                         "NewGroup", &ok);
    if(!ok) return;

    if(text.isEmpty()) {
        QMessageBox msgBox;
        msgBox.setText("Group name can't be empty");
        msgBox.exec();
        //todo: also check A-Za-z0-9 template, this text will be converted to stdString
        return;
    }

    new DAtomicGroupItem(this->get_idx(), text);
}

