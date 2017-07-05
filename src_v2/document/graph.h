//
// Created by morrigan on 27/06/17.
//

#ifndef DAIMAN_GRAPH_H_H
#define DAIMAN_GRAPH_H_H

#include <memory>

#include <boost/graph/adjacency_list.hpp>

using namespace boost;

//A polymorphic type is needed to define Graph type with pointers able to be downcasted
struct Polymorph{
    virtual ~Polymorph(){};
    bool autoremove_vertex = true;
    bool weak(){return is_weak;}

//protected:
    bool is_weak;
};

/*
 * DocTree is a singleton container that store and monitor dependencies between any objects.
 * However, there are some objects, like Qt widgets, that are managed by their own containers.
 * Explicit storage of this objects will conflict with their primary containers.
 * For this reason comes a requirement to work with two types of smart pointers:
 * a shared_ptr with regular destructor and a shared_ptr with empty destructor
 * todo: try to search for other solutions
 * (a variant of error-prone weak_ptr, that do not guarantee an existence of
 * pointer to not existing memory)
 *
 * Type of the storage is enumerated as follows:
 */
enum class StorageType{Shared, Weak};

/*
 * To avoid destructor deadlocks, and control a destruction order it is useful
 * to create weak dependencies, e.g. a weak_ptr pointers onto existing objects
 * that are stored inside or outside of DocTree
 */
enum class DependencyType{Shared, Weak};

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
};

/*
 * Here we monitor a dependency. If weak_ptr is pointing onto shared_ptr - dependency
 * is DependencyType::Weak. In the opposite case, it is DependencyType::Shared.
 */
struct edge_info{
    DependencyType dt;
};

/*
 * We use a bidirectional graph to have a chance to follow the graph
 * edges in both directions.
 *
 * //initial try is a vecS... vertex invalidate is unacceptable property!!! trying listS!
 */
typedef adjacency_list<vecS, vecS, bidirectionalS, vertex_info, edge_info> Graph;
typedef typename graph_traits<Graph>::edge_descriptor edge_desc_t;
typedef typename graph_traits<Graph>::vertex_descriptor node_desc_t;

#endif //DAIMAN_GRAPH_H_H
