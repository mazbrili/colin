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


hingeExtended::hingeExtended(QWidget *parent):
	QGroupBox(parent)
{

	freeButtons = new QButtonGroup(this);
	springButtons = new QButtonGroup(this);
	basicButtons = new QButtonGroup(this);

	this->setTitle("["+tr("hinges")+"]");
	QVBoxLayout *layout = new QVBoxLayout(this);
	setLayout(layout);

	ColinHMultiButton *basicsM = new ColinHMultiButton(this);
	basicLeft = new ColinPushButtonPart(colinIcons::instance().icon(Colin::drawJoint), "");
	basicRight =  new ColinPushButtonPart(colinIcons::instance().icon(Colin::RightJoint), "");

	basicsM->addButton(basicLeft);
	basicsM->addButton(basicRight);

	basicLeft->setCheckable(true);
	basicRight->setCheckable(true);

	basicButtons->addButton(basicLeft, 2);
	basicButtons->addButton(basicRight, 5);

	layout->addWidget(basicsM);


	moreBox = new QGroupBox(this);

	layout->addWidget(moreBox);
	moreBox->setTitle("["+tr("more...")+"]");


	QGridLayout *mL = new QGridLayout(moreBox);
	moreBox->setLayout(mL);
	moreBox->setCheckable(true);

	springCs = new QLineEdit*[6];


	for(int i=0; i<6; i++)
	{
		ColinPushButtonPart *frees;
		ColinPushButtonPart *springs = new ColinPushButtonPart(colinIcons::instance().icon(Colin::Spring), "");

		if(i%3==0)
			frees = new ColinPushButtonPart(colinIcons::instance().icon(Colin::drawJointN), "");
		if(i%3==1)
			frees = new ColinPushButtonPart(colinIcons::instance().icon(Colin::drawJointQ), "");
		if(i%3==2)
			frees = new ColinPushButtonPart(colinIcons::instance().icon(Colin::drawJointM), "");

		ColinHMultiButton *hm = new ColinHMultiButton(moreBox);

		springCs[i] = new QLineEdit(this);
		hm->addButton(frees);
		hm->addButton(springs);
		mL->addWidget(hm, i, 0, 1, 1);
		mL->addWidget(springCs[i], i, 1, 1, 1);

		if(i%3==2)
			mL->addWidget(new QLabel(unitSettings::instance().FMeh()), i, 2, 1, 1);
		else
			mL->addWidget(new QLabel(unitSettings::instance().Feh()), i, 2, 1, 1);

		springButtons->addButton(springs, i);
		freeButtons->addButton(frees, i);

		springs->setCheckable(true);
		frees->setCheckable(true);

		connect(springCs[i],		SIGNAL(editingFinished()),
				this,				SLOT(setSpringConstants()));
	}

	basicButtons->setExclusive(false);
	springButtons->setExclusive(false);
	freeButtons->setExclusive(false);

	connect(basicButtons,			SIGNAL(buttonClicked(int)),
			this,					SLOT(setBasic()));

	connect(springButtons,			SIGNAL(buttonClicked(int)),
			this,					SLOT(setSpring()));

	connect(freeButtons,			SIGNAL(buttonClicked(int)),
			this,					SLOT(setBasic()));

	connect(moreBox,				SIGNAL(clicked(bool)),
			this,					SLOT(extended(bool)));
}

hingeExtended::~hingeExtended()
{
	delete [] springCs;
}

void hingeExtended::extended(bool show)
{
	foreach(QWidget *w, this->moreBox->findChildren<QWidget *>())
	{
		w->setShown(show);
	}
}

void hingeExtended::setCurrentItem(const int &i)
{
	if(!filelist::instance().currentFile())
		return;
	ColinStruct &t = *filelist::instance().currentFile();

	currentItem = i;

	basicLeft->setChecked(t.beam(i).joint(2));
	basicRight->setChecked(t.beam(i).joint(5));

	for(int j=0; j<6; j++)
	{
		freeButtons->button(j)->setChecked(t.beam(i).joint(j));
		springButtons->button(j)->setChecked(t.beam(i).spring(j)!=0);
	}


}

void hingeExtended::setBasic()
{
	ColinBeam b = filelist::instance().currentFile()->beam(currentItem);
	if(sender() == freeButtons)
	{
		for(int j=0; j<6; j++)
		{
			b.setJoint(j, freeButtons->button(j)->isChecked());
		}
	}
	else
	{
		b.setJoint(2, basicLeft->isChecked());
		b.setJoint(5, basicRight->isChecked());
	}
	filelist::instance().currentFile()->setBeam(currentItem, b);
}

void hingeExtended::setSpring()
{

}

void hingeExtended::setSpringConstants()
{

}

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

void beamDetail::setMode(const int &M)
{
	toDraw = static_cast<mode>(M);
	update();
}

