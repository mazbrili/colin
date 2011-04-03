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

#include "jmenu.h"

jMenu::jMenu(QAction *a, QWidget *parent) :
    QMenu(parent)
{
    const int bs = colinIcons::iconSize().width()+18;  //buttonsize
    const int as = 3;                          //space between 2 buttons
    const int is = colinIcons::iconSize().width()+8;   //iconsize

    this->setAttribute(Qt::WA_TranslucentBackground, true);

    QPushButton *n = new QPushButton(colinIcons::instance().icon(Colin::drawJointN), "", this);
    QPushButton *q = new QPushButton(colinIcons::instance().icon(Colin::drawJointQ), "", this);
    QPushButton *m = new QPushButton(colinIcons::instance().icon(Colin::drawJointM), "", this);

    n->setGeometry(0, 0, bs, bs);
    q->setGeometry(0, bs+as, bs, bs);
    m->setGeometry(0, (bs+as)*2, bs, bs);

    group = new QButtonGroup(this);
    group->addButton(n, Colin::drawJointN);
    group->addButton(q, Colin::drawJointQ);
    group->addButton(m, Colin::drawJointM);

    foreach(QAbstractButton *p, group->buttons())
    {
        p->setCheckable(true);
        p->setIconSize(QSize(is, is));
    }

    group->setExclusive(true);

    m->setChecked(true);

    this->setFixedSize(bs, bs*3+as*2);

    theAction = a;
    a->setMenu(this);
    a->setIcon(group->buttons().at(2)->icon());
    a->setData(QVariant(Colin::drawJointM));

    connect(group,              SIGNAL(buttonClicked(int)),
            this,               SLOT(hide()));

    connect(group,              SIGNAL(buttonClicked(int)),
            this,               SLOT(click(int)));

}

void jMenu::click(int nr)
{
    theAction->setIcon(group->button(nr)->icon());
    theAction->setData(QVariant(nr));
    theAction->trigger();
}
