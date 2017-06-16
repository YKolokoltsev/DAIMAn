#include <iostream>
#include <memory>

#include <boost/graph/adjacency_list.hpp>

using namespace std;
using namespace boost;

/*
 * The main nodes in this tree are mathematical objects. For example:
 * point, line, surface, volume, etc. These objects understood as "analytical".
 * For example, if a line is straight, than we can deduce any point
 * that lies on it analytically, given a pair of points used to create this line
 * object.
 *
 * Now, suppose that we already have two points as separate objects and a line
 * object created by passing these points into the constructor of the line class
 * because of some physical causality relation between them. Afterwards, we would
 * like to recalculate points to polish it's positions. If we store points as copies
 * inside a line object during it's creation, than we obtain an error - the line
 * will not pass through given points with new positions. Therefore, there is a
 * strong necessity to store a pointers onto the point's objects within a line class.
 * In this case, each time we need a point's coordinates within line class algorithm,
 * we will automatically use the actual data.
 *
 * A situation is more interesting if the line is not straight and is calculated using
 * any other additional data. In a generic case, changes in the dependencies of the
 * objects shell be monitored to avoid miss interpretation of the results.
 *
 * Such monitoring requirement is the main reason why we decided to use a graph data
 * structure. In this graph, which is a tree, we store causality dependencies
 * (graph edges) between the objects (graph nodes).
 */

// Another moment is to monitor cyclic dependencies, that is crucial
// at least for the deletion of shared_ptr objects (deadlock is possible)


//a base object for all auto-managed objects
struct DocNode{};

class DOCTree{
public:
    typedef std::shared_ptr<DocNode> node_t;
    DOCTree(node_t root){
        g[add_vertex(g)].ptr = root;
    }
private:
    typedef struct{node_t ptr;} vertex_info;
    typedef adjacency_list<vecS, vecS, directedS, vertex_info> Graph;

private:
    Graph g;
};


int main(){
    DOCTree dt(DOCTree::node_t(new DocNode));
    cout << "hello from new arch!" << endl;
}