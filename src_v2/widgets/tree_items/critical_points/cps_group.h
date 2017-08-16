//
// Created by morrigan on 12/08/17.
//

#ifndef DAIMAN_DCRITICALPOINTSGROUP_H
#define DAIMAN_DCRITICALPOINTSGROUP_H

#include <list>

#include "iitem.h"
#include "critical_points_item.h"
#include "ref_decorator.hpp"
#include "critical_point.hpp"

class DCriticalPointsGroup : public IItem{
    Q_OBJECT
public:
    DCriticalPointsGroup(QString name, node_desc_t dcp_idx);
    std::unique_ptr<QMenu> context_menu(QWidget*);
    void add_cps(tTypeIdxList idxs);

private slots:

private:
    ext_weak_ptr_t<DCriticalPointsItem> dcp_item;
};


#endif //DAIMAN_DCRITICALPOINTSGROUP_H
