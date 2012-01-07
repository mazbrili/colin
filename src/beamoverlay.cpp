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

#include "beamoverlay.h"

#include <QtGui/QScrollArea>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QButtonGroup>
#include <QtGui/QGroupBox>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <limits>

#include "detailpainter.h"
#include "filelist.h"
#include "colinicons.h"
#include "unitsettings.h"

beamDetail::beamDetail(QWidget *parent)
{
	currentItem = -1;
	currentCLS = 0;
	toDraw = externForces;
	x=0;
}

void beamDetail::paintEvent(QPaintEvent *e)
{
	QPainter p(this);
	if(currentItem<0)
		return;
	detailPainter dP;
	switch(toDraw)
	{
	case externForces:
		dP.drawBeamExtern(&p, *filelist::instance().currentFile(), currentItem, QList<int>() << currentCLS);
	case internForces:
		dP.drawBeamIntern(&p, *filelist::instance().currentFile(), currentItem, x, QList<int>() << currentCLS);
	default:
		dP.drawBeamFunctions(&p, *filelist::instance().currentFile(), currentItem, QList<int>() << currentCLS);
	}


}

void beamDetail::setCurrentItem(const int &i)
{
	currentItem = i;
	update();
}

void beamDetail::setCurrentCLS(const int &i)
{
	currentCLS = i;
	update();
}

void beamDetail::setMode(const mode &M)
{
	toDraw = M;
}

void beamDetail::setCutAt(const double X)
{
	x = X;
}

beamOverlay::beamOverlay(QWidget *parent) :
	abstractOverlay(parent)
{
	item->setIcon(colinIcons::instance().icon(Colin::drawBeam));
	QVBoxLayout *l = static_cast<QVBoxLayout*>(layout());

	QHBoxLayout *hl = new QHBoxLayout();
	l->addLayout(hl);

	QScrollArea *scrollArea = new QScrollArea(this);
	QWidget *area = new QWidget(scrollArea);
	scrollArea->setWidgetResizable(true);
	scrollArea->setFrameStyle(0);
	scrollArea->setWidget(area);
	scrollArea->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
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

	QGroupBox *nodes = new QGroupBox(this);
	vl1->addWidget(nodes, Qt::AlignLeft);

	nodes->setTitle("["+tr("nodes")+"]");
	nodes->setMinimumWidth(250);

	QGridLayout *nl = new QGridLayout(nodes);
	nodes->setLayout(nl);
	leftNlabel = new QLabel(tr("from"));
	leftNlabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	leftN = new QLineEdit();
	nl->addWidget(leftNlabel, 0, 0, 1, 1);
	nl->addWidget(leftN, 0, 1, 1, 1);


	rightNlabel = new QLabel(tr("to"));
	rightNlabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	rightN = new QLineEdit();
	nl->addWidget(rightNlabel, 1, 0, 1, 1);
	nl->addWidget(rightN, 1, 1, 1, 1);


	QGroupBox *geoBox = new QGroupBox(this);
	vl1->addWidget(geoBox, Qt::AlignLeft);

	geoBox->setTitle("["+tr("geometrie")+"]");
	geoBox->setMinimumWidth(250);

	QVBoxLayout *gl = new QVBoxLayout(geoBox);
	geoBox->setLayout(gl);
	angleInfo = new QLabel(geoBox);
	lengthInfo = new QLabel(geoBox);
	gl->addWidget(angleInfo);
	gl->addWidget(lengthInfo);



	QGroupBox *propBox = new QGroupBox(this);
	vl1->addWidget(propBox, Qt::AlignLeft);

	propBox->setTitle("["+tr("properties")+"]");
	propBox->setMinimumWidth(250);

	QGridLayout *pl = new QGridLayout(propBox);
	propBox->setLayout(pl);


	matlabel = new QLabel(tr("material"));
	cslabel = new QLabel(tr("cross section"));

	mat = new QPushButton(colinIcons::instance().icon(Colin::Material), "");
	cs = new QPushButton(colinIcons::instance().icon(Colin::Profile), "");

	mat->setFlat(true);
	cs->setFlat(true);

	pl->addWidget(matlabel, 0, 0, 1, 1);
	pl->addWidget(cslabel, 1, 0, 1, 1);
	pl->addWidget(mat, 0, 1, 1, 1);
	pl->addWidget(cs, 1, 1, 1, 1);

	QGroupBox *clipbox = new QGroupBox(this);
	vl1->addWidget(clipbox, Qt::AlignLeft);

	clipbox->setTitle("["+tr("clipboard")+"]");
	clipbox->setMinimumWidth(250);

	copyButton = new ColinPushButtonPart(colinIcons::instance().icon(Colin::Copy), tr("copy"));
	cutButton = new ColinPushButtonPart(colinIcons::instance().icon(Colin::Cut), tr("cut"));

	hl = new QHBoxLayout(clipbox);
	clipbox->setLayout(hl);
	ColinHMultiButton *clip = new ColinHMultiButton(clipbox);
	clip->addButton(copyButton);
	clip->addButton(cutButton);
	hl->addWidget(clip);

	vl1->addStretch(1000);

	QGroupBox *detailbox = new QGroupBox(this);

	detailbox->setTitle("["+tr("view")+"]");

	QVBoxLayout *dl = new QVBoxLayout(detailbox);
	detailbox->setLayout(dl);
	vl2->addWidget(detailbox);

	clsButton = new ColinHMultiButton(this);
	cls = new QButtonGroup(this);
	cls->setExclusive(true);
	dl->addWidget(clsButton);
	clsButton->hide();

	ColinHMultiButton *modeButton = new ColinHMultiButton(detailbox);
	modeExtern = new ColinPushButtonPart(tr("extern"), modeButton);
	modeIntern = new ColinPushButtonPart(tr("intern"), modeButton);
	modeFunctions = new ColinPushButtonPart(tr("functions"), modeButton);

	modeExtern->setCheckable(true);
	modeIntern->setCheckable(true);
	modeFunctions->setCheckable(true);

	modeExtern->setChecked(true);

	modeGroup = new QButtonGroup(modeButton);
	modeGroup->addButton(modeExtern, beamDetail::externForces);
	modeGroup->addButton(modeIntern, beamDetail::internForces);
	modeGroup->addButton(modeFunctions, beamDetail::functions);
	modeGroup->setExclusive(true);

	modeButton->addButton(modeExtern);
	modeButton->addButton(modeIntern);
	modeButton->addButton(modeFunctions);

	dl->addWidget(modeButton);

	detailWidget = new beamDetail(this);
	detailWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	QSizePolicy sP = detailWidget->sizePolicy();
	sP.setHeightForWidth(true);
	detailWidget->setSizePolicy(sP);
	dl->addWidget(detailWidget);
	vl2->addStretch(10000);

}

