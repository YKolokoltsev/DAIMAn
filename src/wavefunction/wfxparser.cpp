#include "wfxparser.h"

WFXParser::WFXParser()
{
    file = NULL;
    wfn = NULL;
}

WFXParser::~WFXParser()
{
    if(file != NULL)
    {
        file->close();
        delete file;
    }
}

bool WFXParser::setWFN(WFNData* data)
{
    if(data == NULL) return false;
    wfn = data;
    return true;
}

bool WFXParser::openFile(QString path)
{
    file = new QFile(path);
    if(file == NULL) return false;
    if(!file->open(QIODevice::ReadOnly)) return false;

    return true;
}

bool WFXParser::parse()
{
    if(!file->isOpen()) return false;

    QString line;
    QString tag;
    QString text;
    bool    command = false;

    bool (*process)(QString,WFNData*);

    text.clear();
    tag = "This tag exactly not exist 8-)";

    while(file->pos() < file->size())
    {
        line = file->readLine();
        command = false;

        if(line.contains(QString("<Title>")))
        {
            tag = "</Title>";
            text.clear();
            command = true;
            process = &(WFXParser::pr_title);
        }

        if(line.contains(QString("<Keywords>")))
        {
            tag = "</Keywords>";
            text.clear();
            command = true;
            process = &(WFXParser::pr_keywords);
        }

        if(line.contains(QString("<Number of Nuclei>")))
        {
            tag = "</Number of Nuclei>";
            text.clear();
            command = true;
            process = &(WFXParser::pr_nat);
        }

        if(line.contains(QString("<Number of Primitives>")))
        {
            tag = "</Number of Primitives>";
            text.clear();
            command = true;
            process = &(WFXParser::pr_nprim);
        }

        if(line.contains(QString("<Number of Occupied Molecular Orbitals>")))
        {
            tag = "</Number of Occupied Molecular Orbitals>";
            text.clear();
            command = true;
            process = &(WFXParser::pr_norb);
        }

        if(line.contains(QString("<Number of Perturbations>")))
        {
            tag = "</Number of Perturbations>";
            text.clear();
            command = true;
            process = &(WFXParser::pr_npert);
        }

        if(line.contains(QString("<Nuclear Names>")))
        {
            tag = "</Nuclear Names>";
            text.clear();
            command = true;
            process = &(WFXParser::pr_atnames);
        }

        if(line.contains(QString("<Atomic Numbers>")))
        {
            tag = "</Atomic Numbers>";
            text.clear();
            command = true;
            process = &(WFXParser::pr_atnumbers);
        }

        if(line.contains(QString("<Nuclear Charges>")))
        {
            tag = "</Nuclear Charges>";
            text.clear();
            command = true;
            process = &(WFXParser::pr_atcharges);
        }

        if(line.contains(QString("<Nuclear Cartesian Coordinates>")))
        {
            tag = "</Nuclear Cartesian Coordinates>";
            text.clear();
            command = true;
            process = &(WFXParser::pr_atcoords);
        }

        if(line.contains(QString("<Net Charge>")))
        {
            tag = "</Net Charge>";
            text.clear();
            command = true;
            process = &(WFXParser::pr_netcharge);
        }

        if(line.contains(QString("<Number of Electrons>")))
        {
            tag = "</Number of Electrons>";
            text.clear();
            command = true;
            process = &(WFXParser::pr_nelectrons);
        }

        if(line.contains(QString("<Number of Alpha Electrons>")))
        {
            tag = "</Number of Alpha Electrons>";
            text.clear();
            command = true;
            process = &(WFXParser::pr_nalpha);
        }

        if(line.contains(QString("<Number of Beta Electrons>")))
        {
            tag = "</Number of Beta Electrons>";
            text.clear();
            command = true;
            process = &(WFXParser::pr_nbeta);
        }

        if(line.contains(QString("<Electronic Spin Multiplicity>")))
        {
            tag = "</Electronic Spin Multiplicity>";
            text.clear();
            command = true;
            process = &(WFXParser::pr_multi);
        }

        if(line.contains(QString("<Model>")))
        {
            tag = "</Model>";
            text.clear();
            command = true;
            process = &(WFXParser::pr_model);
        }

        if(line.contains(QString("<Primitive Centers>")))
        {
            tag = "</Primitive Centers>";
            text.clear();
            command = true;
            process = &(WFXParser::pr_icent);
        }

        if(line.contains(QString("<Primitive Types>")))
        {
            tag = "</Primitive Types>";
            text.clear();
            command = true;
            process = &(WFXParser::pr_itype);
        }

        if(line.contains(QString("<Primitive Exponents>")))
        {
            tag = "</Primitive Exponents>";
            text.clear();
            command = true;
            process = &(WFXParser::pr_alpha);
        }

        if(line.contains(QString("<Molecular Orbital Occupation Numbers>")))
        {
            tag = "</Molecular Orbital Occupation Numbers>";
            text.clear();
            command = true;
            process = &(WFXParser::pr_occup);
        }

        if(line.contains(QString("<Molecular Orbital Energies>")))
        {
            tag = "</Molecular Orbital Energies>";
            text.clear();
            command = true;
            process = &(WFXParser::pr_orben);
        }

        if(line.contains(QString("<Molecular Orbital Spin Types>")))
        {
            tag = "</Molecular Orbital Spin Types>";
            text.clear();
            command = true;
            process = &(WFXParser::pr_orspin);
        }


        //********************************
        //not regular call
        if(line.contains(QString("<Molecular Orbital Primitive Coefficients>")))
        {
            tag = "This tag exactly not exist 8-)";
            text.clear();
            command = true;
            if(!allocCoef()) return false;
            if(!parseMOC(file)) return false;
        }
        //********************************

        if(line.contains(QString("<Energy = T + Vne + Vee + Vnn>")))
        {
            tag = "</Energy  = T + Vne + Vee + Vnn>";
            text.clear();
            command = true;
            process = &(WFXParser::pr_etotal);
        }

         if(line.contains(QString("<Virial Ratio (-V/T)>")))
        {
            tag = "</Virial Ratio (-V/T)>";
            text.clear();
            command = true;
            process = &(WFXParser::pr_rvirial);
        }

        if(line.contains(tag))
        {
            if(tag == "</Molecular Orbital Spin Types>")
                text.replace(QRegExp(QString("[\a\f\n\r\t\v]")),QString("\n"));
            else
                text.replace(QRegExp(QString("[\a\f\n\r\t\v]")),QString(" "));

            command = true;
            if(!process(text,wfn)) return false;
            text.clear();
        }

        if(!command) text += line;
    }

    return true;
}

