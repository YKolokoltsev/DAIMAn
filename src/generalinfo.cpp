#include "generalinfo.h"
#include <QUrl>
#include <QFile>
#include "stdio.h"
#include "functions.h"

GeneralInfo::GeneralInfo(MainWindow* parent)
{
    papa = parent;
}

void GeneralInfo::CalculateInfo()
{
    ;
}

void GeneralInfo::LoadInfo()
{
    QFile file(QString("/home/data/unamwork/Programming/Analysis/Html/GeneralInfo.html"));
    if(!file.open(QIODevice::ReadOnly)) return;
    QString html = file.readAll();
    file.close();

    char str[100];

    //id0001 = Number of atoms
    memset(str,'\0',100*sizeof(char));
    sprintf(str,"%3i",papa->doc->get_wfn()->nat);
    html.replace(QRegExp(QString("id0001")),QString(str));

    //id0002 = Total number of electrons
    memset(str,'\0',100*sizeof(char));
    sprintf(str,"%3i",(int)calcTotElectrons(papa->doc));
    html.replace(QRegExp(QString("id0002")),QString(str));

    //id0003 = Number of occupied orbitals
    memset(str,'\0',100*sizeof(char));
    sprintf(str,"%3i",papa->doc->get_wfn()->norb);
    html.replace(QRegExp(QString("id0003")),QString(str));

    //id0004 = Number of primitives (exps)
    memset(str,'\0',100*sizeof(char));
    sprintf(str,"%3i",papa->doc->get_wfn()->nprim);
    html.replace(QRegExp(QString("id0004")),QString(str));

    //id0005 = Total molecular energy
    memset(str,'\0',100*sizeof(char));
    sprintf(str,"%3E",papa->doc->get_wfn()->etotal);
    html.replace(QRegExp(QString("id0005")),QString(str));

    //id0006 = Table with orbitals
    html.replace(QRegExp(QString("id0006")),OrbitalsTable());

    this->setHtml(html);
}

QString GeneralInfo::OrbitalsTable()
{
    if(!papa) return QString();
    if(!papa->doc) return QString();
    WFNData * wfn = papa->doc->get_wfn();
    if(!wfn) return QString();

    QString ret;
    QString orbital;
    char num[20];
    for(int i = 0; i < wfn->norb; i++)
    {
        orbital.clear();
        orbital += "<tr>\n";

        orbital += "<td>";
        memset(num,'\0',20*sizeof(char));
        sprintf(num,"%3i",i+1);
        orbital += num;
        orbital += "</td\n>";

        orbital += "<td>";
        memset(num,'\0',20*sizeof(char));
        sprintf(num,"%3.3E",wfn->normorb(i));
        orbital += num;
        orbital += "</td\n>";

        orbital += "<td>";
        memset(num,'\0',20*sizeof(char));
        sprintf(num,"%3.3E",wfn->orben[i]);
        orbital += num;
        orbital += "</td\n>";

        orbital += "</tr>\n";
        ret += orbital;
    }
    return ret;
}
