#include "icons.h"

#include "whatsthismaker.h"
#include "math.h"

double iconMaker::lineWidth = 1;
bool iconMaker::light = false;
iconMaker::icon iconMaker::current = iconMaker::placeholder;
bool iconMaker::shadow = false;
bool iconMaker::shadowMode = false;

iconMaker *iconMaker::create(icon i)
{
	switch(i)
	{
	case whatsThis:
		return new whatsThisMaker();
	default:
		return new iconMaker();
	}
}

QString iconMaker::iconName(icon i)
{
	switch(i)
	{
	case placeholder:
		return "placeholder";
	case whatsThis:
		return "whatsThis";
	default:
		return "";
	}
}

iconMaker::icon iconMaker::fromString(QString s)
{
	for(int i=0; i<100; i++){
		if(iconName(static_cast<icon>(i))==s)
			return static_cast<icon>(i);
	}
	return static_cast<icon>(0);
}

QBrush iconMaker::gradLight(QLinearGradient grad)
{
	if(shadowMode)
	{
		grad.setColorAt(0, QColor(200, 200, 200));
		grad.setColorAt(1, QColor(200, 200, 200));
	}
	else
	{
		grad.setColorAt(0, QColor(230, 230, 230));
		grad.setColorAt(1, QColor(178, 178, 178));
	}
	return QBrush(grad);
}

QBrush iconMaker::gradDark(QLinearGradient grad)
{
	if(shadowMode)
	{
		grad.setColorAt(0, QColor(200, 200, 200));
		grad.setColorAt(1, QColor(200, 200, 200));
	}
	else
	{
		grad.setColorAt(0, QColor(30, 30, 30));
		grad.setColorAt(1, QColor(90, 90, 90));
	}
	return QBrush(grad);
}


void iconMaker::paint(QPainter *p, bool light)
{
	if(shadow)
	{
		p->translate(1./64.*double(p->device()->width()), 1./64.*double(p->device()->width()));
		shadowMode = true;
		this->paint(p, light, shadowMode);
		p->resetMatrix();
	}
	shadowMode = false;
	this->paint(p, light, shadowMode);
}

void iconMaker::paint(QPainter *p, bool light, bool shadow)
{
	QSize size = QSize(p->device()->width(), p->device()->height());
	p->fillRect(QRectF(0, 0, size.width(), size.height()), Qt::red);
}