bool WFXParser::pr_title(QString text,WFNData* wfn)
{

    wfn->title = new char[text.size()+1];
    if(wfn->title == NULL) return false;
    memset(wfn->title,'\0',text.size()+1);
    memcpy(wfn->title,text.toStdString().data(),text.size());
    return true;
}

bool WFXParser::pr_keywords(QString text, WFNData* wfn)
{
    wfn->keywords = new char[text.size()+1];
    if(wfn->keywords == NULL) return false;
    memset(wfn->keywords,'\0',text.size()+1);
    memcpy(wfn->keywords,text.toStdString().data(),text.size());
    return true;
}

bool WFXParser::pr_nat(QString text,WFNData* wfn)
{
    bool ok;
    wfn->nat = text.toInt(&ok);
    return ok;
}

bool WFXParser::pr_nprim(QString text,WFNData* wfn)
{
    bool ok;
    wfn->nprim = text.toInt(&ok);
    return ok;
}

bool WFXParser::pr_norb(QString text, WFNData* wfn)
{
    bool ok;
    wfn->norb = text.toInt(&ok);
    return ok;
}

bool WFXParser::pr_npert(QString text, WFNData* wfn)
{
    bool ok;
    wfn->npert = text.toInt(&ok);
    return ok;
}

bool WFXParser::pr_atnames(QString text, WFNData* wfn)
{
    QStringList list = text.split(" ",QString::SkipEmptyParts);
    if(wfn->nat != list.count()) return false;

    wfn->atnames = new char* [wfn->nat];
    if(wfn->atnames == NULL) return false;

    for(int i = 0; i < wfn->nat; i++)
    {
        wfn->atnames[i] = new char[list.at(i).size()+1];
        if(wfn->atnames[i] == NULL)
        {
            i = i -1;
            while(i != 0)
                delete [] wfn->atnames[i];
            delete [] wfn->atnames;
            wfn->atnames = NULL;
            return false;
        }
        memset(wfn->atnames[i],'\0',list.at(i).size()+1);
        memcpy(wfn->atnames[i],list.at(i).toStdString().data(),list.at(i).size());
    }

    return true;
}

bool WFXParser::pr_atnumbers(QString text, WFNData* wfn)
{
    QStringList list = text.split(" ",QString::SkipEmptyParts);
    if(wfn->nat != list.count()) return false;

    wfn->atnumbers = new int[wfn->nat];
    if(wfn->atnumbers == NULL) return false;

    bool ok;

    for(int i = 0; i < wfn->nat; i++)
    {
        wfn->atnumbers[i] = list.at(i).toInt(&ok);
        if(!ok)
        {
            delete [] wfn->atnumbers;
            wfn->atnumbers = NULL;
            return false;
        }
    }

    return true;
}

