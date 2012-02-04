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

#include "loadoverlay.h"

#include <QtGui/QScrollArea>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QButtonGroup>
#include <QtGui/QGroupBox>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QComboBox>
#include <limits>

#include "detailpainter.h"
#include "filelist.h"
#include "colinicons.h"
#include "unitsettings.h"

loadOverlay::loadOverlay(QWidget *parent):
	abstractOverlay(parent)
{

	item->setIcon(colinIcons::instance().icon(Colin::drawNLoad));
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

	QGroupBox *posBox =  new QGroupBox(this);
	posBox->setTitle("["+tr("position")+"]");

	vl1->addWidget(posBox);

	QHBoxLayout *posL = new QHBoxLayout(posBox);
	posBox->setLayout(posL);
	posLabel = new QLabel();
	posL->addWidget(posLabel);
	pos = new QLineEdit();
	posL->addWidget(pos);



	QGroupBox *typBox = new QGroupBox(this);
	typBox->setTitle("["+tr("type")+"]");

	vl1->addWidget(typBox);

	QVBoxLayout *typL = new QVBoxLayout(typBox);
	typBox->setLayout(typL);

	typeGroup = new QButtonGroup(this);
	typeGroup->setExclusive(true);

	force = new ColinPushButtonPart(colinIcons::instance().icon(Colin::drawNLoad), tr("nodal load"));
	moment = new ColinPushButtonPart(colinIcons::instance().icon(Colin::drawMoment), tr("moment"));

	nodeLoads = new ColinHMultiButton(typBox);
	nodeLoads->addButton(force);
	nodeLoads->addButton(moment);

	typL->addWidget(nodeLoads);

	typeGroup->addButton(force, ColinLoad::nodeLoad);
	typeGroup->addButton(moment, ColinLoad::moment);

	increasing = new ColinPushButtonPart(colinIcons::instance().icon(Colin::drawILoad), tr("increasing"));
	uniform = new ColinPushButtonPart(colinIcons::instance().icon(Colin::drawULoad), tr("uniformly"));
	decreasing = new ColinPushButtonPart(colinIcons::instance().icon(Colin::drawDLoad), tr("decreasing"));

	increasing->setCheckable(true);
	uniform->setCheckable(true);
	decreasing->setCheckable(true);

	linearLoads = new ColinHMultiButton(typBox);
	linearLoads->addButton(increasing);
	linearLoads->addButton(uniform);
	linearLoads->addButton(decreasing);
	typL->addWidget(linearLoads);


	typeGroup->addButton(increasing, ColinLoad::increasingLinearLoad);
	typeGroup->addButton(uniform, ColinLoad::uniformlyDistibutedLoad);
	typeGroup->addButton(decreasing, ColinLoad::decreasingLinearLoad);

	leftSide = new ColinPushButtonPart(colinIcons::instance().icon(Colin::leftSide), tr("left Side"));
	rightSide = new ColinPushButtonPart(colinIcons::instance().icon(Colin::rightSide), tr("rightSide"));

	leftSide->setCheckable(true);
	rightSide->setCheckable(true);

	side = new ColinHMultiButton(typBox);
	side->addButton(leftSide);
	side->addButton(rightSide);
	typL->addWidget(side);

	typeGroup->addButton(leftSide, ColinLoad::doubleLoadLeft);
	typeGroup->addButton(rightSide, ColinLoad::doubleLoadRight);

	tempChange = new ColinPushButtonPart(colinIcons::instance().icon(ColinLoad::tempChange), tr("change"));
	tempDifference = new ColinPushButtonPart(colinIcons::instance().icon(ColinLoad::tempDiffrence), tr("difference"));

	tempChange->setCheckable(true);
	tempDifference->setCheckable(true);

	typeGroup->addButton(tempChange, ColinLoad::tempChange);
	typeGroup->addButton(tempDifference, ColinLoad::tempDiffrence);

	temps = new ColinHMultiButton(typBox);
	temps->addButton(tempChange);
	temps->addButton(tempDifference);
	typL->addWidget(temps);

	QGroupBox *values =  new QGroupBox(this);
	values->setTitle("["+tr("loads")+"]");

	vl1->addWidget(values);

	QGridLayout *valL = new QGridLayout(values);
	values->setLayout(valL);

	xL = new QLabel("P<sub>x</sub>");
	zL = new QLabel("P<sub>z</sub>");
	mL = new QLabel("M");
	valL->addWidget(xL, 0, 0, 1, 1);
	valL->addWidget(zL, 1, 0, 1, 1);
	valL->addWidget(mL, 2, 0, 1, 1);

	x = new QLineEdit();
	z = new QLineEdit();
	m = new QLineEdit();

	valL->addWidget(x, 0, 1, 1, 1);
	valL->addWidget(z, 1, 1, 1, 1);
	valL->addWidget(m, 2, 1, 1, 1);

	xEH = new QLabel();
	zEH = new QLabel();
	mEH = new QLabel();

	valL->addWidget(xEH, 0, 2, 1, 1);
	valL->addWidget(zEH, 1, 2, 1, 1);
	valL->addWidget(mEH, 2, 2, 1, 1);


	QGroupBox *setBox = new QGroupBox(this);
	setBox->setTitle("["+tr("load set")+"]");

	vl1->addWidget(setBox);

	QHBoxLayout *setL = new QHBoxLayout(setBox);
	setBox->setLayout(setL);
	setLabel = new QLabel(tr("set"), setBox);
	set = new QComboBox(setBox);

	setL->addWidget(setLabel);
	setL->addWidget(set);


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


	vl1->addStretch(10000);

	connect(pos,			SIGNAL(editingFinished()),
			this,			SLOT(changed()));

	connect(x,				SIGNAL(editingFinished()),
			this,			SLOT(changed()));

	connect(z,				SIGNAL(editingFinished()),
			this,			SLOT(changed()));

	connect(m,				SIGNAL(editingFinished()),
			this,			SLOT(changed()));

	connect(typeGroup,		SIGNAL(buttonClicked(int)),
			this,			SLOT(setType(int)));

	connect(set,			SIGNAL(activated(int)),
			this,			SLOT(changed()));


	connect(copyButton,		SIGNAL(clicked()),
			this,			SLOT(copy()));

	connect(cutButton,		SIGNAL(clicked()),
			this,			SLOT(cut()));

	connect(&filelist::instance(),		SIGNAL(currentChanged(ColinStruct*)),
			this,						SLOT(setTw(ColinStruct*)));

	ColinStruct &t = *filelist::instance().currentFile();

	connect(&t,					SIGNAL(changedLoad(int)),
			this,				SLOT(loadChanged(int)));

	connect(&t,					SIGNAL(calculationFinished()),
			this,				SLOT(loadChanged()));

	connect(&t,					SIGNAL(addedBLS(int)),
			this,				SLOT(clsChanged()));

	connect(&t,					SIGNAL(erasedBLS(int)),
			this,				SLOT(clsChanged()));

	connect(&t,					SIGNAL(edited(int,ColinBLS)),
			this,				SLOT(clsChanged()));



}



