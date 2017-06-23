#include <iostream>

#include <QTextStream>
#include <QStandardPaths>
#include <QFile>
#include <QFileInfo>

#include "gui_config.h"

using namespace std;

/*
 * Search for configuration file ~/.daiman.xml, and create an empty one
 * if not exist. Prepare QDomDocument on the base of this file.
 */
GuiConfig::GuiConfig() :
  max_recent{5},
  gui_conf_fname{QStandardPaths::locate( QStandardPaths::HomeLocation,
                                         QString(),
                                         QStandardPaths::LocateDirectory) + ".daiman.xml"}
{
    QFile gui_conf_file(gui_conf_fname);

    if(!gui_conf_file.exists()){
        //prepare dummy doc
        auto root = doc.createElement("daiman_gui_conf");
        doc.appendChild(root);
        root.appendChild(doc.createElement("recent_files"));

        //todo: make file hidden under all OS
        //write dummy doc
        gui_conf_file.open(QIODevice::WriteOnly);
        QTextStream ts(&gui_conf_file);
        ts << doc.toString(5); //indent = 5
    }else{
        gui_conf_file.open(QIODevice::ReadOnly);
        doc.setContent(&gui_conf_file, true, NULL, NULL, NULL);

        auto root = doc.documentElement();
        if(root.tagName() != "daiman_gui_conf"){
            gui_conf_file.close();
            throw runtime_error("~/.daiman.xml incorrect, delete it and restart application");
        }
    }

    gui_conf_file.close();
    read_recent();
}

GuiConfig::~GuiConfig()
{
    QFile file(gui_conf_fname);
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);

    QTextStream ts(&file);
    ts << doc.toString(5);

    file.close();
}

QDomNode GuiConfig::recents_node(){
    auto recent_files = doc.documentElement().firstChildElement("recent_files");
    if(recent_files.isNull()) throw runtime_error("~/.daiman.xml DOM has no <recent_files>");
    return recent_files;
}

QStringList GuiConfig::read_recent()
{
    QStringList retVal;
    auto recent_files = recents_node();

    for(QDomNode n = recent_files.firstChild(); !n.isNull(); n = n.nextSibling()){
        auto el = n.toElement();
        if(el.tagName() == "recent"){
            retVal.append(el.attributeNode("path").value());
        }
    }

    return retVal;
}

void GuiConfig::add_recent(QString path)
{
    auto recent_files = recents_node();

    //if path alredy exist, delete it to add at the beginning
    for(QDomNode n = recent_files.firstChild(); !n.isNull(); n = n.nextSibling()){
        auto el = n.toElement();
        if(el.tagName() == "recent" && el.attributeNode("path").value() == path)
            recent_files.removeChild(n);
    }

    //add recent at the beginning
    auto new_node = doc.createElement("recent");
    new_node.setAttribute("path",path);
    recent_files.insertBefore(new_node,recent_files.firstChild());

    //while num of childes > max_recent, remove least one
    while(recent_files.childNodes().size() > max_recent)
        recent_files.removeChild(recent_files.lastChild());
}

QString GuiConfig::recent_dir(){
    auto recent_files = recents_node();

    //if no info, the recent dir is the user's home
    if(recent_files.firstChild().isNull())
        return QStandardPaths::locate( QStandardPaths::HomeLocation,
                                       QString(),
                                       QStandardPaths::LocateDirectory);

    //if there are records, the recent dir is the dir of the first recent file
    auto fpath = recent_files.firstChild().toElement().attributeNode("path").value();
    return QFileInfo(fpath).absolutePath();
}
