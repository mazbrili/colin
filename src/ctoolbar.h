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

#ifndef CTOOLBAR_H
#define CTOOLBAR_H

#include <QToolBar>

class cToolBar : public QToolBar
{
    Q_OBJECT
public:
    explicit cToolBar(QString title, QWidget *parent = 0):QToolBar(title, parent){}

    void leaveEvent(QEvent *){emit mouseLeft();}
signals:
    void mouseLeft();

};

#endif // CTOOLBAR_H
