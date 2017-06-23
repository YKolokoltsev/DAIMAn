#include <iostream>
#include <QApplication>

#include "doc_tree.hpp"
#include "main_window.h"

using namespace std;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    new DMainWindow();

    auto ret = app.exec();
    doc_clear();
    cout << "nodes: " << doc_node_count() << ", edges: " << doc_edge_count() << endl;
    return ret;
}