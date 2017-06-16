#include <QtGui>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QInputDialog>
#include <QLineEdit>
#include <QList>

#include "mainwindow.h"
#include "wavefunction/wfndata.h"
#include "wavefunction/wfxparser.h"
#include "projecttree.h"
#include <QTreeWidgetItem>
#include "functions.h"
#include "aim_sum_r.h"

#include <boost/spirit/include/qi.hpp>
#include <string>
#include <sstream>
#include <fstream>

//#include "chem_bonds.h"

using namespace boost::spirit::qi;

MainWindow::MainWindow()
{
    parameters = new Parameters();
    glWidget = new GLWidget(this);
    treeWidget = new ProjectTree(this);
    mainWidget = new QSplitter(this);
    tb = new Toolbar(this);
    doc = new Document();

    fileMenu = NULL;
    objectsMenu = NULL;
    recentFilesMenu = NULL;

    createActions();
    createMenus();
    createSubmenus();

    treeWidget->setColumnCount(1);
    mainWidget->addWidget(treeWidget);
    mainWidget->addWidget(glWidget);
    setCentralWidget(mainWidget);


    addToolBar(tb);
}

MainWindow::~MainWindow()
{
    if(parameters) delete parameters;

    if(mainWidget)
    {
        setCentralWidget(NULL);
        delete mainWidget;
    }
    if(tb)
    {
        removeToolBar(tb);
        delete tb;
    }
    if(doc) delete doc;
}


void MainWindow::createActions()
{
     openAct = new QAction(tr("&Open..."), this);
     openAct->setShortcuts(QKeySequence::Open);
     openAct->setStatusTip(tr("Open wavefunction file"));
     connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

     exitAct = new QAction(tr("E&xit"), this);
     exitAct->setShortcuts(QKeySequence::Quit);
     exitAct->setStatusTip(tr("Exit the application"));
     connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

     addSurfaceAct = new QAction(tr("Add &Surface"), this);
     addSurfaceAct->setStatusTip(tr("Add an arbitrary surface as a set of triangles"));
     connect(addSurfaceAct, SIGNAL(triggered()), this, SLOT(addSurface()));

     addPlaneAct = new QAction(tr("Add &Plane"), this);
     addPlaneAct->setStatusTip(tr("Creates an intersection plane"));
     connect(addPlaneAct, SIGNAL(triggered()), this, SLOT(addPlane()));

     calcBondPathsAct = new QAction(tr("AbInitio &Bond Paths"), this);
     calcBondPathsAct->setStatusTip(tr("Calculates bond paths between atoms"));
     connect(calcBondPathsAct, SIGNAL(triggered()), this, SLOT(addBondPaths()));

     addNNAsAct = new QAction(tr("Search &NNAs"), this);
     addNNAsAct->setStatusTip(tr("Gradient search for NNAs in a box"));
     connect(addNNAsAct, SIGNAL(triggered()), this, SLOT(addNNAs()));

     delObjectAct = new QAction(tr("&Delete Object"), this);
     delObjectAct->setStatusTip(tr("Removes a selected object"));
     connect(delObjectAct, SIGNAL(triggered()), this, SLOT(delObject()));
}

void MainWindow::createMenus()
{
     fileMenu = menuBar()->addMenu(tr("&File"));
     objectsMenu = menuBar()->addMenu(tr("&Objects"));

     fileMenu->addAction(openAct);

     recentFilesMenu = new QMenu("Recent Files",this);
     fileMenu->addMenu(recentFilesMenu);
     recentFilesMenu->setDisabled(true);

     fileMenu->addSeparator();
     fileMenu->addAction(exitAct);

     objectsMenu->addAction(addSurfaceAct);
     objectsMenu->addAction(addPlaneAct);
     objectsMenu->addAction(calcBondPathsAct);
     objectsMenu->addAction(addNNAsAct);
     objectsMenu->addAction(delObjectAct);
}

