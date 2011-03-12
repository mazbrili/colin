#ifndef CTOOLBAR_H
#define CTOOLBAR_H

#include <QtGui/QToolBar>

class cToolBar : public QToolBar
{
    Q_OBJECT
public:
    explicit cToolBar(QString title, QWidget *parent = 0):QToolBar(title, parent){}

    void leaveEvent(QEvent *){emit mouseLeft();}
signals:
    void mouseLeft();

};

#endif // CTOOLBAR_H
