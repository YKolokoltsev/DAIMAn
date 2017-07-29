//
// Created by morrigan on 23/07/17.
//

#ifndef DAIMAN_BASETASK_H
#define DAIMAN_BASETASK_H

#include <thread>
#include <memory>
#include <atomic>

#include "base_vertex.hpp"
#include "ref_decorator.hpp"
#include "thread_pool.h"


/*
 * do not use any bindings to GUI in child classes,
 * this is plain computational code that shell be
 * accessible from console app without any GUI libraries
 */

class BaseTask;
struct TaskParams{
    virtual ~TaskParams() {};
    std::function<void(const BaseTask*)> report_progress = 0;
    std::function<void(const BaseTask*)> post_res_ok = 0;
    std::function<void(const BaseTask*)> post_res_err = 0;
    std::function<void(const BaseTask*)> post_res_incomplete = 0;
};

struct TaskResult{
    TaskResult(std::shared_ptr<TaskParams> params) : params{params} {}
    enum ResultCode{RES_OK, RES_ERR, RES_INCOMPLETE};

    virtual void add_to_document(const BaseTask* task){

        if(res_code == RES_OK && params->post_res_ok){
            params->post_res_ok(task);
        }else if(res_code == RES_ERR && params->post_res_err){
            params->post_res_err(task);
        }else if(res_code == RES_INCOMPLETE && params->post_res_incomplete){
            params->post_res_incomplete(task);
        }
    };

    virtual void discard(std::string err_msg){
        this->err_msg = err_msg;
        res_code = RES_ERR;
    };

    std::string err_msg;
    ResultCode res_code{RES_INCOMPLETE};
    std::shared_ptr<TaskParams> params;
};

class DThreadPool;
class BaseTask : public ThreadSafeBaseObject{
    friend class TaskResult;
public:
    BaseTask(node_desc_t idx_thread_pool);
    ~BaseTask();
    void join(){if(th->joinable()) th->join();};
    void add_result();
    bool get_stop() const {return stop;}
    bool get_progress() const {return progress;}

protected:
    virtual void main_loop() = 0;
    void update_progress(double perc); //update reasonably often in "main_loop"

private:
    static void main(BaseTask* p_task);

protected:
    std::shared_ptr<TaskResult> result{nullptr}; //derive TaskResult and store copy in child

private:
    ext_shared_ptr_t<DThreadPool> thread_pool;
    std::unique_ptr<thread> th{nullptr};

    double progress{0.0}; //from 0 to 1
    atomic<bool> stop{false}; //thread shell quit "main_loop" if stop==true
};


#endif //DAIMAN_BASETASK_H
