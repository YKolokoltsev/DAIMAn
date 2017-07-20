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
    first_nearest_visitor<T*> vis;

    try{
        breadth_first_search(dt->g, v, visitor(vis));
    }catch(found_node_exception& ex){
        return ex.v;
    }

    return dt->null_vertex;
}

template<typename T>
node_desc_t doc_first_nearest_child(node_desc_t v){
    auto dt = DocTree::inst();
    if(dt->null_vertex == v) throw runtime_error("can't start BFS child from null vertex");
    auto vis = visitor(first_nearest_visitor<T*>());

    try{
        breadth_first_search(make_reverse_graph<Graph>(dt->g), v, vis);
    }catch(found_node_exception& ex){
        return ex.v;
    }

    return dt->null_vertex;
}

/*
 * Child nodes form edges to their parent nodes, meaning that a child can't exist without it's
 * father. However a father can exist without it's dependent objects. So what happens if a child
 * function will change a graph,  will add/remove another child that can change a graph. In this
 * case, if one need to access a child functionality from the father, it is inconsistent
 * to get it's pointer by ref_decorator, that will produce dependency loop in a tree. This
 * function solves the problem.
 */
template<typename T, class Function>
void for_each_child(node_desc_t v, Function f){
    auto dt = DocTree::inst();

    next_dep_visitor<T*> vis;
    //these cases are to prevent possible instabilities
    //if root index changes from 'f'
    auto root_ptr = dynamic_pointer_cast<BaseObj>(dt->g[v].ptr);

    while(1){
        try{
            if(root_ptr->get_idx() == dt->null_vertex) throw runtime_error("fix: root node removed");
            breadth_first_search(make_reverse_graph<Graph>(dt->g), root_ptr->get_idx(), visitor(vis));
            return; //if bfs did not throw found_node_exception, there is no nodes left to process
        }catch(found_node_exception& ex){
            f(static_cast<T*>(dt->g[ex.v].ptr.get()));
        }
    }
};

void remove_recursive(node_desc_t);

#endif //DAIMAN_ALGORITHMS_H
