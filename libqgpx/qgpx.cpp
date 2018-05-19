#include "qgpx.h"

#include <QDebug> // TODO removeme

#include <QFile>
#include <iostream>
using namespace std;

QGPX::QGPX(){
    is_empty = true;
}

void QGPX::__element2wpt(QDomElement &e){
    QDomNamedNodeMap atmap = e.attributes();
    for(auto i = 0; i < atmap.count(); ++i){
        QDomNode at = atmap.item(i);
        qDebug() << at.nodeName() << "=" << at.nodeValue();
    }

    QDomNode n = e.firstChild();
}

void QGPX::__element2trk(QDomElement &e){

}

void QGPX::__element2trkpt(QDomElement &e){

}

QGPX::QGPX(QString filename, QString *errmsgs){
    QFileDevice::Permissions tmp_perms;

    QGPX* rv = new QGPX();
    if(nullptr == rv){
        if(nullptr != errmsgs){
            errmsgs->append("Allocation of GPX object failed\n");
        }
        return;
    }
    if(!QFile::exists(filename)){
        if(nullptr != errmsgs){
            errmsgs->append("ERROR: Not found file \"" +
                            filename + "\"\n");
        }
        return;
    }
    tmp_perms = QFile::permissions(filename);
    if(!(tmp_perms & QFileDevice::ReadUser)){
        if(nullptr != errmsgs){
            errmsgs->append("ERROR: Not readable file \"" +
                            filename + "\"\n");
        }
        return;
    }
    QFile tmpfile(filename);
    if(!tmpfile.open(QIODevice::ReadOnly | QIODevice::Text)){
        if(nullptr != errmsgs){
            errmsgs->append("ERROR: opening \"" +
                            filename + "\"failed:\n" +
                            tmpfile.errorString() + "\n");
        }
        return;
    }

    QDomDocument doc("gpx");
    if(!doc.setContent(&tmpfile)){
        tmpfile.close();
        errmsgs->append("Failed to set content file for DOM document\n");
        return;
    }
    tmpfile.close();

    QDomElement docElem = doc.documentElement();
    QDomNode n = docElem.firstChild();
    while(!n.isNull()){
        QDomElement e = n.toElement();
        if(!e.isNull()){
            cout << qPrintable(e.tagName()) << endl;
            if(e.tagName() == "wpt"){
                __element2wpt(e);
            }
        }
        n = n.nextSibling();
    }
}


bool QGPX::empty(){
    return this->is_empty;
}
