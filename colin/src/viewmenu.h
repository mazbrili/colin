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

#ifndef VIEWMENU_H
#define VIEWMENU_H

#include "colinmenu.h"
#include "scalewidget.h"
#include "filelist.h"

class viewMenu : public ColinMenu
{
    Q_OBJECT
public:
    explicit viewMenu(QWidget *parent = 0);
    viewMenu(const QString &title, QWidget *parent = 0);
    QAction *zoomAll,
            *zoomIn,
            *zoomOut;
    scaleWidget *uScale,
                *pScale,
                *mScale;

public slots:
    void setTw(wgv_tw *tw);
    void dumpTw(QObject *o);

    //scaleSlots connected with the struct
    void scaleU(const double &scale);
    void scaleP(const double &scale);
    void scaleM(const double &scale);

    //scaleSlots connected with the sliders
    void scaleU(const int &scale);
    void scaleP(const int &scale);
    void scaleM(const int &scale);

    //scaleUpDownSlots connected with the buttons
    void scaleDownU();
    void scaleUpU();
    void scaleDownP();
    void scaleUpP();
    void scaleDownM();
    void scaleUpM();

    //resets the Range for all sliders
    void setRanges();

    void autoScaleU();
    void autoScaleP();
    void autoScaleM();

private:
    void init();

    bool adjust;

    wgv_tw *t;



};

#endif // VIEWMENU_H
