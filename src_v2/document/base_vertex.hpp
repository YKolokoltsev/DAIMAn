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
struct BaseObj : public Polymorph{
public:

    virtual ~BaseObj(){
        autoremove();
        cout << this << " deleted" << endl;
    }

    node_desc_t get_idx(){
        graph_traits<Graph>::vertex_iterator vi, vi_end;
        DocTree* dt = DocTree::inst();
        tie(vi, vi_end) = vertices(dt->g);
        for(;vi != vi_end; vi++){
            if(dt->g[*vi].ref_ptr == this) return *vi;
        }
        return dt->g.null_vertex();
    }

protected:
    template<typename T>
    void reg(T* ptr, bool is_weak){
        ptr->is_weak = is_weak;
        DocTree* dt = DocTree::inst();
        auto v = add_vertex(dt->g);
        dt->g[v].ref_ptr = this;

        auto hash = typeid(ptr).hash_code();
        dt->g[v].type_hash = hash;

        dt->g[v].ptr = std::shared_ptr<BaseObj>(ptr,[](BaseObj* p){
            if(!p->is_weak) {
                cout << "delete from shared" << endl;
                p->autoremove();
                delete p;
            }
        });

        DocTree::inst()->thash_name_map[hash] = typeid(ptr).name();
        std::cout << "registered: " << typeid(ptr).name() << ", node: " << v;
        std::cout << ", ref_ptr: " << dt->g[v].ref_ptr << std::endl;
    }

private:
    void autoremove(){
        is_weak = true;
        if(!autoremove_vertex) return;
        autoremove_vertex = false;
        auto v = get_idx();
        auto dt = DocTree::inst();
        if(v == dt->g.null_vertex()) return;
        auto hash = dt->g[v].type_hash;
        clear_vertex(v, dt->g);
        remove_vertex(v, dt->g);
        cout << "removed vertex for: " << DocTree::inst()->thash_name_map[hash] << endl;
    }
};



#endif //DAIMAN_BASE_VERTEX_H
