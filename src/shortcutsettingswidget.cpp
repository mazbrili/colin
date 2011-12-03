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

#include "shortcutsettingswidget.h"

shortcutSettingsWidget::shortcutSettingsWidget(QWidget *parent) :
    QWidget(parent)
{
    shortcutSettings &scS = shortcutSettings::instance();
    layout = new QGridLayout(this);


    old = -1;

    setter = new QButtonGroup(this);
    restorer = new QButtonGroup(this);

    QPushButton *setB = NULL;
    QPushButton *restoreB = NULL;
    QLabel *icon = NULL;
    QLabel *name = NULL;
    QLabel *shortcut = NULL;


	tools = new ColinBoolSlider(this);
	toolLabel = new QLabel(tr("menus beside action"));

	tools->setCheckable(true);
	tools->setChecked(shortcutSettings::instance().menuBeside());


	layout->addWidget(toolLabel, 0, 3, 1, 1);
	layout->addWidget(tools, 0, 4, 1, 1);

    editor = new QLineEdit(this);
    editor->hide();
	int i = 1;
    foreach(act a, scS.actions())
    {
        setB = new QPushButton(tr("change"), this);
        setB->setFixedWidth(180);
        restoreB = new QPushButton(tr("restore"), this);

        icon = new QLabel(this);
        icon->setPixmap(a.a->icon().pixmap(32));
        icon->setFixedHeight(32);

        name = new QLabel(a.a->text(), this);

        shortcut = new QLabel(this);
        shortcut->setPixmap(colinIcons::instance().icon(a.a->shortcut(), true));

        shortcuts.append(shortcut);


        setter->addButton(setB, i);
        restorer->addButton(restoreB, i);

        layout->addWidget(icon, i, 0, 1, 1);
        layout->addWidget(name, i, 1, 1, 1);
        layout->addWidget(shortcut, i, 2, 1, 1);
        layout->addWidget(setB, i, 3, 1, 1);
        layout->addWidget(restoreB, i, 4, 1, 1);

        i++;
    }

    QLabel *explain = new QLabel(this);

	explain->setText(tr("Use the appelations like \"Ctrl\", \"Alt\"and \"Space\" \nor simply characters divided with  \"+\" to change the shortcuts:\n\n"));

    layout->addWidget(explain, 1, 6, i, 1, Qt::AlignTop);

    layout->setColumnMinimumWidth(5, 50);


	connect(setter,							SIGNAL(buttonClicked(int)),
			this,							SLOT(buttonClicked(int)));

	connect(editor,							SIGNAL(returnPressed()),
			this,							SLOT(setShortCut()));

	connect(restorer,						SIGNAL(buttonClicked(int)),
			this,							SLOT(restoreShortCut(int)));

	connect(tools,							SIGNAL(clicked(bool)),
			&shortcutSettings::instance(),	SLOT(setMenuBeside(bool)));
}

void shortcutSettingsWidget::buttonClicked(const int &i)
{
    if(old != -1)
        setter->button(old)->show();
    setter->button(i)->hide();
    layout->addWidget(editor, i, 3, 1, 1);
    editor->setText(shortcutSettings::instance().action(i).a->shortcut().toString(QKeySequence::NativeText));
    editor->show();
    editor->setFocus();
    old = i;

}

void shortcutSettingsWidget::setShortCut()
{

    QKeySequence newOne(editor->text());

    int i=0;
    foreach(act a, shortcutSettings::instance().actions())
    {
        if(a.a->shortcut() == newOne && old != i && newOne.toString() != "")
        {
            if(!QMessageBox::question(this,
                                      tr("shortcut in use!"),
                                      QString(tr("The shortcut you choose is ocupied by the action")+" \"%1\".\n"+tr("Replace?")).arg(a.a->text()),
                                      tr("yes"),
                                      tr("no")))
            {
                return;
            }

        }
        i++;
    }
    shortcutSettings::instance().setShortut(old, newOne);
    setter->button(old)->show();

    if(old==shortcutSettings::instance().Count()-1)
        setter->button(0)->setFocus();
    else
        setter->button(old+1)->setFocus();


    shortcuts[old]->setPixmap(colinIcons::instance().icon(newOne, true));


    old = -1;
    editor->hide();


}


void shortcutSettingsWidget::restoreShortCut(const int &i)
{
    shortcutSettings::instance().restore(i);
    shortcuts[i]->setPixmap(colinIcons::instance().icon(
            shortcutSettings::instance().action(i).a->shortcut(), true));

}