void loadOverlay::setUnits()
{
	if(!filelist::instance().currentFile())
		return;
	ColinStruct &t = *filelist::instance().currentFile();

#ifdef OVERLAY_VERBOSE
	qDebug() << "loadOverlay::setting Layout for tpye " << t.load(currentItem).typ();
#endif
	xL->show();
	zL->show();
	mL->show();
	x->show();
	z->show();
	m->show();
	xEH->show();
	zEH->show();
	mEH->show();

	nodeLoads->hide();
	linearLoads->hide();
	temps->hide();
	side->hide();

	switch(t.load(currentItem).typ())
	{
	case ColinLoad::uniformlyDistibutedLoad:
	case ColinLoad::increasingLinearLoad:
	case ColinLoad::decreasingLinearLoad:
		posLabel->setText(tr("on beam"));
		xEH->setText(unitSettings::instance().Peh()+"/m");
		zEH->setText(unitSettings::instance().Peh()+"/m");
		xL->show();
		zL->show();
		x->show();
		z->show();
		xEH->show();
		zEH->show();

		mL->hide();
		m->hide();
		mEH->hide();
		linearLoads->show();
		return;
	case ColinLoad::moment:
		posLabel->setText(tr("on node"));
		mL->show();
		m->show();
		mEH->show();

		xL->hide();
		zL->hide();
		x->hide();
		z->hide();
		xEH->hide();
		zEH->hide();
		mEH->setText(unitSettings::instance().Meh());
		return;
	case ColinLoad::nodeLoad:
		posLabel->setText(tr("on node"));
		xEH->setText(unitSettings::instance().Peh());
		zEH->setText(unitSettings::instance().Peh());
		mL->show();
		m->show();
		mEH->show();

		mL->hide();
		m->hide();
		mEH->hide();
		return;
	case ColinLoad::doubleLoadLeft:
	case ColinLoad::doubleLoadRight:
		posLabel->setText(tr("on beam"));
		xEH->setText(unitSettings::instance().Peh());
		zEH->setText(unitSettings::instance().Peh());
		mEH->setText(unitSettings::instance().Meh());
		xL->show();
		zL->show();
		mL->show();
		x->show();
		z->show();
		m->show();
		xEH->show();
		zEH->show();
		mEH->show();

		side->show();
		return;
	case ColinLoad::tempChange:
		posLabel->setText(tr("on beam"));
		xEH->setText("K");
		xL->show();
		x->show();
		xEH->show();

		zL->hide();
		mL->hide();
		z->hide();
		m->hide();
		zEH->hide();
		mEH->hide();
		temps->show();
		return;
	case ColinLoad::tempDiffrence:
		posLabel->setText(tr("on beam"));
		zEH->setText("K");
		zL->show();
		z->show();
		zEH->show();

		xL->hide();
		mL->hide();
		x->hide();
		m->hide();
		xEH->hide();
		mEH->hide();
		temps->show();
		return;
	default:
		return;
	}
}

