//
// Created by morrigan on 23/06/17.
//

#ifndef DAIMAN_VISITORS_H
#define DAIMAN_VISITORS_H

#include <boost/graph/breadth_first_search.hpp>

#include "graph.h"

/*
 * Used to stop BFS search, as it is recommended by GSL documentation
 */
class found_node_exception : public std::exception{
public:
    found_node_exception(node_desc_t v): v{v} {};
    //todo: a ono nado?
    //virtual const char* what() const throw() {
    //    return nullptr;
    //}
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

//todo: traverse dependencies only
template<typename TNodeDesc>
class list_all_visitor : public boost::default_bfs_visitor{
public:
    typedef std::shared_ptr<std::list<TNodeDesc>> t_list;
    list_all_visitor(t_list nlist):nlist{nlist}{};

    template < typename Vertex, typename Graph >
    void discover_vertex(Vertex u, const Graph& g) const {
        nlist->push_back(u);
        std::cout << "visited: " << u << std::endl;
    }

private:
    std::shared_ptr<std::list<TNodeDesc>> nlist;
};


#endif //DAIMAN_VISITORS_H
