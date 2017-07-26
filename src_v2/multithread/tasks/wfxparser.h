#ifndef WFXPARSER_H
#define WFXPARSER_H

#include <QString>
#include <QFile>
#include <QStringList>

#include "wfndata.h"
#include "base_task.h"
#include "main_window.h"

//todo: https://habrahabr.ru/post/168951/
//todo: not use Qt here, use boost, the project shell compile without any Qt
//todo: requirements (plain console app)

struct WFXParserParams : public TaskParams{
    QString path;
};

struct WFXParserResult : TaskResult{
    WFXParserResult(std::shared_ptr<WFXParserParams> params) :
            TaskResult(params), p_wfn{new WFNData()}{}

    ~WFXParserResult() {
        if(p_wfn != nullptr && res_code != RES_OK){
            cout << "RRRRRRRRRRRRRRRRRRRRRR" << endl;
            delete p_wfn;
        }
    }

    std::shared_ptr<const WFXParserParams> get_params() const {
        return dynamic_pointer_cast<const WFXParserParams>(params);
    }

    void discard(std::string err_msg){
        if(p_wfn != nullptr){delete p_wfn;}
        p_wfn = nullptr;
        TaskResult::discard(err_msg);
    }

    virtual void add_to_document(const BaseTask* task){
        if(res_code != RES_ERR) {
            if(p_wfn->calc_helpers()) {
                p_wfn->reg(p_wfn, false);
            }else{
                discard("failed to wfx->calc_helpers()");
            }
        }
        TaskResult::add_to_document(task);
    };

    WFNData* p_wfn;
};


class WFXParserTask : public BaseTask {
public:
    WFXParserTask(std::shared_ptr<WFXParserParams> params, node_desc_t idx_thread_pool);
    std::shared_ptr<const WFXParserResult> get_result() const {
        return dynamic_pointer_cast<const WFXParserResult>(result);
    }

protected:
    virtual void main_loop();

private:
    static bool pr_title(QString text, WFNData* wfn);
    static bool pr_keywords(QString text, WFNData* wfn);
    static bool pr_nat(QString text, WFNData* wfn);
    static bool pr_nprim(QString text, WFNData* wfn);
    static bool pr_norb(QString text, WFNData* wfn);
    static bool pr_npert(QString text, WFNData* wfn);
    static bool pr_atnames(QString text, WFNData* wfn);
    static bool pr_atnumbers(QString text, WFNData* wfn);
    static bool pr_atcharges(QString text, WFNData* wfn);
    static bool pr_atcoords(QString text, WFNData* wfn);
    static bool pr_netcharge(QString text, WFNData* wfn);
    static bool pr_nelectrons(QString text, WFNData* wfn);
    static bool pr_nalpha(QString text, WFNData* wfn);
    static bool pr_nbeta(QString text, WFNData* wfn);
    static bool pr_multi(QString text, WFNData* wfn);
    static bool pr_model(QString text, WFNData* wfn);
    static bool pr_icent(QString text, WFNData* wfn);
    static bool pr_itype(QString text, WFNData* wfn);
    static bool pr_alpha(QString text, WFNData* wfn);
    static bool pr_occup(QString text, WFNData* wfn);
    static bool pr_orben(QString text, WFNData* wfn);
    static bool pr_orspin(QString text, WFNData* wfn);
    static bool pr_etotal(QString text, WFNData* wfn);
    static bool pr_rvirial(QString text, WFNData* wfn);

    bool allocCoef(WFNData* wfn);
    bool parseMOC(WFNData* wfn);
    bool readnMO(QString text, int* nMO);
    bool readMOC(QString text, int nMO, WFNData* wfn);

    std::shared_ptr<QFile> file;
};

#endif // WFXPARSER_H
