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

#ifndef SHORTCUTSETTINGS_H
#define SHORTCUTSETTINGS_H

#include <QtCore/QList>
#include <QtCore/QSettings>
#include <QtGui/QAction>
#include "colin_namespace.h"

class act{
public:
    act(QAction *a_, const QKeySequence &system_, const bool &nF)
        {a=a_;system=system_;reqFile=nF;}
    QAction *a;
    QKeySequence system;
    bool reqFile;
};

class shortcutSettings
{
public:
    static shortcutSettings &instance()
    {
        if(!instance_)
            instance_ = new shortcutSettings();
        return *instance_;
    }

    void addAction(QAction *a, const bool &needsFile = false);
    void setShortut(const int &i, const QKeySequence &c);
    void restore(const int &i);
    void loadSettings();
    void saveSettings() const;
    const act &action(const int &i)const {return actions_.at(i);}
    QAction *actionWithData(const Colin::otherAction &a)const;
    int Count() const {return actions_.size();}
    const QList<act> &actions() const {return actions_;}
    void hasFile(const bool &t);

signals:

public slots:

private:
    shortcutSettings(){}

    QList<act> actions_;
    static shortcutSettings *instance_;
};

#endif // SHORTCUTSETTINGS_H
