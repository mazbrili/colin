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
#include <QtGui/QListWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QSlider>

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

	vl1->addStretch(10000);




	QGroupBox *clsets = new QGroupBox(this);
	clsets->setTitle("["+tr("combined load sets")+"]");
	clsets->setCheckable(true);
	QHBoxLayout *clsetslayout = new QHBoxLayout(clsets);

	clsView = new QListWidget(clsets);
	clsetslayout->addWidget(clsView, 1);

	QVBoxLayout *clsEditorWraperLayout = new QVBoxLayout();
	clsEditor = new QWidget();
	clsEditorLayout = new QGridLayout(clsEditor);
	clsetslayout->addLayout(clsEditorWraperLayout, 4);
	clsEditorWraperLayout->addWidget(clsEditor);
	clsEditorWraperLayout->addStretch(1000);



	vl2->addWidget(clsets);



	QGroupBox *blsets = new QGroupBox(this);
	blsets->setTitle("["+tr("basic load sets")+"]");
	blsets->setCheckable(true);
	QHBoxLayout *blsetslayout = new QHBoxLayout(blsets);

	blsView = new QListWidget(blsets);
	blsetslayout->addWidget(blsView);


	vl2->addWidget(blsets);

	vl2->addStretch(10000);

	clipBoardChanged();
	clsChanged();

	connect(&ColinPasteBuffer::instance(),		SIGNAL(changedBuffer()),
			this,								SLOT(clipBoardChanged()),
			Qt::QueuedConnection);

	connect(paste,					SIGNAL(clicked(bool)),
			this,					SLOT(hideMyChildren(bool)));

	connect(blsets,					SIGNAL(clicked(bool)),
			this,					SLOT(hideMyChildren(bool)));

	connect(clsets,					SIGNAL(clicked(bool)),
			this,					SLOT(hideMyChildren(bool)));

	connect(clsView,				SIGNAL(activated(QModelIndex)),
			this,					SLOT(loadCLS(QModelIndex)));

	ColinStruct *tw = filelist::instance().currentFile();

	connect(tw,						SIGNAL(addedBLS(int)),
			this,					SLOT(clsChanged()));

	connect(tw,						SIGNAL(addedCLS(int)),
			this,					SLOT(clsChanged()));

	connect(tw,						SIGNAL(erasedBLS(int)),
			this,					SLOT(clsChanged()));

	connect(tw,						SIGNAL(erasedCLS(int)),
			this,					SLOT(clsChanged()));

	connect(tw,						SIGNAL(changedBLS(int)),
			this,					SLOT(clsChanged()));

	connect(tw,						SIGNAL(changedCLS(int)),
			this,					SLOT(clsChanged()));


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


void generalOverlay::loadCLS(QModelIndex clsIndex)
{
	loadCLS(clsIndex.row());
}

void generalOverlay::loadCLS(int clsIndex)
{
	foreach(QWidget *w, clsEditor->findChildren<QWidget*>())
	{
		w->deleteLater();
	}
	ColinStruct *tw = filelist::instance().currentFile();

	const ColinCLS &cls = tw->cls(clsIndex);

	QLabel *txt = new QLabel("<b>"+QString(tr("cls %1: %2")).arg(clsIndex).arg(cls.name())+"</b>", this);
	clsEditorLayout->addWidget(txt, 0, 0, 1, 3);

	for(int i=0; i<tw->cls(clsIndex).count(); i++)
	{
		QLabel *blsName = new QLabel(tw->bls(cls.bls(i)).name(), this);
		QSlider *factor = new QSlider(this);
		factor->setMinimum(0);
		factor->setMaximum(10);
		factor->setOrientation(Qt::Horizontal);
		QPushButton *remove = new QPushButton(colinIcons::instance().icon(Colin::Close), "", this);
		remove->setFlat(true);
		clsEditorLayout->addWidget(blsName, i+1, 0, 1, 1);
		clsEditorLayout->addWidget(factor, i+1, 1, 1, 1);
		clsEditorLayout->addWidget(remove, i+1, 2, 1, 1);

	}
}

void generalOverlay::clsChanged()
{
	blsView->clear();
	clsView->clear();


	ColinStruct *tw = filelist::instance().currentFile();

	for(int i=0; i<tw->bls_n(); i++)
	{
		QListWidgetItem *item = new QListWidgetItem(blsView);
		item->setText(tw->bls(i).name());
		item->setIcon(colinIcons::instance().icon(tw->bls(i).color()));
		item->setBackground(QBrush(QColor(0, 0, 0, 0)));
		blsView->addItem(item);
	}
	for(int i=0; i<tw->cls_n(); i++)
	{
		QListWidgetItem *item = new QListWidgetItem(clsView);
		item->setText(tw->cls(i).name());
		item->setIcon(colinIcons::instance().icon(Colin::CLS));
		item->setBackground(QBrush(QColor(0, 0, 0, 0)));
		clsView->addItem(item);
	}
}
