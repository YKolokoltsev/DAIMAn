//
// Created by morrigan on 20/06/17.
//

#ifndef DAIMAN_DTREECTRL_H
#define DAIMAN_DTREECTRL_H

#include <QTreeWidget>

#include "doc_tree.hpp"
#include "client_splitter.h"

class DClientSplitter;
class DTreeCtrl: public QTreeWidget, public DocTree::BaseObj {
Q_OBJECT
public:
    DTreeCtrl(DocTree::node_desc_t);

private:
    DocTree::ext_node_ptr_t<DClientSplitter> client_edge;
};


#endif //DAIMAN_DTREECTRL_H
