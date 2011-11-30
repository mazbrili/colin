#include "colinrubberband.h"
#include <QtGui/QPainter>
#include <QtCore/QDebug>

#ifdef WIN32
ColinRubberBand::ColinRubberBand(QRubberBand::Shape s, QWidget *parent) :
QRubberBand (s, parent)
{

}
#else
ColinRubberBand::ColinRubberBand(QRubberBand::Shape s, QWidget *parent) :
QRubberBand (QRubberBand::Line, parent)
{

}
#endif

void ColinRubberBand::paintEvent(QPaintEvent *e)
{
#ifdef WIN32
	QRubberBand::paintEvent(e);
#else
	QPainter p(this);
	QColor c = palette().color(QPalette::Highlight);
	p.setPen(QPen(c,2));
	c.setAlpha(100);
	p.setBrush(QBrush(c));
	//qDebug() << p.pen().color() << p.pen().brush().color();
	p.drawRect(rect());
#endif

}
