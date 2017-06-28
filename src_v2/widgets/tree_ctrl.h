//
// Created by morrigan on 20/06/17.
//

#ifndef DAIMAN_DTREECTRL_H
#define DAIMAN_DTREECTRL_H

#include <QTreeWidget>
#include <QEvent>

#include "graph.h"
#include "ref_decorator.hpp"
#include "client_splitter.h"

class TreeCtrlEvent : public QEvent{
public:
    enum TYPE{DeleteWfxItem};
    TreeCtrlEvent(TYPE type) : QEvent(QEvent::User), type{type} {}
    TYPE type;
};

class DClientSplitter;
class DTreeCtrl: public QTreeWidget, public BaseObj {
Q_OBJECT
public:
    DTreeCtrl(node_desc_t);
    void add_wfx_item(QString, node_desc_t);

protected:
    void contextMenuEvent(QContextMenuEvent*);

private:
    ext_weak_ptr_t<DClientSplitter> client_edge;
};


#endif //DAIMAN_DTREECTRL_H
