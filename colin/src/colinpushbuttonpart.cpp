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

#include "colinpushbuttonpart.h"

void ColinPushButtonPart::paintEvent(QPaintEvent *)
{
    QStyleOptionButton option;
    initStyleOption(&option);

    if((pos & ColinPushButtonPart::Left) == ColinPushButtonPart::Left)
    {
        option.rect.moveLeft(-4);
        option.rect.setWidth(option.rect.width()+4);
    }

    if((pos & ColinPushButtonPart::Right) == ColinPushButtonPart::Right)
    {
        option.rect.setWidth(option.rect.width()+4);
    }

    if((pos & ColinPushButtonPart::Up) == ColinPushButtonPart::Up)
    {
        option.rect.moveTop(-4);
        option.rect.setHeight(option.rect.width()+4);
    }

    if((pos & ColinPushButtonPart::Bottom) == ColinPushButtonPart::Bottom)
    {
        option.rect.setHeight(option.rect.width()+4);
    }

    QPainter painter(this);
    style()->drawControl(QStyle::CE_PushButton, &option, &painter, this);

/*
    style()->drawItemText(&painter, QRect(2, 2, width()-4, height()-4),
                          Qt::AlignCenter, palette(), isEnabled(), text(), QPalette::Text);

    QRect r = rect();

    r.setLeft(r.x()+2);
    r.setRight(r.right()-2);
    r.setTop(r.y()+2);
    r.setBottom(r.bottom()-2);
    icon().paint(&painter, r, Qt::AlignCenter,
                 isEnabled() ? QIcon::Normal
                             : QIcon::Disabled,
                 isChecked() || isDown() ? QIcon::On
                                         : QIcon::Off);
*/
}

void ColinPushButtonPart::setCutted(ColinPushButtonPart::Position pos_, bool cuted)
{
    if (cuted)
        pos = static_cast<ColinPushButtonPart::Position>(pos | pos_);
    else
        pos = static_cast<ColinPushButtonPart::Position>(pos & ~pos_);
}
