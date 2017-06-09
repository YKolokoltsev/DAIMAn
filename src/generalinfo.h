#ifndef GENERALINFO_H
#define GENERALINFO_H

#include <QTextBrowser>
#include "gui/mainwindow.h"
#include <QString>

class GeneralInfo : public QTextBrowser
{

    Q_OBJECT

public:
    GeneralInfo(MainWindow* parent);

    void CalculateInfo();
    void LoadInfo();
    QString OrbitalsTable();

private:
    MainWindow* papa;
};

#endif // GENERALINFO_H
