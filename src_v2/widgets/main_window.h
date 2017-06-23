#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "doc_tree.hpp"
#include "gui_config.h"

class DMainWindow : public QMainWindow, public DocTree::BaseObj {
Q_OBJECT
public:
    DMainWindow();
};

#endif // MAINWINDOW_H
