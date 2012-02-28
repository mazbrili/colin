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
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QFileDialog>

#include "previewrenderer.h"
#include "colinpastebuffer.h"
#include "pastepreviewwidget.h"


bool generalOverlay::showPaste = true;
bool generalOverlay::showPicture = true;
bool generalOverlay::showCls = true;
bool generalOverlay::showBls = true;

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


	exportBox = new QGroupBox(this);
	exportBox->setTitle("["+tr("export picture")+"]");
	exportBox->setCheckable(true);
	QGridLayout *exportLayout = new QGridLayout(exportBox);
	exportBox->setLayout(exportLayout);
	vl2->addWidget(exportBox);


	vl2->addStretch(10000);

	exportPreview = new QLabel(exportBox);
	exportPreview->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

	saveB = new QPushButton(tr("save as picture"), exportBox);
	leftMargin = new QDoubleSpinBox(exportBox);
	rightMargin = new QDoubleSpinBox(exportBox);
	topMargin = new QDoubleSpinBox(exportBox);
	bottomMargin = new QDoubleSpinBox(exportBox);

	QLabel *leftL = new QLabel(tr("left"), exportBox);
	QLabel *rightL = new QLabel(tr("right"), exportBox);
	QLabel *topL = new QLabel(tr("top"), exportBox);
	QLabel *bottomL = new QLabel(tr("bottom"), exportBox);

	leftL->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	rightL->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	topL->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	bottomL->setAlignment(Qt::AlignRight | Qt::AlignVCenter);


	xRes = new QDoubleSpinBox(exportBox);
	yRes = new QDoubleSpinBox(exportBox);

	QList <QDoubleSpinBox*> spinBoxes;
	spinBoxes << leftMargin << rightMargin << topMargin << bottomMargin;
	foreach(QDoubleSpinBox *sB, spinBoxes)
	{
		sB->setMinimum(-1000);
		sB->setMaximum(1000);
		sB->setSingleStep(0.1);
	}

	xRes->setMaximum(10000);
	xRes->setMinimum(100);
	yRes->setMaximum(10000);
	yRes->setMinimum(100);


	QLabel *xResL = new QLabel(tr("resolution (x)"), exportBox);
	QLabel *yResL = new QLabel(tr("resolution (z)"), exportBox);

	xResL->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	yResL->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

	preScale = new QDoubleSpinBox(exportBox);

	QLabel *preL = new QLabel(tr("scale text"), exportBox);
	preL->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

	exportLayout->addWidget(exportPreview, 0, 0, 1, 4);

	exportLayout->addWidget(leftL, 2, 0, 1, 1);
	exportLayout->addWidget(rightL, 3, 0, 1, 1);
	exportLayout->addWidget(topL, 4, 0, 1, 1);
	exportLayout->addWidget(bottomL, 5, 0, 1, 1);
	exportLayout->addWidget(leftMargin, 2, 1, 1, 1);
	exportLayout->addWidget(rightMargin, 3, 1, 1, 1);
	exportLayout->addWidget(topMargin, 4, 1, 1, 1);
	exportLayout->addWidget(bottomMargin, 5, 1, 1, 1);

	exportLayout->addWidget(xResL, 2, 2, 1, 1);
	exportLayout->addWidget(yResL, 3, 2, 1, 1);
	exportLayout->addWidget(xRes, 2, 3, 1, 1);
	exportLayout->addWidget(yRes, 3, 3, 1, 1);

	exportLayout->addWidget(preL, 4, 2, 1, 1);
	exportLayout->addWidget(preScale, 4, 3, 1, 1);


	exportLayout->addWidget(saveB, 5, 2, 1, 2);


	clsets = new QGroupBox(this);
	clsets->setTitle("["+tr("combined load sets")+"]");
	clsets->setCheckable(true);
	QHBoxLayout *clsetslayout = new QHBoxLayout(clsets);

	clsView = new QListWidget(clsets);
	clsView->setFrameStyle(QFrame::NoFrame);
	clsetslayout->addWidget(clsView, 1);
	vl3->addWidget(clsets);


	blsets = new QGroupBox(this);
	blsets->setTitle("["+tr("basic load sets")+"]");
	blsets->setCheckable(true);
	QHBoxLayout *blsetslayout = new QHBoxLayout(blsets);

	blsView = new QListWidget(blsets);
	blsView->setFrameStyle(QFrame::NoFrame);
	blsetslayout->addWidget(blsView);
	vl3->addWidget(blsets);

	vl3->addStretch(10000);

	currentItem = -1;

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

	connect(leftMargin,				SIGNAL(valueChanged(double)),
			this,					SLOT(updatePicture()));

	connect(rightMargin,			SIGNAL(valueChanged(double)),
			this,					SLOT(updatePicture()));

	connect(topMargin,				SIGNAL(valueChanged(double)),
			this,					SLOT(updatePicture()));

	connect(bottomMargin,			SIGNAL(valueChanged(double)),
			this,					SLOT(updatePicture()));

	connect(preScale,				SIGNAL(valueChanged(double)),
			this,					SLOT(updatePicture()));

	connect(xRes,					SIGNAL(valueChanged(double)),
			this,					SLOT(updatePicture()));

	connect(yRes,					SIGNAL(valueChanged(double)),
			this,					SLOT(updatePicture()));

	connect(saveB,					SIGNAL(clicked()),
			this,					SLOT(savePicture()));

	connect(exportBox,				SIGNAL(clicked(bool)),
			this,					SLOT(hideMyChildren(bool)));

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

	connect(tw,						SIGNAL(changedMScale(double)),
			this,					SLOT(updatePicture()));

	connect(tw,						SIGNAL(changedPScale(double)),
			this,					SLOT(updatePicture()));

	connect(tw,						SIGNAL(changedUScale(double)),
			this,					SLOT(updatePicture()));

	connect(&viewPortSettings::instance(),	SIGNAL(settingschanged()),
			this,							SLOT(updatePicture()));

	paste->setChecked(generalOverlay::showPaste);
	exportBox->setChecked(generalOverlay::showPicture);
	clsets->setChecked(generalOverlay::showCls);
	blsets->setChecked(generalOverlay::showBls);

	QList<QGroupBox*> boxes;
	boxes << paste << exportBox << clsets << blsets;

	foreach(QGroupBox *gb, boxes)
	{
		if(!gb->isChecked())
		{
			foreach(QWidget *w, gb->findChildren<QWidget* >())
				w->setHidden(true);
		}
	}

}

