//
// Created by morrigan on 27/06/17.
//

#ifndef DAIMAN_REF_DECORATOR_H
#define DAIMAN_REF_DECORATOR_H

#include "doc_tree.hpp"
#include "base_vertex.hpp"

/*
 * A decorator that guarantees deletion of the edge in the Graph
 * when a smart pointer is deleted. This way Graph automatically monitors all
 * data exchange between the objects.
 */

template<typename T_Ptr>
struct NodeRef {
    //todo: T_Ptr can be only std::shared_ptr or std::weak_ptr
    typedef  typename T_Ptr::element_type T_Obj;
    static_assert(std::is_base_of<BaseObj, T_Obj>::value, "T must be derived of BaseObj");

    NodeRef(T_Ptr ptr, BaseObj* owner): ptr{ptr}, owner{owner} {};
    T_Ptr ptr;

    ~NodeRef(){
        auto dt = DocTree::inst();
        auto v1 = dt->null_vertex;
        auto v2 = dt->null_vertex;

        if(ptr.use_count() == 0) return;
        try {
            v1 = owner->get_idx();
            v2 = ref_idx(ptr);
        }catch(...){
            //if at least one node has no index, there should not be any edges
            //because we clear each node before it's remove
            cout << "edge was deleted explicitly" << endl;
            return;
        }
        try {
            //todo: multiply edges is a code idiotizm. It would mean that
            //same node got at least two pointers to the same object
            //it should not happen, and it is a todo, to detect that shit
            //and produce an exception
            remove_edge(v1,v2, DocTree::inst()->g);
            cout << "edge removed explicitly" << endl;
        }catch(...){
            cerr << "edge exception shell not happen" << endl;
        }
    }

private:
    node_desc_t ref_idx(std::shared_ptr<T_Obj>& ptr){
        dynamic_pointer_cast<BaseObj>(ptr)->get_idx();
    }

    node_desc_t ref_idx(std::weak_ptr<T_Obj>& ptr){
        dynamic_pointer_cast<BaseObj>(ptr.lock())->get_idx();
    }
private:
    BaseObj* owner;
};

template <typename T>
using ext_shared_ptr_t = std::unique_ptr<NodeRef<std::shared_ptr<T>>>;
template <typename T>
using ext_weak_ptr_t = std::unique_ptr<NodeRef<std::weak_ptr<T>>>;

/*
    * Returns a shared pointer onto the object with the same counter as stored in the
    * Graph node. The object will live until the last shared pointer is deleted;
    * If this is a weak node, than we shell emphasize this fact by creating only weak pointers
    * to it, so a shared pointer to a weak node is artificially prohibited.
    */
template<typename T>
ext_shared_ptr_t<T> get_obj_ptr(BaseObj* who, node_desc_t v) {
    auto dt = DocTree::inst();
    if(v == dt->null_vertex) throw runtime_error("null_vertex has no object");
    if(who == nullptr) throw runtime_error("can't add edge to null object");
    if(dt->g[v].ptr.use_count() == 0) throw runtime_error("requested object node not initialized");
    if(dt->g[v].st == StorageType::Weak) throw runtime_error("shared links to weak objects not permitted");

    auto edge = dt->add_dependency(who->get_idx(),v);
    dt->g[edge].dt = DependencyType::Shared;
    auto ptr = dynamic_pointer_cast<T>(dt->g[v].ptr);
    return ext_shared_ptr_t<T>(new NodeRef<std::shared_ptr<T>>(ptr, who));
}

/*
 * Weak pointers can be used both for shared and weak objects. In the case of
 * shared object this pointer permits to delete an object that took this
 * pointer before the object that it points to.
 */
template<typename T>
ext_weak_ptr_t<T> get_weak_obj_ptr(BaseObj* who, node_desc_t v) {
    auto dt = DocTree::inst();
    if(v == dt->null_vertex) throw runtime_error("null_vertex has no object");
    if(who == nullptr) throw runtime_error("can't add edge to null object");
    if(dt->g[v].ptr.use_count() == 0) throw runtime_error("requested object node not initialized");

    auto edge = dt->add_dependency(who->get_idx(),v);
    dt->g[edge].dt = DependencyType::Weak;
    auto ptr = dynamic_pointer_cast<T>(dt->g[v].ptr);
    return ext_weak_ptr_t<T>(new NodeRef<std::weak_ptr<T>>(ptr, who));

}

#endif //DAIMAN_REF_DECORATOR_H