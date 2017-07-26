#include <iostream>
#include <QApplication>

#include "doc_tree.hpp"
#include "main_window.h"

using namespace std;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    auto wnd = new DMainWindow();

    auto ret = app.exec();
    auto dt = DocTree::inst();
    dt->clear();
    delete wnd;

    cout << "nodes: " << dt->node_count() << ", edges: " << dt->edge_count() << endl;
    return ret;
}