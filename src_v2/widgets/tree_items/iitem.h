//
// Created by morrigan on 11/07/17.
//

#ifndef DAIMAN_IITEM_H
#define DAIMAN_IITEM_H

#include <QTreeWidgetItem>
#include <QMenu>

#include "base_vertex.hpp"

/*class WfxItemEvent : public QEvent{
public:
    enum TYPE{Context, Focus};
    WfxItemEvent(TYPE type, QPoint pt) : QEvent(QEvent::User), type{type}, pt{pt} {}
    TYPE type;
    QPoint pt;
};*/

class IItem : public QObject, public QTreeWidgetItem, public BaseObj {
public:

    /*bool event( QEvent *evt ){
        auto my_event = dynamic_cast<WfxItemEvent*>(evt);
        if(my_event->type == WfxItemEvent::Context){
            show_context_menu(my_event->pt);
        }else if(my_event->type == WfxItemEvent::Focus){
            cout << "focus event stub" << endl;
        }
        return QObject::event(evt);
    }*/

    virtual std::unique_ptr<QMenu> context_menu(QWidget* menu_parent) = 0;
    virtual void setChecked(bool checked){};
};


#endif //DAIMAN_IITEM_H
