#ifndef PROJECTTREE_H
#define PROJECTTREE_H

#include <memory>

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

using namespace std;

class ProjectTree : public QTreeWidget
{
      Q_OBJECT

public:
    ProjectTree(MainWindow* papa) : papa{papa}{
        setHeaderLabel(QString("Project Tree"));
    }

    ~ProjectTree(){};

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
   const shared_ptr<MainWindow> papa;
   shared_ptr<CameraSetup> c_setup;
   shared_ptr<GeneralInfo> generalinfo;
};

#endif // PROJECTTREE_H
