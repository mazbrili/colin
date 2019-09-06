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

#include "colinhmultibutton.h"


ColinHMultiButton::ColinHMultiButton(QWidget *parent)
    : QWidget(parent)
{

}


ColinHMultiButton::~ColinHMultiButton()
{

}



void ColinHMultiButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QStyleOptionButton option;
    option.initFrom(this);
    option.rect = QRect(0, 0, width(), height());
    painter.translate(0, height());
    painter.scale(1, -1);
    style()->drawControl(QStyle::CE_PushButton, &option, &painter, this);
}


void ColinHMultiButton::resizeEvent(QResizeEvent *event)
{
    adjustButtons(event->size() );
}


void ColinHMultiButton::adjustButtons()
{
    adjustButtons(size());
}

void ColinHMultiButton::adjustButtons(const QSize &size)
{
    int HMultiButtonth_ = (size.width()-buttonlist.size()+1)/(int)buttonlist.size();
    int rest = (size.width()-buttonlist.size()+1)%(int)buttonlist.size();

    for(int i = 0, xpos = 0; i<buttonlist.size(); i++)
    {
        buttonlist[i]->setGeometry(QRect(xpos, 0, HMultiButtonth_+((i<rest)? 1 : 0), height()));
        xpos+=HMultiButtonth_+((i<rest)? 2 : 1);
    }
}


void ColinHMultiButton::addButton(ColinPushButtonPart *but)
{
    if(!buttonlist.isEmpty())
    {
        buttonlist.last()->setCutted(ColinPushButtonPart::Right, true);
        but->setCutted(ColinPushButtonPart::Left, true);
    }


    buttonlist.append(but);
    but->setParent(this);
    adjustButtons();
}