void MainWindow::createSubmenus()
{
    int i = 0;
    QAction* current = NULL;

    //RECENT FILES SUBMENU

    recentFilesMenu->clear();

    while(openRecentAct.count() != 0)
    {
        current = openRecentAct.at(0);
        openRecentAct.removeFirst();
        if(current != NULL)
        {
            disconnect(current, 0, 0, 0);
            delete current;
            current = NULL;
        }
    }


    if(parameters->str_recent.count() == 0)
        recentFilesMenu->setDisabled(true);
    else
    {
        recentFilesMenu->setDisabled(false);

       //Creating actions
       for (i = 0; i < parameters->str_recent.count(); i++)
       {
           openRecentAct.append((QAction*) new QAction(QFileInfo(parameters->str_recent.at(i)).fileName(),this));
           openRecentAct.last()->setData(parameters->str_recent.at(i));
           connect(openRecentAct.last(), SIGNAL(triggered()),this, SLOT(openRecentFile()));
       }

       //Setting up menu items
       for(i = 0; i < openRecentAct.count(); i++)
         recentFilesMenu->addAction(openRecentAct.at(i));
    }
}

void MainWindow::open()
{
    QFileDialog* p_Dialog = new QFileDialog(this);
    QRect scr = QGuiApplication::primaryScreen()->availableGeometry();
    scr.setRect(scr.width()/4,scr.height()/4,scr.width()/2,scr.height()/2);
    p_Dialog->setGeometry(scr);

    p_Dialog->setNameFilter(tr("AIM Wave Function Files (*.wfx)"));
    p_Dialog->setAcceptMode(QFileDialog::AcceptOpen);
    p_Dialog->setFileMode(QFileDialog::ExistingFile);
    p_Dialog->setViewMode(QFileDialog::Detail);

    if(parameters->str_recent.count() > 0)
     p_Dialog->setDirectory(QFileInfo(parameters->str_recent.at(0)).absolutePath());

    QStringList file_list;

    if(p_Dialog->exec())
        file_list = p_Dialog->selectedFiles();

    if(file_list.count() != 0)
    {
        //todo: bug - adding possibly not a wfx file and creating submenus!?!
        parameters->setRecent(file_list.at(0));
        createSubmenus();

        if(QFileInfo(file_list.at(0)).completeSuffix() == QString("wfx"))
            if(readWFX(file_list.at(0)))
            {
                treeWidget->clear();
                QTreeWidgetItem * root = new QTreeWidgetItem((QTreeWidgetItem*) NULL);
                root->setText(0,QFileInfo(parameters->str_recent.at(0)).fileName());
                root->setText(1,QString("structure"));
                treeWidget->insertTopLevelItem(0,root);
            };
    }

    delete p_Dialog;

    //Open files atomatically, calculate, and close
    /*
    QStringList file_list;
    QList<int> atom_list;

    //Intersections (atoms & porphyrins)
    //file_list.push_front("/home/data/unamwork/awork/120110/wfx/Co");
    //file_list.push_front("/home/data/unamwork/awork/120110/wfx/CoP");
    //file_list.push_front("/home/data/unamwork/awork/120110/wfx/Co_Ion");
    //file_list.push_front("/home/data/unamwork/awork/120110/wfx/Cu");
    //file_list.push_front("/home/data/unamwork/awork/120110/wfx/CuP");
    //file_list.push_front("/home/data/unamwork/awork/120110/wfx/Cu_Ion");
    //file_list.push_front("/home/data/unamwork/awork/120110/wfx/Fe");
    //file_list.push_front("/home/data/unamwork/awork/120110/wfx/FeP");
    //file_list.push_front("/home/data/unamwork/awork/120110/wfx/Fe_Ion");
    //file_list.push_front("/home/data/unamwork/awork/120110/wfx/H");
    //file_list.push_front("/home/data/unamwork/awork/120110/wfx/H2P");
    //file_list.push_front("/home/data/unamwork/awork/120110/wfx/Mg");
    //file_list.push_front("/home/data/unamwork/awork/120110/wfx/MgP");
    //file_list.push_front("/home/data/unamwork/awork/120110/wfx/Mg_Ion");
    //file_list.push_front("/home/data/unamwork/awork/120110/wfx/Mg2_Ion");
    //file_list.push_front("/home/data/unamwork/awork/120110/wfx/Mn");
    //file_list.push_front("/home/data/unamwork/awork/120110/wfx/MnClP");
    //file_list.push_front("/home/data/unamwork/awork/120110/wfx/MnP");
    //file_list.push_front("/home/data/unamwork/awork/120110/wfx/Ni");
    //file_list.push_front("/home/data/unamwork/awork/120110/wfx/NiP");
    //file_list.push_front("/home/data/unamwork/awork/120110/wfx/Ni_Ion");
    //file_list.push_front("/home/data/unamwork/awork/120110/wfx/Pd");
    //file_list.push_front("/home/data/unamwork/awork/120110/wfx/PdP");
    //file_list.push_front("/home/data/unamwork/awork/120110/wfx/Pd_Ion");
    //file_list.push_front("/home/data/unamwork/awork/120110/wfx/Zn");
    //file_list.push_front("/home/data/unamwork/awork/120110/wfx/ZnP");
    //file_list.push_front("/home/data/unamwork/awork/120110/wfx/Zn_Ion");

    //file_list.push_front("/home/data/unamwork/awork/120414/wfx/CoP_C60");
    //atom_list.push_front(96);
    //file_list.push_front("/home/data/unamwork/awork/120414/wfx/CuP_C60");
    //atom_list.push_front(24);
    //file_list.push_front("/home/data/unamwork/awork/120414/wfx/FeClP_C60");
    //atom_list.push_front(24);
    //file_list.push_front("/home/data/unamwork/awork/120414/wfx/FeP_C60");
    //atom_list.push_front(24);
    //file_list.push_front("/home/data/unamwork/awork/120414/wfx/H2P_C60");
    //atom_list.push_front(???);
    //file_list.push_front("/home/data/unamwork/awork/120414/wfx/MgP_C60");
    //atom_list.push_front(24);
    //file_list.push_front("/home/data/unamwork/awork/120414/wfx/NiP_C60");
    //atom_list.push_front(24);
    //file_list.push_front("/home/data/unamwork/awork/120414/wfx/PdP_C60");
    //atom_list.push_front(24);
    //file_list.push_front("/home/data/unamwork/awork/120414/wfx/ZnP_C60");
    //atom_list.push_front(24);

    //file_list.push_front("/home/data/unamwork/awork/120523/wfx/FeClP");
    //atom_list.push_front(24);
    //file_list.push_front("/home/data/unamwork/awork/120523/wfx/Mn");
    //atom_list.push_front(0);
    //file_list.push_front("/home/data/unamwork/awork/120523/wfx/Mn_Ion");
    //atom_list.push_front(0);
    //file_list.push_front("/home/data/unamwork/awork/120523/wfx/MnClP");
    //atom_list.push_front(25);
    file_list.push_front("/home/data/unamwork/awork/120523/wfx/H2P");
    atom_list.push_front(0);
    //file_list.push_front("/home/data/unamwork/awork/120523/wfx/MnP");
    //atom_list.push_front(24);


    QTreeWidgetItem * item;
    QTreeWidgetItem * root;
    GeomObject * object;
    Volumetrics box;
    for(int i = 0; i < file_list.count(); i++){

        parameters->setRecent(file_list.at(i)+".wfx");
        createSubmenus();

        fprintf(stderr,file_list.at(i).toAscii()+".wfx\n");
        if(readWFX(file_list.at(i)+".wfx"))
        {
            treeWidget->clear();
            root = new QTreeWidgetItem((QTreeWidgetItem*) NULL);
            root->setText(0,QFileInfo(parameters->str_recent.at(0)).fileName());
            root->setText(1,QString("structure"));
            treeWidget->insertTopLevelItem(0,root);

            //ADD SURFCE
            Surface surf("AutomaticSurface");
            surf.is_active = true;
            QList<Surface> newss = doc->get_surfaces();
            newss.append(surf);
            doc->set_Surfaces(newss);

            item = new QTreeWidgetItem(root);
            item->setText(0,"AutomaticSurface");
            item->setText(1,QString("surface"));

            //CALCULATE INTERSECTIONS (was commented)

            object = doc->get_geomobject();
            for(int j = 1; j < 4; j++){

                if(j == 1){
                    object->camera.norm = QVector3D(1,0,0);
                }else if(j == 2){
                    object->camera.norm = QVector3D(0,1,0);
                }else if(j == 3){
                    object->camera.norm = QVector3D(0,0,1);
                }

                object->camera.calcExEy();

                if(j == 1){
                    TakePhoto(file_list.at(i)+"_inc_1.dat",doc);
                }else if(j == 2){
                    TakePhoto(file_list.at(i)+"_inc_2.dat",doc);
                }else if(j == 3){
                    TakePhoto(file_list.at(i)+"_inc_3.dat",doc);
                }

            }

            //CALCULATE VOLUMES

            doc->get_wfn()->getMolBoxA(&box,atom_list.at(i));
            fprintf(stderr,file_list.at(i).toAscii());
            fprintf(stderr," i=%d: DX=%2.3f DY=%2.3f DZ=%2.3f VOL=%2.3f \n\n",i,box.x_max-box.x_min, box.y_max-box.y_min, box.z_max-box.z_min,
            (box.x_max-box.x_min)*(box.y_max-box.y_min)*(box.z_max-box.z_min));

            TakeVolume(file_list.at(i)+"_box.dat",doc,box);

            //CALCULATE Bond Path's (was commented)

            LS_VECTORLIST points;
            LS_VECTORLIST::iterator it_pts;

            getBondPts("FeP_C60_Fe25-C39",&points);
            fprintf(stderr,"Number of points: %d\n\n",points.size());


            for(it_pts = points.begin(); it_pts != points.end(); ++it_pts){
                fprintf(stderr,"%f %f %f %f %f %f\n",
                        calcDistance(*points.begin(),(*it_pts)),
                        (*it_pts).x(),
                        (*it_pts).y(),
                        (*it_pts).z(),
                        doc->get_wfn()->calc_elf((*it_pts).x(),(*it_pts).y(),(*it_pts).z()),
                        doc->get_wfn()->calc_rho((*it_pts).x(),(*it_pts).y(),(*it_pts).z()));
            }

        };

    }*/

}

