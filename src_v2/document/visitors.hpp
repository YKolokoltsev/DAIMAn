//
// Created by morrigan on 23/06/17.
//

#ifndef DAIMAN_VISITORS_H
#define DAIMAN_VISITORS_H

#include <set>
#include <boost/graph/breadth_first_search.hpp>

#include "graph.h"

/*
 * Used to stop BFS search, as it is recommended by GSL documentation
 */
class found_node_exception : public std::exception{
public:
    found_node_exception(node_desc_t v): v{v} {};
    const node_desc_t v;
};

template<typename TTarget>
class first_nearest_visitor : public boost::default_bfs_visitor {
public:
    first_nearest_visitor():type_hash( typeid(TTarget).hash_code()) {};

    template < typename Vertex, typename Graph >
    void discover_vertex(Vertex u, const Graph& g) const {
        if(type_hash == g[u].type_hash) throw found_node_exception(u);
    }

private:
    const size_t type_hash;
};

template<typename TNodeDesc>
class list_deps_visitor : public boost::default_bfs_visitor{
public:
    typedef std::shared_ptr<std::list<TNodeDesc>> t_list;
    list_deps_visitor(t_list nlist, bool inverse, bool print):nlist{nlist}, inverse{inverse}, print{print} {};

    template < typename Vertex, typename Graph >
    void discover_vertex(Vertex u, const Graph& g) const {
        if(inverse){
            nlist->push_front(u);
        }else{
            nlist->push_back(u);
        }
        if(print) cout << "discovered: " << u << endl;
    }

private:
    std::shared_ptr<std::list<TNodeDesc>> nlist;
    bool inverse;
    bool print;
};

template<typename TTarget>
class next_dep_visitor  : public boost::default_bfs_visitor{
public:
    next_dep_visitor():
            type_hash( typeid(TTarget).hash_code()),
            discovered(new std::set<void*>()){};

    template < typename Vertex, typename Graph >
    void discover_vertex(Vertex u, const Graph& g) {
        if(type_hash == g[u].type_hash && discovered->find(g[u].ref_ptr) == discovered->end()) {
            discovered->insert(g[u].ref_ptr);
            throw found_node_exception(u);
        }
    }

private:
    //share same set over all next_dep_visitor copies
    std::shared_ptr<std::set<void*>> discovered;
    const size_t type_hash;
};


#endif //DAIMAN_VISITORS_H
