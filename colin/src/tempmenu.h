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

#ifndef TEMPMENU_H
#define TEMPMENU_H

#include <QtGui/QLineEdit>
#include <QtGui/QButtonGroup>

#include "colinmenu.h"

#include "colinhmultibutton.h"


class tempMenu : public ColinMenu
{
    Q_OBJECT
public:
    static tempMenu &instance(){
        if(!instance_)
            instance_ = new tempMenu();
        return *instance_;
    }

    void set(const int &i);
signals:

public slots:

private slots:
    void formChanged(const int &i);
    void posChanged();
    void tempChanged();
    void actionTriggered();
private:
    int cTemp;
    tempMenu();
    static tempMenu *instance_;
    QLineEdit *temp,
              *pos;
    ColinPushButtonPart *h,
                        *v;
    QButtonGroup *g;
};

#endif // TEMPMENU_H
