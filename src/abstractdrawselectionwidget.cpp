#include "abstractdrawselectionwidget.h"

#include "viewportsettings.h"

#include <QtGui/QLayout>

abstractDrawSelectionWidget::abstractDrawSelectionWidget(QWidget *parent) :
    QWidget(parent)
{

	animation.setTargetObject(this);
	animation.setPropertyName("fixedSize");
	this->animation.setDuration(200);
	this->animation.setEasingCurve(QEasingCurve::Linear);

}



void abstractDrawSelectionWidget::drawBackground(QPainter *p)
{
	if(!mouseOver && size() == sizeHint())
		return;
	QLinearGradient backG(0, 0, 0, height());
	backG.setColorAt(0, palette().color(QPalette::Light));
	backG.setColorAt(1, palette().color(QPalette::Window));

	p->setPen(palette().color(QPalette::Dark));
	p->setBrush(QBrush(backG));
	p->drawRect(rect().adjusted(0, 0, -1, -1));
}

void abstractDrawSelectionWidget::drawRect(QPainter *p, QRect rect, bool down, bool checked, bool mouseover)
{
	QLinearGradient backG(0, rect.top(), 0, rect.bottom());
	backG.setColorAt(mouseover?1:0, palette().color(QPalette::Light));
	backG.setColorAt(mouseover?0:1, palette().color(QPalette::Window));

	if(!checked)
		p->setPen(palette().color(QPalette::Dark));
	p->setBrush(QBrush(backG));
	if(mouseover){
		if(down){
			p->drawRect(rect);
			p->setBrush(Qt::NoBrush);
		}
		p->drawRect(rect.adjusted(-1, -1, 1, 1));
	}
	else
		p->drawRect(rect);
}

void abstractDrawSelectionWidget::enterEvent(QEvent *)
{
	mouseOver = true;
	this->setFocus();
	requestResize(sizeHint());
	repaint();
}

void abstractDrawSelectionWidget::leaveEvent(QEvent *)
{
	mouseOver = false;
	requestResize(sizeHint());
	this->parentWidget()->setFocus();
	repaint();
}

void abstractDrawSelectionWidget::requestResize(QSize newSize)
{
	animation.setStartValue(size());
	animation.setEndValue(newSize);
	animation.start();
	parentWidget()->layout()->setEnabled(true);
}
