#include <iostream>
#include <exception>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/reverse_graph.hpp>

using namespace boost;

struct idx_data{
    int desc;
    double fi;
    float eps;
};

class found_exception : public std::exception{
    virtual const char* what() const throw() {
        std::cout << "gogo" << std::endl;
        return nullptr;
    }
};

typedef adjacency_list<vecS, vecS, bidirectionalS> Graph;
//typedef property_map<Graph, vertex_index_t>::type vertex_pm;

typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
typedef boost::graph_traits<Graph>::edge_descriptor Edge;

class custom_bfs_visitor : public boost::default_bfs_visitor {
public:
    template < typename Vertex, typename Graph >
    void discover_vertex(Vertex u, const Graph & g) const {
        if(u == 1) throw found_exception();
        std::cout << "At " << u << std::endl;
    }

    template < typename Edge, typename Graph >
    void examine_edge(Edge e, const Graph& g) const {
        //std::cout << "Examining edge " << source(e) << target(e) << std::endl;
    }
};

int main(){
    Graph g;

    auto v1 = add_vertex(g);
    auto v2 = add_vertex(g);
    add_edge (v1, v2, g);
    //add_edge (3, 18, g);
    //add_edge (2, 20, g);
    //add_edge (3, 2, g);
    //add_edge (1, 1, g);
    //add_edge (3, 7, g);

    custom_bfs_visitor vis;

    try{
        breadth_first_search(g, v2, visitor(vis));
    }catch(found_exception& ex){
        std::cout << "found" << std::endl;
    }

    try{
        breadth_first_search(reverse_graph<Graph>(g), v2, visitor(vis));
    }catch(found_exception& ex){
        std::cout << "found" << std::endl;
    }


}