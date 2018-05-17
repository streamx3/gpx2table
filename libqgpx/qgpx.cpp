#include "qgpx.h"

#include <QDebug> // TODO removeme

#include <QFile>
#include <QXmlStreamReader>

QGPX::QGPX(){
    is_empty = true;
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
    QXmlStreamReader inputStream(&tmpfile);
    QXmlStreamReader::TokenType token_t;
    while(!inputStream.atEnd() && !inputStream.hasError()){
        token_t = inputStream.readNext();
        qDebug() << inputStream.name().string();
    }
}


bool QGPX::empty(){
    return this->is_empty;
}
