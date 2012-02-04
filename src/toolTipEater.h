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

#ifndef TOOLTIPEATER_H
#define TOOLTIPEATER_H

#include <QtCore/QObject>
#include <QtCore/QEvent>
#include <QtCore/QDebug>
#include <QtGui/QWhatsThisClickedEvent>

#include "colinmanual.h"

class toolTipEater : public QObject
{

	Q_OBJECT

protected:
	bool eventFilter(QObject *obj, QEvent *event){
		if(event->type() == QEvent::ToolTip && obj->inherits("QToolButton")){
#ifdef TOOLTIPEATER_VERBOSE
			qDebug() << "eat tooltip!";
#endif
			return true;
		}
		if(event->type() == QEvent::WhatsThisClicked){
			colinManual::launch(static_cast<QWhatsThisClickedEvent*>(event)->href());
			return true;
		}
		else
			return QObject::eventFilter(obj, event);
	}
};

#endif // TOOLTIPEATER_H
