#ifndef VIEWMENU2_H
#define VIEWMENU2_H

#include <QtGui/QMenu>
#include <QtGui/QActionGroup>

#include "viewportpresenter.h"
#include "cwidget.h"


class viewMenu2 : public QMenu
{
    Q_OBJECT
public:
    explicit viewMenu2(cWidget *c, QWidget *parent = 0);

    void showEvent(QShowEvent *);
signals:
    void fullScreenRequested(const bool &on);
public slots:
    //void setWidgetCount(QAction *a);
    void emitFullScreen(const bool &on);
private:
    int neededHeight;
    viewPortPresenter *vp;
    //QActionGroup *g;
    cWidget *centralWidget;
};

#endif // VIEWMENU2_H
