/***********************************************************
 * $Id$
 *
 * Colin
 *
 * Copyright (C) 2011 Matthias Rauter (matthias.rauter@student.uibk.ac.at)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details."
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see
 *
 * http://www.gnu.org/licenses/.
 *
 * Author: Matthias Rauter
 *
 ***********************************************************/

#ifndef STRUCTPAINTER_H
#define STRUCTPAINTER_H

#include <QtCore/QObject>
#include <QtGui/QPainter>

#include "colinstruct.h"
#include "viewportsettings.h"
#include "colinicons.h"

class structPainter
{
public:
    structPainter();
    void drawStruct(const ColinStruct &t, QPainter *painter, QTransform *trans, const Colin::Elements &toDraw_);
    void drawNode(const ColinNode &n, const int &i);
    void drawBearing(const ColinSupport &b, const QPointF &po, bool refconf = false);
	void drawBeam(const ColinBeam &s, const int &i, const ColinStruct &tw);
    void drawStLoad(const ColinLoad &l);
    void drawStLoad(const ColinLoad &l, QLineF *line);
    void drawLoad(const ColinLoad &l, const QPointF &po);
    void drawMoment(const ColinLoad &l, const QPointF &po);
    void drawTemp(const ColinLoad &l, const QLineF &line);
    void drawDoubleLoad(const ColinLoad &l, const QLineF &line);

    static void drawThermometer(QPainter *p, const QRect &boundingRect, const double &value, const bool &hasFocus = false, double *max = 0, double *min = 0);

    QTransform *trm;


	void drawFunction(QPainter *p, const QList<const function*> &f, const double &l, const double &scale, const double &prefix);

	static void setHighlightedObject(catcher::CatchCases c, int object);
	static void setHighlightedObject(catcher::CatchCases c);

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

	void setFunctionGradient(QPainter *p, QColor c1, QColor c2);
	void setColor(QPainter *p, const ColinLoad &l, bool hotspot = false);


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

	QPolygonF* cache;
	const ColinStruct *t;

	static catcher::CatchCases highlightMode;
	static int highlight;
};

#endif // STRUCTPAINTER_H
