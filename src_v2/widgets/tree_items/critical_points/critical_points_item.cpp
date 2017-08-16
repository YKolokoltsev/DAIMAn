//
// Created by morrigan on 19/07/17.
//

#include <QApplication>

#include "critical_points_item.h"
#include "cps_group.h"
#include "mgp_parser.h"

DCriticalPointsItem::DCriticalPointsItem(node_desc_t wfx_item_idx){
    reg(this, true);
    setText(0, QString("Critical Points"));

    wfx_item = std::move(get_weak_obj_ptr<DWfxItem>(this,wfx_item_idx));
    wfx_item->ptr.lock()->insertChild(0,this);
}

std::unique_ptr<QMenu> DCriticalPointsItem::context_menu(QWidget* menu_parent){
    std::unique_ptr<QMenu> menu(new QMenu(menu_parent));
    menu->addAction(tr("Read from *.mgp"), this, SLOT(sl_open_mgp()) );
    return menu;
}

void DCriticalPointsItem::sl_open_mgp(){
    auto v_main_window = doc_first_nearest_father<DMainWindow>(get_idx());
    auto main_window = get_weak_obj_ptr<DMainWindow>(this, v_main_window);
    auto mw = main_window->ptr.lock();

    QStringList file_list = mw->open_file_dlg(tr("AIM molecular graph points (*.mgp)"));

    //open mgp document
    if(file_list.count() != 0)
        openMgp(file_list.at(0));
}

bool DCriticalPointsItem::event( QEvent *evt ){
    if(evt->type() == QEvent::User){
        auto add_cps = dynamic_cast<DCriticalPointsAddMgpEvent*>(evt);
        if(add_cps){
            auto lock_inst = DocTree::inst();

            //find indexes of new critical points
            tTypeIdxList cp_idxs;
            auto v_wfn = doc_first_nearest_father<WFNData>(get_idx());
            const auto time_id = add_cps->time_id;
            for_each_child<MgpCriticalPoints::MGPCriticalPoint>(v_wfn,
               [&time_id, &cp_idxs](MgpCriticalPoints::MGPCriticalPoint* pt){
                   if(pt->is_time_id(time_id))
                       cp_idxs.push_back(make_pair(pt->type, pt->get_idx()));
            });


            auto group = new DCriticalPointsGroup("MGP", get_idx());
            group->add_cps(cp_idxs);
            return true;
        }
    }
    return DCriticalPointsItem::event(evt);
}

void DCriticalPointsItem::openMgp(QString path){
    auto lock_inst = DocTree::inst();

    std::shared_ptr<MGPParserParams> params(new MGPParserParams);

    params->path.reset(new std::string(path.toStdString()));

    params->time_id = std::chrono::high_resolution_clock::now();

    params->post_res_err = [](const BaseTask* bt) -> void {
        auto parser = dynamic_cast<const MGPParserTask*>(bt);
        auto result = parser->get_result();

        //todo: msgBox("Error: " + QString(result->err_msg.c_str()));
        cerr << result->err_msg << endl;
    };

    params->post_res_ok = [](const BaseTask* bt) -> void {
        auto parser = dynamic_cast<const MGPParserTask*>(bt);
        auto result = parser->get_result();

        auto v_wfn = doc_first_nearest_father<WFNData>(bt->get_idx());
        auto v_cpts_item = doc_first_nearest_child<DCriticalPointsItem>(v_wfn);
        auto p_cpts_item = get_weak_obj_ptr<DCriticalPointsItem>(bt, v_cpts_item);

        QApplication::postEvent(p_cpts_item->ptr.lock().get(),
                                new DCriticalPointsAddMgpEvent(result->get_params()->time_id));
    };
    
    //todo: add params functions
    
    auto v_main_window = doc_first_nearest_father<DMainWindow>(get_idx());
    auto v_thread_pool = doc_first_nearest_child<DThreadPool>(v_main_window);
    auto v_wfx = doc_first_nearest_father<WFNData>(get_idx());
    new MGPParserTask(params, v_thread_pool, v_wfx);
}