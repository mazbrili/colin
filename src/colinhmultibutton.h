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

#ifndef COLINHMULTIBUTTON_H
#define COLINHMULTIBUTTON_H

#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <QtCore/QCoreApplication>
#include <QtCore/QList>
#include <QtGui/QResizeEvent>
#include "colinpushbuttonpart.h"

class ColinHMultiButton : public QWidget
{
    Q_OBJECT

public:
    ColinHMultiButton(QWidget *parent = 0);

    ~ColinHMultiButton();


    void addButton(ColinPushButtonPart *but);
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *event);
    void adjustButtons();
    void adjustButtons(const QSize &size);
	QSize sizeHint() const;

private:
    QList<ColinPushButtonPart*> buttonlist;

};

#endif // COLINHMULTIBUTTON_H
