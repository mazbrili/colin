#ifndef STRUCTPRINTER_H
#define STRUCTPRINTER_H

#include <QtGui/QPrinter>
#include "structpainter.h"
#include "viewportsettings.h"
#include "polynom.h"

class structPrinter: public QPrinter
{
    Q_DECLARE_TR_FUNCTIONS(structPrinter)
public:
    structPrinter(wgv_tw *t);
    void print();

private:
    wgv_tw *tw;
    int pageCount;
    int dx;

    void addHeader(QPainter *p);
    void addPage(QPainter *p);

    void writeMats(QPainter *p);
    void writeNodes(QPainter *p);
    void writeBeams(QPainter *p);
    void writeLoads(QPainter *p);
    void writeResultsNodes(QPainter *p);
    void writeResultsBeams(QPainter *p);
    void usedSpace(int h, QPainter *p);

};

#endif // STRUCTPRINTER_H
