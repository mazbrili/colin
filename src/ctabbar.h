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


#ifndef CTABBAR_H
#define CTABBAR_H

#include <QtGui/QToolBar>
#include <QtGui/QStylePainter>
#include <QtGui/QApplication>
#include <QtGui/QStyleOptionToolBar>
#include <QtGui/QMouseEvent>

#include "filelist.h"
#include "colinicons.h"

class cTabBar : public QToolBar
{
public:
    explicit cTabBar(QWidget *parent = 0);
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *){repaint();}
    void leaveEvent(QEvent *){repaint();}
    void mousePressEvent(QMouseEvent *e);
private:
    QRect closeRect(int i);
    QRect plusRect();
    int dw();
    int tablenght();
    int dwtab();
};

#endif // CTABBAR_H
