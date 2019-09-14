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

#ifndef DRAWSELECTIONWIDGET_H
#define DRAWSELECTIONWIDGET_H

#include <QWidget>
#include "viewportsettings.h"
#include "colinicons.h"
#include <QMouseEvent>
#include <QPainter>


class drawSelectionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit drawSelectionWidget(int _id, QWidget *parent = 0);

    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

    void paintEvent(QPaintEvent *);

    void leaveEvent(QEvent *);
    void enterEvent(QEvent *);

    QSize sizeHint() const;
signals:

public slots:

private:
    int id;
    bool mouseOver;
    bool mousePressed;
    QRect uR, nR, qR, mR,       //u, N, Q, M
          bR, jR, kR, lR;       //bearings, joints, nodes, loads


};

#endif // DRAWSELECTIONWIDGET_H
