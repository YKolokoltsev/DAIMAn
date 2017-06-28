//
// Created by morrigan on 16/06/17.
//

#ifndef DAIMAN_DOC_TREE_HPP_H
#define DAIMAN_DOC_TREE_HPP_H

#include <iostream>
#include <memory>
#include <map>
#include <set>
#include <list>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>

#include "graph.h"

using namespace std;
using namespace boost;

/*
 * The most important nodes in this tree are mathematical objects. For example:
 * point, line, surface, volume, etc. These objects understood as "analytical".
 * For example, if a line is straight, than we can deduce any point
 * that lies on it analytically given a pair of points used to create this line
 * object.
 *
 * Now, suppose that we already have two points as separate objects and a line
 * object created by passing these points into the constructor of the line class
 * because of some physical causality relation between them. Afterwards, we would
 * like to recalculate points to polish them positions. If we store points as copies
 * inside a line object during it's creation, than we obtain an error - the line
 * will not pass through given points with new positions. Therefore, there is a
 * strong necessity to store a pointers onto the point's objects within a line class.
 * In this case, each time we need a point's coordinates within line class algorithm,
 * we will automatically use the most recent data.
 *
 * A situation is more interesting if the line is not straight and is calculated using
 * any other additional data. In a generic case, changes in the dependencies of the
 * objects shell be monitored to avoid miss interpretation of the results.
 *
 * Such monitoring requirement is the main reason why we decided to use graph data
 * structure to store dynamic objects. In this graph, which is a tree, we store
 * causality dependencies (graph edges) between the objects (graph nodes).
 */

class DocTree{
public:
    class BaseObj; //todo: required only for clear function, move clear to algorithms!!!
    static DocTree* inst();
    ~DocTree(){clear();}

    void clear();
    edge_desc_t add_dependency(node_desc_t, node_desc_t);
    size_t node_count();
    size_t edge_count();

public:
    const node_desc_t null_vertex;
    Graph g;
    std::map<size_t, std::string> thash_name_map; //maps type hash to it's name, for debug purposes

private:
    static DocTree* instance;
    DocTree() : g(), null_vertex{g.null_vertex()} {};
};

#endif //DAIMAN_DOC_TREE_HPP_H
