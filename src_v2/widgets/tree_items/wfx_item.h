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
#include "iitem.h"

class DTreeCtrl;
class DWfxItem : public IItem{
Q_OBJECT
public:
    DWfxItem(QString, node_desc_t, node_desc_t);

    std::unique_ptr<QMenu> context_menu(QWidget*);

private slots:
    void sl_activate_scene();

private:
    ext_weak_ptr_t<DTreeCtrl> tree_ctrl;
    ext_shared_ptr_t<WFNData> wfn;
    const QString path;
};

#endif //DAIMAN_WFX_ITEM_H
