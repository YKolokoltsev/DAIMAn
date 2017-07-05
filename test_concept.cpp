#include <iostream>
#include <memory>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/named_graph.hpp>

using namespace boost;
using namespace std;

struct Polymorph{
    virtual ~Polymorph(){};
    bool autoremove_vertex = true;
protected:
    bool is_weak;
};

struct vertex_props{
    std::shared_ptr<Polymorph> ptr;
    void* ref_ptr;
};

using Graph = adjacency_list<vecS, vecS, bidirectionalS, vertex_props>;
using Vertex = Graph::vertex_descriptor;
Graph g;

struct A : public Polymorph{
    A(bool is_weak){
        this->is_weak = is_weak;
        auto v = add_vertex(g);
        g[v].ref_ptr = this;
        g[v].ptr = std::shared_ptr<A>(this,[](A* p){
            if(!p->is_weak) {
                cout << "delete from shared" << endl;
                p->autoremove();
                delete p;
            }
        });
    }

    ~A(){
        autoremove();
        cout << "--deleted" << endl;
    }

    Vertex get_idx(){
        graph_traits<Graph>::vertex_iterator vi, vi_end;
        tie(vi, vi_end) = vertices(g);
        for(;vi != vi_end; vi++)
            if(g[*vi].ref_ptr == this) return *vi;
        cerr << "idx not found" << endl;
        return g.null_vertex();
    }

private:
    void autoremove(){
        is_weak = true;
        if(!autoremove_vertex) return;
        autoremove_vertex = false;
        auto v = get_idx();
        if(v == g.null_vertex()) return;
        cout << "autoremove start" << endl;
        clear_vertex(v, g);
        remove_vertex(v, g);
        cout << "autoremove end" << endl;
    }
};

struct B : public A{
    B() : A(false){}
};

int main(){
    auto a = new A(false);
    auto v = a->get_idx();
    auto ptr = g[v].ptr;
    g[v].ptr.reset();
    ptr.reset();
    //g.clear();

    cout << "num_vertices: " << num_vertices(g) << endl;
}

//remove_vertex(v,g);
/*
 *  - Vertex can't contain an invalid pointer, and has to be destroyed
 *  - Invalid (already destroyed) vertex can't contain edges
 *
 * 1. count = 0 -> last pointer (ptr destructor call, even in shared case)
 *   appears when:
 *         a) remove_vertex called explicitly, and this is the last pointer
 *            so first a vertex will be invalid, and afterwards comes a
 *            shared_ptr destructor call, even in case of artificial weak_ptr
 *         b) ptr.reset() called explicitly, and this is the last pointer
 *
 * 2. count != 0 -> there is a pointer outside a graph (NO destructor calls)
 *   appears when:
 *         a) remove_vertex called explicitly, and there is a pointer outside
 *         b) ptr.reset() called explicitly, and there is a pointer outside
 *
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