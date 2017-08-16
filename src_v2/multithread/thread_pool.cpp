//
// Created by morrigan on 20/07/17.
//

#include "thread_pool.h"
#include "algorithm.hpp"
#include "base_task.h"

DThreadPool::DThreadPool(node_desc_t idx_main_window){
    reg(this, false);
    main_window = std::move(get_weak_obj_ptr<DMainWindow>(this, idx_main_window));
    task_controller.reset(new thread(main_loop, this));
}

void DThreadPool::stop_notify(void* notify_uid){
    this->notify_uid = notify_uid;
    cond.notify_one();
}

DThreadPool::~DThreadPool(){
    mtx.lock();
    stop = true;
    stop_notify((void*) this);
    mtx.unlock();
    task_controller->join();
}

void DThreadPool::main_loop(DThreadPool* tp){
    while(1){
        std::unique_lock<mutex> lck(tp->mtx);
        tp->cond.wait(lck, [tp]{return tp->notify_uid != nullptr;});
        if(tp->stop){
            //task_controller can be stopped ONLY from destructor of DThreadPool
            break;
        }else{
            //This lock DocTree && locks ThreadPool at the same time
            for_each_child<BaseTask>(tp->get_idx(),[&](BaseTask* bt){
                //DocTree unlocked
                if(tp->notify_uid == bt){
                    tp->notify_uid = nullptr;
                    if(bt->get_stop()) {
                        bt->join();
                        bt->add_result();
                        remove_recursive(bt->get_idx());
                    }
                    /*any other generic "notification"*/
                }
                //DocTree locked
            });
        }
    }
};