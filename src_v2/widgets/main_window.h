#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "base_vertex.hpp"
#include "gui_config.h"

class DMainWindow : public QMainWindow, public BaseObj {
Q_OBJECT
public:
    DMainWindow();
};

#endif // MAINWINDOW_H
