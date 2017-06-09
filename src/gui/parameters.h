#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <QIODevice>
#include <QXmlStreamReader>
#include <QDomDocument>
#include <QFile>

class Parameters
{   
    public:
    Parameters();
    ~Parameters();

    public:
    void defaults();
    void read_recent();
    void setRecent(QString path);

    private:
    QDomDocument doc;
    QDomElement root;

    private:
    //An XML file with parameters
    QFile      *file;

    //remarks if we work with a file
    bool     opened;

    public:
    //recent files
    QStringList str_recent;
    int         max_recent;
};

#endif // PARAMETERS_H
