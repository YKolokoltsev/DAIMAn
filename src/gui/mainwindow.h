#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "parameters.h"
#include "glwidget.h"
#include "toolbar.h"
#include "document.h"
#include "projecttree.h"
#include <QSplitter>

class QAction;
class QMenu;
class GLWidget;
class Toolbar;
class ProjectTree;
class Document;
class CameraSetup;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

public:
    void setActiveWidget(QWidget* main);

private slots:
    void open();
    void openRecentFile();

    void addSurface();
    void addPlane();
    void addBondPaths();
    void delObject();

private:
    void createActions();
    void createMenus();
    void createSubmenus();
    bool readWFX(QString path);

private:
    QMenu   *fileMenu;
    QMenu   *objectsMenu;
    QMenu   *recentFilesMenu;

    QAction *openAct;
    QList<QAction*> openRecentAct;
    QAction *exitAct;

    QAction *addSurfaceAct;
    QAction *addPlaneAct;
    QAction *calcBondPathsAct;
    QAction *delObjectAct;

public:
    Parameters*  parameters;
    GLWidget*    glWidget;
    ProjectTree* treeWidget;
    QSplitter*   mainWidget;
    Toolbar*     tb;
    Document*    doc;
};

#endif // MAINWINDOW_H