bool WFXParser::pr_atcharges(QString text, WFNData* wfn)
{
    QStringList list = text.split(" ",QString::SkipEmptyParts);
    if(wfn->nat != list.count()) return false;

    wfn->atcharges = new double[wfn->nat];
    if(wfn->atcharges == NULL) return false;

    bool ok;

    for(int i = 0; i < wfn->nat; i++)
    {
        wfn->atcharges[i] = list.at(i).toDouble(&ok);
        if(!ok)
        {
            delete [] wfn->atcharges;
            wfn->atcharges = NULL;
            return false;
        }
    }
    return true;
}

bool WFXParser::pr_atcoords(QString text, WFNData *wfn)
{
    QStringList list = text.split(" ",QString::SkipEmptyParts);
    if(3*wfn->nat != list.count()) return false;

    wfn->atcoords = new double* [wfn->nat];
    if(wfn->atcoords == NULL) return false;

    bool ok;
    double coord = 0;

    for(int i = 0; i < wfn->nat; i++)
    {
        wfn->atcoords[i] = new double[3];

        coord = list.at(3*i).toDouble(&ok);
        if(ok && wfn->atcoords[i] != NULL)
            wfn->atcoords[i][0] = coord;

        coord = list.at(3*i+1).toDouble(&ok);
        if(ok && wfn->atcoords[i] != NULL)
            wfn->atcoords[i][1] = coord;

        coord = list.at(3*i+2).toDouble(&ok);
        if(ok && wfn->atcoords[i] != NULL)
            wfn->atcoords[i][2] = coord;

        if(wfn->atcoords[i] == NULL || !ok)
        {
            i = i-1;
            while(i >= 0)
                delete [] wfn->atcoords[i], i = i-1;
            delete [] wfn->atcoords;
            wfn->atcoords = NULL;
            return false;
        }
    }

    return true;
}

bool WFXParser::pr_netcharge(QString text, WFNData *wfn)
{
    bool ok;
    wfn->netcharge = text.toDouble(&ok);
    return ok;
}

bool WFXParser::pr_nelectrons(QString text, WFNData* wfn)
{
    bool ok;
    wfn->nelectrons = text.toInt(&ok);
    return ok;
}

bool WFXParser::pr_nalpha(QString text, WFNData* wfn)
{
    bool ok;
    wfn->nalpha = text.toInt(&ok);
    return ok;
}

bool WFXParser::pr_nbeta(QString text, WFNData* wfn)
{
    bool ok;
    wfn->nbeta = text.toInt(&ok);
    return ok;
}

bool WFXParser::pr_multi(QString text, WFNData* wfn)
{
    bool ok;
    wfn->multi = text.toInt(&ok);
    return ok;
}

bool WFXParser::pr_model(QString text, WFNData* wfn)
{
    wfn->model = new char[text.size() + 1];
    if(wfn->model == NULL) return false;

    memset(wfn->model,'\0',text.size()+1);
    memcpy(wfn->model,text.toStdString().data(),text.size());
    return true;
}

bool WFXParser::pr_icent(QString text, WFNData* wfn)
{
    QStringList list = text.split(" ",QString::SkipEmptyParts);
    if(wfn->nprim != list.count()) return false;

    wfn->icent = new int [wfn->nprim];
    if(wfn->icent == NULL) return false;

    bool ok = true;
    for(int i = 0; i < wfn->nprim; i++)
    {
        wfn->icent[i] = list.at(i).toInt(&ok) - 1; //We will start numeration from 0
        if(!ok)
        {
            delete [] wfn->icent;
            wfn->icent = NULL;
            return false;
        }
    }
    return true;
}

bool WFXParser::pr_itype(QString text, WFNData* wfn)
{
    QStringList list = text.split(" ",QString::SkipEmptyParts);
    if(wfn->nprim != list.count()) return false;

    wfn->itype = new int [wfn->nprim];
    if(wfn->itype == NULL) return false;

    bool ok = true;
    for(int i = 0; i < wfn->nprim; i++)
    {
        wfn->itype[i] = list.at(i).toInt(&ok);
        if(!ok)
        {
            delete [] wfn->itype;
            wfn->itype = NULL;
            return false;
        }
    }
    return true;
}