void MainWindow::openRecentFile()
 {
     QAction *action = qobject_cast<QAction *>(sender());
     if (action)
     {
         parameters->setRecent(action->data().toString());
         createSubmenus();

       if(QFileInfo(parameters->str_recent.at(0)).completeSuffix() == QString("wfx"))
            if(readWFX(parameters->str_recent.at(0)))
            {
                treeWidget->clear();
                QTreeWidgetItem * root = new QTreeWidgetItem((QTreeWidgetItem*) NULL);
                root->setText(0,QFileInfo(parameters->str_recent.at(0)).fileName());
                root->setText(1,QString("structure"));
                treeWidget->insertTopLevelItem(0,root);
           };
     }

 }

void MainWindow::addSurface()
{
    bool ok = false;

    QTreeWidgetItem *root = treeWidget->topLevelItem(0);
    if(!root)
    {
        msgBox("Open data file first");
        return;
    }

    QString name =
    QInputDialog::getText(this,
                          QString("New Surface Name"),
                          NULL,
                          QLineEdit::Normal,
                          QString("Surface_")+QString().setNum(doc->get_surfaces()->count() + 1),
                          &ok);
    if(ok)
    {
        doc->get_surfaces()->append(Surface(name));

        QTreeWidgetItem * item = new QTreeWidgetItem(root);
        item->setText(0,name);
        item->setText(1,QString("surface"));
    }
}

