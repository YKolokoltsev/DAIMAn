#include <QGuiApplication>
#include <QApplication>
#include <QScreen>
#include "gui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QRect scr = QGuiApplication::primaryScreen()->availableGeometry();

    scr.setRect(scr.width()/10,scr.height()/10,scr.width()*4/5,scr.height()*4/5);
    MainWindow w;
    w.setGeometry(scr);
    w.show();

    return app.exec();
}
