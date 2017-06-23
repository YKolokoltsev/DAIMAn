//
// Created by morrigan on 20/06/17.
//

#ifndef DAIMAN_CLIENTEDGE_H
#define DAIMAN_CLIENTEDGE_H

#include <QSplitter>
#include "doc_tree.hpp"
#include "main_window.h"

class DMainWindow;

class DClientSplitter : public QSplitter, public DocTree::BaseObj {
Q_OBJECT
public:
    DClientSplitter(DocTree::node_desc_t);
private:
    DocTree::ext_node_ptr_t<DMainWindow> main_window;
};


#endif //DAIMAN_CLIENTEDGE_H
