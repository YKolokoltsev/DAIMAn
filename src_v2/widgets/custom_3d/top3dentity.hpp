//
// Created by morrigan on 10/07/17.
//

#ifndef DAIMAN_TOP3DENTITY_H
#define DAIMAN_TOP3DENTITY_H

#include <memory>

#include <Qt3DCore/QEntity>
#include <Qt3DRender/QObjectPicker>

#include "ref_decorator.hpp"
#include "gl_screen.h"
#include "algorithm.hpp"
#include "wfndata.h"
#include "scene_entity.h"
#include "line_entity.h"

/*
 * Each 3D object with all it's attached components, such as mesh,  materials,  etc..
 * is described and handled by it's top level entity, the Qt3DCore::QEntity.
 * This entity is then attached to the curr_scene root entity. However, this entity
 * can depend on the other objects in the DocGraph, and this dependency
 * shell be explicitly introduced into it. This class wraps such dependency.
 *
 * ...may be some callbacks are needed for updates, the moment is that a
 * TParent shell know details of the Top3DEntity content and implement
 * all specifics...
 */
class DGlScreen;
class DSceneEntity;
class Top3DEntityPicker;

struct Top3DEntityCheckable{
    virtual void setChecked(bool){};
};

template<typename TParent>
struct Top3DEntity : public Qt3DCore::QEntity, public BaseObj, public Top3DEntityCheckable{
    Top3DEntity(node_desc_t parent_idx){
        reg(this, false);
        parent = std::move(get_weak_obj_ptr<TParent>(this, parent_idx));

        ///

        Qt3DRender::QObjectPicker* const objectPicker = new Top3DEntityPicker(this);
        addComponent(objectPicker);
    }

    void add_to_scene();

    virtual void setChecked(bool checked){
        //just stand-alone geometric object, no sense to check
        if(parent == nullptr) return;

        //if there are many scenes hidden objects shell not be checked
        //todo: even with "QPickingSettings::AllPicks" this moment is little buggy (not critical)
        auto scene = parentNode();
        if(scene == nullptr || !scene->isEnabled()) return;

        //parent is not IItem, so add here the other cases if needed
        auto iitem = std::dynamic_pointer_cast<IItem>(parent->ptr.lock());
        if(iitem == nullptr) return;
        iitem->setChecked(checked);
    };

    ext_weak_ptr_t<TParent> parent;
};

class Top3DEntityPicker : public Qt3DRender::QObjectPicker{
Q_OBJECT
public:

    Top3DEntityPicker(Qt3DCore::QEntity * entity) : Qt3DRender::QObjectPicker(entity){
        connect( this, SIGNAL(clicked(Qt3DRender::QPickEvent* )),
                 this, SLOT(clicked_sl(Qt3DRender::QPickEvent*)) );
    }

public slots:
    void clicked_sl(Qt3DRender::QPickEvent *pick){

        try{
            auto top3DEntity = dynamic_cast<Qt3DCore::QEntity*>(this->parentNode());
            if(top3DEntity == nullptr) throw runtime_error("Top3DEntityPicker has no parent entity");

            auto boundingBoxEntity = top3DEntity->findChild<Qt3DCore::QEntity*>("boundingBoxEntity");
            if(boundingBoxEntity == nullptr) throw runtime_error("boundingBoxEntity not defined");
            boundingBoxEntity->setEnabled(!boundingBoxEntity->isEnabled());

            auto top3DEntityCheckable = dynamic_cast<Top3DEntityCheckable*>(this->parentNode());
            if(top3DEntityCheckable == nullptr) throw runtime_error("geometry parent incompatible with Top3DEntityCheckable");
            top3DEntityCheckable->setChecked(boundingBoxEntity->isEnabled());
        }catch(runtime_error& e){
            std::cerr << e.what() << endl;
        }

    }
};

#endif //DAIMAN_TOP3DENTITY_H
