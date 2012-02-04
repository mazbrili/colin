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

#ifndef SWIDGET_H
#define SWIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QPainter>
#include <QtGui/QIcon>
#include <QtGui/QLinearGradient>
#include <QtGui/QScrollArea>
#include <QtGui/QStackedWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QButtonGroup>
#include <QtGui/QPushButton>


#include "colorsettingswidget.h"
#include "shortcutsettingswidget.h"
#include "miscsettingswidget.h"
#include "slidingstackedwidget.h"

class sWidget : public QWidget
{
    Q_OBJECT
public:
    explicit sWidget(QWidget *parent = 0);
	void paintEvent(QPaintEvent *e);

signals:
    void closeMe();
public slots:
    void restore();
private:
	SlidingStackedWidget *central;
    colorSettingsWidget *colors;
    shortcutSettingsWidget *shortcuts;
    miscSettingsWidget *misc;
    QButtonGroup *group;
};

#endif // SWIDGET_H
