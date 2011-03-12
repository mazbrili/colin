#ifndef VIEWPORTTOOLTIP_H
#define VIEWPORTTOOLTIP_H

#include <QtCore/QEvent>
#include <QtGui/QMoveEvent>

#include "abstractcolintooltip.h"
#include "QtGui/QGridLayout"


class viewPortToolTip : public abstractColinToolTip
{
public:
    viewPortToolTip(QWidget *parent = 0);
    void set(const QIcon &i, const QString &s, const QString &more);
protected:
    bool eventFilter(QObject *obj, QEvent *event);
private:
    QLabel *icon_,
	   *text_,
	   *extend_;
};

#endif // VIEWPORTTOOLTIP_H
