//
// Created by morrigan on 17/06/17.
//

#include <QScreen>
#include <QGuiApplication>
#include <QAction>
#include <QFileDialog>

#include "main_window.h"
#include "main_menu.h"
#include "client_splitter.h"
#include "thread_pool.h"

DMainWindow::DMainWindow() {
    reg(this, true);

    QRect scr = QGuiApplication::primaryScreen()->availableGeometry();
    scr.setRect(scr.width()/10,scr.height()/10,scr.width()*4/5,scr.height()*4/5);
    setGeometry(scr);

    new DMainMenu(get_idx());
    new DClientSplitter(get_idx());
    new DThreadPool(get_idx());

    show();
}

QStringList DMainWindow::open_file_dlg(const QString& name_filter){
    QFileDialog dialog(this);

    //set dialog geometry
    QRect scr = QGuiApplication::primaryScreen()->availableGeometry();
    scr.setRect(scr.width()/4,scr.height()/4,scr.width()/2,scr.height()/2);
    dialog.setGeometry(scr);

    //configure dialog
    dialog.setNameFilter(name_filter);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setDirectory(GuiConfig::inst().recent_dir());

    //run dialog
    QStringList file_list;
    if(dialog.exec())
        file_list = dialog.selectedFiles();

    //open wfx document
    return file_list;
}