void beamOverlay::setCurrentItem(const int &i)
{
	this->setFocusPolicy(Qt::StrongFocus);
	currentItem = i;
	detailWidget->setCurrentItem(i);
	clsChanged();

	this->item->setText("#"+QString::number(i));

	ColinStruct &t = *filelist::instance().currentFile();

	if(filelist::instance().currentFile()){
		connect(&t,					SIGNAL(changedBeam(int)),
				this,				SLOT(beamChanged(int)));

		connect(&t,					SIGNAL(calculationFinished()),
				this,				SLOT(beamChanged()));

		connect(&t,					SIGNAL(addedCLS(int)),
				this,				SLOT(clsChanged()));

		connect(&t,					SIGNAL(erasedCLS(int)),
				this,				SLOT(clsChanged()));

	}

	leftN->setText(QString::number(t.beam(i).leftNodeI()));
	rightN->setText(QString::number(t.beam(i).rightNodeI()));

	angleInfo->setText(tr("<b>apha</b>")+" = "+QString::number(t.beam(i).angle()*ANGLEPREFIX, 'f', PRECISON)+" [g]");
	lengthInfo->setText(tr("<b>length</b>")+" = "+QString::number(t.beam(i).l(), 'f', PRECISON)+" [m]");

	mat->setText(t.beam(i).Mat().name());
	cs->setText(t.beam(i).Profile().name());


}

void beamOverlay::changed()
{

}

void beamOverlay::setTw(ColinStruct *t)
{
	this->deleteLater();
}

void beamOverlay::beamChanged()
{
	beamChanged(currentItem);
}

void beamOverlay::beamChanged(int i)
{
	if(i==currentItem)
		this->setCurrentItem(i);
}

void beamOverlay::nextItem()
{
	if(currentItem == filelist::instance().currentFile()->beam_n()-1)
		setCurrentItem(0);
	else
		setCurrentItem(currentItem+1);
}

void beamOverlay::previousItem()
{
	if(currentItem == 0)
		setCurrentItem(filelist::instance().currentFile()->beam_n()-1);
	else
		setCurrentItem(currentItem-1);
}

void beamOverlay::clsChanged()
{

	ColinStruct &t = *filelist::instance().currentFile();

	int current = cls->checkedId();
	if(!(current>-1 && current<t.cls_n()))
		current = 0;

	clsButton->clear();


	if(t.cls_n()==0)
	{
		clsButton->hide();
		return;
	}

	for(int i=0; i<t.cls_n(); i++)
	{
		ColinPushButtonPart *b = new ColinPushButtonPart(colinIcons::instance().icon(Colin::CLS), t.cls(i).name(), clsButton);
		b->setCheckable(true);
		if(i==current)
			b->setChecked(true);
		cls->addButton(b, i);
		clsButton->addButton(b);
	}
	 clsButton->show();
	 foreach(QAbstractButton *b, cls->buttons())
		 b->setShown(true);
}
void beamOverlay::copy()
{
	toClipBoard();
	filelist::instance().currentFile()->selectNode(currentItem, false);
	this->deleteLater();

}
void beamOverlay::cut()
{
	toClipBoard();
	filelist::instance().currentFile()->removeNode(currentItem);
	this->deleteLater();
}

void beamOverlay::toClipBoard()
{
	QClipboard *clipboard = QApplication::clipboard();
	QByteArray data;
	QMimeData *mimeData = new QMimeData();
	XmlWriter writer(&data);
	ColinStruct *tw = filelist::instance().currentFile();
	tw->selectNode(currentItem, true);
	writer.writeSelection(*tw, tw->node(currentItem).toQPointF());
	mimeData->setData("text/colinfile", data);
	clipboard->setMimeData(mimeData);
	return;
}
