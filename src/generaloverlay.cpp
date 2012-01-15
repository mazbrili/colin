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

#include "generaloverlay.h"

#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QScrollArea>
#include <QtGui/QGroupBox>
#include <QtGui/QApplication>

#include "previewrenderer.h"
#include "colinpastebuffer.h"
#include "pastepreviewwidget.h"

generalOverlay::generalOverlay(QWidget *parent):
	abstractOverlay(parent)
{
	before->parentWidget()->hide();
	QLabel *title = new QLabel("["+tr("general settings")+"]", this);
	QFont tf = title->font();
	tf.setBold(true);
	title->setFont(tf);
	headerlayout->insertWidget(2, title);


	QVBoxLayout *l = static_cast<QVBoxLayout*>(layout());

	QHBoxLayout *hl = new QHBoxLayout();
	l->addLayout(hl);

	QScrollArea *scrollArea = new QScrollArea(this);
	QWidget *area = new QWidget(scrollArea);
	scrollArea->setWidgetResizable(true);
	scrollArea->setFrameStyle(0);
	scrollArea->setWidget(area);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	scrollArea->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
	QVBoxLayout *vl1 = new QVBoxLayout(area);
	area->setLayout(vl1);
	hl->addWidget(scrollArea);

	scrollArea = new QScrollArea(this);
	area = new QWidget(scrollArea);
	scrollArea->setWidgetResizable(true);
	scrollArea->setFrameStyle(0);
	scrollArea->setWidget(area);
	QVBoxLayout *vl2 = new QVBoxLayout(area);
	area->setLayout(vl2);
	hl->addWidget(scrollArea);


	scrollArea = new QScrollArea(this);
	area = new QWidget(scrollArea);
	scrollArea->setWidgetResizable(true);
	scrollArea->setFrameStyle(0);
	scrollArea->setWidget(area);
	scrollArea->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
	QVBoxLayout *vl3 = new QVBoxLayout(area);
	area->setLayout(vl3);
	hl->addWidget(scrollArea);

	hl->setStretch(0, 100);
	hl->setStretch(1, 160);
	hl->setStretch(2, 100);


	paste = new QGroupBox(this);

	paste->setTitle("["+tr("paste")+"]");
	paste->setCheckable(true);
	QVBoxLayout *pasteLayout = new QVBoxLayout(paste);
	paste->setLayout(pasteLayout);
	vl1->addWidget(paste);

	connect(paste,					SIGNAL(clicked(bool)),
			this,					SLOT(hideMyChildren(bool)));

	vl1->addStretch(10000);

	clipBoardChanged();

	connect(&ColinPasteBuffer::instance(),		SIGNAL(changedBuffer()),
			this,								SLOT(clipBoardChanged()),
			Qt::QueuedConnection);
}


void generalOverlay::clipBoardChanged()
{
	QVBoxLayout *layout = static_cast<QVBoxLayout*>(paste->layout());
	foreach(QWidget *w, paste->findChildren<QWidget*>())
	{
		w->deleteLater();
	}
	for(int i=0; i<ColinPasteBuffer::instance().size(); i++)
	{
		pastePreviewWidget *w = new pastePreviewWidget(i, this);
		layout->addWidget(w);
		connect(w,					SIGNAL(pasteRequest()),
				this,				SLOT(deleteLater()));
	}
}
