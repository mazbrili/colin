#ifndef STRUCTPAINTER_H
#define STRUCTPAINTER_H

#include <QtCore/QObject>
#include <QtGui/QPainter>

#include "wgv_tw.h"
#include "viewportsettings.h"
#include "colinicons.h"


class structPainter
{
public:
    structPainter();
    void drawStruct(const wgv_tw &t, QPainter *painter, QTransform *trans, const Colin::Elements &toDraw_);
    void drawNode(const wgv_node &n, const int &i);
    void drawBearing(const wgv_bearing &b, const QPointF &po, bool refconf = false);
    void drawBeam(const wgv_beam &s, const int &i);
    void drawStLoad(const wgv_load &l);
    void drawStLoad(const wgv_load &l, QLineF *line);
    void drawLoad(const wgv_load &l, const QPointF &po);
    void drawMoment(const wgv_load &l, const QPointF &po);
    void drawTemp(const wgv_load &l, const QLineF &line);
    void drawDoubleLoad(const wgv_load &l, const QLineF &line);

    static void drawThermometer(QPainter *p, const QRect &boundingRect, const double &value, const bool &hasFocus = false, double *max = 0, double *min = 0);

    QTransform *trm;


    static void drawHSpring(QPainter *p, const QPointF &po, const double &d = 0);
    inline static void drawVSpring(QPainter *p, const QPointF &po, const double &d = 0){p->save(); p->translate(po); p->rotate(-90); drawHSpring(p, QPointF(0, 0), d); p->restore();}
    static void drawMSpring(QPainter *p, const QPointF &po);

    inline static void drawHBear(QPainter *p, const QPointF &po){p->save(); p->translate(po); p->rotate(90); drawVBear(p, QPointF(0, 0)); p->restore();}
    inline static void drawVBear(QPainter *p, const QPointF &po){drawHVBear(p, po, true);}

    inline static void drawVMBear(QPainter *p, const QPointF &po){drawHVMBear(p, po, true);}
    inline static void drawHMBear(QPainter *p, const QPointF &po){p->save(); p->translate(po); p->rotate(90); drawVMBear(p, QPointF(0, 0)); p->restore();}

    static void drawMBear(QPainter *p, const QPointF &po);

    static void drawHVMBear(QPainter *p, const QPointF &po, bool space = false);

    static void drawHVBear(QPainter *p, const QPointF &po, bool space = false);

    static void drawGround(QPainter *p, const QLineF &l);

    void ignoreHotSpots(const bool &ignore);
    void ignoreSelection(const bool &ignore);

private:
    QPainter *p;
    viewPortSettings *v;

    double scaleM,
           scaleP,
           scaleU;

    bool drawResults;

    bool ignoreSelection_;
    bool ignoreHotSpots_;
    Colin::Elements toDraw;



};

#endif // STRUCTPAINTER_H
