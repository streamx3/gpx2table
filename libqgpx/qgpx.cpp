#include "qgpx.h"

#include <QDebug> // TODO removeme

#include <QFile>
#include <iostream>
using namespace std;

// From OSMtracker
QStringList QGPX::known_CDATAs = { "Photo", "Voice recording", "School",
    "Police", "Fire station", "Bank", "Playground", "Pub", "Hotel", "Motel",
    "Hostel", "Restaurant", "Fast food", "Camp site", "Bus stop", "Railway",
    "Telephone", "Post box", "ATM", "Bollard", "Toilets", "Shelter",
    "Surveillance", "Max 30", "Max 50", "Max 80", "Max 100", "Max 110",
    "Max 120", "No exit", "Traffic light", "One way", "Fuel station", "Parking",
    "Emergency Phone", "Turning circle", "Speed camera", "View point",
    "Information", "Picnic site", "Attraction", "Theme park", "Castle",
    "Monument", "Museum", "Cinema", "Bench", "Water", "Pharmacy", "Shop",
    "Marina", "Sport", "Taxi", "Hospital, Doctors", "Recycling",
    "Place of worship", "Post office", "Library", "Bridge", "Zebra crossing",
    "Motorway", "Trunk", "Primary", "Secondary", "Tertiary", "Residential",
    "Service", "Track", "Cycleway", "Footway", "Bridleway", "Pedestrian",
    "Grade 1", "Grade 2", "Grade 3", "Grade 4", "Grade 5", "Farm", "Landfill",
    "Basin", "Reservoir", "Forest", "Allotments", "Cemetery",
    "Recreation ground"};

QGPX::QGPX(){}

void QGPX::__element2wpt(QDomElement &e){
    QGPXwpt tmpwpt;
    QDomNamedNodeMap atmap = e.attributes();
    for(auto i = 0; i < atmap.count(); ++i){
        QDomNode at = atmap.item(i);
        if(at.nodeName() == "lat"){
            tmpwpt.coordinate.setLatitude(at.nodeValue().toDouble());
        }else if (at.nodeName() == "lon"){
            tmpwpt.coordinate.setLongitude(at.nodeValue().toDouble());
        }
    }

    QDomNode n = e.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement();
        if(e.tagName() == "ele"){
            tmpwpt.coordinate.setAltitude(e.text().toDouble());
        }else if(e.tagName() == "name"){
            tmpwpt.CDATA = e.text().toStdString().c_str();
        }else if(e.tagName() == "time"){
            tmpwpt.time = QDateTime::fromString(
                        e.text().toStdString().c_str(),
                        Qt::DateFormat::ISODate);
        }else if(e.tagName() == "sat"){
            tmpwpt.sat = e.text().toStdString().c_str();
        }else if(e.tagName() == "link"){
            tmpwpt.link = e.text().toStdString().c_str();
        }
        n = n.nextSibling();
    }
    if(!QGPX::isKnownCDATA(tmpwpt.CDATA)){
        tmpwpt.link = tmpwpt.CDATA;
        tmpwpt.CDATA = "Text";
    }
    this->waypoints.append(tmpwpt);
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
            if(e.tagName() == "wpt"){
                __element2wpt(e);
            }
            if(e.tagName() ==  "trk"){
                __element2trk(e);
            }
        }
        n = n.nextSibling();
    }
}


bool QGPX::empty(){
    if(!waypoints.empty())
        return false;

    if(!tracks.empty()){
        foreach (auto &track, tracks) {
            if(!track.segments.empty()){
                foreach (auto &seg, track.segments) {
                    if(!seg.empty())
                        return false;
                }
            }
        }
    }

    return true;
}

void QGPX::dump_wpts(QString &s){
    size_t i = 0;

    foreach (auto &wpt, waypoints) {
        s.append(QString::number(i++));
        s.append(":{\n");
        s.append("\tlat = " + QString::number(wpt.coordinate.latitude())
                 + "\n");
        s.append("\tlon = " + QString::number(wpt.coordinate.longitude())
                 + "\n");
        s.append("\tele = " + QString::number(wpt.coordinate.altitude())
                 + "\n");
        s.append("\ttime = " + wpt.time.toString() + "\n");
        s.append("\tCDATA = " + wpt.CDATA + "\n");
        s.append("\tlink = " + wpt.link + "\n");
        s.append("\tsat = " + wpt.sat + "\n");
        s.append("}\n");
    }
}

QVector<QGPXwpt> QGPX::getWaypoints(){
    return waypoints;
}

bool QGPX::isKnownCDATA(QString str){
    if(known_CDATAs.contains(str));
}