bool WFXParser::pr_alpha(QString text, WFNData* wfn)
{
    QStringList list = text.split(" ",QString::SkipEmptyParts);
    if(wfn->nprim != list.count()) return false;

    wfn->alpha = new double [wfn->nprim];
    if(wfn->alpha == NULL) return false;

    bool ok = true;
    for(int i = 0; i < wfn->nprim; i++)
    {
        wfn->alpha[i] = list.at(i).toDouble(&ok);
        if(!ok)
        {
            delete [] wfn->alpha;
            wfn->alpha = NULL;
            return false;
        }
    }
    return true;
}

bool WFXParser::pr_occup(QString text, WFNData* wfn)
{
    QStringList list = text.split(" ",QString::SkipEmptyParts);
    if(wfn->norb != list.count()) return false;

    wfn->occup = new double [wfn->norb];
    if(wfn->occup == NULL) return false;

    bool ok = true;
    for(int i = 0; i < wfn->norb; i++)
    {
        wfn->occup[i] = list.at(i).toDouble(&ok);
        if(!ok)
        {
            delete [] wfn->occup;
            wfn->occup = NULL;
            return false;
        }
    }
    return true;
}

bool WFXParser::pr_orben(QString text, WFNData* wfn)
{
    QStringList list = text.split(" ",QString::SkipEmptyParts);
    if(wfn->norb != list.count()) return false;

    wfn->orben = new double [wfn->norb];
    if(wfn->orben == NULL) return false;

    bool ok = true;
    for(int i = 0; i < wfn->norb; i++)
    {
        wfn->orben[i] = list.at(i).toDouble(&ok);
        if(!ok)
        {
            delete [] wfn->orben;
            wfn->orben = NULL;
            return false;
        }
    }
    return true;
}

bool WFXParser::pr_orspin(QString text, WFNData* wfn)
{
    QStringList list = text.split("\n",QString::SkipEmptyParts);
    if(wfn->norb != list.count()) return false;

    wfn->orspin = new int[wfn->norb];
    if(wfn->orspin == NULL) return false;

    for(int i = 0; i < wfn->norb; i++)
    {
        wfn->orspin[i] = 0;
        if(list.at(i).contains(QString("Alpha")))
            wfn->orspin[i] += 1;
        if(list.at(i).contains(QString("Beta")))
            wfn->orspin[i] += 2;

        if(wfn->orspin[i] == 0)
        {
            delete [] wfn->orspin;
            wfn->orspin = NULL;
            return false;
        }
    }
    return true;
}

bool WFXParser::allocCoef()
{
    wfn->c = new double* [wfn->norb];
    if(wfn->c == NULL) return false;

    for(int i = 0; i < wfn->norb; i++)
    {
        wfn->c[i] = new double[wfn->nprim];
        if(wfn->c[i] == NULL)
        {
            i = i-1;
            while(i >= 0)
                delete[] wfn->c[i], i = i -1;
            delete [] wfn->c;
            return false;
        }
    }

    return true;
}

bool WFXParser::parseMOC(QFile* file)
{
    QString line;
    QString text;
    bool command = false;
    int     nMO = 0;

    text.clear();

    while(file->pos() < file->size())
    {
        line = file->readLine();
        command = false;

        if(line.contains(QString("<MO Number>")))
        {
            command = true;
            if(nMO != 0)
               if(!readMOC(text,nMO)) return false;
            text.clear();
        }

        if(line.contains(QString("</MO Number>")))
        {
            command = true;
            if(!readnMO(text,&nMO)) return false;
            text.clear();
        }

        if(line.contains(QString("</Molecular Orbital Primitive Coefficients>")))
            return readMOC(text,nMO);

        if(!command) text += line;
    }

    return false;
}

bool WFXParser::readnMO(QString text, int* nMO)
{
    bool ok;
    *nMO = text.toInt(&ok);
    return ok;
}

bool WFXParser::readMOC(QString text, int nMO)
{
    if(nMO > wfn->norb || nMO <= 0) return false;
    text.replace(QRegExp(QString("[\a\f\n\r\t\v]")),QString(" "));

    QStringList list = text.split(" ",QString::SkipEmptyParts);

    if(wfn->nprim != list.count()) return false;

    bool ok;
    for(int i = 0; i < wfn->nprim; i++)
    {
        wfn->c[nMO-1][i] = list.at(i).toDouble(&ok);
        if(!ok) return false;
    }

    return true;
}

bool WFXParser::pr_etotal(QString text, WFNData* wfn)
{
    bool ok;
    wfn->etotal = text.toDouble(&ok);
    return ok;
}


bool WFXParser::pr_rvirial(QString text, WFNData* wfn)
{
    bool ok;
    wfn->rvirial = text.toDouble(&ok);
    return ok;
}