void loadOverlay:: setType(int t)
{
#ifdef OVERLAY_VERBOSE
	qDebug() << "loadOverlay::setting Type to " << t;
#endif
	filelist::instance().currentFile()->setLoadTyp(currentItem, static_cast<ColinLoad::form>(t));
}


void loadOverlay::nextItem()
{
	if(currentItem == filelist::instance().currentFile()->load_n()-1)
		setCurrentItem(0);
	else
		setCurrentItem(currentItem+1);
}

void loadOverlay::previousItem()
{
	if(currentItem == 0)
		setCurrentItem(filelist::instance().currentFile()->load_n()-1);
	else
		setCurrentItem(currentItem-1);
}


void loadOverlay::clsChanged()
{
	this->set->clear();
	ColinStruct &t = *filelist::instance().currentFile();
	set->addItem(colinIcons::instance().icon(Colin::Close), tr("none"));
	for(int i=0; i<t.bls_n(); i++)
	{
		set->addItem(colinIcons::instance().icon(Colin::BLS), t.bls(i).name());
	}
	set->setCurrentIndex(t.load(currentItem).set()+1);
}

void loadOverlay::setCurrentItem(const int &i)
{

#ifdef OVERLAY_VERBOSE
	qDebug() << "loadOverlay::setting currentItem to" << i;
#endif
	blockSignals(true);
	item->setText(QString("#%1").arg(i));





	const ColinLoad &load = filelist::instance().currentFile()->load(i);
	pos->setText(QString::number(load.at()));
	x->setText(QString::number(load.Px()*PPREFIX, 'f', PRECISON));
	z->setText(QString::number(load.Pz()*PPREFIX, 'f', PRECISON));
	m->setText(QString::number(load.M()*MPREFIX, 'f', PRECISON));
	set->setCurrentIndex(load.set()+1);

	typeGroup->button(load.typ())->setChecked(true);

	currentItem = i;

	clsChanged();

	setUnits();

	blockSignals(false);
}

void loadOverlay::changed()
{
#ifdef OVERLAY_VERBOSE
	qDebug() << "loadOverlay::chaning load";
#endif
	bool ok;
	if(sender() == pos)
	{
		int newPos = pos->text().toInt(&ok);
		if(!ok && newPos != filelist::instance().currentFile()->load(currentItem).at())
			pos->setText(QString::number(filelist::instance().currentFile()->load(currentItem).at()));
		else
			filelist::instance().currentFile()->setPos(currentItem, newPos);
	}
	else if(sender() == x || sender() == z || sender() == m)
	{
		double newVal = static_cast<QLineEdit*>(sender())->text().toDouble(&ok);
		if(!ok)
			setCurrentItem(currentItem);
		else
		{
			if(sender() == x && newVal != filelist::instance().currentFile()->load(currentItem).Px())
				filelist::instance().currentFile()->setPx(currentItem, newVal);
			if(sender() == z && newVal != filelist::instance().currentFile()->load(currentItem).Pz())
				filelist::instance().currentFile()->setPz(currentItem, newVal);
			if(sender() == m && newVal != filelist::instance().currentFile()->load(currentItem).M())
				filelist::instance().currentFile()->setM(currentItem, newVal);

		}
	}
	else if(sender() == set)
	{
		int newBLS = filelist::instance().currentFile()->getBLSIDbyName(set->currentText());
		filelist::instance().currentFile()->setLoadSet(currentItem, newBLS);
	}
}

void loadOverlay::setTw(ColinStruct *t)
{
	this->deleteLater();
}

void loadOverlay::loadChanged()
{
	setCurrentItem(currentItem);
}

void loadOverlay::loadChanged(int i)
{
	if(i==currentItem)
		loadChanged();
}

void loadOverlay::copy()
{
	toClipBoard();
	this->deleteLater();
}

void loadOverlay::cut()
{
	toClipBoard();
	filelist::instance().currentFile()->removeLoad(currentItem);
	this->deleteLater();
}

void loadOverlay::toClipBoard()
{
	QClipboard *clipboard = QApplication::clipboard();
	QByteArray data;
	QMimeData *mimeData = new QMimeData();
	XmlWriter writer(&data);
	writer.writeLoad(*filelist::instance().currentFile(), currentItem);
	mimeData->setData("text/colinfile", data);
	clipboard->setMimeData(mimeData);
	return;
}
