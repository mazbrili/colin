#include "viewmenu2.h"
#include "shortcutsettings.h"

viewMenu2::viewMenu2(cWidget *c, QWidget *parent) :
    QMenu(parent)
{
    centralWidget = c;

    QAction *a = addAction(tr("fullscreen"));
    a->setCheckable(true);
    addAction(QIcon(":/icons/zoom_all.png"), tr("show all"), c, SLOT(showAll()));

    //QMenu *countM = new QMenu(tr("set Widget Count"), this);
    //g = new QActionGroup(this);
    //for(int i=2; i<13; i++)
    //{
    //     QAction *a = g->addAction(countM->addAction(QString("%1").arg(i) + "  widgets"));
    //     a->setData(QVariant(i));
    //}
    //addMenu(countM);


    neededHeight = 0;

    vp = new viewPortPresenter(c, this);
    vp->setFixedSize(250, 250);
    setFixedWidth(254);


    //connect(g,              SIGNAL(triggered(QAction*)),
    //        this,           SLOT(setWidgetCount(QAction*)));

    connect(a,		    SIGNAL(toggled(bool)),
	    this,	    SLOT(emitFullScreen(bool)));
}

void viewMenu2::showEvent(QShowEvent *)
{
    if(neededHeight==0)
    {
        setFixedHeight(height()+254);
        neededHeight = height();
        vp->setGeometry(2, height()-252, 250, 250);
    }
}


//void viewMenu2::setWidgetCount(QAction *a)
//{
//    centralWidget->setViewCount(a->data().toInt());
//}


void viewMenu2::emitFullScreen(const bool &on)
{
    emit fullScreenRequested(on);
}
