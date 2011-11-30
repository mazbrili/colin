#ifndef SLIDINGHBOXLAYOUT_H
#define SLIDINGHBOXLAYOUT_H

#include <QtGui/QHBoxLayout>
#include <QtCore/QPropertyAnimation>
#include <QtCore/QEasingCurve>

class SlidingHBoxLayout : public QHBoxLayout
{
public:
	SlidingHBoxLayout(QWidget *parent = 0);
};

#endif // SLIDINGHBOXLAYOUT_H
