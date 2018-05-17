#ifndef QGPX_H
#define QGPX_H

#include "libqgpx_global.h"

#include <QTime>
#include <QList>
#include <QString>
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
        QString xmlns;
        QString version;
        QString creator;
        QString xmlns_xsi;
        QString xsi_schemaLocation;
    };
public:
    QGPX();
    QGPX(QString filename, QString *errmsgs = nullptr);

    bool empty();
private:
    bool            is_empty;
    QString         encoding;
    Header          header;
    QGPXtrk         track;
    QList<QGPXwpt>  waypoints;
};


#endif // QGPX_H
