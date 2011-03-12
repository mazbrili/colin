#include "cwidget.h"


cWidget::cWidget(QWidget *parent) :
    QWidget(parent)
{
    layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(QMargins(0, 0, 0, 0));
    setViewCount(4);
    views.at(1)->hide();
    views.at(2)->hide();
    views.at(3)->hide();
    views.at(0)->setFullyArea(true);
    hasFullArea = 0;
    shown = 1;

}

void cWidget::idrequest(int* id)
{
    for(*id=0; *id<views.size(); (*id)++)
    {
        if(views.at(*id)==QObject::sender())
            return;
    }
    *id=-1;
}

void cWidget::setViewCount(int i)
{
    viewPortSettings::instance().setWidgetCount(i);
    if(i>views.size())
    {
        for(int j=views.size(); j<i; j++)
        {
            viewport* v = new viewport(this);
            views.append(v);
            connect(v,          SIGNAL(idrequest(int*)),
                    this,       SLOT(idrequest(int*)));

            connect(v,          SIGNAL(maximizeMe()),
                    this,       SLOT(maximizeView()));

            connect(v,          SIGNAL(hideMe()),
                    this,       SLOT(hideView()));

            connect(v,          SIGNAL(showAll()),
                    this,       SLOT(showAll()));

            connect(v,          SIGNAL(basePoint(QPointF)),
                    this,       SLOT(emitBasePoint(QPointF)));
        }
    }
    else if(i<views.size())
    {
        for(int j=views.size(); j>i; j--)
        {
            views.takeLast()->deleteLater();
        }
    }
    else
    {
        return;
    }
    rowmax_ = sqrt(views.size());
    if(rowmax_*rowmax_ != views.size())
        rowmax_++;
    QList<QVBoxLayout*> hlayouts;
    for(int j=0; j<rowmax_; j++)
    {
        hlayouts.append(new QVBoxLayout());
        layout->addLayout(hlayouts.at(j));
    }
    for(int j=0; j<views.size(); j++)
    {
        hlayouts.at(j%rowmax_)->addWidget(views.at(j));
    }
    showAll();
}


void cWidget::setTw(wgv_tw* tw)
{
    if(tw == NULL)
    {
        foreach(viewport* view, views)
        {
            view->hide();
        }
    }
}


void cWidget::setVisibleViews(const QByteArray &vis)
{
    if(vis.size()<views.size())
        return;
    if(vis == QByteArray(12, false))
        return;

    if(hasFullArea != -1)
        views.at(hasFullArea)->setFullyArea(false);
    int j=0;
    for(int i=0; i<views.size(); i++)
    {
        views.at(i)->setVisible(vis.at(i));
        if(vis.at(i))
        {
            j++;
            hasFullArea = i;
        }
    }
    shown = j;
    if (shown != 1)
        hasFullArea = -1;
    else
        views.at(hasFullArea)->setFullyArea(true);
}


void cWidget::maximizeView()
{
    for(int i=0; i<views.size(); i++)
    {
        if(views.at(i)!=sender())
        {
            views.at(i)->hide();
        }
        else
        {
            hasFullArea = i;
            views.at(i)->setFullyArea(true);
        }
    }
    static_cast<QWidget*>(sender())->show();
    shown = 1;
    emit viewChanged();
}

void cWidget::hideView()
{
    static_cast<QWidget*>(sender())->hide();
    shown--;
    if(shown==1)
    {
        for(int i=0; i<views.size(); i++)
        {
            if(!views.at(i)->isHidden())
            {
                hasFullArea = i;
                views.at(i)->setFullyArea(true);
                break;
            }
        }
    }
    emit viewChanged();
}

void cWidget::showAll()
{
    foreach(viewport *v, views)
    {
        v->show();
    }
    shown = views.size();
    if(hasFullArea>-1 && hasFullArea<views.size())      //valgrind warning... i dont get it...
        views.at(hasFullArea)->setFullyArea(false);
    hasFullArea = -1;
    emit viewChanged();
}

void cWidget::repaintyourChildren()
{
    foreach(QWidget *w, views)
    {
        w->repaint();
    }
}
