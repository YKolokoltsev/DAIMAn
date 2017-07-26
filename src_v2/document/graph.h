//
// Created by morrigan on 27/06/17.
//

#ifndef DAIMAN_GRAPH_H_H
#define DAIMAN_GRAPH_H_H

#include <memory>
#include <iostream>

#include <boost/graph/adjacency_list.hpp>

using namespace boost;

//A polymorphic type is needed to define Graph type with pointers able to be downcasted
struct Polymorph{
    virtual ~Polymorph(){};
    //be sure to disable autoremove_vertex each time
    //when vertex remove is called explicitly
    bool autoremove_vertex = true;

    //All objects are stored within shared_ptr pointer containers.
    //If an object is "weak", it means that it shell be deleted
    //from outside of a graph or there will happen a memory leak.
    //This is the case for those libraries where nested objects are
    //deleted automatically, so these are "weak" objects,
    //this way we prevent a double delete calls.
    bool is_weak;

    //todo: hide this variables from outside world
};

/*
 * An interior bundled property of each vertex include a container, whose lifetime
 * coincide with a lifetime of Graph vertex, a storage type, and an exact hash
 * of the data type of the object addressed by the vertex. A data storage, therefore,
 * is the "ptr" variable.
 */
struct vertex_info{
    std::shared_ptr<Polymorph> ptr;
    size_t type_hash;
    void* ref_ptr;

    vertex_info() : type_hash{0}, ref_ptr{nullptr} {}; //regular constructor
    /*
     * At least when using vecS in adjacency_list, during node removal
     * process, some nodes get shifted inside of vector. Without an explicit
     * copy constructor this causes an implicit deletion of those objects that
     * are stored in the graph (shared_ptr destructor is called).
     */
    vertex_info(const vertex_info& vi){ //copy constructor
        ptr = vi.ptr;
        type_hash = vi.type_hash;
        ref_ptr = vi.ref_ptr;
    }

};

/*
 * To avoid destructor deadlocks, or sometimes to control a destruction order it is useful
 * to create weak dependencies, e.g. a weak_ptr pointers onto existing objects
 * that are stored inside or outside of DocTree.
 * For example, we can have a "weak" object in the graph. Also we can register
 * a weak dependency, meaning that some other object holds a pointer onto this one.
 * If an object is deleted from outside, a weak pointer will know this automatically,
 * and we can check it's accessibility at any time. (todo: test this use case)
 * In a case of computational threads we register a ThreadShared link to an object.
 * In this case given object can't be changed form any of the threads and can't be deleted.
 * "ThreadShared" dependency can't be created for "weak" objects. This is because
 * weak objects can be accessed via plain pointers from outside world of libraries
 * and violate a "hold-still" requirement.
 * //todo: check all .get() calls to the objects held in tree, these are now potentially
 * //todo: dangerous because can in principle return a plain pointer stored
 * //todo: outside a tree. Say ".get()" is permitted and is really necesarry only for
 * //todo: "weak" objects
 */
enum class DependencyType{Shared, Weak, HoldShared};

/*
 * Here we monitor a dependency. If weak_ptr is pointing onto shared_ptr - dependency
 * is DependencyType::Weak. In the opposite case, it is DependencyType::Shared.
 * For example, shared dependency permit to detach "shared" object from the graph
 * no the fly.
 */
struct edge_info{
    DependencyType dt;
};

/*
 * We use a bidirectional graph to have a chance to follow the graph
 * edges in both directions.
 *
 * This graph has been tested for vecS. It has a vertex descriptor invalidate property
 * and there is a lot of fucking with it. However, no simple solution has been
 * found even with listS.
 */
typedef adjacency_list<vecS, vecS, bidirectionalS, vertex_info, edge_info> Graph;
typedef typename graph_traits<Graph>::edge_descriptor edge_desc_t;
typedef typename graph_traits<Graph>::vertex_descriptor node_desc_t;

#endif //DAIMAN_GRAPH_H_H

/*
* problem:
* can't store vertex descriptor, it invalidates uncontrollable
* object can be created in dynamic memory as well as in static memory? hmm interesting
*
* solution:
*   a pointer object has to be bound to a vertex, their lifetimes are the same
*   may be a unique vertex identifier can help?
*   //https://stackoverflow.com/questions/32296206/bgl-indexing-a-vertex-by-keys
*   //https://stackoverflow.com/questions/26697477/cut-set-of-a-graph-boost-graph-library/26699982#26699982
*   //https://stackoverflow.com/questions/10540338/how-to-create-a-named-graph-with-boost-graph-library
*   //bi-index map!!!! vertex->prop && prop->vertex
*/
