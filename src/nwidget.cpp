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

#include "nwidget.h"
#include "colinicons.h"
#include <QtGui/QFileDialog>
#include <QtGui/QGraphicsDropShadowEffect>


QPixmap *nWidget::back = 0;

nWidget::nWidget(QWidget *parent) :
    QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    QHBoxLayout *buttonbox = new QHBoxLayout();
	QHBoxLayout *bottombox = new QHBoxLayout();

    newB = new QPushButton(tr("new"), this);
    openB = new QPushButton(tr("open"), this);
    settingsB = new QPushButton(tr("settings"), this);
    libB = new QPushButton(tr("clear library"), this);


    //layout->setColumnMinimumWidth(0, 20);
    layout->addLayout(buttonbox, 0, 0, 1, 6, Qt::AlignTop);
    buttonbox->addWidget(newB);
    buttonbox->addWidget(openB);
    buttonbox->addWidget(settingsB);
    buttonbox->addWidget(libB);
    //layout->setColumnMinimumWidth(5, 20);
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 3);
    layout->setColumnStretch(2, 3);
    layout->setColumnStretch(3, 3);
    layout->setColumnStretch(4, 3);
    layout->setColumnStretch(5, 1);
    layout->setRowStretch(0, 1);
    layout->setRowStretch(1, 1);
    layout->setRowStretch(2, 3);
    layout->setRowStretch(3, 3);
	layout->addLayout(bottombox, 4, 0, 1, 6, Qt::AlignBottom);
    layout->setRowMinimumHeight(1, 20);



    newB->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    openB->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    settingsB->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    libB->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);


	newB->setWhatsThis("<b>"+tr("new file")+"</b><br /><br />"+
					   tr("Create a new empty file."));

	openB->setWhatsThis("<b>"+tr("open file")+"</b><br /><br />"+
						tr("Open a file dialog. Specify the file to open there."));

	settingsB->setWhatsThis("<b>"+tr("settings")+"</b> "+
							tr("<a href=\"settings\">open manual</a><br /><br />")+
							tr("Goto settings page."));

	libB->setWhatsThis("<b>"+tr("clear library")+"</b> "+
					   tr("<a href=\"library\">open manual</a><br /><br />")+
					   tr("Remove all materials and cross sections form the library. This can not be done while files are open to edit!"));

    renderer = new previewRenderer(QSize(196, 135), this);
    previewWidget::renderer = renderer;

	connect(&filelist::instance(),      SIGNAL(recUsedCanged()),
			this,						SLOT(update()));

    for(int i=0; i<8; i++)
    {
        previews[i] = new previewWidget(this);

		connect(&filelist::instance(),      SIGNAL(recUsedCanged()),
				previews[i],                SLOT(actualUrl()));

        connect(renderer,                   SIGNAL(finished(QString,QImage)),
                previews[i],                SLOT(giefPix(QString,QImage)));



	layout->addWidget(previews[i], 2+i/4, 1+i%4, 1, 1);
    }

    layout->setRowMinimumHeight(4, 140);


	QLabel *colinIcon = new QLabel(this);
	colinIcon->setPixmap(QPixmap(colinIcons::icondir_+"/tooltip/node_dark_64.png"));
	bottombox->addWidget(colinIcon, 0, Qt::AlignLeft | Qt::AlignVCenter);
	QLabel *colinText = new QLabel("[Colin]");
	QFont bigF = colinText->font();
	bigF.setPixelSize(40);
	colinText->setFont(bigF);
	bottombox->addWidget(colinText, 0, Qt::AlignLeft | Qt::AlignVCenter);
	bottombox->addStretch(1000);

	QGraphicsDropShadowEffect *eff = new QGraphicsDropShadowEffect(colinIcon);
	eff->setBlurRadius(10);
	eff->setColor(Qt::white);
	eff->setOffset(0);
	colinIcon->setGraphicsEffect(eff);

	eff = new QGraphicsDropShadowEffect(colinIcon);
	eff->setBlurRadius(10);
	eff->setColor(Qt::white);
	eff->setOffset(0);
	colinText->setGraphicsEffect(eff);



	setWallpaper = new QPushButton(tr("change wallpaper"), this);
	setWallpaper->setFlat(true);

	QFont miniFont = setWallpaper->font();
	miniFont.setPointSize(7);
	setWallpaper->setFont(miniFont);

	eff = new QGraphicsDropShadowEffect(setWallpaper);
	eff->setBlurRadius(10);
	eff->setColor(Qt::white);
	eff->setOffset(0);
	setWallpaper->setGraphicsEffect(eff);


	bottombox->addWidget(setWallpaper, 0, Qt::AlignBottom | Qt::AlignHCenter);

	bottombox->addStretch(1000);


#ifdef CLAZZES_DOT_ORG_ICON
	QLabel *clazzesText = new QLabel("clazzes.org");
	clazzesText->setFont(bigF);
	eff = new QGraphicsDropShadowEffect(clazzesText);
	eff->setBlurRadius(10);
	eff->setColor(Qt::white);
	eff->setOffset(0);
	clazzesText->setGraphicsEffect(eff);
	bottombox->addWidget(clazzesText, 0, Qt::AlignVCenter | Qt::AlignRight);

	QLabel *clazzesIcon = new QLabel(this);
	clazzesIcon->setPixmap(QPixmap(colinIcons::icondir_+"clazzes-logo-alpha_64x64.png"));
	eff = new QGraphicsDropShadowEffect(clazzesText);
	eff->setBlurRadius(10);
	eff->setColor(Qt::white);
	eff->setOffset(0);
	clazzesIcon->setGraphicsEffect(eff);
	bottombox->addWidget(clazzesIcon, 0, Qt::AlignVCenter | Qt::AlignRight);

#endif

	if(!back){
		QSettings settings("clazzes.org", "Colin");
		back = new QPixmap(settings.value("wallpaper","../graphics/ChrisM70_at_flickr.png").toString());
	}

	connect(setWallpaper,				SIGNAL(pressed()),
			this,						SLOT(openWallpaper()));

}

nWidget::~nWidget()
{
	delete back;
	back = 0;
}

void nWidget::openWallpaper()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("open picture"), QDir::homePath(), tr("Images (*.png *.xpm *.jpg)"));
	if(back)
		delete back;
	QSettings settings("clazzes.org", "Colin");
	settings.setValue("wallpaper", fileName);
	back = new QPixmap(fileName);

	update();

}

void nWidget::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(Qt::NoPen);

	p.setClipRegion(e->region());
	p.drawPixmap(0, 0, back->width(), back->height(), *back);






}
