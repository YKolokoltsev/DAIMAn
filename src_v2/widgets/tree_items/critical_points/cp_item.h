//
// Created by morrigan on 19/07/17.
//

#ifndef DAIMAN_DFOLDERNNACPITEM_H
#define DAIMAN_DFOLDERNNACPITEM_H

#include <QTreeWidgetItem>

#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>

#include "base_vertex.hpp"
#include "ref_decorator.hpp"
#include "wfndata.h"
#include "cps_folder_type_item.h"
#include "critical_point.hpp"

class DCpsFolderTypeItem;
class DCpItem  : public IItem{
Q_OBJECT
public:
    DCpItem(QString name, node_desc_t cp_folder_idx, node_desc_t cp_idx);

    std::unique_ptr<QMenu> context_menu(QWidget*);
    virtual void setChecked(bool checked);
    virtual void setData(int column, int role, const QVariant &value);

    void add3DEntity();
    void remove3DEntity();
    bool check_entity_exist();

private:
    ext_weak_ptr_t<DCpsFolderTypeItem> cp_folder;
    ext_shared_ptr_t<CriticalPoint> cp;
    Qt3DCore::QEntity* boundingBoxEntity;
};


#endif //DAIMAN_DFOLDERNNACPITEM_H
