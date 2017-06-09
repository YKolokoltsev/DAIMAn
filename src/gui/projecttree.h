#ifndef PROJECTTREE_H
#define PROJECTTREE_H

#include <QTreeWidget>
#include <QMenu>
#include <QContextMenuEvent>
#include "gui/mainwindow.h"
#include "camera/camerasetup.h"
#include "generalinfo.h"
#include "gui/progressdlg.h"

class MainWindow;
class CameraSetup;
class GeneralInfo;

class ProjectTree : public QTreeWidget
{
      Q_OBJECT

public:
    ProjectTree(MainWindow* parent);
    ~ProjectTree();

protected:
   void selectionChanged (const QItemSelection & selected, const QItemSelection & deselected);
   void contextMenuEvent(QContextMenuEvent* p_Context);

private slots:
   void sl_SetupCamera();
   void sl_SaveShot();
   void sl_GeneralInfo();

private:
   QMenu* CreateSurfaceMenu();
   QMenu* CreateMoleculeMenu();

private:
   MainWindow* papa;
   CameraSetup* c_setup;
   GeneralInfo* generalinfo;
};

#endif // PROJECTTREE_H
