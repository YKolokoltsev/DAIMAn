//
// Created by morrigan on 19/06/17.
//

#ifndef DAIMAN_DMAINMENU_H
#define DAIMAN_DMAINMENU_H

#include <QMenuBar>
#include <QMainWindow>

#include "base_vertex.hpp"
#include "graph.h"
#include "ref_decorator.hpp"
#include "gui_config.h"
#include "main_window.h"

class DMainWindow;

class DMainMenu : public QMenuBar, public BaseObj{
Q_OBJECT
public:
    DMainMenu(node_desc_t);

private slots:
    void sl_open();
    void sl_open_recent();

private:
    void msgBox(QString);
    void updateRecentSubmenu();
    void openWfx(QString);

private:
    QMenu  *recentFilesMenu;
    ext_weak_ptr_t<DMainWindow> main_window;
};


#endif //DAIMAN_DMAINMENU_H
