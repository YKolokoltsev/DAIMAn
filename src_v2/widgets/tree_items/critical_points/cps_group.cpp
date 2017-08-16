//
// Created by morrigan on 12/08/17.
//

#include <set>

#include "cps_group.h"
#include "cps_folder_type_item.h"

DCriticalPointsGroup::DCriticalPointsGroup(QString name, node_desc_t dcp_idx){
    reg(this, true);

    setText(0, QString(name));

    dcp_item = std::move(get_weak_obj_ptr<DCriticalPointsItem>(this,dcp_idx));
    dcp_item->ptr.lock()->insertChild(0,this);
}

void DCriticalPointsGroup::add_cps(tTypeIdxList typed_idxs){

    std::set<CriticalPoint::CP_TYPE> present_types;
    for(const auto& ti : typed_idxs)
        present_types.insert(ti.first);

    for(const auto& type : present_types){
        list<node_desc_t> idxs;
        for(const auto& ti : typed_idxs)
            if(ti.first == type) idxs.push_back(ti.second);

        cout << "Adding folder for " << idxs.size() << " " << CriticalPoint::type_name(type) << endl;
        new DCpsFolderTypeItem(type, get_idx(), idxs);
    }

}

std::unique_ptr<QMenu> DCriticalPointsGroup::context_menu(QWidget*){
    return std::unique_ptr<QMenu>();
}