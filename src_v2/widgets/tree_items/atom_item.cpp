//
// Created by morrigan on 5/07/17.
//

#include <Qt3DExtras>

#include <openbabel/data.h>

#include "atom_item.h"
#include "top3dentity.hpp"

DAtomItem::DAtomItem(node_desc_t loaded_atoms_item_idx, node_desc_t wfn_idx, int atom_no) :
        atom_no{atom_no},
        boundingBoxEntity{nullptr}{
    reg(this, true);

    setCheckState(1,Qt::Unchecked);

    wfn = std::move(get_weak_obj_ptr<WFNData>(this, wfn_idx));
    setText(0, QString(wfn->ptr.lock()->atnames[atom_no]));

    loaded_atoms_item = std::move(get_weak_obj_ptr<DLoadedAtomsItem>(this,loaded_atoms_item_idx));
    loaded_atoms_item->ptr.lock()->insertChild(0,this);
};

void DAtomItem::setData(int column, int role, const QVariant &value){
    const bool isCheckChange = column == 1
           && role == Qt::CheckStateRole
           && data(column, role).isValid() // Don't "change" during initialization
           && checkState(1) != value;

    if(isCheckChange && boundingBoxEntity != nullptr){
        if(boundingBoxEntity->isEnabled() != (Qt::CheckState::Checked == value))
            boundingBoxEntity->setEnabled(Qt::CheckState::Checked == value);
    }

    QTreeWidgetItem::setData(column, role, value);
}

void DAtomItem::setChecked(bool checked){
    QVariant value = checked ? Qt::CheckState::Checked : Qt::CheckState::Unchecked;
    setData(1, Qt::CheckStateRole, value);
}

bool DAtomItem::check_entity_exist(){
    //todo: valid only if there is just one entity, but there can be many
    auto v_entity = doc_first_nearest_child<Top3DEntity<DAtomItem>>(get_idx());
    return (v_entity != DocTree::inst()->null_vertex);
}

void DAtomItem::remove3DEntity(){
    //todo: valid only if there is just one entity, but there can be many
    auto v_entity = doc_first_nearest_child<Top3DEntity<DAtomItem>>(get_idx());
    if(v_entity == DocTree::inst()->null_vertex) return;
    remove_recursive(v_entity);
    boundingBoxEntity = nullptr;
}

void DAtomItem::add3DEntity(){
    if(check_entity_exist()) return;
    auto atomEntity = new Top3DEntity<DAtomItem>(get_idx());

    //data
    auto data = wfn->ptr.lock();
    OpenBabel::OBElementTable etab;
    int atnumber = (int)data->atcharges[atom_no];

    //atomic sphere
    Qt3DCore::QEntity* shpereEntity = new Qt3DCore::QEntity;

        //atomic sphere (material)
        Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial;
        std::vector<double> rgb = etab.GetRGB(atnumber);
        material->setDiffuse(QColor((int)(255.0*rgb[0]),(int)(255.0*rgb[1]),(int)(255.0*rgb[2])));

        //atom sphere (mesh)
        Qt3DExtras::QSphereMesh *sphereMesh = new Qt3DExtras::QSphereMesh;
        sphereMesh->setRadius(0.5f);

        //atom sphere (transform)
        Qt3DCore::QTransform *atomTransform = new Qt3DCore::QTransform();
        atomTransform->setScale(1.0f);
        atomTransform->setTranslation(QVector3D(data->atcoords[atom_no][0],
                                                data->atcoords[atom_no][1],
                                                data->atcoords[atom_no][2]));

    shpereEntity->addComponent(atomTransform);
    shpereEntity->addComponent(material);
    shpereEntity->addComponent(sphereMesh);
    shpereEntity->setParent(atomEntity);

    //bounding box
    boundingBoxEntity = new Qt3DCore::QEntity;

        //bounding box(mesh)
        Qt3DExtras::QSphereMesh *boundingBoxMesh = new Qt3DExtras::QSphereMesh;
        boundingBoxMesh->setRadius(0.55f);

        //bounding box(transform)
        Qt3DCore::QTransform *cuboidTransform = new Qt3DCore::QTransform();
        cuboidTransform->setScale(1.0f);
        cuboidTransform->setTranslation(QVector3D(data->atcoords[atom_no][0],
                                                data->atcoords[atom_no][1],
                                                data->atcoords[atom_no][2]));

        //bounding box(material)
        Qt3DExtras::QPhongAlphaMaterial *alpha_material = new Qt3DExtras::QPhongAlphaMaterial;
        alpha_material->setDiffuse(QColor(0,255,0));
        alpha_material->setAlpha(0.4f);

    boundingBoxEntity->addComponent(boundingBoxMesh);
    boundingBoxEntity->addComponent(alpha_material);
    boundingBoxEntity->addComponent(cuboidTransform);
    boundingBoxEntity->setParent(atomEntity);
    boundingBoxEntity->setObjectName("boundingBoxEntity");
    boundingBoxEntity->setEnabled(checkState(1) == Qt::CheckState::Checked);
}