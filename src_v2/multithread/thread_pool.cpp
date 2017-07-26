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

void DThreadPool::stop_notify(){
    notify_count++;
    cond.notify_one();
}

DThreadPool::~DThreadPool(){
    mtx.lock();
    stop = true;
    stop_notify();
    mtx.unlock();
    task_controller->join();
    //cout << "TODO --- DThreadPool release MainWindow" << endl;
    //main_window.release();
    //cout << "DThreadPool MainWindow released" << endl;
}

void DThreadPool::main_loop(DThreadPool* tp){
    while(1){
        std::unique_lock<mutex> lck(tp->mtx);
        tp->cond.wait(lck, [tp]{return tp->notify_count > 0;});
        if(tp->stop){
            //task_controller can be stopped ONLY from destructor of DThreadPool
            break;
        }else{
            for_each_child<BaseTask>(tp->get_idx(),[&](BaseTask* bt){
                std::cout << "check is stopped, and release object" << std::endl;
                if(bt->get_stop()){
                    tp->notify_count--;
                    bt->join();
                    bt->add_result();
                    remove_recursive(bt->get_idx());
                }
            });
        }
    }
};