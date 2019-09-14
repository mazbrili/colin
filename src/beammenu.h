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

#ifndef BEAMMENU_H
#define BEAMMENU_H

#include <QLineEdit>
#include <QButtonGroup>
#include <QComboBox>
#include <QMimeData>

#include "colinmenu.h"
#include "wgv_tw.h"
#include "filelist.h"
#include "colinicons.h"

#include "colinhmultibutton.h"


class beamMenu : public ColinMenu
{
    Q_OBJECT
public:
    static beamMenu &instance(){
        if(!instance_)
            instance_ = new beamMenu();
        return *instance_;
    }

    void set(const int &i);
signals:

public slots:

private slots:
    void leftChanged();
    void rightChanged();
    void matChanged(const int &i);
    void profChanged(const int &i);
    void lJChanged();
    void rJChanged();
    void someJointChanged();
    void actionTriggered();
private:
    int cBeam;
    beamMenu();
    static beamMenu *instance_;
    QLineEdit *left,
              *right;
    ColinPushButtonPart *lJ,
                        *rJ;
    QMenu *jointM;
    QComboBox *mat,
              *prof;

    ColinMenu *jointMenu;

    QButtonGroup *g;
    QComboBox* springs[6];
};

#endif // BEAMMENU_H
