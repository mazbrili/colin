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

#ifndef COLINMENU_H
#define COLINMENU_H

#include <QMenu>
#include <QtCore/QList>
#include <QtCore/QPair>
#include <QPainter>
#include <QStyleOptionMenuItem>
#include <QPaintEvent>

typedef QPair<QAction *, QWidget *> ActionWidgetPair;

class ColinMenu : public QMenu
{
    Q_OBJECT
public:
    explicit ColinMenu(QWidget *parent = 0) : QMenu(parent) {}
    ColinMenu(const QString &title, QWidget *parent = 0) : QMenu(title, parent) {}

    inline bool addActionWidgetPair(ActionWidgetPair pair){
        addAction(pair.first); return addWidgetForAction(pair.first, pair.second);}
    bool addWidgetForAction(QAction *action, QWidget *widget);
    void removeWidgetFromAction(QAction *action);

    void paintEvent(QPaintEvent *e);
    void showEvent(QShowEvent *);

private:
    void setChildrenGeometry();
    int mWidth;

public slots:
    inline void removeWidgetFromAction(QObject *action){
        removeWidgetFromAction(dynamic_cast<QAction*>(action)); }

private:
    QList<QPair<QAction *, QWidget *> > actionWidgets;

};

#endif // COLINMENU_H
