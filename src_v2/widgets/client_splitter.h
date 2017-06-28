//
// Created by morrigan on 20/06/17.
//

#ifndef DAIMAN_CLIENTEDGE_H
#define DAIMAN_CLIENTEDGE_H

#include <QSplitter>
#include "base_vertex.hpp"
#include "ref_decorator.hpp"
#include "main_window.h"

class DMainWindow;

class DClientSplitter : public QSplitter, public BaseObj {
Q_OBJECT
public:
    DClientSplitter(node_desc_t);
private:
    ext_weak_ptr_t<DMainWindow> main_window;
};


#endif //DAIMAN_CLIENTEDGE_H
