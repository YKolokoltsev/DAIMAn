//
// Created by morrigan on 19/06/17.
//

#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QGuiApplication>
#include <QScreen>

#include "main_menu.h"
#include "doc_tree.hpp"

DMainMenu::DMainMenu(DocTree::node_desc_t main_wnd_idx) {
    reg_weak(this);
    main_window = std::move(doc_get_weak_obj_ptr<DMainWindow>(this,main_wnd_idx));
    main_window->ptr->setMenuBar(this);

    //main menu
    auto fileMenu = addMenu(tr("&File"));
    fileMenu->addAction(tr("&Open..."), this, SLOT(sl_open()) );
    recentFilesMenu = new QMenu("Recent Files",main_window->ptr.get());
    fileMenu->addMenu(recentFilesMenu);
    recentFilesMenu->setDisabled(true);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("E&xit"), main_window->ptr.get(),SLOT(close()) );

    //recent submenu
    for(int i = 0; i < GuiConfig::inst().max_recent; i++)
    recentFilesMenu->addAction("",this,SLOT(sl_open_recent()));
    updateRecentSubmenu();
};

void DMainMenu::sl_open(){
    QFileDialog dialog(main_window->ptr.get());

    //set dialog geometry
    QRect scr = QGuiApplication::primaryScreen()->availableGeometry();
    scr.setRect(scr.width()/4,scr.height()/4,scr.width()/2,scr.height()/2);
    dialog.setGeometry(scr);

    //configure dialog
    dialog.setNameFilter(tr("AIM Wave Function Files (*.wfx)"));
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setDirectory(GuiConfig::inst().recent_dir());

    //run dialog
    QStringList file_list;
    if(dialog.exec())
        file_list = dialog.selectedFiles();

    //open wfx document
    if(file_list.count() != 0)
        openWfx(file_list.at(0));
}

void DMainMenu::sl_open_recent(){
    QAction *action = qobject_cast<QAction *>(sender());
    openWfx(action->data().toString());
}

//PRIVATE FUNCTIONS
void DMainMenu::msgBox(QString msg){
    QMessageBox msgBox(main_window->ptr.get());
    msgBox.setText(msg);
    msgBox.exec();
}

void DMainMenu::updateRecentSubmenu(){
       auto files = GuiConfig::inst().read_recent();

       if(files.isEmpty()){
           recentFilesMenu->setDisabled(true);
       }else{
           recentFilesMenu->setDisabled(false);
           int i = 0;
           for(auto act : recentFilesMenu->actions()){
               if(i < files.size()){
                   act->setText(QFileInfo(files.at(i)).fileName());
                   act->setData(files.at(i));
                   act->setDisabled(false);
                   act->setVisible(true);
               }else{
                   act->setDisabled(true);
                   act->setVisible(false);
               }
               i++;
           }
       }
}

void DMainMenu::openWfx(QString path){
    cout << "opening file: " << path.toStdString() << endl;
    /*
    WFNData* wfn = new WFNData();

    WFXParser parser;
    parser.setWFN(wfn);
    parser.openFile(path);

    if(parser.parse())
    {
        if(wfn->calc_helpers())
        {
            doc->set_WFN(wfn);
            doc->set_wfn_file_path(path);
            glWidget->update();
            tb->initActions();
            return true;
        }
        delete wfn;
        return false;
    }
    else
        delete wfn;
    return false;

     ////
     if(addWFX(file_list.at(0))) {
            GuiConfig::inst().add_recent(file_list.at(0)); //save to recent

            //todo: create a tree node
            //QTreeWidgetItem * root = new QTreeWidgetItem((QTreeWidgetItem*) NULL);
            //root->setText(0,QFileInfo(parameters->str_recent.at(0)).fileName());
            //root->setText(1,QString("structure"));
            //treeWidget->insertTopLevelItem(0,root);
}else{
msgBox("Could not read " + file_list.at(0));
};
     */
}