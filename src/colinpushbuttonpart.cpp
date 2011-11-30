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

	QPainter painter(this);

	option.rect=QRect(QPoint(0,0), this->size());
    if((pos & ColinPushButtonPart::Left) == ColinPushButtonPart::Left)
	{
		painter.translate(-4, 0);
		option.rect.setWidth(option.rect.width()+4);
    }

    if((pos & ColinPushButtonPart::Right) == ColinPushButtonPart::Right)
    {
        option.rect.setWidth(option.rect.width()+4);
    }

    if((pos & ColinPushButtonPart::Up) == ColinPushButtonPart::Up)
    {
		painter.translate(0,-4);
		option.rect.setHeight(option.rect.height()+4);
    }

    if((pos & ColinPushButtonPart::Bottom) == ColinPushButtonPart::Bottom)
    {
		option.rect.setHeight(option.rect.height()+4);
    }

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

void ColinPushButtonPart::setCutted(ColinPushButtonPart::Positions pos_, bool cuted)
{
    if (cuted)
		pos = (pos | pos_);
    else
		pos = (pos & ~pos_);
}
