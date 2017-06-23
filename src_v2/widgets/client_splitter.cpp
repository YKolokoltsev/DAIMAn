//
// Created by morrigan on 20/06/17.
//

#include "client_splitter.h"
#include "tree_ctrl.h"
#include "gl_screen.h"

DClientSplitter::DClientSplitter(DocTree::node_desc_t main_wnd_idx){
    reg_weak(this);
    main_window = std::move(doc_get_weak_obj_ptr<DMainWindow>(this,main_wnd_idx));

    setOpaqueResize(false);

    new DTreeCtrl(get_idx());
    new DGlScreen(get_idx());

    main_window->ptr->setCentralWidget(this);
}