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

class WfxItemEvent : public QEvent{
public:
    enum TYPE{Context, Focus};
    WfxItemEvent(TYPE type, QPoint pt) : QEvent(QEvent::User), type{type}, pt{pt} {}
    TYPE type;
    QPoint pt;
};

class DWfxItem : public QObject, public QTreeWidgetItem, public BaseObj {
Q_OBJECT
public:
    DWfxItem(QString path, node_desc_t tree_ctrl_idx, node_desc_t wfn_idx) : path{path} {
        reg_weak(this);
        setText(0, QFileInfo(path).fileName());

        tree_ctrl = std::move(get_weak_obj_ptr<DTreeCtrl>(this,tree_ctrl_idx));
        wfn = std::move(get_weak_obj_ptr<WFNData>(this,wfn_idx));

        tree_ctrl->ptr->insertTopLevelItem(0, this);
    }

    ~DWfxItem(){
        if(wfn->ptr) {
            auto wfn_idx = wfn->ptr->get_idx();
            //wfn.release();
            //cout << "TRY TO REMOVE RECURSIVE" << endl;
            //remove_recursive(wfn_idx);
            //cout << "REMOVED" << endl;
        }
    }

    bool event( QEvent *evt ){
        auto my_event = dynamic_cast<WfxItemEvent*>(evt);
        if(my_event->type == WfxItemEvent::Context){
            show_context_menu(my_event->pt);
        }else if(my_event->type == WfxItemEvent::Focus){
            cout << "focus event stub" << endl;
        }
        return QObject::event(evt);
    }

private:
    void show_context_menu(QPoint pt){
        std::unique_ptr<QMenu> menu(new QMenu(dynamic_cast<QTreeWidget*>(tree_ctrl->ptr.get())));

        menu->addAction(tr("&delete"), this, SLOT(sl_delete()) );

        menu->exec(pt);
        menu->clear();
    }

private slots:
    void sl_delete(){
        cout << "try to delete" << endl;
        //todo: send msg? tree_ctrl->ptr->removeItemWidget(this, 0);
        cout << "deleted" << endl;
    };



private:
    ext_weak_ptr_t<DTreeCtrl> tree_ctrl;
    ext_weak_ptr_t<WFNData> wfn;
    const QString path;
};

#endif //DAIMAN_WFX_ITEM_H
