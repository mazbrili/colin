#include "colinmenu.h"



bool ColinMenu::addWidgetForAction(QAction *action, QWidget *widget)
{
    bool containsAction = false;

    for(QList<QAction *>::const_iterator a_i = actions().constBegin();
        a_i != actions().constEnd();
        ++a_i)
    {
        if(*a_i==action)
        {
            containsAction = true;
            continue;
        }
    }
    if(!containsAction)
        return false;

    widget->setParent(this);
    actionWidgets.append(QPair<QAction *, QWidget *>(action, widget));
    action->setDisabled(true);
    QAction *d = new QAction("dummy", this);
    insertAction(action, d);
    d->setDisabled(true);
    setChildrenGeometry();

    connect(action,             SIGNAL(destroyed(QObject *)),
            this,               SLOT(removeWidgetFromAction(QObject *)));

    connect(action,             SIGNAL(destroyed(QObject *)),
            d,                  SLOT(deleteLater()));

    return true;
}

void ColinMenu::removeWidgetFromAction(QAction *action)
{
    if(action == NULL)
        return;

    for(QList<QPair<QAction *, QWidget *> >::const_iterator w_i = actionWidgets.constBegin();
        w_i != actionWidgets.constEnd();
        ++w_i)
    {
        if(w_i->first == action)
        {
            delete w_i->second;
            actionWidgets.removeOne(*w_i);
        }
    }
}

void ColinMenu::setChildrenGeometry()
{
    int minimalWidth = 0;
    const int rightSpace = 5;
    foreach(ActionWidgetPair w_i, actionWidgets)
    {
        minimalWidth = qMax((sizeHint().width()+w_i.second->geometry().width()+rightSpace),
                            minimalWidth);
    }
    mWidth = minimalWidth;
}


void ColinMenu::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QRegion emptyArea = QRegion(rect());

    bool dummy(false);
    QStyleOptionMenuItem menuOpt;
    menuOpt.initFrom(this);
    menuOpt.state = QStyle::State_None;
    menuOpt.checkType = QStyleOptionMenuItem::NotCheckable;
    menuOpt.maxIconWidth = 0;
    menuOpt.tabWidth = 0;
    style()->drawPrimitive(QStyle::PE_PanelMenu, &menuOpt, &p, this);

    for (int i = 0; i < actions().count(); ++i)
    {
       QAction *action = actions().at(i);
       QRect adjustedActionRect = actionGeometry(actions().at(i));
       if(actions().at(i)->text()=="dummy")
       {
           dummy = true;
           continue;
       }

       if(dummy)
           adjustedActionRect.setTop(adjustedActionRect.top()-adjustedActionRect.height());
       QRegion adjustedActionReg(adjustedActionRect);
       emptyArea -= adjustedActionReg;
       p.setClipRegion(adjustedActionReg);

       QStyleOptionMenuItem opt;
       initStyleOption(&opt, action);
        opt.rect = adjustedActionRect;
       if(dummy)
       {
           opt.state = QStyle::State_Enabled;
       }/*
       if(adjustedActionRect.contains(mapFromGlobal(QCursor::pos())))
           //opt.state |= QStyle::State_MouseOver;
           //opt.state |= QStyle::State_HasFocus;
           //opt.state |= QStyle::State_On;
           //opt.state |= QStyle::State_Sunken;
           opt.state |= QStyle::State_Selected;*/
       style()->drawControl(QStyle::CE_MenuItem, &opt, &p, this);
       dummy = false;
    }
}


void ColinMenu::showEvent(QShowEvent *)
{
    setFixedWidth(mWidth);
    const int rightSpace = 5;
    foreach(ActionWidgetPair w_i, actionWidgets)
    {
        int h;
        if(w_i.second->sizePolicy().verticalPolicy() == QSizePolicy::Preferred ||
           w_i.second->sizePolicy().verticalPolicy() == QSizePolicy::Minimum)
            h = w_i.second->sizeHint().height();
        else
            h = actionGeometry(w_i.first).height()*2-4;
        w_i.second->setGeometry(width()-w_i.second->width()-rightSpace,
                                actionGeometry(w_i.first).top()-h/2,
                                w_i.second->width(),
                                h);
    }
}
