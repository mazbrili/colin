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

#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <QtGui/QWheelEvent>
#include <QtGui/QPushButton>
#include <QtGui/QStyle>
#include <QtCore/QTime>
#include <QtGui/QBitmap>
#include <QtGui/QApplication>
#include <QtCore/QPropertyAnimation>



#include "grid.h"
#include "filelist.h"
#include "structpainter.h"
#include "catcher.h"
#include "colinicons.h"
#include "termovaluewidget.h"
#include "drawselectionwidget.h"
#include "drawselectionwidget2.h"
#include "viewporttooltip.h"
#include "resultvisualwidget.h"
#include "colinrubberband.h"


struct highlight
{
	catcher::CatchCases highlightingMode;
	QPointF pos;
	int objectIndex;
	int objectIndex2;
};

class viewport : public QWidget
{
	Q_OBJECT

	Q_PROPERTY(int strech READ strech WRITE setStrech)

public:
    explicit viewport(QWidget *parent = 0);
    ~viewport();
    void paintEvent(QPaintEvent *);
    void wheelEvent(QWheelEvent *e);
    void resizeEvent(QResizeEvent *);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
	void hideEvent(QHideEvent *e);
	void leaveEvent(QEvent *e);
	void focusInEvent(QFocusEvent *e);
	void focusOutEvent(QFocusEvent *e);
	void keyPressEvent(QKeyEvent *e);

	void requestResize(int target);
	void setStrech(int val);
	int strech();
	QSize sizeHint(){return QSize(100, 100);}

	void hideToolTip();
    void zoomRect(const QRectF &r, bool keepSpace = false);
    static void setToPaste(ColinStruct *tw){toPaste_ = tw;}
	static void removeToPaste(){delete toPaste_; toPaste_ = 0; viewPortSettings::instance().setClipBoard(Colin::noRequest);}


signals:
    void idrequest(int*);
    void maximizeMe();
    void hideMe();
    void showAll();
    void basePoint(QPointF);
public slots:
    void setTw(ColinStruct* t);
    void setFullyArea(bool doI);
    void emit_maximizeMe(){emit maximizeMe();}
    void emit_hideMe(){emit hideMe();}
    void emit_showAll(){emit showAll();}
    void setDrawCursor(Colin::Actions a);
    void setClipCursor(Colin::clipboardAction a);
	void newTemp(double val, int type);
private:
    const int &id() const;
    int id_;

    void addNode(QPointF p);
    void addBeam(QPointF p);
    void addBearing(QPointF p);
    void addNLoad(QPointF p);
    void addSLoad(QPointF p);
    void addTemp(QPointF p);
    void addJoint(QPointF p);
    void addDoubleLoad(QPointF p);

    void launchMenuNode(const int &i);
    void launchMenuBeam(const int &i);
    void launchMenuLoad(const int &i);
    void launchMenuTemp(const int &i);

    void findMoveObject(QPointF p);

	void findHighLightedObject();
    void drawStruct(QPainter *p);
    void drawCursorAxes(QPainter *p);
    void drawCurrentObject(QPainter *p, const QPointF &cursorpos);
    void translate(const QPoint &p);
    void selectRect(const QRect &r);
    void selectNearest(QPointF p);
    void zoomClick(const QPointF &p);

    QTransform &globalMatrix();

    QPoint lastMousePosition;
    Qt::MouseButtons lastMouseButtons;

    QPushButton *maximizer,
                *hider,
                *allshower;

    drawSelectionWidget *selec;
	drawSelectionWidget2 *selec2;
    viewPortToolTip *tooltip;

	ColinRubberBand *rubberband;
    bool mouseIsOverChild;

    static ColinStruct* tw;
    static viewPortSettings *vS;
    //static QPoint lastClick;
    //static int drawingSteps;
    static catcher::CatchCases movingObject;
    static int viewCounter;
    static ColinStruct* toPaste_;
    static structPainter sPainter;
	static highlight highlightedObject;

    void inline fuzzyNull(QPointF *p) const {
        if(qFuzzyIsNull(p->x())) p->setX(0);
        if(qFuzzyIsNull(p->y())) p->setY(0);}

	QTransform globalMatrix_exact;
	QSize lastKnownSize;

	QPropertyAnimation animation;

};

#endif // VIEWPORT_H
