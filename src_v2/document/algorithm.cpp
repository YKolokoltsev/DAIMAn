//
// Created by morrigan on 1/07/17.
//

#include "doc_tree.hpp"
#include "algorithm.hpp"

void remove_recursive(node_desc_t v_root){
    auto dt = DocTree::inst();
    if(v_root == dt->null_vertex) throw runtime_error("can't start BFS from null vertex");

    using t_visitor = list_deps_visitor<node_desc_t>;
    auto vertices = t_visitor::t_list(new t_visitor::t_list::element_type);
    t_visitor vis(vertices, true);

    breadth_first_search(make_reverse_graph<Graph>(dt->g), v_root, visitor(vis));

    //root_ptr can be unstable during sequential vertex remove process
    //so we recover it on each loop iteration from this shared_ptr
    //after the root vertex removed the loops breaks, and an object
    //is deleted automatically at function exit
    auto root_ptr = dynamic_pointer_cast<BaseObj>(dt->g[v_root].ptr);
    int count = vertices->size();
    while(!vertices->empty()){
        auto v = vertices->front();
        auto hash = dt->g[v].type_hash;
        clear_vertex(v, dt->g);
        remove_vertex(v, dt->g);
        count--;
        cout << "removed vertex "<< v <<" for " << dt->thash_name_map[hash] << endl;
        if(vertices->size() == 1){
            break;
        }else{
            vertices->clear();
            //some vertices can change their descriptors during remove process
            //so we rediscover the bottom each time
            breadth_first_search(make_reverse_graph<Graph>(dt->g), root_ptr->get_idx(), visitor(vis));
        }
    }

    //implicit vertex remove should not happen, it would mean
    //that one vertex, during it's remove process, most likely
    //in it's top level destructor, called a remove vertex
    if(count != 0) cerr << "check for lost vertices" << endl;
}