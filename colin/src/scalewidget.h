#ifndef SCALEWIDGET_H
#define SCALEWIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QSlider>
#include <QtGui/QPainter>
#include <QtGui/QResizeEvent>
#include <QtGui/QApplication>


#include "colinpushbuttonpart.h"
#include "colinicons.h"


class scaleWidget : public QWidget
{
public:
    explicit scaleWidget(QWidget *parent = 0);

    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *event);
    void adjustButtons();
    void adjustButtons(const QSize &size);

    QSlider *slider;
    ColinPushButtonPart *down,
                        *up;
    QPushButton *restore;


};

#endif // SCALEWIDGET_H
