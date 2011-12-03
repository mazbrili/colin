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

#include "swidget.h"
#include "unitsettings.h"
#include "viewportsettings.h"
#include "shortcutsettings.h"

sWidget::sWidget(QWidget *parent) :
    QWidget(parent)
{
    group = new QButtonGroup(this);     //these two together kick asses!!!1!11
    central = new QStackedWidget(this);

    colors = new colorSettingsWidget(this);
    QPushButton *colorB = new QPushButton(tr("colors"), this);
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidget(colors);
    scrollArea->setWidgetResizable(true);
    group->addButton(colorB, central->addWidget(scrollArea));

    shortcuts = new shortcutSettingsWidget(this);
	QPushButton *shortcutB = new QPushButton(tr("tools"), this);
    scrollArea = new QScrollArea(this);
    scrollArea->setWidget(shortcuts);
    group->addButton(shortcutB, central->addWidget(scrollArea));

    misc = new miscSettingsWidget(this);
    QPushButton *miscB = new QPushButton(tr("miscellaneous"), this);
    scrollArea = new QScrollArea(this);
    scrollArea->setWidget(misc);
    group->addButton(miscB, central->addWidget(scrollArea));


    QPushButton *restoreB = new QPushButton(tr("restore Settings"), this);


    foreach(QAbstractButton *b, group->buttons())
    {
        b->setCheckable(true);
    }
    group->setExclusive(true);
    colorB->setChecked(true);
    central->setCurrentIndex(0);

    connect(group,                  SIGNAL(buttonClicked(int)),
            central,                SLOT(setCurrentIndex(int)));

    connect(restoreB,		    SIGNAL(clicked()),
	    this,		    SLOT(restore()));

    QGridLayout *layout = new QGridLayout(this);


    layout->addWidget(colorB, 0, 0, 1, 1);
    layout->addWidget(shortcutB, 0, 1, 1, 1);
    layout->addWidget(miscB, 0, 2, 1, 1);
    layout->addWidget(restoreB, 0, 3, 1, 1);

    layout->addWidget(central, 1, 0, 1, 4);



}

/*
void sWidget::paintEvent(QPaintEvent *)
{
    //set painter & pen
    QPainter p(this);
    QFont f = p.font();
    f.setPixelSize(60);
    p.setFont(f);
    QLinearGradient grad;
    grad.setStart(0, height()-15-128);
    grad.setFinalStop(0, height()-15);
    grad.setColorAt(0, QColor(30, 30, 30));
    grad.setColorAt(1, QColor(90, 90, 90));
    p.setPen(QPen(QBrush(grad), 2));

    //draw Icon and text
    QIcon i("./icons/settings_128.png");
    i.paint(&p, QRect(15, height()-15-128, 128, 128));
    p.drawText(QRect(15+128, height()-15-128,  width(), 128),
               "["+tr("settings")+"]", Qt::AlignVCenter | Qt::AlignLeft );

}*/

void sWidget::restore()
{
    QSettings settings("clazzes.org", "Colin");
    settings.remove("colors");
    settings.remove("elements");
    settings.remove("modifier");
    settings.remove("drawing");
    settings.remove("EH");
    unitSettings::instance().loadSettings();
    viewPortSettings::instance().loadSettings();
    shortcutSettings::instance().loadSettings();
    emit closeMe();
}
