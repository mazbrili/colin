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

#ifndef LOADMENU_H
#define LOADMENU_H

#include <QLineEdit>
#include <QButtonGroup>
#include <QComboBox>

#include "colinmenu.h"
#include "wgv_tw.h"
#include "filelist.h"
#include "colinicons.h"

#include "colinhmultibutton.h"


class loadMenu : public ColinMenu
{
    Q_OBJECT
public:
    static loadMenu &instance(){
        if(!instance_)
            instance_ = new loadMenu();
        return *instance_;
    }

    void set(const int &i);
signals:

public slots:

private slots:
    void xChanged();
    void zChanged();
    void posChanged();
    void formChanged(const int &i);
    void actionTriggered();
private:
    int cLoad;
    loadMenu();
    static loadMenu *instance_;
    QLineEdit *x,
              *z,
              *pos;
    ColinPushButtonPart *increasing,
                        *decreasing,
                        *uniformly;
    QButtonGroup *g;
};

#endif // LOADMENU_H
