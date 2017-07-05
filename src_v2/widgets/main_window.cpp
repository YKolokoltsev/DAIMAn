//
// Created by morrigan on 17/06/17.
//

#include <QScreen>
#include <QGuiApplication>
#include <QAction>

#include "main_window.h"
#include "main_menu.h"
#include "client_splitter.h"

DMainWindow::DMainWindow() {
    reg(this, false);

    QRect scr = QGuiApplication::primaryScreen()->availableGeometry();
    scr.setRect(scr.width()/10,scr.height()/10,scr.width()*4/5,scr.height()*4/5);
    setGeometry(scr);

    new DMainMenu(get_idx());
    new DClientSplitter(get_idx());

    show();
}