void MainWindow::addPlane()
{
    msgBox("This function is not yet implemented");
}

void MainWindow::delObject()
{   
    QList<QTreeWidgetItem*> selected = treeWidget->selectedItems();
    if(selected.count() == 0){
        msgBox("Nothing to delete");
        return;
    }

    if(selected.at(0)->text(1) == QString("surface"))
    {

        QList<Surface> * surfaces = doc->get_surfaces();
        for(int i = 0; i < surfaces->count(); i++)
           if(surfaces->at(i).is_active)
               surfaces->removeAt(i);

        selected.at(0)->parent()->removeChild(selected.at(0));
    }
}

 bool MainWindow::readWFX(QString path)
 {
     WFNData* wfn = new WFNData();

     WFXParser parser;
     parser.setWFN(wfn);
     parser.openFile(path);

     if(parser.parse())
     {
        if(wfn->calc_helpers())
        {
            doc->set_WFN(wfn);
            doc->set_wfn_file_path(path);
            glWidget->update();
            tb->initActions();
            return true;
        }
        delete wfn;
        return false;
     }
     else
        delete wfn;
        return false;
 }

void MainWindow::setActiveWidget(QWidget* main)
{
    if(!main) return;
    bool found = false;
    int i,sum = 0;

    QList<int> sz = mainWidget->sizes();
    for(i = 0; i < sz.count(); i++)
        sum += sz.at(i);
    sum = sum - sz.at(0);



    for(i = 1; i < mainWidget->count(); i++)
        if(mainWidget->widget(i) != main) sz.replace(i,0), mainWidget->widget(i)->hide();
        else found = true, sz.replace(i,sum), mainWidget->widget(i)->show();

    if(!found)
        mainWidget->addWidget(main), sz.append(sum);

    mainWidget->setSizes(sz);
}

