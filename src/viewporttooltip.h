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

#ifndef VIEWPORTTOOLTIP_H
#define VIEWPORTTOOLTIP_H

#include <QtCore/QEvent>
#include <QtGui/QMoveEvent>

#include "abstractcolintooltip.h"
#include "QtGui/QGridLayout"


class viewPortToolTip : public abstractColinToolTip
{
public:
    viewPortToolTip(QWidget *parent = 0);
	void set(const QIcon &i, const QString &s, const QString &more);
protected:
    bool eventFilter(QObject *obj, QEvent *event);
private:
    QLabel *icon_,
	   *text_,
	   *extend_;
};

#endif // VIEWPORTTOOLTIP_H
