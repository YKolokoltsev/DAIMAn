//
// Created by morrigan on 16/06/17.
//

#ifndef DAIMAN_DOC_TREE_HPP_H
#define DAIMAN_DOC_TREE_HPP_H

#include <iostream>
#include <memory>
#include <map>
#include <set>
#include <list>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>

using namespace std;
using namespace boost;

/*
 * The most important nodes in this tree are mathematical objects. For example:
 * point, line, surface, volume, etc. These objects understood as "analytical".
 * For example, if a line is straight, than we can deduce any point
 * that lies on it analytically given a pair of points used to create this line
 * object.
 *
 * Now, suppose that we already have two points as separate objects and a line
 * object created by passing these points into the constructor of the line class
 * because of some physical causality relation between them. Afterwards, we would
 * like to recalculate points to polish them positions. If we store points as copies
 * inside a line object during it's creation, than we obtain an error - the line
 * will not pass through given points with new positions. Therefore, there is a
 * strong necessity to store a pointers onto the point's objects within a line class.
 * In this case, each time we need a point's coordinates within line class algorithm,
 * we will automatically use the most recent data.
 *
 * A situation is more interesting if the line is not straight and is calculated using
 * any other additional data. In a generic case, changes in the dependencies of the
 * objects shell be monitored to avoid miss interpretation of the results.
 *
 * Such monitoring requirement is the main reason why we decided to use graph data
 * structure to store dynamic objects. In this graph, which is a tree, we store
 * causality dependencies (graph edges) between the objects (graph nodes).
 */

class DocTree{
private:
    ~DocTree(){clear();}

    //A polymorphic type is needed to define Graph type with dummy pointers
    struct Polymorph{
        virtual ~Polymorph(){};
    };

    /*
     * Basically DocTree is a singleton container, which automatically
     * monitors dependencies between objects. However, there are some objects,
     * like Qt widgets, that are managed by their own containers.
     * For this case, to monitor dependency tree, such objects are stored here as a weak pointers.
     * Type of the storage is monitored with the next structure.
     */
    enum class StorageType{Shared, Weak};

    /*
     * To avoid destructor deadlocks,  basically for Qt objects, it is also necessary to use
     * weak dependencies, that do not manage objects lifetime; This is the edge property.
     */
    enum class DependencyType{Shared, Weak};

public:
    typedef std::shared_ptr<Polymorph> node_ptr_t;

    typedef struct{
        node_ptr_t ptr;
        StorageType st;
        size_t type_hash;
    } vertex_info;

    typedef struct{
        DependencyType dt;
    } edge_info;

    typedef adjacency_list<vecS, vecS, bidirectionalS, vertex_info, edge_info> Graph;
    typedef typename graph_traits<Graph>::edge_descriptor edge_desc_t;
    typedef typename graph_traits<Graph>::vertex_descriptor node_desc_t;

    /*
     * BaseObj stores a unique index of the object in the DocTree. An index is assigned
     * automatically, before the object overloaded constructor.
     * However, to preserve physical pointers consistency, register of the
     * pointer in it's container (weak or shared) shell be done as a first
     * line of it's constructor. Use reg_shared/reg_weak for this purpose.
     */
    class BaseObj : public Polymorph{
    public:
        friend class DocTree;
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
            throw runtime_error("no index");
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
                cout << "delete(node autoremove): " << dt->thash_name_map[dt->g[v].type_hash] << endl;
            }catch(...){
                cout << "delete(node empty): " << this << endl;
            }
        };

    private:
        template<typename T>
        static inline void reg_maps(DocTree* dt, T* ptr){
            auto hash = typeid(ptr).hash_code();
            dt->g[ptr->v].type_hash = hash;
            dt->thash_name_map[hash] = typeid(ptr).name();
            cout << "registered: " << typeid(ptr).name() << ", node: " << ptr->v;
            cout << "ref_ptr: " << dynamic_cast<BaseObj*>(ptr)->ref_ptr << endl;
        }

        node_desc_t v{DocTree::inst()->g.null_vertex()};
        const void* ref_ptr;
    };

    /*
     * A decorator that guarantees deletion of the edge in the Graph
     * when a smart pointer is deleted. This way Graph automatically monitors all
     * data exchange between the objects.
     */
    template<typename T = BaseObj>
    struct NodeRef {
        static_assert(std::is_base_of<BaseObj, T>::value, "T must be derived of BaseObj");
        friend class DocTree;
        NodeRef(std::shared_ptr<T> ptr, BaseObj* owner): ptr{ptr}, owner{owner} {};
        std::shared_ptr<T> ptr;
        ~NodeRef(){
            auto dt = DocTree::inst();
            auto v1 = dt->null_vertex;
            auto v2 = dt->null_vertex;

            if(ptr == nullptr) return;
            try {
                v1 = owner->get_idx();
                v2 = dynamic_pointer_cast<BaseObj>(ptr)->get_idx();
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
                cout << "edge exception shell not happen" << endl;
            }
        }
    private:
        BaseObj* owner;
    };
    friend struct NodeRef<BaseObj>;
    template <typename T>
    using ext_node_ptr_t = std::unique_ptr<NodeRef<T>>;

    /*
     * Framework interface is represented with global access functions.
     * These wrappers just hide DocTree::inst() calls, that is private
     * to standardize an interface.
     * //todo: work around reindexing problem...
     */
    template<typename T>
    friend ext_node_ptr_t<T> doc_get_weak_obj_ptr(BaseObj*, node_desc_t);
    template<typename T>
    friend ext_node_ptr_t<T> doc_get_obj_ptr(BaseObj*, node_desc_t);
    friend void doc_clear();
    friend size_t doc_node_count();
    friend size_t doc_edge_count();

    static DocTree* inst();