void MainWindow::addNNAs(){
    //todo: implement it
    cout << "add NNAs action" << endl;
}

void MainWindow::addBondPaths() {

    QTreeWidgetItem *root = treeWidget->topLevelItem(0);
    if(!root) {
        msgBox("Open data file first");
        return;
    }

    bool ok;
    QString name =
    QInputDialog::getText(this,
                          QString("New Bonds Set Name"),
                          NULL,
                          QLineEdit::Normal,
                          QString("Bonds_")+QString().setNum(doc->get_bondNetworks()->count() + 1),
                          &ok);
    if(ok)
    {
        BondNetwork bn(name);
        Cslwl* cslwl;

        WFNData * wfn = doc->get_wfn();
        if(wfn == NULL){
            msgBox("Wavefunction is not set");
            return;
        }

        //Find if there are any stored BCPs
        double x,y,z;
        int i;
        int ab[2];
        QString bcp_file_name =
                QFileInfo(doc->get_wfn_file_path()).dir().absolutePath() + "/" +
                QFileInfo(doc->get_wfn_file_path()).completeBaseName() + ".bcp";

        QFile qtFile(bcp_file_name);
        if(qtFile.exists()){
            printf("Reading BCP file: %s\n",bcp_file_name.toStdString().data());
            std::ifstream file(bcp_file_name.toStdString());
            std::string line;

            while( std::getline(file,line))
            {
                if(phrase_parse(line.begin(), line.end(), "" >> int_ >>
                                int_ >>
                                int_ >>
                                double_ >> double_ >> double_ >> "", space, i, ab[0], ab[1], x, y, z)){
                    printf("%d-%d BCP(% .14e, % .14e,% .14e)\n",ab[0],ab[1],x,y,z);

                    for(int i =0; i < 2; i++){
                    cslwl = findBond(wfn,ab[i]-1,x,y,z);
                    if(cslwl != NULL){
                       bn.bondPaths.append(new BondPath(cslwl));
                       doc->get_BCPs()->append(new BCP(cslwl));
                       delete cslwl;
                    }}
                };

            }
        }else{
            printf("Calculating bond path's and BCP's from the first principles\n");

            for(int i = 0; i < wfn->nat; i++){
                for(int j = 0; j < i; j++){
                    cslwl = findBond(wfn, i, j);
                    if(cslwl != NULL){
                        bn.bondPaths.append(new BondPath(cslwl));
                        doc->get_BCPs()->append(new BCP(cslwl));
                        delete cslwl;
                    }
                }
            }
        }

        //save bond network
        doc->get_bondNetworks()->append(bn);
        QTreeWidgetItem * item = new QTreeWidgetItem(root);
        item->setText(0,name);
        item->setText(1,QString("bond_network"));
    }
}

void MainWindow::msgBox(QString msg){
    QMessageBox msgBox(this);
    msgBox.setText(msg);
    msgBox.exec();
}
