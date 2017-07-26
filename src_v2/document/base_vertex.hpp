//
// Created by morrigan on 27/06/17.
//

#ifndef DAIMAN_BASE_VERTEX_H
#define DAIMAN_BASE_VERTEX_H

#include <exception>
#include <thread>
#include "graph.h"
#include "doc_tree.hpp"

using namespace boost;

/*
 * BaseObj stores a unique index of the object in the DocTree. An index is assigned
 * automatically, before the object overloaded constructor.
 * However, to preserve physical pointers consistency, register of the
 * pointer in it's container (weak or shared) shell be done as a first
 * line of it's constructor. Use reg_shared/reg_weak for this purpose.
 */
//todo: rename to DocVertex, standardize variables names
struct BaseObj : public Polymorph{
public:

    virtual ~BaseObj(){
        autoremove();
        cout << this << " deleted" << endl;
    }

    node_desc_t get_idx(std::shared_ptr<DocTree> dt) const{
        graph_traits<Graph>::vertex_iterator vi, vi_end;
        tie(vi, vi_end) = vertices(dt->g);
        for(;vi != vi_end; vi++){
            if(dt->g[*vi].ref_ptr == (void*) this) return *vi;
        }
        return dt->g.null_vertex();
    }

    node_desc_t get_idx() const {
        auto dt = DocTree::inst();
        return get_idx(dt);
    }

    template<typename T>
    void reg(T* ptr, bool is_weak){
        ptr->is_weak = is_weak;
        auto dt = DocTree::inst();
        auto v = add_vertex(dt->g);
        dt->g[v].ref_ptr = this;

        auto hash = typeid(ptr).hash_code();
        dt->g[v].type_hash = hash;

        dt->g[v].ptr = std::shared_ptr<BaseObj>(ptr,[](BaseObj* p){
            if(!p->is_weak) {
                cout << "delete from shared " << p << endl;
                p->autoremove();
                delete p;
            }
        });

        dt->thash_name_map[hash] = typeid(ptr).name();
        std::cout << "registered " << (is_weak ? "(weak):   " : "(shared): ") << typeid(ptr).name() << ", node: " << v;
        std::cout << ", ref_ptr: " << dt->g[v].ref_ptr << std::endl;
    }

private:
    void autoremove(){
        is_weak = true;
        if(!autoremove_vertex) return;
        autoremove_vertex = false;
        cout << "AUTOREMOVE CALLED" << endl;
        auto dt = DocTree::inst();
        auto v = get_idx(dt);
        if(v == dt->g.null_vertex()) return;
        auto hash = dt->g[v].type_hash;
        clear_vertex(v, dt->g);
        remove_vertex(v, dt->g);
        cout << "removed vertex for: " << DocTree::inst()->thash_name_map[hash] << endl;
    }
};

struct ThreadSafeBaseObject : public BaseObj{
    ThreadSafeBaseObject() : BaseObj(), hold_counter(new char){}
    // copy of this object stored in ref_decorator
    // will increment the counter of shared_ptr,
    // and each time decorator destructs, the counter will decrement
    // automatically
    std::shared_ptr<char> hold_counter;

    //any calls to functions of the child objects
    //is hidden within ref_decorator functor and inclosed
    //by mtx.lock() and mtx.release(), this is
    //how thread safety works. It is not permitted
    //to access an object without decorator functor.
    mutex mtx;
};

/*
 * In the case when father wants to call it's child function, the access is done
 * via external functions. In the case of "shared" chils, the mutex has to be appliyed.
 * Also hold control is important.
 * This function traits substitute required implementation at compile tyme.
 */

template <class T>
inline typename std::enable_if<!std::is_base_of<ThreadSafeBaseObject, T>::value>::type
exec_trait(T* arg, std::function<void(T*)> f){
    f(arg);
};

template <class T>
inline typename std::enable_if<std::is_base_of<ThreadSafeBaseObject, T>::value>::type
exec_trait(T* arg, std::function<void(T*)> f){
    arg->mtx.lock();
    if(arg->hold_counter.use_count() > 1) throw runtime_error("can't write held object");
    f(arg);
    arg->mtx.unlock();
};

template <class T>
inline typename std::enable_if<!std::is_base_of<ThreadSafeBaseObject, T>::value>::type
exec_trait(T* arg, std::function<void(const T*)> f){
    f(arg);
};

template <class T>
inline typename std::enable_if<std::is_base_of<ThreadSafeBaseObject, T>::value>::type
exec_trait(T* arg, std::function<void(const T*)> f){
    arg->mtx.lock();
    f(arg);
    arg->mtx.unlock();
};



#endif //DAIMAN_BASE_VERTEX_H
