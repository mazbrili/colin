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

#include "bmenu.h"

bMenu::bMenu(QAction *a, QWidget *parent) :
    QMenu(parent)
{
    const int bs = colinIcons::iconSize().width()+18;  //buttonsize
    const int as = 3;                          //space between 2 buttons
    const int is = colinIcons::iconSize().width()+8;   //iconsize

    this->setAttribute(Qt::WA_TranslucentBackground, true);

    QPushButton *h = new QPushButton(colinIcons::instance().icon(Colin::drawBearingH), "", this);
    QPushButton *v = new QPushButton(colinIcons::instance().icon(Colin::drawBearingV), "", this);
    QPushButton *m = new QPushButton(colinIcons::instance().icon(Colin::drawBearingM), "", this);

    h->setGeometry(0, 0, bs, bs);
    v->setGeometry(0, bs+as, bs, bs);
    m->setGeometry(0, (bs+as)*2, bs, bs);

    group = new QButtonGroup(this);
    group->addButton(h, Colin::drawBearingH);
    group->addButton(v, Colin::drawBearingV);
    group->addButton(m, Colin::drawBearingM);

    foreach(QAbstractButton *p, group->buttons())
    {
        p->setCheckable(true);
        p->setIconSize(QSize(is, is));
    }
    group->setExclusive(false);

    h->setChecked(true);
    v->setChecked(true);

    this->setFixedSize(bs, bs*3+as*3);

    theAction = a;
    a->setMenu(this);
    a->setIcon(colinIcons::instance().icon(Colin::drawBearingHV));
    a->setData(QVariant(Colin::drawBearingHV));


    connect(group,              SIGNAL(buttonClicked(int)),
            this,               SLOT(click()));

}

void bMenu::click()
{
    int nr = Colin::drawBearing;
    foreach(QAbstractButton *pb, group->buttons())
    {
        if(pb->isChecked())
            nr |= group->id(pb);
    }

    theAction->setIcon(colinIcons::instance().icon(static_cast<Colin::Actions>(nr)));
    theAction->setData(QVariant(nr));
    theAction->trigger();

}
