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

#ifndef COLINPUSHBUTTONPART_H
#define COLINPUSHBUTTONPART_H

#include <QtGui/QPushButton>
#include <QtGui/QStyleOptionButton>
#include <QtGui/QStyle>
#include <QtGui/QPainter>


class ColinPushButtonPart : public QPushButton
{
    Q_OBJECT

public:

    enum Position {
        Alone   = 0,        //00000000
        Left    = 1,        //00000001
        Right   = 2,        //00000010
        Up      = 4,        //00000100
        Bottom  = 8,        //00001000
        HCenter = 3,        //00000011
        VCenter = 12,       //00001100
        Center  = 15        //00001111
    } pos;

    Q_DECLARE_FLAGS(Positions, Position);

    explicit ColinPushButtonPart(QWidget *parent = 0):QPushButton(parent){pos = ColinPushButtonPart::Alone;}
    ColinPushButtonPart(const QString &text, QWidget *parent = 0):QPushButton(text, parent){pos = ColinPushButtonPart::Alone;}
    ColinPushButtonPart(const QIcon &icon, const QString &text, QWidget *parent = 0):QPushButton(icon, text, parent){pos = ColinPushButtonPart::Alone;}

    void paintEvent(QPaintEvent *);

    void setCutted(ColinPushButtonPart::Position pos_, bool free = true);



};

#endif // COLINPUSHBUTTONPART_H
