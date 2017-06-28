//
// Created by morrigan on 23/06/17.
//

#ifndef DAIMAN_ALGORITHMS_H
#define DAIMAN_ALGORITHMS_H

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/reverse_graph.hpp>

#include "doc_tree.hpp"
#include "base_vertex.hpp"
#include "visitors.hpp"

template<typename T>
node_desc_t doc_first_nearest_father(node_desc_t v){
    auto dt = DocTree::inst();
    if(dt->null_vertex == v) throw runtime_error("can't start BFS father from null vertex");
    first_nearest_visitor<T> vis;

    try{
        breadth_first_search(dt->g, v, visitor(vis));
    }catch(found_node_exception& ex){
        std::cout << "found: " << ex.v << std::endl;
        return ex.v;
    }

    return dt->null_vertex;
}

template<typename T>
node_desc_t doc_first_nearest_child(node_desc_t v){
    auto dt = DocTree::inst();
    if(dt->null_vertex == v) throw runtime_error("can't start BFS child from null vertex");
    first_nearest_visitor<T> vis;

    try{
        breadth_first_search(make_reverse_graph<Graph>(dt->g), v, visitor(vis));
    }catch(found_node_exception& ex){
        std::cout << "found: " << ex.v << std::endl;
        return ex.v;
    }

    return dt->null_vertex;
}

//todo: finish it!!!!
void remove_recursive(node_desc_t v){
    auto dt = DocTree::inst();
    if(dt->null_vertex == v) throw runtime_error("can't start BFS from null vertex");

    using t_visitor = list_all_visitor<node_desc_t>;
    auto vertices = t_visitor::t_list(new t_visitor::t_list::element_type);
    t_visitor vis(vertices);

    breadth_first_search(dt->g, v, visitor(vis));
    std::cout << "got: " << vertices->size() << " vertices to remove" << std::endl;
}

#endif //DAIMAN_ALGORITHMS_H
