//
// Created by morrigan on 19/07/17.
//

#ifndef DAIMAN_DNACPITEM_H
#define DAIMAN_DNACPITEM_H

#include "base_vertex.hpp"
#include "ref_decorator.hpp"
#include "wfndata.h"
#include "cps_group.h"
#include "critical_point.hpp"

class DCriticalPointsGroup;
class DCpsFolderTypeItem : public IItem{
Q_OBJECT
public:
    DCpsFolderTypeItem(CriticalPoint::CP_TYPE type,
                       node_desc_t cp_group_idx,
                       list<node_desc_t> cp_idxs);
    std::unique_ptr<QMenu> context_menu(QWidget*);

public slots:
    void sl_show_all();
    void sl_hide_all();

private:
    ext_weak_ptr_t<DCriticalPointsGroup> cp_group;
    const CriticalPoint::CP_TYPE type;
};


#endif //DAIMAN_DNACPITEM_H
