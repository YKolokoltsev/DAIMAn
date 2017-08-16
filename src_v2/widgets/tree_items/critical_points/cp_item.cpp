//
// Created by morrigan on 19/07/17.
//

#include "cp_item.h"
#include "top3dentity.hpp"
//todo: make base for items-3d objects
DCpItem::DCpItem(QString name, node_desc_t cp_folder_idx, node_desc_t cp_idx) :
        boundingBoxEntity{nullptr}{
    reg(this, true);

    setText(0, name);
    setCheckState(1,Qt::Unchecked);

    cp_folder = std::move(get_weak_obj_ptr<DCpsFolderTypeItem>(this,cp_folder_idx));
    cp_folder->ptr.lock()->insertChild(0,this);

    cp = get_shared_obj_ptr<CriticalPoint>(this, cp_idx);
}

void DCpItem::setData(int column, int role, const QVariant &value){
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

void DCpItem::setChecked(bool checked){
    QVariant value = checked ? Qt::CheckState::Checked : Qt::CheckState::Unchecked;
    setData(1, Qt::CheckStateRole, value);
}

bool DCpItem::check_entity_exist(){
    //todo: valid only if there is just one entity, but there can be many
    auto v_entity = doc_first_nearest_child<Top3DEntity<DCpItem>>(get_idx());
    return (v_entity != DocTree::inst()->null_vertex);
}

void DCpItem::remove3DEntity(){
    //todo: valid only if there is just one entity, but there can be many
    auto v_entity = doc_first_nearest_child<Top3DEntity<DCpItem>>(get_idx());
    if(v_entity == DocTree::inst()->null_vertex) return;
    remove_recursive(v_entity);
    boundingBoxEntity = nullptr;
}

void DCpItem::add3DEntity(){
    if(check_entity_exist()) return;
    auto cpEntity = new Top3DEntity<DCpItem>(get_idx());

    double x,y,z;
    cp->exec_r([&x, &y, &z](const CriticalPoint* cp){
        x = cp->xyz[0];
        y = cp->xyz[1];
        z = cp->xyz[2];
    });

    //atomic sphere
    Qt3DCore::QEntity* shpereEntity = new Qt3DCore::QEntity;

    //atomic sphere (material)
    Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial;
    material->setDiffuse(QColor((int)(0),(int)(255.0),(int)(0)));

    //atom sphere (mesh)
    Qt3DExtras::QSphereMesh *sphereMesh = new Qt3DExtras::QSphereMesh;
    sphereMesh->setRadius(0.1f);

    //atom sphere (transform)
    Qt3DCore::QTransform *atomTransform = new Qt3DCore::QTransform();
    atomTransform->setScale(1.0f);
    atomTransform->setTranslation(QVector3D(x, y, z));

    shpereEntity->addComponent(atomTransform);
    shpereEntity->addComponent(material);
    shpereEntity->addComponent(sphereMesh);
    shpereEntity->setParent(cpEntity);

    //bounding box
    boundingBoxEntity = new Qt3DCore::QEntity;

    //bounding box(mesh)
    Qt3DExtras::QSphereMesh *boundingBoxMesh = new Qt3DExtras::QSphereMesh;
    boundingBoxMesh->setRadius(0.15f);

    //bounding box(transform)
    Qt3DCore::QTransform *cuboidTransform = new Qt3DCore::QTransform();
    cuboidTransform->setScale(1.0f);
    cuboidTransform->setTranslation(QVector3D(x, y, z));

    //bounding box(material)
    Qt3DExtras::QPhongMaterial *alpha_material = new Qt3DExtras::QPhongMaterial;
    alpha_material->setDiffuse(QColor(200,0,0));

    boundingBoxEntity->addComponent(boundingBoxMesh);
    boundingBoxEntity->addComponent(alpha_material);
    boundingBoxEntity->addComponent(cuboidTransform);
    boundingBoxEntity->setParent(cpEntity);
    boundingBoxEntity->setObjectName("boundingBoxEntity");
    boundingBoxEntity->setEnabled(checkState(1) == Qt::CheckState::Checked);

    cpEntity->add_to_scene();
}


std::unique_ptr<QMenu> DCpItem::context_menu(QWidget*){
    return std::unique_ptr<QMenu>(nullptr);
}