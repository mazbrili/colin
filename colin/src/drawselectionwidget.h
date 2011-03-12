#ifndef DRAWSELECTIONWIDGET_H
#define DRAWSELECTIONWIDGET_H

#include <QtGui/QWidget>
#include "viewportsettings.h"
#include "colinicons.h"
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>


class drawSelectionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit drawSelectionWidget(int _id, QWidget *parent = 0);

    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

    void paintEvent(QPaintEvent *);

    void leaveEvent(QEvent *);
    void enterEvent(QEvent *);

    QSize sizeHint() const;
signals:

public slots:

private:
    int id;
    bool mouseOver;
    bool mousePressed;
    QRect uR, nR, qR, mR,       //u, N, Q, M
          bR, jR, kR, lR;       //bearings, joints, nodes, loads


};

#endif // DRAWSELECTIONWIDGET_H
