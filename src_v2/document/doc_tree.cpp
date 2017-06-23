//
// Created by morrigan on 20/06/17.
//

#include "doc_tree.hpp"

DocTree* DocTree::instance = nullptr;

DocTree* DocTree::inst() {
    if(DocTree::instance == nullptr){
        DocTree::instance = new DocTree();
    }else{
        return DocTree::instance;
    }
}

void DocTree::clear(){
    //todo: make a smart remove following dependencies tree
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
    for(auto v : c){
        auto ptr = static_cast<BaseObj*>(g[v].ptr.get());
        clear_vertex(v, g);
        remove_vertex(v, g);
        //node shell know that it is deleted from graph
        ptr->v = null_vertex;
        cout << "cleared: " << thash_name_map[g[v].type_hash] << endl;
    }

    thash_name_map.clear();
}

DocTree::edge_desc_t DocTree::add_dependency(DocTree::node_desc_t from, DocTree::node_desc_t to){
    //cout << "adding edge: " << from << "->" << to << endl;
    auto eb = add_edge(from,to,g);
    if(!eb.second) throw runtime_error("can't add dependency edge");

    //if there is a cycle, an exception will happen
    std::vector< DocTree::node_desc_t > c;
    topological_sort(g, std::back_inserter(c));
    return eb.first;
}

//**********************************non-template interface functions

void doc_clear(){
    DocTree::inst()->clear();
}

size_t doc_node_count(){
    return num_vertices(DocTree::inst()->g);
}

size_t doc_edge_count(){
    return num_edges(DocTree::inst()->g);
}