#include <iostream>
#include <QItemSelection>
#include <QMessageBox>
#include <QScreen>
#include <QFileDialog>
#include <QApplication>
#include <QDesktopWidget>
#include <QSplitter>

#include "projecttree.h"
#include "functions.h"

using namespace std;

void ProjectTree::selectionChanged (const QItemSelection & selected, const QItemSelection & deselected)
{
    if(!currentItem()) return;

    if(currentItem()->text(1) == QString("surface") || currentItem()->text(1) == QString("bond_network"))
    {
        //surfaces
        for(auto & surf : *papa->doc->get_surfaces())
            surf.is_active = currentItem()->text(0) == surf.name;

        //bond networks
        for(auto & network : *papa->doc->get_bondNetworks())
            network.is_active = currentItem()->text(0) == network.name;
    }

    if(currentItem()->text(1) == QString("info")){
        papa->setActiveWidget(generalinfo.get());
        cout << "orbitals table size: " << generalinfo->OrbitalsTable().size() << endl;
    } else {
        papa->setActiveWidget(papa->glWidget);
    }

    papa->glWidget->updateGL();

}

//********************** SLOTS

void ProjectTree::sl_SetupCamera()
{
    c_setup.reset(new CameraSetup(papa.get()));
    c_setup->show();
}

void ProjectTree::sl_SaveShot()
{
    QFileDialog* p_Dialog = new QFileDialog(this);
    QRect scr = QApplication::desktop()->availableGeometry(0);
    scr.setRect(scr.width()/4,scr.height()/4,scr.width()/2,scr.height()/2);

    p_Dialog->setGeometry(scr);
    p_Dialog->setAcceptMode(QFileDialog::AcceptSave);
    p_Dialog->setFileMode(QFileDialog::AnyFile);
    p_Dialog->setNameFilter(tr("Gnuplot data file (*.dat)"));
    p_Dialog->setViewMode(QFileDialog::Detail);

    if(papa->parameters->str_recent.count() > 0)
     p_Dialog->setDirectory(QFileInfo(papa->parameters->str_recent.at(0)).absolutePath());

    QStringList file_list;

    if(p_Dialog->exec())
        file_list = p_Dialog->selectedFiles();
    delete p_Dialog;

    if(file_list.count() == 0) return;

    QString path = file_list.at(0);

    //TakeVolume(path,papa->doc);
    //TakePhoto(path,papa->doc);
    //CalcBasins(papa->doc);
}

void ProjectTree::sl_GeneralInfo()
{
    QTreeWidgetItem * root = this->currentItem();
    if(!root) return;

    generalinfo.reset(new GeneralInfo(papa.get()));
    generalinfo->LoadInfo();

    for(int i = 0; i < root->childCount(); i++)
        if(root->child(i)->text(1) == QString("info")) return;

    QTreeWidgetItem * p_Item = new QTreeWidgetItem(root);
    p_Item->setText(0,QString("Information"));
    p_Item->setText(1,QString("info"));
}

//************************** END OF SLOTS

QMenu* ProjectTree::CreateSurfaceMenu()
{
    QMenu* pMenu = new QMenu(this);
    if(!pMenu) return pMenu;

    QAction* p_SetupCamera = new QAction("Setup Camera", this);
    connect(p_SetupCamera, SIGNAL(triggered()), this, SLOT(sl_SetupCamera()));
    pMenu->addAction(p_SetupCamera);

    QAction* p_SaveShot = new QAction("Save camera shot", this);
    connect(p_SaveShot, SIGNAL(triggered()), this, SLOT(sl_SaveShot()));
    pMenu->addAction(p_SaveShot);

    return pMenu;
}

QMenu* ProjectTree::CreateMoleculeMenu()
{
    QMenu* pMenu = new QMenu(this);
    if(!pMenu) return NULL;


    QAction* p_CalcMainInfo = new QAction("Basic Info", this);
    connect(p_CalcMainInfo, SIGNAL(triggered()), this, SLOT(sl_GeneralInfo()));
    pMenu->addAction(p_CalcMainInfo);

    return pMenu;
}

void ProjectTree::contextMenuEvent(QContextMenuEvent* p_Context)
{
    QTreeWidgetItem * p_Item = itemAt(p_Context->x(),p_Context->y());
    if(!p_Item) return;
    setCurrentItem(p_Item);

    QMenu* p_Menu = NULL;


    if(p_Item->text(1) == QString("surface"))
    {
        p_Menu = CreateSurfaceMenu();
    }

    if(p_Item->text(1) == QString("structure"))
    {
        p_Menu = CreateMoleculeMenu();
    }

    if(p_Item->text(1) == QString("info"))
    {
        p_Menu = NULL;
    }

    if(!p_Menu) return;

    p_Menu->exec(p_Context->globalPos());
    p_Menu->clear();
    delete p_Menu;
}
