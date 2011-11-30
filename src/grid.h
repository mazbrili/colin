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

#ifndef COLINGRID_H
#define COLINGRID_H

#include <QtGui/QPixmap>
#include <QtGui/QPainter>
#include <QtCore/QSettings>
#include <cmath>

#include "viewportsettings.h"


class grid : public QObject
{
    Q_OBJECT
public:
    static grid& instance()
    {
        if(!instance_)
            instance_ = new grid();
        return *instance_;
    }
    QPixmap gridmap(double *zoomf);
    void drawCordsys(QPainter *p);
    void drawScale(QPainter *p, double scale);
    bool gridCatch(QPointF *point, const QTransform &t, const int &gridRange_);
    bool gridCatch(double *length, double z, const int &gridRange_);
    double mod(double dend, double visor);

    const QColor &backgroundColor()  const { return viewPortSettings::instance().color(Colin::C_Background);}
    const QColor &gridColor()  const{ return viewPortSettings::instance().color(Colin::C_Grid);}
    const int &mAbs() const;

    void loadSettings();
    void saveSettings();
    void setAbs(const int &i);

signals:
    void settingschanged();
private:
    Q_DISABLE_COPY(grid)
    grid():QObject(){loadSettings();}
    ~grid(){saveSettings();}
    static grid *instance_;
    int maxabs;
    int minabs;
    double pFac;



};

#endif // COLINGRID_H
