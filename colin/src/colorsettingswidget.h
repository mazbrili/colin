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

#ifndef COLORSETTINGSWIDGET_H
#define COLORSETTINGSWIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QColorDialog>
#include <QtGui/QPushButton>
#include <QtGui/QButtonGroup>
#include <QtGui/QKeyEvent>

#include "viewportsettings.h"
#include "colinicons.h"
#include "grid.h"

class colorSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit colorSettingsWidget(QWidget *parent = 0);
    void keyPressEvent(QKeyEvent *e);

signals:

public slots:

private slots:
    void setColor(const int &i);
    void changeCurrentColor(const QColor &c);

private:
    QColorDialog *dialog;
    QButtonGroup *group;


};

#endif // COLORSETTINGSWIDGET_H
