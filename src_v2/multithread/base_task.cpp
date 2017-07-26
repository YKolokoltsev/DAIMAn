//
// Created by morrigan on 23/07/17.
//

#include "base_task.h"
#include "algorithm.hpp"

BaseTask::BaseTask(node_desc_t idx_thread_pool) {
    reg(this, false);
    thread_pool = std::move(get_shared_obj_ptr<DThreadPool>(this, idx_thread_pool));
    th.reset(new std::thread(main, this));
}

BaseTask::~BaseTask(){
    stop = true;
    if(th->joinable()) th->join();
};

void BaseTask::main(BaseTask* p_task){
    if(p_task->result == nullptr) throw runtime_error("need base_result object initialized");
    p_task->main_loop();
    p_task->stop = true;
    p_task->thread_pool->exec_rw([&](DThreadPool* tp){tp->stop_notify();});
}

void BaseTask::add_result(){
    if(!result) throw runtime_error("task base_result not initialized");
    result->add_to_document(this);
}

void BaseTask::update_progress(double perc){
    mtx.lock();
    progress = perc;
    if(result->params->report_progress)
        result->params->report_progress(this);
    mtx.unlock();
}