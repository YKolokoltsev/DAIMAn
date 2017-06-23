#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <boost/serialization/singleton.hpp>

#include <QDomDocument>
#include <QDomNodeList>

using namespace std;

/*
 * in a constructor we search for configuration file at <usr_home>/.daiman.xml
 * and if it is not found we create it and fill the minimal 'doc' structure,
 * if the file exist, we read the doc structure from it.
 *
 * at runtime all data (and probable changes) are saved within a 'doc',  and when
 * the destructor is called, we store the most recent 'doc' in the configuration file.
 *
 * ... for the moment this class stores only recently opened files, however can
 * be extended.
 */
class GuiConfig
{   
public:
    ~GuiConfig();

    QStringList read_recent();
    void add_recent(QString path);
    QString recent_dir();
    static GuiConfig& inst() {
        return boost::serialization::singleton<GuiConfig>::get_mutable_instance();
    }

protected:
    GuiConfig();

private:
    QDomNode recents_node();

private:
    QDomDocument doc;
    const QString gui_conf_fname;

public:
    const int max_recent;
};

#endif // PARAMETERS_H
