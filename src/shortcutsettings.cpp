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

#include "shortcutsettings.h"


shortcutSettings *shortcutSettings::instance_ = NULL;



void shortcutSettings::addAction(QAction *a, const bool &needsFile)
{

    QSettings settings("clazzes.org", "Colin");
    QKeySequence key = a->shortcut();
    a->setShortcut(QKeySequence(settings.value(QString("shortcuts/%1").arg(a->text()),
					       QVariant(key.toString())).toString()));

    actions_.append(act(a, key, needsFile));
}

void shortcutSettings::setShortut(const int &i, const QKeySequence &c)
{
    actions_.at(i).a->setShortcut(c);
}

void shortcutSettings::restore(const int &i)
{
    actions_.at(i).a->setShortcut(actions_.at(i).system);
}

void shortcutSettings::loadSettings()
{
    for(int i=0; i<actions_.size(); i++)
        restore(i);
	QSettings settings("clazzes.org", "Colin");

	menusBesideActions = settings.value("tools/menusbeside", false).toBool();

}


QAction *shortcutSettings::actionWithData(const Colin::otherAction &a)const
{
    foreach(act ac, actions_)
    {
        Colin::otherAction  data = static_cast<Colin::otherAction>(ac.a->data().toInt());
        if(data == a)
        {
            return ac.a;
        }
    }
    return 0;
}

void shortcutSettings::saveSettings() const
{

    QSettings settings("clazzes.org", "Colin");
    for(int i=0; i<actions_.size(); i++)
    {
        settings.setValue(QString("shortcuts/%1").arg(actions_.at(i).a->text()),QVariant(actions_.at(i).a->shortcut().toString(QKeySequence::PortableText)));
    }
}

void shortcutSettings::hasFile(const bool &t)
{
    foreach(act a, actions_)
    {
        if(a.reqFile)
            a.a->setDisabled(!t);
    }
}

void shortcutSettings::setMenuBeside(bool beside)
{
	menusBesideActions=beside;
	emit menusBesideChanged(beside);
}

bool shortcutSettings::menuBeside() const
{
	return menusBesideActions;
}

QAction *shortcutSettings::lastAction() const
{
	return actions_.last().a;
}
