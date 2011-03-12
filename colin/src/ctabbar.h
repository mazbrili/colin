/*****************************************************************
 ** tabbar for switching between open files taken from filelist **
 **     order is always (if visible...):                        **
 **         - open files                                        **
 **         - settingstab                                       **
 **         - libtab                                            **
 **         - startertab                                        **
 *****************************************************************/


#ifndef CTABBAR_H
#define CTABBAR_H

#include <QtGui/QToolBar>
#include <QtGui/QStylePainter>
#include <QtGui/QApplication>
#include <QtGui/QStyleOptionToolBar>
#include <QtGui/QMouseEvent>

#include "filelist.h"
#include "colinicons.h"

class cTabBar : public QToolBar
{
public:
    explicit cTabBar(QWidget *parent = 0);
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *){repaint();}
    void leaveEvent(QEvent *){repaint();}
    void mousePressEvent(QMouseEvent *e);
private:
    QRect closeRect(int i);
    QRect plusRect();
    int dw();
    int tablenght();
    int dwtab();
};

#endif // CTABBAR_H
