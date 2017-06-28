#ifndef WFXPARSER_H
#define WFXPARSER_H

#include <QString>
#include <QFile>
#include <QStringList>
#include "wfndata.h"


class WFXParser
{
    public:
    WFXParser();
    ~WFXParser();

    public:
    bool openFile(QString path);
    bool setWFN(WFNData* data);
    bool parse();

    private:
    static bool pr_title(QString text, WFNData* wfn);
    static bool pr_keywords(QString text, WFNData* wfn);
    static bool pr_nat(QString text, WFNData* wfn);
    static bool pr_nprim(QString text, WFNData* wfn);
    static bool pr_norb(QString text, WFNData* wfn);
    static bool pr_npert(QString text, WFNData* wfn);
    static bool pr_atnames(QString text, WFNData* wfn);
    static bool pr_atnumbers(QString text, WFNData* wfn);
    static bool pr_atcharges(QString text, WFNData* wfn);
    static bool pr_atcoords(QString text, WFNData* wfn);
    static bool pr_netcharge(QString text, WFNData* wfn);
    static bool pr_nelectrons(QString text, WFNData* wfn);
    static bool pr_nalpha(QString text, WFNData* wfn);
    static bool pr_nbeta(QString text, WFNData* wfn);
    static bool pr_multi(QString text, WFNData* wfn);
    static bool pr_model(QString text, WFNData* wfn);
    static bool pr_icent(QString text, WFNData* wfn);
    static bool pr_itype(QString text, WFNData* wfn);
    static bool pr_alpha(QString text, WFNData* wfn);
    static bool pr_occup(QString text, WFNData* wfn);
    static bool pr_orben(QString text, WFNData* wfn);
    static bool pr_orspin(QString text, WFNData* wfn);
    static bool pr_etotal(QString text, WFNData* wfn);
    static bool pr_rvirial(QString text, WFNData* wfn);

    bool allocCoef();
    bool parseMOC(QFile* file);
    bool readnMO(QString text, int* nMO);
    bool readMOC(QString text, int nMO);


    public:
    QFile* file;
    WFNData* wfn;

    private:

};

#endif // WFXPARSER_H
