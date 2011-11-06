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

#ifndef CMENU_H
#define CMENU_H

#include <QtGui/QMenu>
#include <QtGui/QPushButton>
#include <QtGui/QButtonGroup>
#include <QtGui/QSlider>

#include "catcher.h"
#include "colinicons.h"

class cMenu : public QMenu
{
    Q_OBJECT
public:
    explicit cMenu(QAction *a, QWidget *parent = 0);

    void paintEvent(QPaintEvent *);

signals:

private slots:
    void click();
    void setRange(int i);
    void allOn(bool on);
private:
    QButtonGroup *group;
    QAction *theAction;
    QSlider *range;
};

#endif // CMENU_H
