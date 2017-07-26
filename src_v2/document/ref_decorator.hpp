//
// Created by morrigan on 27/06/17.
//

#ifndef DAIMAN_REF_DECORATOR_H
#define DAIMAN_REF_DECORATOR_H

#include "doc_tree.hpp"
#include "base_vertex.hpp"

/*
 * A decorator that guarantees deletion of the edge in the Graph
 * when a smart pointer is deleted.
 */
//todo: investigate on ptr.lock().get() - this is a typical Qt pointer violence
template<typename T>
struct NodeRef {
    static_assert(std::is_base_of<BaseObj, T>::value, "T must be derived of BaseObj");

    NodeRef(std::shared_ptr<T> ptr, std::shared_ptr<BaseObj> owner): ptr{ptr}, owner{owner} {};

    ~NodeRef(){
        auto dt = DocTree::inst();

        if(ptr.use_count() == 0) return;
        if(owner.use_count() == 0) return;

        auto v1 = owner.lock()->get_idx(dt);
        auto v2 = ptr.lock()->get_idx(dt);
        if(v1 == dt->null_vertex || v2 == dt->null_vertex) return;

        remove_edge(v1,v2, dt->g);
    }

public:
    std::weak_ptr<T> ptr;

private:
    std::weak_ptr<BaseObj> owner;
};

template <typename T>
using ext_weak_ptr_t = std::unique_ptr<NodeRef<T>>;

/*
 * Weak pointers can be used both for shared and weak objects. In the case of
 * shared object this pointer permits to delete an object that took this
 * pointer before the object that it points to.
 */
template<typename T>
ext_weak_ptr_t<T> get_weak_obj_ptr(const BaseObj* who, node_desc_t v) {
    static_assert(std::is_base_of<BaseObj, T>::value, "T must be derived of BaseObj");
    static_assert(!std::is_base_of<ThreadSafeBaseObject, T>::value, "weak link to shared object violates thread-safety");

    auto dt = DocTree::inst();

    if(who == nullptr) throw runtime_error("can't add edge to null object");
    auto u = who->get_idx(dt);
    if(u == dt->null_vertex) throw runtime_error("requesting object not registered");

    if(v == dt->null_vertex) throw runtime_error("null_vertex has no object");
    if(dt->g[v].ptr.use_count() == 0) throw runtime_error("bug: requested object node not initialized, graph inconsistent");

    auto edge = dt->add_dependency(u,v);
    dt->g[edge].dt = DependencyType::Weak;

    auto ptr = dynamic_pointer_cast<T>(dt->g[v].ptr);
    auto who_ptr = dynamic_pointer_cast<BaseObj>(dt->g[u].ptr);
    return ext_weak_ptr_t<T>(new NodeRef<T>(ptr, who_ptr));
}

template<typename T>
struct ThreadSafeNodeRef{
public:
    static_assert(std::is_base_of<ThreadSafeBaseObject, T>::value, "T must be derived of ThreadSafeBaseObject");
    ThreadSafeNodeRef(std::shared_ptr<T> ptr, std::shared_ptr<BaseObj> owner, bool hold = false):
            ptr{ptr}, owner{owner}, hold_counter{nullptr} {
        if(hold) hold_counter = ptr->hold_counter;
    };

    //read/write access, can't be applied if object is frozen by anyone
    void exec_rw(std::function<void(T*)> f){
        ptr->mtx.lock();
        if(ptr->hold_counter.use_count() > 1) throw runtime_error("can't write held object");
        f(ptr.get());
        ptr->mtx.unlock();
    }

    //mutexed read-only access, can be used if hold = false in this ThreadSafeNodeRef object
    //but any other thread can hold/unhold referenced node
    void exec_r(std::function<void(const T*)> f) {
        ptr->mtx.lock();
        f(ptr.get());
        ptr->mtx.unlock();
    }

    //todo: WRITE A FUNCTION
    //todo: if hold = true in this object, it is safe to create weak_ptr<const T*>
    //todo: and return it to computational algorithm (for eficcient access), this weak
    //todo: pointer shell be released automatically if THIS ThreadSafeNodeRef object dies

    node_desc_t get_idx(){
        return ptr->get_idx();
    }

    ~ThreadSafeNodeRef(){
        auto dt = DocTree::inst();

        if(owner.use_count() == 0) return;
        auto v1 = owner.lock()->get_idx(dt);
        auto v2 = ptr->get_idx(dt);
        if(v1 == dt->null_vertex || v2 == dt->null_vertex)
            return;

        remove_edge(v1,v2, dt->g);
        ptr->mtx.lock();
        hold_counter.reset();
        ptr->mtx.unlock();
    };

private:
    std::shared_ptr<T> ptr;
    std::weak_ptr<BaseObj> owner;
    std::shared_ptr<char> hold_counter;
};

template <typename T>
using ext_shared_ptr_t = std::unique_ptr<ThreadSafeNodeRef<T>>;

   /*
    * Returns a shared pointer onto the object with the same counter as stored in the
    * Graph node. The object will live until the last shared pointer is deleted;
    * Another benefit is a thread safety.
    */
template<typename T>
ext_shared_ptr_t<T> get_shared_obj_ptr(const BaseObj* who, node_desc_t v, DependencyType dep = DependencyType::Shared) {
    static_assert(std::is_base_of<ThreadSafeBaseObject, T>::value, "shell be derived from ThreadSafeBaseObject");

    auto dt = DocTree::inst();
    if(dep == DependencyType::Weak) throw runtime_error("DependencyType::Weak not permitted");

    if(who == nullptr) throw runtime_error("requesting object is null");
    auto u = who->get_idx(dt);
    if(u == dt->null_vertex) throw runtime_error("requesting object not registered");

    if(v == dt->null_vertex) throw runtime_error("null_vertex has no object");
    if(dt->g[v].ptr.use_count() == 0) throw runtime_error("bug: requested vertex contains nullptr, graph consistency violation");

    auto edge = dt->add_dependency(u,v);
    dt->g[edge].dt = dep;

    auto ptr = dynamic_pointer_cast<T>(dt->g[v].ptr);
    auto who_ptr = dynamic_pointer_cast<BaseObj>(dt->g[u].ptr);
    return ext_shared_ptr_t<T>(new ThreadSafeNodeRef<T>(ptr, who_ptr));
}

#endif //DAIMAN_REF_DECORATOR_H
