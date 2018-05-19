#ifndef QGPX_H
#define QGPX_H

#include "libqgpx_global.h"

#include <QTime>
#include <QList>
#include <QString>
#include <QDomDocument>
#include <QGeoCoordinate>

struct QGPXwpt{
    QGeoCoordinate  coordinate;
    QTime           timestamp;
    QString         CDATA;
    QString         mediaName;
    quint8          sat; // TODO research
};

struct QGPXtrkpt{
    QGeoCoordinate  coordinate;
    float           speed;
    // TODO check what else could be added here, eg direction
};

struct QGPXtrk{
    /** Some explanation here:
     * Each track consists of track serments,
     * each segment consists of trackpoints
     */
    QList<QList<QGPXtrkpt>> trkpt;
    QString CDATA; /// Just a text string
};

class LIBQGPXSHARED_EXPORT QGPX{
public:
    struct Header{
        QString xml_encodong;
        QString xmlns;
        QString gpx_version;
        QString creator;
        QString xmlns_xsi;
        QString xsi_schemaLocation;
    };
public:
    QGPX();
    QGPX(QString filename, QString *errmsgs = nullptr);
    bool empty();

private:
    void __element2wpt(QDomElement &e);
    void __element2trk(QDomElement &e);
    void __element2trkpt(QDomElement &e);
private:
    bool            is_empty;
    QString         encoding;
    Header          header;
    QGPXtrk         track;
    QList<QGPXwpt>  waypoints;
};


#endif // QGPX_H
