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

#ifndef COLINTOOLTIP_H
#define COLINTOOLTIP_H

#include <QtGui/QWidget>
#include <QtGui/QPainter>
#include <QtGui/QLinearGradient>
#include <QtGui/QLabel>
#include <QtGui/QGridLayout>
//#include <QtGui/QGraphicsDropShadowEffect>
//#include <QtGui/QGraphicsBlurEffect>




#include "colinicons.h"
#include "abstractcolintooltip.h"

class colinToolTip : public abstractColinToolTip
{
    Q_OBJECT
public:
    explicit colinToolTip(QWidget *parent = 0);


signals:

public slots:
    void popup(const QString &title_, const QString &text_, const QPixmap &icon_,
               const QKeySequence &shortcut_, const QPoint &pos, bool r);
private:
    QLabel *title,
           *icon,
           *description,
           *shortcut,
           *sequence;

};

#endif // COLINTOOLTIP_H
