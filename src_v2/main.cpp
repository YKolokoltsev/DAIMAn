#include <iostream>
#include <QApplication>

#include "doc_tree.hpp"
#include "main_window.h"

using namespace std;

int main(int argc, char *argv[])
{
    auto dt = DocTree::inst();
    QApplication app(argc, argv);
    new DMainWindow();

    auto ret = app.exec();
    dt->clear();
    cout << "nodes: " << dt->node_count() << ", edges: " << dt->edge_count() << endl;
    return ret;
}