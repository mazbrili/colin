#include "whatsthismaker.h"
#include "math.h"

#include <QtGui/QPainterPath>
#include <QtCore/QDebug>

whatsThisMaker::whatsThisMaker()
{
}

void whatsThisMaker::paint(QPainter *p, bool light, bool shadow)
{
	QSize size = QSize(p->device()->width(), p->device()->height());
	p->setBrush(Qt::NoBrush);

	QPainterPath path;

	p->scale(double(size.width())/32., double(size.height())/32.);
	path.moveTo(16.-8./sqrt(2), 9+8./sqrt(2));
	path.arcTo(8, 1, 16, 16, 225, -270);
	QPointF f = path.currentPosition();
	path.cubicTo(f+QPointF(-4, 4), QPointF(18, 17), QPointF(18, 22));
	f = path.currentPosition();
	path.arcTo(QRectF(f, QSizeF(2,2)),180, 90);
	path.lineTo(path.currentPosition()+QPointF(2, 0));
	f = path.currentPosition();
	path.arcTo(QRectF(f-QPointF(0.75,0), QSizeF(1.5, 1.5)), 90, -135);
	double dx = path.currentPosition().x()-16;
	path.lineTo(path.currentPosition()+QPointF(-dx, dx));
	path.lineTo(path.currentPosition()+QPointF(-dx, -dx));
	f = QPointF(32-f.x(), f.y());
	path.arcTo(QRectF(f-QPointF(0.75,0), QSizeF(1.5, 1.5)), 225, -135);
	path.lineTo(path.currentPosition()+QPointF(2, 0));
	f = path.currentPosition();
	path.arcTo(QRectF(f+QPointF(-1, -2), QSizeF(2,2)),-90, 90);

	path=path.toReversed();
	path.arcTo(12, 5, 8, 8, 225, -270);
	f = path.currentPosition();
	path.cubicTo(f+QPointF(-6, 6), QPointF(14, 17), QPointF(14, 23));

	QLinearGradient grad(0, 0, 0, 32);
	if(light)
		p->setBrush(iconMaker::gradLight(grad));
	else
		p->setBrush(iconMaker::gradDark(grad));
	p->setPen(QPen(p->brush(), iconMaker::lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	p->drawPath(path);


	qDebug() << "paintEvent " << (shadow?"shadow":"foreground");
}