void beamDetail::setCutAt(const double X)
{
	x = X;
	update();
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


	hinges = new hingeExtended(this);
	vl1->addWidget(hinges);

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

	modeExtern->setCheckable(true);
	modeIntern->setCheckable(true);

	modeExtern->setChecked(true);

	modeGroup = new QButtonGroup(modeButton);
	modeGroup->addButton(modeExtern, beamDetail::externForces);
	modeGroup->addButton(modeIntern, beamDetail::internForces);
	modeGroup->setExclusive(true);

	modeButton->addButton(modeExtern);
	modeButton->addButton(modeIntern);

	dl->addWidget(modeButton);

	detailWidget = new beamDetail(this);
	detailWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	QSizePolicy sP = detailWidget->sizePolicy();
	sP.setHeightForWidth(true);
	detailWidget->setSizePolicy(sP);
	dl->addWidget(detailWidget);
	vl2->addStretch(10000);


	QGroupBox *dispBox = new QGroupBox(this);
	dispBox->setTitle("["+tr("displacement")+"]");
	dispBox->setCheckable(true);

	vl3->addWidget(dispBox);
	dl = new QVBoxLayout(dispBox);
	dispBox->setLayout(dl);
	displacement = new QLabel("");
	displacement->setTextInteractionFlags(Qt::TextBrowserInteraction | Qt::TextSelectableByKeyboard);
	dl->addWidget(displacement);


	QGroupBox *efBox = new QGroupBox(this);
	efBox->setTitle("["+tr("beam forces")+"]");
	efBox->setCheckable(true);

	vl3->addWidget(efBox);
	dl = new QVBoxLayout(efBox);
	dispBox->setLayout(dl);
	endforces = new QLabel("");
	endforces->setTextInteractionFlags(Qt::TextBrowserInteraction | Qt::TextSelectableByKeyboard);
	dl->addWidget(endforces);

	vl3->addStretch(10000);



	connect(modeGroup,		SIGNAL(buttonClicked(int)),
			detailWidget,	SLOT(setMode(int)));

	connect(cls,			SIGNAL(buttonClicked(int)),
			detailWidget,	SLOT(setCurrentCLS(int)));

	connect(dispBox,		SIGNAL(clicked(bool)),
			this,			SLOT(hideMyChildren(bool)));

	connect(efBox,			SIGNAL(clicked(bool)),
			this,			SLOT(hideMyChildren(bool)));

	connect(copyButton,		SIGNAL(clicked()),
			this,			SLOT(copy()));

	connect(cutButton,		SIGNAL(clicked()),
			this,			SLOT(cut()));

	connect(&filelist::instance(),		SIGNAL(currentChanged(ColinStruct*)),
			this,						SLOT(setTw(ColinStruct*)));

	ColinStruct &t = *filelist::instance().currentFile();

	connect(&t,					SIGNAL(changedBeam(int)),
			this,				SLOT(beamChanged(int)));

	connect(&t,					SIGNAL(calculationFinished()),
			this,				SLOT(beamChanged()));

	connect(&t,					SIGNAL(addedCLS(int)),
			this,				SLOT(clsChanged()));

	connect(&t,					SIGNAL(erasedCLS(int)),
			this,				SLOT(clsChanged()));



}

