#ifndef XMLWRITER_H
#define XMLWRITER_H

#include <QtCore/QXmlStreamWriter>
#include "wgv_tw.h"

class XmlWriter : public QXmlStreamWriter
{
public:
    XmlWriter(QIODevice *device);
    XmlWriter(QByteArray *array);
    void writeTw(const wgv_tw &tw);
    void writeSelection(const wgv_tw &tw, QPointF translation = QPointF());
    void writeLoad(const wgv_tw &tw, const int &nr);
private:
    void writeRequredLibEntries(const wgv_tw &tw, bool onlySelection = false);
    void writeNode(const wgv_node &n);
    void writeBeam(const wgv_beam &s);
    void writeLoad(const wgv_load &l);
    void writeBearing(const wgv_bearing &b);
    void writeMat(const wgv_mat &m);
    void writeProfile(const wgv_profile &p);
};

#endif // XMLWRITER_H
