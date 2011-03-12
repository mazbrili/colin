#ifndef XMLREADER_H
#define XMLREADER_H

#include <QtCore/QXmlStreamReader>
#include "wgv_tw.h"


class XmlReader : public QXmlStreamReader
{
    friend class wgv_tw;
public:
    XmlReader(wgv_tw *t);
    bool read(QIODevice *device);
    bool read(const QByteArray &array);
    void readTw();
    void readNode();
    void readBeam();
    void readLoad();
    void readMat();
    void readProfile();
    wgv_bearing readBearing();
    QString errors() const;

private:
    wgv_tw *tw;

    QMap<QString, int> materialMap;
    QMap<QString, int> profileMap;
};

#endif // XMLREADER_H
