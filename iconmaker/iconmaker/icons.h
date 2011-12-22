#ifndef ICONS_H
#define ICONS_H

#include <QtCore/QString>
#include <QtGui/QBrush>
#include <QtGui/QLinearGradient>
#include <QtGui/QPainter>


class iconMaker{
public:
	enum icon{
		placeholder,
		whatsThis
	};

	static iconMaker *create(icon i);
	iconMaker(){};
	static QString iconName(icon i);
	static iconMaker::icon fromString(QString s);

	static QBrush gradLight(QLinearGradient grad);
	static QBrush gradDark(QLinearGradient grad);

	static double lineWidth;
	static bool light;
	static icon current;
	static bool shadow;

	void paint(QPainter *p, bool light);
protected:
	virtual void paint(QPainter *p, bool light, bool shadow);

};

#endif // ICONS_H
