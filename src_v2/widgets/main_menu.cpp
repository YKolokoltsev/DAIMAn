//
// Created by morrigan on 19/06/17.
//

#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QGuiApplication>
#include <QApplication>

#include "main_menu.h"
#include "tree_ctrl.h"
#include "wfxparser.h"
#include "scene_entity.h"

DMainMenu::DMainMenu(node_desc_t main_wnd_idx) {
    reg(this, true);
    main_window = std::move(get_weak_obj_ptr<DMainWindow>(this, main_wnd_idx));

    auto mw = main_window->ptr.lock();

        //main menu
    mw->setMenuBar(this);
    auto fileMenu = this->addMenu(tr("&File"));
    fileMenu->addAction(tr("&Open..."), this, SLOT(sl_open()) );
    recentFilesMenu = new QMenu("Recent Files",mw.get());
    fileMenu->addMenu(recentFilesMenu);
    recentFilesMenu->setDisabled(true);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("E&xit"), mw.get(), SLOT(close()) );

    //recent submenu
    for(int i = 0; i < GuiConfig::inst().max_recent; i++)
    recentFilesMenu->addAction("",this,SLOT(sl_open_recent()));
    updateRecentSubmenu();
};

void DMainMenu::sl_open(){
    auto mw = main_window->ptr.lock();

    QFileDialog dialog(mw.get());

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

bool DMainMenu::event( QEvent *evt ){
    if(evt->type() == QEvent::User){
        auto addWfn = dynamic_cast<DMainMenuAddWfnEvent*>(evt);
        if(addWfn){
            //todo: p_wfn object can be destroyed from another thread
            //update recent files menu
            updateRecentSubmenu();

            //find tree_ctrl and add wfx to tree
            auto v_main_window = doc_first_nearest_father<DMainWindow>(get_idx());
            auto v_tree_ctrl = doc_first_nearest_child<DTreeCtrl>(v_main_window);
            auto p_tree_ctrl = get_weak_obj_ptr<DTreeCtrl>(this, v_tree_ctrl);
            p_tree_ctrl->ptr.lock()->add_wfx_item(QString(addWfn->p_wfn->path.c_str()), addWfn->p_wfn->get_idx());

            //add new scene to wfx and show it
            v_main_window = doc_first_nearest_father<DMainWindow>(get_idx());
            auto gl_idx = doc_first_nearest_child<DGlScreen>(v_main_window);
            new DSceneEntity(addWfn->p_wfn->get_idx(), gl_idx);
            auto gl = get_weak_obj_ptr<DGlScreen>(this, gl_idx);
            gl->ptr.lock()->switch_scene(addWfn->p_wfn->get_idx());
        }
    }
    return QMenuBar::event(evt);
}

void DMainMenu::openWfx(QString path){

    std::shared_ptr<WFXParserParams> params(new WFXParserParams);
    params->path = path;

    params->post_res_ok = [](const BaseTask* bt) -> void {
        auto parser = dynamic_cast<const WFXParserTask*>(bt);
        auto result = parser->get_result();

        auto v_main_window = doc_first_nearest_father<DMainWindow>(bt->get_idx());
        auto v_main_menu = doc_first_nearest_child<DMainMenu>(v_main_window);
        auto p_main_menu = get_weak_obj_ptr<DMainMenu>(bt, v_main_menu);

        GuiConfig::inst().add_recent(result->get_params()->path);

        QApplication::postEvent(p_main_menu->ptr.lock().get(), new DMainMenuAddWfnEvent(result->p_wfn));
    };

    params->post_res_err = [](const BaseTask* bt) -> void {
        auto parser = dynamic_cast<const WFXParserTask*>(bt);
        auto result = parser->get_result();

        //todo: msgBox("Error: " + QString(result->err_msg.c_str()));
        cerr << result->err_msg << endl;
    };

    auto v_main_window = doc_first_nearest_father<DMainWindow>(get_idx());
    auto v_thread_pool = doc_first_nearest_child<DThreadPool>(v_main_window);
    new WFXParserTask(params,v_thread_pool);
}


void DMainMenu::msgBox(QString msg){
    auto mw = main_window->ptr.lock();
    QMessageBox msgBox(mw.get());
    msgBox.setText(msg);
    msgBox.exec();
}