void beamOverlay::setCurrentItem(const int &i)
{
	this->setFocusPolicy(Qt::StrongFocus);
	currentItem = i;
	detailWidget->setCurrentItem(i);
	hinges->setCurrentItem(i);
	clsChanged();

	this->item->setText("#"+QString::number(i));

	ColinStruct &t = *filelist::instance().currentFile();


	leftN->setText(QString::number(t.beam(i).leftNodeI()));
	rightN->setText(QString::number(t.beam(i).rightNodeI()));

	angleInfo->setText(tr("<b>apha</b>")+" = "+QString::number(t.beam(i).angle()*ANGLEPREFIX, 'f', PRECISON)+" [g]");
	lengthInfo->setText(tr("<b>length</b>")+" = "+QString::number(t.beam(i).l(), 'f', PRECISON)+" [m]");

	mat->setText(t.beam(i).Mat().name());
	cs->setText(t.beam(i).Profile().name());


	if(t.isCalculated())
	{
		QString txt;
		for(int side=0; side<2; side++)
		{
			txt.append(QString("<b>u<sub>%1</sub></b>").arg(side?"r":"l"));
			if(t.cls_n()==0){
				txt.append(QString(" = %1 m<br/>").arg(t.beam(i).u(0, side?t.beam(i).l():0)));
			}else{
				double max = 0;
				foreach(int cls, t.activeCLS()) max = qMax(fabs(t.beam(i).u(cls, side?t.beam(i).l():0)), max);
				if(qFuzzyIsNull(max)) max=0;
				txt.append(QString("<b><sub> max</sub> = %1 m</b><br/>").arg(max));
				if(max!=0){
					foreach(int cls, t.activeCLS()){
						double stress = t.beam(i).u(cls, side?t.beam(i).l():0);
						if(qFuzzyIsNull(stress)) stress = 0;
						txt.append(QString("u<sub>%1 %2</sub> = %3 m<br/>").arg(side?"r":"l").arg(cls).arg(stress));
					}
				}
				txt.append("<br/>");
			}

			txt.append(QString("<b>w<sub>%1</sub></b>").arg(side?"r":"l"));
			if(t.cls_n()==0){
				txt.append(QString(" = %1 m<br/>").arg(t.beam(i).w(0, side?t.beam(i).l():0)));
			}else{
				double max = 0;
				foreach(int cls, t.activeCLS()) max = qMax(fabs(t.beam(i).w(cls, side?t.beam(i).l():0)), max);
				if(qFuzzyIsNull(max)) max=0;
				txt.append(QString("<b><sub> max</sub> = %1 m</b><br/>").arg(max));
				if(max!=0){
					foreach(int cls, t.activeCLS()){
						double stress = t.beam(i).w(cls, side?t.beam(i).l():0);
						if(qFuzzyIsNull(stress)) stress = 0;
						txt.append(QString("w<sub>%1 %2</sub> = %3 m<br/>").arg(side?"r":"l").arg(cls).arg(stress));
					}
				}
				txt.append("<br/>");
			}

			txt.append(QString("<b>phi<sub>%1</sub></b>").arg(side?"r":"l"));
			if(t.cls_n()==0){
				txt.append(QString(" = %1 m<br/>").arg(t.beam(i).phi(0, side?t.beam(i).l():0)));
			}else{
				double max = 0;
				foreach(int cls, t.activeCLS()) max = qMax(fabs(t.beam(i).phi(cls, side?t.beam(i).l():0)), max);
				if(qFuzzyIsNull(max)) max=0;
				txt.append(QString("<b><sub> max</sub> = %1 m</b><br/>").arg(max));
				if(max!=0){
					foreach(int cls, t.activeCLS()){
						double stress = t.beam(i).phi(cls, side?t.beam(i).l():0);
						if(qFuzzyIsNull(stress)) stress = 0;
						txt.append(QString("phi<sub>%1 %2</sub> = %3 m<br/>").arg(side?"r":"l").arg(cls).arg(stress));
					}
				}
			}

		if(side==0)
			txt.append("<hr/><br/>");
		}
		displacement->setText(txt);
	}
	else
	{
		displacement->setText("N/A");
	}


	if(t.isCalculated())
	{
		QString txt;
		for(int side=0; side<2; side++)
		{
			//N
			txt.append(QString("<font color=\"%1\">")
								.arg(viewPortSettings::instance().color(Colin::C_Np).name()));
			txt.append(QString("<b>N<sub>%1</sub></b>").arg(side?"r":"l"));
			if(t.cls_n()==0){
				txt.append(QString(" = %1 %2<br/>").arg(t.beam(i).N(0, side?t.beam(i).l():0)*PPREFIX).arg(unitSettings::instance().Peh()));
			}else{
				double max = -std::numeric_limits<double>::max();
				double min = std::numeric_limits<double>::max();
				foreach(int cls, t.activeCLS()){
					max = qMax(t.beam(i).N(cls, side?t.beam(i).l():0), max);
					min = qMin(t.beam(i).N(cls, side?t.beam(i).l():0), min);
				}
				if(qFuzzyIsNull(max)) max=0;
				if(qFuzzyIsNull(min)) min=0;
				txt.append(QString("<b><sub> max</sub> = %1 %2</b><br/>").arg(max*PPREFIX).arg(unitSettings::instance().Peh()));
				txt.append(QString("<font color=\"%1\">")
									.arg(viewPortSettings::instance().color(Colin::C_Np).name()));
				txt.append(QString("<b>N<sub>%1</sub></b>").arg(side?"r":"l"));
				txt.append(QString("<b><sub> min</sub> = %1 %2</b><br/>").arg(min*PPREFIX).arg(unitSettings::instance().Peh()));
				txt.append("</font>");
				if(max!=0 || min!=0){
					foreach(int cls, t.activeCLS()){
						double stress = t.beam(i).N(cls, side?t.beam(i).l():0);
						if(qFuzzyIsNull(stress)) stress = 0;
						if(stress<0)
							txt.append(QString("<font color=\"%1\">")
												.arg(viewPortSettings::instance().color(Colin::C_Np).name()));
						txt.append(QString("N<sub>%1 %2</sub> = %3 %4<br/>").arg(side?"r":"l").arg(cls).arg(stress*PPREFIX).arg(unitSettings::instance().Peh()));
						if(stress<0)
							txt.append("</font>");
					}
				}
				txt.append("</font>");
				txt.append("<br/>");
			}

			//Q
			txt.append(QString("<font color=\"%1\">")
								.arg(viewPortSettings::instance().color(Colin::C_Qp).name()));
			txt.append(QString("<b>Q<sub>%1</sub></b>").arg(side?"r":"l"));
			if(t.cls_n()==0){
				txt.append(QString(" = %1 %2<br/>").arg(t.beam(i).Q(0, side?t.beam(i).l():0)*PPREFIX).arg(unitSettings::instance().Peh()));
			}else{
				double max = -std::numeric_limits<double>::max();
				double min = std::numeric_limits<double>::max();
				foreach(int cls, t.activeCLS()){
					max = qMax(t.beam(i).Q(cls, side?t.beam(i).l():0), max);
					min = qMin(t.beam(i).Q(cls, side?t.beam(i).l():0), min);
				}
				if(qFuzzyIsNull(max)) max=0;
				if(qFuzzyIsNull(min)) min=0;
				txt.append(QString("<b><sub> max</sub> = %1 %2</b><br/>").arg(max*PPREFIX).arg(unitSettings::instance().Peh()));
				txt.append(QString("<font color=\"%1\">")
									.arg(viewPortSettings::instance().color(Colin::C_Qp).name()));
				txt.append(QString("<b>Q<sub>%1</sub></b>").arg(side?"r":"l"));
				txt.append(QString("<b><sub> min</sub> = %1 %2</b><br/>").arg(min*PPREFIX).arg(unitSettings::instance().Peh()));
				txt.append("</font>");
				if(max!=0 || min!=0){
					foreach(int cls, t.activeCLS()){
						double stress = t.beam(i).Q(cls, side?t.beam(i).l():0);
						if(qFuzzyIsNull(stress)) stress = 0;
						if(stress<0)
							txt.append(QString("<font color=\"%1\">")
												.arg(viewPortSettings::instance().color(Colin::C_Qp).name()));
						txt.append(QString("Q<sub>%1 %2</sub> = %3 %4<br/>").arg(side?"r":"l").arg(cls).arg(stress*PPREFIX).arg(unitSettings::instance().Peh()));
						if(stress<0)
							txt.append("</font>");
					}
				}
				txt.append("</font>");
				txt.append("<br/>");
			}

			//M
			txt.append(QString("<font color=\"%1\">")
								.arg(viewPortSettings::instance().color(Colin::C_Mp).name()));
			txt.append(QString("<b>M<sub>%1</sub></b>").arg(side?"r":"l"));
			if(t.cls_n()==0){
				txt.append(QString(" = %1 %2<br/>").arg(t.beam(i).M(0, side?t.beam(i).l():0)*MPREFIX).arg(unitSettings::instance().Meh()));
			}else{
				double max = -std::numeric_limits<double>::max();
				double min = std::numeric_limits<double>::max();
				foreach(int cls, t.activeCLS()){
					max = qMax(t.beam(i).M(cls, side?t.beam(i).l():0), max);
					min = qMin(t.beam(i).M(cls, side?t.beam(i).l():0), min);
				}
				if(qFuzzyIsNull(max)) max=0;
				if(qFuzzyIsNull(min)) min=0;
				txt.append(QString("<b><sub> max</sub> = %1 %2</b><br/>").arg(max*MPREFIX).arg(unitSettings::instance().Meh()));
				txt.append(QString("<font color=\"%1\">")
									.arg(viewPortSettings::instance().color(Colin::C_Mp).name()));
				txt.append(QString("<b>M<sub>%1</sub></b>").arg(side?"r":"l"));
				txt.append(QString("<b><sub> min</sub> = %1 %2</b><br/>").arg(min*MPREFIX).arg(unitSettings::instance().Meh()));
				txt.append("</font>");
				if(max!=0 || min!=0){
					foreach(int cls, t.activeCLS()){
						double stress = t.beam(i).M(cls, side?t.beam(i).l():0);
						if(qFuzzyIsNull(stress)) stress = 0;
						if(stress<0)
							txt.append(QString("<font color=\"%1\">")
												.arg(viewPortSettings::instance().color(Colin::C_Mp).name()));
						txt.append(QString("M<sub>%1 %2</sub> = %3 %4<br/>").arg(side?"r":"l").arg(cls).arg(stress*MPREFIX).arg(unitSettings::instance().Meh()));
						if(stress<0)
							txt.append("</font>");
					}
				}
				txt.append("</font>");
				txt.append("<br/>");
			}


		if(side==0)
			txt.append("<hr/><br/>");
		}
		endforces->setText(txt);
	}
	else
	{
		endforces->setText("N/A");
	}

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
