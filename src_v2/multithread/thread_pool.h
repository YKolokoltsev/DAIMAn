//
// Created by morrigan on 20/07/17.
//

#ifndef DAIMAN_DTHREADPOOL_H
#define DAIMAN_DTHREADPOOL_H

#include <thread>
#include <memory>
#include <condition_variable>

#include "base_vertex.hpp"
#include "ref_decorator.hpp"
#include "main_window.h"

/*
 * This is a root node for all primary computational threads.
 * Thread start happens from the main program flow
 * but after it's finish, there has to be a generic mechanism that
 * will handle this event. The task_controller thread of DThreadPool
 * awakes when a stop signal comes from any of the threads, it searches
 * which task has been terminated and maybe awake any other process
 * that had to wait for CPU resources.
 */
class DMainWindow;
class DThreadPool : public ThreadSafeBaseObject{
public:
    DThreadPool(node_desc_t idx_main_window);
    void stop_notify(void* notify_uid);
    ~DThreadPool();

private:
    static void main_loop(DThreadPool* thread_pool);

private:
    ext_weak_ptr_t<DMainWindow> main_window;
    std::unique_ptr<thread> task_controller;
    bool stop{false};
    std::condition_variable cond;
    void* notify_uid{nullptr};
};


#endif //DAIMAN_DTHREADPOOL_H
