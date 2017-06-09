
#include <QTextStream>
#include "parameters.h"
#include <iostream>

const QString conf_file("/home/morrigan/.wfnanalysis/config.xml");

Parameters::Parameters()
{
    defaults();

    QFile file(conf_file);

    if(!file.open(QIODevice::ReadOnly)){
        std::cerr << conf_file.toStdString() << " - not found\n\n";
        return;
    }
    doc.setContent((QIODevice *) &file, true, NULL, NULL, NULL);
    file.close();

    root = doc.documentElement();
    if(root.tagName() != "wfnan_config") return;

    opened = true;

    read_recent();
}

Parameters::~Parameters()
{
   QDomDocument new_doc("config");
   QDomElement  new_root = new_doc.createElement("wfnan_config");
   int          i = 0;

   new_doc.appendChild(new_root);

   new_root.appendChild(new_doc.createElement("recent_files"));
   new_root.elementsByTagName("recent_files").at(0).
           toElement().setAttribute("max", (int) max_recent);

   for(i = 0; i < str_recent.count(); i++)
   {
       new_root.elementsByTagName("recent_files").at(0).
               appendChild(new_doc.createElement("rpath"));

       new_root.elementsByTagName("rpath").
               at(new_root.elementsByTagName("rpath").count()-1).
               toElement().setAttribute("path",str_recent.at(i));
   }

  QFile file(conf_file);
  file.open(QIODevice::WriteOnly);

  QTextStream ts(&file);
  ts << new_doc.toString(5);

  file.close();

}

void Parameters::defaults()
{
    opened = false;
    file = NULL;
    max_recent = 5;
}

void Parameters::read_recent()
{
    if(!opened) return;

    QDomNode node = root.firstChild();
    QDomAttr     a_max;
    QDomNodeList l_paths;
    bool         ok = false;

    while (!node.isNull())
    {
        if (node.toElement().tagName() == "recent_files")
        {
            if(node.toElement().hasAttribute("max"))
                a_max = node.toElement().attributeNode("max");

            if(node.toElement().hasChildNodes())
                l_paths = node.childNodes();

            break;
        }
        node = node.nextSibling();
    }

    max_recent = a_max.value().toInt(&ok);

    if(max_recent < 1 || max_recent > 10 || !ok)
    {
        ok = false;
        max_recent = 5;
    }


    if(l_paths.isEmpty()) return;

    for(int i = 0; i < l_paths.count() && i < max_recent; i++)
        if(l_paths.item(i).toElement().hasAttribute("path"))
            if(QFile(l_paths.item(i).toElement().attributeNode("path").value()).exists())
                str_recent.append(l_paths.item(i).toElement().attributeNode("path").value());
}

void Parameters::setRecent(QString path)
{   
    for(int i = 0; i < str_recent.count(); i++)
        if(path == str_recent.at(i))
            str_recent.removeAt(i);

    str_recent.insert(0,path);

    while(str_recent.count() > max_recent)
        str_recent.removeLast();
}
