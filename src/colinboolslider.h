#ifndef COLINBOOLSLIDER_H
#define COLINBOOLSLIDER_H

#include <QtGui/QAbstractButton>
#include <QtCore/QPropertyAnimation>

class ColinBoolSlider : public QAbstractButton
{
    Q_OBJECT
	Q_PROPERTY(double sliderPos READ sliderPos WRITE setSliderPos)
public:
	explicit ColinBoolSlider(QWidget *parent = 0);
	void paintEvent(QPaintEvent *e);
	void setIcon(const QIcon &icon, bool on);
	void setText(const QString &text, bool on);
	double sliderPos();
	void setSliderPos(double p);

signals:

public slots:
	void animateTo(bool state);

private:
	QIcon onIcon, offIcon;
	QString onText, offText;
	double animationstate;
	QPropertyAnimation animation;

};

#endif // COLINBOOLSLIDER_H
