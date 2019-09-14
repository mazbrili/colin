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

#ifndef SCALEWIDGET_H
#define SCALEWIDGET_H

#include <QWidget>
#include <QSlider>
#include <QPainter>
#include <QResizeEvent>
#include <QApplication>


#include "colinpushbuttonpart.h"
#include "colinicons.h"


class scaleWidget : public QWidget
{
public:
    explicit scaleWidget(QWidget *parent = 0);

    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *event);
    void adjustButtons();
    void adjustButtons(const QSize &size);

    QSlider *slider;
    ColinPushButtonPart *down,
                        *up;
    QPushButton *restore;


};

#endif // SCALEWIDGET_H
