#include "colinboolslider.h"
#include <QtGui/QPainter>
#include <QtGui/QStyleOptionButton>


ColinBoolSlider::ColinBoolSlider(QWidget *parent) :
    QAbstractButton(parent)
{
	this->setCheckable(true);
	animationstate = 0.0;

	this->setMinimumSize(80, 27);


	animation.setTargetObject(this);
	animation.setPropertyName("sliderPos");
	this->animation.setDuration(100);
	this->animation.setEasingCurve(QEasingCurve::Linear);

	connect(this,					SIGNAL(toggled(bool)),
			this,					SLOT(animateTo(bool)));
}


void ColinBoolSlider::paintEvent(QPaintEvent *e)
{
	QPainter painter(this);
	QStyleOptionButton option;
	option.initFrom(this);
	option.rect = QRect(0, 0, width(), height());
	option.state &= ~ QStyle::State_Sunken;
	painter.translate(0, height());
	painter.scale(1, -1);
	style()->drawControl(QStyle::CE_PushButton, &option, &painter, this);
	painter.resetMatrix();
	//onIcon.paint(&painter, QRect(15, 6, height()-12, height()-12));
	//offIcon.paint(&painter, QRect(width()-(height()-12)-15, 6, height()-12, height()-12));

	painter.drawText(QRect(0, 0, width()/2, height()), onText, Qt::AlignHCenter | Qt::AlignVCenter);
	painter.drawText(QRect(width()/2, 0, width()/2, height()), offText, Qt::AlignHCenter | Qt::AlignVCenter);

	option.initFrom(this);
	option.state &= ~ QStyle::State_Sunken;

	option.rect = QRect(3, 3, width()/2.-6, height()-6);

	option.rect.moveTo(QPoint(width()/2.*animationstate+3, 3));
	style()->drawControl(QStyle::CE_PushButton, &option, &painter, this);

}

void ColinBoolSlider::setIcon(const QIcon &icon, bool on)
{
	if(on)
		onIcon = icon;
	else
		offIcon = icon;
}

void ColinBoolSlider::setText(const QString &text, bool on)
{
	if(on)
		onText = text;
	else
		offText = text;
}

void ColinBoolSlider::animateTo(bool state)
{
	animation.setStartValue(sliderPos());
	animation.setEndValue(state?1.0:0.0);
	animation.start();
}

double ColinBoolSlider::sliderPos()
{
	return animationstate;
}

void ColinBoolSlider::setSliderPos(double p)
{
	animationstate = p;
	update();
}
