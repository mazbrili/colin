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

#ifndef JMENU_H
#define JMENU_H

#include <QMenu>
#include <QPushButton>
#include <QButtonGroup>

#include "viewportsettings.h"
#include "colinicons.h"

class jMenu : public QMenu
{
    Q_OBJECT
public:
    explicit jMenu(QAction *a, QWidget *parent = 0);

    void paintEvent(QPaintEvent *){}

signals:

public slots:
    void click(int nr);
private:
    QAction *theAction;
    QButtonGroup *group;
};

#endif // JMENU_H
