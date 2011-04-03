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

#include "lmenu.h"

lMenu::lMenu(QAction *a, QWidget *parent) :
    QMenu(parent)
{
    const int bs = colinIcons::iconSize().width()+18;  //buttonsize
    const int as = 3;                          //space between 2 buttons
    const int is = colinIcons::iconSize().width()+8;   //iconsize



    setAttribute(Qt::WA_TranslucentBackground, true);

    //init Button
    QPushButton *nLoad = new QPushButton(this);
    QPushButton *mom = new QPushButton(this);
    QPushButton *uLoad = new QPushButton(this);
    QPushButton *iLoad = new QPushButton(this);
    QPushButton *dLoad = new QPushButton(this);
    QPushButton *temp = new QPushButton(this);
    QPushButton *doublel = new QPushButton(this);

    //adjust buttongeometry
    nLoad->setGeometry(0, 0, bs, bs);
    mom->setGeometry(0, bs+as, bs, bs);
    uLoad->setGeometry(0, (bs+as)*2, bs, bs);
    iLoad->setGeometry(0, (bs+as)*3, bs, bs);
    dLoad->setGeometry(0, (bs+as)*4, bs, bs);
    temp->setGeometry(0, (bs+as)*5, bs, bs);
    doublel->setGeometry(0, (bs+as)*6, bs, bs);

    //...and add to a group...
    group = new QButtonGroup(this);
    group->addButton(nLoad, Colin::drawNLoad);
    group->addButton(mom, Colin::drawMoment);
    group->addButton(uLoad, Colin::drawULoad);
    group->addButton(iLoad, Colin::drawILoad);
    group->addButton(dLoad, Colin::drawDLoad);
    group->addButton(temp, Colin::drawTemp);
    group->addButton(doublel, Colin::drawDoubleLoad);

    //...of checkable buttons...
    foreach(QAbstractButton *p, group->buttons())
    {
        p->setCheckable(true);
        p->setIconSize(QSize(is, is));
        p->setIcon(colinIcons::instance().icon(static_cast<Colin::Action>(group->id(p))));
    }

    //...where only one could  checked
    group->setExclusive(true);

    //startvalue
    nLoad->setChecked(true);

    //set my geometry :)
    this->setFixedSize(bs, bs*7+as*6);

    //a long click on the Action a will trigger this menu....
    theAction = a;
    a->setMenu(this);
    a->setIcon(group->buttons().at(0)->icon());
    a->setData(QVariant(Colin::drawNLoad));

    //pressing a button will close the menu and change the
    //actions Data and Icon
    //(witch will change it in the singleton and everywhere!!11)
    connect(group,          SIGNAL(buttonClicked(int)),
            this,           SLOT(hide()));
    connect(group,          SIGNAL(buttonClicked(int)),
            this,           SLOT(click(int)));
}

void lMenu::click(int nr)
{
    //change Data and Icon
    theAction->setIcon(group->button(nr)->icon());
    theAction->setData(QVariant(nr));

    //let everything know that the current action changed!
    //(the mainwindow will evaluate the Data as an enum (Colin::toDraw)
    theAction->trigger();
}
