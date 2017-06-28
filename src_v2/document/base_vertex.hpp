//
// Created by morrigan on 27/06/17.
//

#ifndef DAIMAN_BASE_VERTEX_H
#define DAIMAN_BASE_VERTEX_H

#include <exception>

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
//todo: rename to DocVertex
class BaseObj : public Polymorph{
public:
    BaseObj(): ref_ptr{this} {
        v = add_vertex(DocTree::inst()->g);
    }

    template<typename T> //template because of typeid
    static void reg_shared(T* ptr){
        auto dt = DocTree::inst();
        dt->g[ptr->v].ptr.reset(ptr);
        dt->g[ptr->v].st = StorageType::Shared;
        reg_maps(dt, ptr);
    }

    template<typename T> //template because of typeid
    static void reg_weak(T* ptr){
        auto dt = DocTree::inst();
        dt->g[ptr->v].ptr = std::shared_ptr<BaseObj>(ptr,[](BaseObj* p){});
        dt->g[ptr->v].st = StorageType::Weak;
        reg_maps(dt, ptr);
    }

    void unreg(){
        try {
            v = get_idx(); //throws
            auto dt = DocTree::inst();
            auto hash = dt->g[v].type_hash;
            clear_vertex(v, dt->g);
            remove_vertex(v, dt->g);
            std::cout << "unregistered: " << dt->thash_name_map[hash] << std::endl;
        }catch(...){
            std::cerr << "already not registered: " << this << std::endl;
        }

    }

    node_desc_t get_idx(){
        /*
         * If in the case of node remove the indexes in 'g'
         * can change. We have to check first, if "v" - node descriptor
         * stored at the node creation points onto the same node.
         * This is the fast solution without deep digging into BGL.
         * People say that listS solves unstability of indexes...
         *
         * here a static cast shell be used to avoid any smart pointers
         * creation that can produce destructor loop
         * => todo: is welcome
         * => todo: check v exists in g ?!?
         */

        auto dt = DocTree::inst();
        bool v_is_valid = true;

        auto ptr = static_cast<BaseObj*>(dt->g[v].ptr.get());
        if(ptr == nullptr){
            v_is_valid = false;
        }else{
            if(ptr->ref_ptr != this) v_is_valid = false;
        }

        if(v_is_valid) return v;

        //find v:
        graph_traits<Graph>::vertex_iterator vi, vi_end;
        tie(vi, vi_end) = vertices(dt->g);
        for(vi; vi != vi_end; vi++) {
            if(dt->g[*vi].ptr == nullptr) continue;
            if (static_cast<BaseObj*>(dt->g[*vi].ptr.get())->ref_ptr == this) {
                v = *vi;
                return v;
            }
        }

        //or return a null_idx?
        throw std::runtime_error("no index");
    }

    virtual ~BaseObj(){
        auto dt = DocTree::inst();
        try{
            get_idx();
            clear_vertex(v,dt->g);
            /*
             * If node is Shared, than DocTree is a container,
             * in this case a destructor can be called only after
             * an explicit remove_vertex call. In this case we can't call
             * remove_vertex inside destructor. If node is Weak, than
             * get_idx() will certainly return an exception and of course
             */
            if(dt->g[v].st == StorageType::Weak)
                remove_vertex(v,dt->g);
            std::cout << "delete(node autoremove): " << dt->thash_name_map[dt->g[v].type_hash] << std::endl;
        }catch(...){
            std::cout << "delete(node empty): " << this << std::endl;
        }
    };

private:
    template<typename T>
    static inline void reg_maps(DocTree* dt, T* ptr){
        auto hash = typeid(ptr).hash_code();
        dt->g[ptr->v].type_hash = hash;
        dt->thash_name_map[hash] = typeid(ptr).name();
        std::cout << "registered: " << typeid(ptr).name() << ", node: " << ptr->v;
        std::cout << "ref_ptr: " << dynamic_cast<BaseObj*>(ptr)->ref_ptr << std::endl;
    }

    node_desc_t v{DocTree::inst()->g.null_vertex()};
    const void* ref_ptr;
};



#endif //DAIMAN_BASE_VERTEX_H
