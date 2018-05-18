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
        switch(token_t){
        case QXmlStreamReader::TokenType::StartDocument:
            errmsgs->append("X StartDoc\n");
            header.xml_encodong = *inputStream.documentEncoding().string();
            errmsgs->append("enc = " + header.xml_encodong + "\n");
            break;
        case QXmlStreamReader::TokenType::EndDocument:
            errmsgs->append("X EndDoc\n");
            break;
        case QXmlStreamReader::TokenType::StartElement:
            errmsgs->append("X StartElement\n");
            foreach (const QXmlStreamAttribute &attr, inputStream.attributes()) {
                errmsgs->append(attr.name().toString() + " "
                                + attr.value().toString());
            }
            break;
        case QXmlStreamReader::TokenType::EndElement:
            errmsgs->append("X EndElement\n");
            break;
        case QXmlStreamReader::TokenType::Characters:
            errmsgs->append("X Characters\n");
            if(inputStream.isCDATA()){
                errmsgs->append(inputStream.text().toString());
            }
            break;
        case QXmlStreamReader::TokenType::Comment:
            errmsgs->append("X Comment\n");
            break;
        case QXmlStreamReader::TokenType::DTD:
            errmsgs->append("X DTD\n");
            break;
        case QXmlStreamReader::TokenType::EntityReference:
            errmsgs->append("X EntityReference\n");
            break;
        case QXmlStreamReader::TokenType::ProcessingInstruction:
            errmsgs->append("X ProcessingInstruction\n");
            break;
        case QXmlStreamReader::TokenType::Invalid:
            errmsgs->append("X Invalid\n");
            break;
        case QXmlStreamReader::TokenType::NoToken:
            errmsgs->append("X NoToken\n");
            break;

        default:
            errmsgs->append("X default\n");

        }

//        QString name = *(inputStream.name().string());
//        qDebug() << name;
    }
}


bool QGPX::empty(){
    return this->is_empty;
}
