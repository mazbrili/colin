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

#ifndef NWIDGET_H
#define NWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include "previewrenderer.h"

#include "previewwidget.h"

#define CLAZZES_DOT_ORG_ICON

class nWidget : public QWidget
{
    Q_OBJECT
public:
    explicit nWidget(QWidget *parent = 0);

    void paintEvent(QPaintEvent *);
signals:
public slots:

public:
    QPushButton *newB,
                *openB,
                *settingsB,
                *libB;
private:
    previewWidget *previews[8];
    previewRenderer *renderer;

    QPixmap colinicon;
#ifdef CLAZZES_DOT_ORG_ICON
    QPixmap clazzesicon;
#endif



};

#endif // NWIDGET_H
