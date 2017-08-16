//
// Created by morrigan on 19/07/17.
//

#ifndef DAIMAN_DCRITICALPOINTSITEM_H
#define DAIMAN_DCRITICALPOINTSITEM_H


#include "iitem.h"
#include "wfx_item.h"
#include "ref_decorator.hpp"

struct DCriticalPointsAddMgpEvent : public QEvent{
    typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimeId;
    DCriticalPointsAddMgpEvent(TimeId time_id) : QEvent(QEvent::User), time_id{time_id} {};
    const TimeId time_id;
};

class DWfxItem;
class DCriticalPointsItem : public IItem{
Q_OBJECT
public:
    DCriticalPointsItem(node_desc_t wfx_item_idx);
    std::unique_ptr<QMenu> context_menu(QWidget*);
    bool event(QEvent *evt);

private:
    void openMgp(QString path);

private slots:
    void sl_open_mgp();

private:
    ext_weak_ptr_t<DWfxItem> wfx_item;
};


#endif //DAIMAN_DCRITICALPOINTSITEM_H
