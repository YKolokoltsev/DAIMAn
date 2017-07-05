//
// Created by morrigan on 20/06/17.
//

#include "client_splitter.h"
#include "tree_ctrl.h"
#include "gl_screen.h"

DClientSplitter::DClientSplitter(node_desc_t main_wnd_idx) {
    reg(this, true);
    main_window = std::move(get_weak_obj_ptr<DMainWindow>(this,main_wnd_idx));

    setOpaqueResize(false);

    new DTreeCtrl(get_idx());
    new DGlScreen(get_idx());

    main_window->ptr.lock()->setCentralWidget(this);
}