private:

    /*
     * Singleton pattern - private constructor and an external function to
     * get access to the single class object
     */
    DocTree() : null_vertex{g.null_vertex()} {};

    /*
     * Remove all nodes couse a wave of destructors
     */
    void clear();

    /*
     * This function checks if both node are properly registered
     * and maintains a directed graph without cycles. If there
     * happens a cyclic dependency - there is something very wrong
     * and shell be redesigned.
     */
    edge_desc_t add_dependency(node_desc_t from, node_desc_t to);

    /*
     * Just print when dependency is removed. This function is called
     * by decorator in it's destructor.
     */
    void remove_dependency(edge_desc_t& e);

    /*
     * At a given node, find nearest node with exact type T
     * following outgoing edges only;
     */
    template <typename T>
    node_desc_t bfs_type_from(size_t){

    }

   /*
    * Returns a shared pointer onto the object with the same counter as stored in the
    * Graph node. The object will live until the last shared pointer is deleted;
    * If this is a weak node, than we shell emphasize this fact by creating only weak pointers
    * to it, so a shared pointer to a weak node is artificially prohibited.
    */
    template<typename T>
    ext_node_ptr_t<T> get_obj_ptr(BaseObj* who, node_desc_t v) {

        if(who == nullptr) throw runtime_error("can't add edge to null object");
        if(g[v].ptr.use_count() == 0) throw runtime_error("requested object node not initialized");
        if(g[v].st == StorageType::Weak) throw runtime_error("shared links to weak objects not permitted");

        auto edge = add_dependency(who->v,v);
        g[edge].dt = DependencyType::Shared;
        return ext_node_ptr_t<T>(new NodeRef<T>(dynamic_pointer_cast<T>(g[v].ptr), who));
    }

    /*
     * Weak pointers can be used both for shared and weak objects. In the case of
     * shared object this pointer permits to delete an object that took this
     * pointer before the object that it points to.
     */
    template<typename T>
    ext_node_ptr_t<T> get_weak_obj_ptr(BaseObj* who, node_desc_t v) {

        if(who == nullptr) throw runtime_error("can't add edge to null object");
        if(g[v].ptr.use_count() == 0) throw runtime_error("requested object node not initialized");

        auto edge = add_dependency(who->v,v);
        g[edge].dt = DependencyType::Weak;
        auto ptr = dynamic_pointer_cast<T>(g[v].ptr);
        return ext_node_ptr_t<T>(new NodeRef<T>(std::shared_ptr<T>(ptr.get(),[](T* p){}), who) );
    }

public:
    const node_desc_t null_vertex;

private:
    Graph g;

    static DocTree* instance;

    //maps type hash to it's name, for debug purposes
    std::map<size_t, std::string> thash_name_map;
};

//**********************************template interface functions

template<typename T>
DocTree::ext_node_ptr_t<T> doc_get_weak_obj_ptr(DocTree::BaseObj* who, DocTree::node_desc_t v){
    return DocTree::inst()->get_weak_obj_ptr<T>(who,v);
}

template<typename T>
DocTree::ext_node_ptr_t<T> doc_get_obj_ptr(DocTree::BaseObj* who, DocTree::node_desc_t v){
    return DocTree::inst()->get_obj_ptr<T>(who,v);
}

//********************************regular functions

void doc_clear();

size_t doc_node_count();

size_t doc_edge_count();

#endif //DAIMAN_DOC_TREE_HPP_H
