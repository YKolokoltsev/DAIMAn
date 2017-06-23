//
// Created by morrigan on 19/06/17.
//

#ifndef DAIMAN_DMAINMENU_H
#define DAIMAN_DMAINMENU_H

#include <QMenuBar>
#include <QMainWindow>

#include "doc_tree.hpp"
#include "gui_config.h"
#include "main_window.h"

class DMainWindow;

class DMainMenu : public QMenuBar, public DocTree::BaseObj{
Q_OBJECT
public:
    DMainMenu(DocTree::node_desc_t);

private slots:
    void sl_open();
    void sl_open_recent();

private:
    void msgBox(QString);
    void updateRecentSubmenu();
    void openWfx(QString);

private:
    QMenu  *recentFilesMenu;
    DocTree::ext_node_ptr_t<DMainWindow> main_window;
};


#endif //DAIMAN_DMAINMENU_H
