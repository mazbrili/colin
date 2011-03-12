#ifndef COLINTOOLTIP_H
#define COLINTOOLTIP_H

#include <QtGui/QWidget>
#include <QtGui/QPainter>
#include <QtGui/QLinearGradient>
#include <QtGui/QLabel>
#include <QtGui/QGridLayout>
//#include <QtGui/QGraphicsDropShadowEffect>
//#include <QtGui/QGraphicsBlurEffect>




#include "colinicons.h"
#include "abstractcolintooltip.h"

class colinToolTip : public abstractColinToolTip
{
    Q_OBJECT
public:
    explicit colinToolTip(QWidget *parent = 0);


signals:

public slots:
    void popup(const QString &title_, const QString &text_, const QPixmap &icon_,
               const QKeySequence &shortcut_, const QPoint &pos, bool r);
private:
    QLabel *title,
           *icon,
           *description,
           *shortcut,
           *sequence;

};

#endif // COLINTOOLTIP_H
