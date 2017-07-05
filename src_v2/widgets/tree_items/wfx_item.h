//
// Created by morrigan on 26/06/17.
//

#ifndef DAIMAN_WFX_ITEM_H
#define DAIMAN_WFX_ITEM_H

#include <QTreeWidgetItem>
#include <QString>
#include <QFileInfo>
#include <QEvent>
#include <QPoint>
#include <QMenu>

#include "base_vertex.hpp"
#include "graph.h"
#include "wfndata.h"
#include "algorithm.hpp"
#include "tree_ctrl.h"

/*class WfxItemEvent : public QEvent{
public:
    enum TYPE{Context, Focus};
    WfxItemEvent(TYPE type, QPoint pt) : QEvent(QEvent::User), type{type}, pt{pt} {}
    TYPE type;
    QPoint pt;
};*/

class DTreeCtrl;
class DWfxItem : public QObject, public QTreeWidgetItem, public BaseObj {
Q_OBJECT
public:
    DWfxItem(QString, node_desc_t, node_desc_t);

    /*bool event( QEvent *evt ){
        auto my_event = dynamic_cast<WfxItemEvent*>(evt);
        if(my_event->type == WfxItemEvent::Context){
            show_context_menu(my_event->pt);
        }else if(my_event->type == WfxItemEvent::Focus){
            cout << "focus event stub" << endl;
        }
        return QObject::event(evt);
    }*/

    void reset();
    QMenu* context_menu();

private slots:
    void sl_load_atoms();

private:
    ext_weak_ptr_t<DTreeCtrl> tree_ctrl;
    ext_weak_ptr_t<WFNData> wfn;
    const QString path;
};

#endif //DAIMAN_WFX_ITEM_H