generalOverlay::~generalOverlay()
{
	generalOverlay::showPaste = paste->isChecked();
	generalOverlay::showPicture = exportBox->isChecked();
	generalOverlay::showCls = clsets->isChecked();
	generalOverlay::showBls = blsets->isChecked();
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

}

void generalOverlay::setCurrentItem(const int &i)
{
	QRectF bR = filelist::instance().currentFile()->boundingRect();
	bR.adjust(-0.5, -0.5, 0.5, 0.5);

	topMargin->setValue(bR.top());
	leftMargin->setValue(bR.left());
	rightMargin->setValue(bR.right());
	bottomMargin->setValue(bR.bottom());

	xRes->setValue(1000);
	yRes->setValue(1000*bR.height()/bR.width());
	preScale->setValue(1);

	exporter = pictureExport(filelist::instance().currentFile(),
							 viewPortSettings::instance().elements(i),
							 bR,
							 1,
							 QSizeF(1000, 1000*bR.height()/bR.width()));
	currentItem=i;

	updatePicture();
}

void generalOverlay::updatePicture()
{

	if(currentItem==-1)
		return;
	xRes->blockSignals(true);
	yRes->blockSignals(true);
	QRectF bR = QRectF(leftMargin->value(),
					   topMargin->value(),
					   rightMargin->value()-leftMargin->value(),
					   bottomMargin->value()-topMargin->value());

	if(sender() == leftMargin	||
	   sender() == rightMargin	||
	   sender() == topMargin	||
	   sender() == bottomMargin	)
	{
		exporter.setBoundingRect(bR);
	}
	else if(sender() == preScale)
	{
		exporter.setPreScale(preScale->value());
	}
	else if(sender() == xRes)
	{
		yRes->setValue(bR.height()/bR.width()*xRes->value());
		exporter.setResolution(QSizeF(xRes->value(), yRes->value()));
	}
	else if(sender() == yRes)
	{
		xRes->setValue(1./bR.height()*bR.width()*yRes->value());
		exporter.setResolution(QSizeF(xRes->value(), yRes->value()));
	}

	xRes->blockSignals(false);
	yRes->blockSignals(false);

	QPixmap pix = exporter.preview().scaledToWidth(exportPreview->width()-5);
	QPainter p(&pix);
	p.setPen(palette().color(QPalette::Dark));
	p.drawRoundedRect(QRect(0, 0, pix.width()-1, pix.height()-1), 4, 4);

#ifdef EXPORT_VERBOSE
	qDebug() << "exportPreview->width() = "  << exportPreview->width();
	qDebug() << "exportBox->width() = "  << exportBox->width();
#endif

	exportPreview->setPixmap(pix);

}

void generalOverlay::savePicture()
{
	QString selectedFilter;
	QString filename = QFileDialog::getSaveFileName(this, tr("save picture"), QDir::homePath(), tr("Portable Network Graphics(*.png);; JPEG(*.jpg)"), &selectedFilter);

	if(!filename.isEmpty()){
		if(!filename.contains("png") && selectedFilter.contains("png"))
			filename.append(".png");
		if(!filename.contains("jpg") && selectedFilter.contains("jpg"))
			filename.append(".jpg");
		exporter.save(filename);
	}
}

void generalOverlay::setTw(ColinStruct *tw)
{
	deleteLater();
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
