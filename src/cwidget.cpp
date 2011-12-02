/***********************************************************
 * $Id$
 *
 * Colin
 *
 * Copyright (C) 2011 Matthias Rauter (matthias.rauter@student.uibk.ac.at)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details."
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see
 *
 * http://www.gnu.org/licenses/.
 *
 * Author: Matthias Rauter
 *
 ***********************************************************/

#include "cwidget.h"
#include <limits>


cWidget::cWidget(QWidget *parent) :
	QSplitter(parent)
{

	sidebar = new ColinSidebar(this);
	this->setHandleWidth(0);

	viewContainer = new QWidget(this);
	hlayout = new ViewPortLayout(viewContainer);
	hlayout->setSpacing(0);
	hlayout->setContentsMargins(QMargins(0, 0, 0, 0));
	hasFullArea = 0;
	shown = 1;
    setViewCount(4);
    views.at(1)->hide();
    views.at(2)->hide();
	views.at(3)->hide();
	views.at(0)->setFullyArea(true);

	hasFullArea = 0;
	shown = 1;

	setStretchFactor(1, 5);
	//QPalette pal(palette());
	//pal.setColor(QPalette::Window, pal.color(QPalette::Window).darker());
	//setPalette(pal);
	//setAutoFillBackground(true);

	views.at(1)->setStrech(0);
	views.at(2)->setStrech(0);
	views.at(3)->setStrech(0);
	views.at(0)->setStrech(std::numeric_limits<unsigned char>::max());
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
			v->setStrech(0);
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
    for(int j=0; j<views.size(); j++)
    {
		//hlayout->addWidget(views.at(j), j/rowmax_, j%rowmax_, 1, 1);
		hlayout->addWidget(views.at(j));
	}
}

void cWidget::setTw(ColinStruct* tw)
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

    if(hasFullArea != -1)
        views.at(hasFullArea)->setFullyArea(false);
    int j=0;
    for(int i=0; i<views.size(); i++)
	{
		if(vis.at(i))
		{
			views.at(i)->requestResize(std::numeric_limits<unsigned char>::max());
			views.at(i)->setVisible(true);
		}
		else
			views.at(i)->requestResize(0);
		//views.at(i)->setVisible(vis.at(i));
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
	QByteArray vis(12, false);
    for(int i=0; i<views.size(); i++)
    {
		if(views.at(i)==sender())
		{
			vis[i]=true;
		}
	}
	setVisibleViews(vis);
}

void cWidget::hideView()
{
	QByteArray vis(12, false);
	for(int i=0; i<views.size(); i++)
	{
		vis[i]=views.at(i)->isVisible();
		if(views.at(i)==sender())
		{
			vis[i]=false;
		}
	}
	setVisibleViews(vis);
}

void cWidget::showAll()
{
	QByteArray vis(12, true);
	setVisibleViews(vis);
}

void cWidget::repaintyourChildren()
{
    foreach(QWidget *w, views)
    {
        w->repaint();
    }
}

void cWidget::hideToolTips()
{
	foreach(viewport *w, views)
	{
		w->hideToolTip();
	}
}

void cWidget::resizeEvent(QResizeEvent *e)
{
	QWidget::resizeEvent(e);
}

void cWidget::showSideBar(bool show)
{
	if(show)
		sidebar->show();
	else
		sidebar->hide();
}
