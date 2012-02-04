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
#include "nwidget.h"

#include <QtGui/QGraphicsDropShadowEffect>
#include <QtGui/QGraphicsBlurEffect>


sWidget::sWidget(QWidget *parent) :
    QWidget(parent)
{
    group = new QButtonGroup(this);     //these two together kick asses!!!1!11
	central = new SlidingStackedWidget(this);


	central->setAnimation(QEasingCurve::Linear);
	central->setSpeed(200);


	QPalette pal = this->palette();
	pal.setColor(QPalette::Base, QColor(255, 255, 255, 50));
	pal.setColor(QPalette::Window, QColor(255, 255, 255, 50));


	QPalette pal2 = this->palette();
	pal2.setColor(QPalette::Base, QColor(255, 255, 255, 0));
	pal2.setColor(QPalette::Window, QColor(255, 255, 255, 0));

	//QGraphicsDropShadowEffect *eff = new QGraphicsDropShadowEffect();
	//eff->setBlurRadius(20);
	//eff->setColor(Qt::white);
	//eff->setOffset(0);
    colors = new colorSettingsWidget(this);
	//colors->setGraphicsEffect(eff);
	colors->setPalette(pal2);
    QPushButton *colorB = new QPushButton(tr("colors"), this);
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidget(colors);
	scrollArea->setWidgetResizable(true);
	group->addButton(colorB, central->addWidget(scrollArea));
	scrollArea->setPalette(pal);

	//eff = new QGraphicsDropShadowEffect();
	//eff->setBlurRadius(20);
	//eff->setColor(Qt::white);
	//eff->setOffset(0);
	shortcuts = new shortcutSettingsWidget(this);
	//shortcuts->setGraphicsEffect(eff);
	QPushButton *shortcutB = new QPushButton(tr("tools"), this);
    scrollArea = new QScrollArea(this);
	scrollArea->setPalette(pal);
    scrollArea->setWidget(shortcuts);
    group->addButton(shortcutB, central->addWidget(scrollArea));
	shortcuts->setPalette(pal2);


	//eff = new QGraphicsDropShadowEffect();
	//eff->setBlurRadius(20);
	//eff->setColor(Qt::white);
	//eff->setOffset(0);
    misc = new miscSettingsWidget(this);
	//misc->setGraphicsEffect(eff);
    QPushButton *miscB = new QPushButton(tr("miscellaneous"), this);
    scrollArea = new QScrollArea(this);
	scrollArea->setPalette(pal);
    scrollArea->setWidget(misc);
    group->addButton(miscB, central->addWidget(scrollArea));
	misc->setPalette(pal2);


    QPushButton *restoreB = new QPushButton(tr("restore Settings"), this);


    foreach(QAbstractButton *b, group->buttons())
    {
        b->setCheckable(true);
    }
    group->setExclusive(true);
    colorB->setChecked(true);
    central->setCurrentIndex(0);

	colorB->setFlat(true);
	shortcutB->setFlat(true);
	miscB->setFlat(true);
	restoreB->setFlat(true);

    connect(group,                  SIGNAL(buttonClicked(int)),
			central,                SLOT(slideInIdx(int)));

    connect(restoreB,		    SIGNAL(clicked()),
	    this,		    SLOT(restore()));

    QGridLayout *layout = new QGridLayout(this);

	layout->setMargin(10);
    layout->addWidget(colorB, 0, 0, 1, 1);
    layout->addWidget(shortcutB, 0, 1, 1, 1);
    layout->addWidget(miscB, 0, 2, 1, 1);
    layout->addWidget(restoreB, 0, 3, 1, 1);

    layout->addWidget(central, 1, 0, 1, 4);


}

void sWidget::paintEvent(QPaintEvent *e)
{

	const QPixmap *back = nWidget::back;
	const QPushButton *colorB = qobject_cast<QPushButton*>(group->button(0));
	if(!colorB)
		return;

	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setPen(Qt::NoPen);

	p.setClipRegion(e->region());
	p.drawPixmap(0, 0, back->width(), back->height(), *back);


	QLinearGradient grad(0, 10, 0, 10+colorB->geometry().bottom());
	grad.setColorAt(0, palette().color(QPalette::Light));
	grad.setColorAt(1, palette().color(QPalette::Window));
	p.setPen(palette().color(QPalette::Dark));
	p.setBrush(grad);
	p.drawRect(QRect(0, 10, width(), colorB->height()));


}

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
