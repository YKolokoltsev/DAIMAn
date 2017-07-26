//
// Created by morrigan on 20/06/17.
//

#include "doc_tree.hpp"

DocTree* DocTree::instance = nullptr;
std::recursive_mutex DocTree::mtx;

std::shared_ptr<DocTree> DocTree::inst() {
    mtx.lock();
    if(DocTree::instance == nullptr){
        DocTree::instance = new DocTree();
    }
    return std::shared_ptr<DocTree>(DocTree::instance,[](DocTree* p){
        DocTree::mtx.unlock();
    });
}

void DocTree::clear(){
    /*
    graph_traits<Graph>::vertex_iterator vi, vi_end;
    tie(vi, vi_end) = vertices(g);
    while(vi != vi_end){
        auto hash = g[*vi].type_hash;
        cout << "cleared: " << thash_name_map[g[*vi].type_hash] << endl;
        clear_vertex(*vi, g);
        remove_vertex(*vi, g);
        tie(vi, vi_end) = vertices(g);
    };*/

    std::list< node_desc_t > c;
    topological_sort(g, std::front_inserter(c));

    while(c.size() > 0){
        auto v = c.front();
        auto hash = g[v].type_hash;
        g[v].ptr->autoremove_vertex = false;
        clear_vertex(v, g);
        remove_vertex(v, g);
        cout << "removed vertex "<< v <<" for " << thash_name_map[hash] << endl;
        c.clear();
        topological_sort(g, std::front_inserter(c));
    }

    thash_name_map.clear();
}

/*
 * This function checks if both node are properly registered
 * and maintains a directed graph without cycles. If there
 * happens a cyclic dependency - there is something very wrong
 * and shell be redesigned.
 */
edge_desc_t DocTree::add_dependency(node_desc_t from, node_desc_t to){
    //cout << "adding edge: " << from << "->" << to << endl;
    auto eb = add_edge(from,to,g);
    //todo: inspect double dependencies, shell not happen
    if(!eb.second) throw runtime_error("can't add dependency edge");

    //if there is a cycle, an exception will happen
    std::vector<node_desc_t > c;
    topological_sort(g, std::back_inserter(c));
    return eb.first;
}