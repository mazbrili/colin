#ifndef VIEWPORTPRESENTER_H
#define VIEWPORTPRESENTER_H

#include <QtGui/QWidget>
#include <QtGui/QPainter>
#include <QtGui/QRadialGradient>
#include <QtGui/QStyleOptionMenuItem>

#include "cwidget.h"

class viewPortPresenter : public QWidget
{
    Q_OBJECT
public:
    explicit viewPortPresenter(cWidget *c, QWidget *parent = 0);
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *);

signals:

public slots:

private:
    cWidget *central;
    QRect mouseRect;




};

#endif // VIEWPORTPRESENTER_H
