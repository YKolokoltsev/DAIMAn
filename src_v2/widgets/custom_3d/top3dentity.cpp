#include "top3dentity.hpp"
#include "atom_item.h"

template<typename TParent>
void Top3DEntity<TParent>::add_to_scene(){
    auto v_wfx = doc_first_nearest_father<WFNData>(get_idx());
    auto v_scene = doc_first_nearest_child<DSceneEntity>(v_wfx);
    auto scene = get_shared_obj_ptr<DSceneEntity>(this, v_scene);
    scene->exec_rw([&](DSceneEntity* p_scene)->void {this->setParent(p_scene);});
}

//todo: remove this add hook, the problem is that DSceneEntity is incomplete in .hpp
template class Top3DEntity<DAtomItem>;

