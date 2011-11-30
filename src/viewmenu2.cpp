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

#include "viewmenu2.h"
#include "shortcutsettings.h"

viewMenu2::viewMenu2(cWidget *c, QWidget *parent) :
    QMenu(parent)
{
    centralWidget = c;

    QAction *a = addAction(tr("fullscreen"));
    a->setCheckable(true);
	addAction(tr("show all"), c, SLOT(showAll()));
	QAction *s = addAction(tr("show sidebar"));
	s->setCheckable(true);
	s->setChecked(true);

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

	connect(a,			SIGNAL(toggled(bool)),
			this,		SLOT(emitFullScreen(bool)));

	connect(s,			SIGNAL(toggled(bool)),
		c,			SLOT(showSideBar(bool)));
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
