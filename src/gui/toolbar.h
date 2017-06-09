#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QToolBar>
#include <QAction>
#include <QMenu>
#include "mainwindow.h"

class MainWindow;

class Toolbar : public QToolBar
{
    Q_OBJECT

public:
    Toolbar(MainWindow* parent);

private slots:
    void sl_rotate();
    void sl_select();
    void sl_ntriangle();
    void sl_dtriangle();
    void sl_npoint();
    void sl_dpoint();
    void sl_nzfxsurf();
    void sl_act_save_eps();

private:
    bool createActions();
    void createMenus();

public:
    void initActions();
    void disableActions();
    void updateState();

public:
    QMenu*   m_ntriangle;
    QMenu*   m_npoint;

    QAction* act_rotate;
    QAction* act_select;
    QAction* act_dtriangle;
    QAction* act_dpoint;
    QAction* act_nzfxsurf;
    QAction* act_save_eps;

private:
    QString icosize; //16x16, 32x32 or 48x48
    QString icopath; //":/Icons"

    bool menu_created;
    MainWindow* papa;

};

#endif // TOOLBAR_H
