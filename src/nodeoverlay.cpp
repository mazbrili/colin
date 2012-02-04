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

#include "nodeoverlay.h"
#include <QtGui/QPainter>
#include <QtGui/QLinearGradient>
#include <QtGui/QGridLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QGroupBox>
#include <QtGui/QScrollArea>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QLineEdit>
#include <QtGui/QLabel>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <limits>

#include "colinicons.h"
#include "colinstruct.h"
#include "detailpainter.h"
#include "unitsettings.h"


bool nodeOverlay::showReactions = true;
bool nodeOverlay::showDisplacement = true;
bool nodeOverlay::showBeamForces = true;
bool nodeOverlay::showSupportExtended = false;


nodeDetail::nodeDetail(QWidget *parent):
	quadWidget(parent)
{
	currentItem = -1;
	currentCLS = 0;
}

void nodeDetail::paintEvent(QPaintEvent *e)
{
	QPainter p(this);
	p.setBrush(QColor(255, 255, 255, 100));
	p.setPen(palette().color(QPalette::Dark));
	p.drawRoundedRect(rect().adjusted(0, 0, -1, -1), 4, 4);
	if(currentItem<0)
		return;
	detailPainter dP;
	dP.drawNode(&p, *filelist::instance().currentFile(), currentItem, QList<int>() << currentCLS);
}

void nodeDetail::setCurrentItem(const int &i)
{
	currentItem = i;
	update();
}

void nodeDetail::setCurrentCLS(const int &i)
{
	currentCLS = i;
	update();
}

supportExtended::supportExtended(QWidget *parent) :
	QGroupBox(parent)
{
	QGridLayout *layout = new QGridLayout(this);
	this->setLayout(layout);
	this->setTitle("["+tr("support")+"]");


	h = new ColinPushButtonPart(colinIcons::instance().icon(Colin::drawBearingH), "");
	v = new ColinPushButtonPart(colinIcons::instance().icon(Colin::drawBearingV), "");
	m = new ColinPushButtonPart(colinIcons::instance().icon(Colin::drawBearingM), "");

	h->setWhatsThis(tr("<b>Horizontal support</b> ")+
					tr("<a href=\"menu/node\">open manual</a><br /><br />")+
					tr("Check or uncheck to add or remove a support on this node!"));
	v->setWhatsThis(tr("<b>Vertical support</b> ")+
					tr("<a href=\"menu/node\">open manual</a><br /><br />")+
					tr("Check or uncheck to add or remove a support on this node!"));
	m->setWhatsThis(tr("<b>Torsion support</b> ")+
					tr("<a href=\"menu/node\">open manual</a><br /><br />")+
					tr("Check or uncheck to add or remove a support on this node!"));

	ColinHMultiButton *supp = new ColinHMultiButton(this);
	supp->addButton(h);
	supp->addButton(v);
	supp->addButton(m);

	layout->addWidget(supp, 0, 0, 1, 3);


	morebox = new QGroupBox(this);
	morebox->setWhatsThis(tr("<b>Add springs</b> ")+
						  tr("<a href=\"menu/node\">open manual</a><br /><br />")+
						  tr("Click to enlarge! ")+
						  tr("Here you can add springs to your supports.<br />")+
						  tr("To do so, click on the spring button and insert the stiffness of the spring in the line right beside it!"));
	layout->addWidget(morebox, 1, 0, 1, 3);
	layout = new QGridLayout(morebox);
	morebox->setLayout(layout);
	morebox->setTitle("["+tr("springs")+"]");
	morebox->setCheckable(true);
	morebox->setChecked(false);


	hlock = new ColinPushButtonPart(colinIcons::instance().icon(Colin::drawBearingH), "");
	hspring = new ColinPushButtonPart(colinIcons::instance().icon(Colin::Spring), "");
	vlock = new ColinPushButtonPart(colinIcons::instance().icon(Colin::drawBearingV), "");
	vspring = new ColinPushButtonPart(colinIcons::instance().icon(Colin::Spring), "");
	mlock = new ColinPushButtonPart(colinIcons::instance().icon(Colin::drawBearingM), "");
	mspring = new ColinPushButtonPart(colinIcons::instance().icon(Colin::Spring), "");

	hc = new QLineEdit(this);
	vc = new QLineEdit(this);
	mc = new QLineEdit(this);

	hUnit = new QLabel("["+unitSettings::instance().Feh()+"]", this);
	vUnit = new QLabel("["+unitSettings::instance().Feh()+"]", this);
	mUnit = new QLabel("["+unitSettings::instance().FMeh()+"]", this);

	ColinHMultiButton *hm = new ColinHMultiButton(this);
	ColinHMultiButton *vm = new ColinHMultiButton(this);
	ColinHMultiButton *mm = new ColinHMultiButton(this);

	hm->addButton(hlock);
	hm->addButton(hspring);

	vm->addButton(vlock);
	vm->addButton(vspring);

	mm->addButton(mlock);
	mm->addButton(mspring);

	layout->addWidget(hm, 0, 0, 1, 1);
	layout->addWidget(vm, 1, 0, 1, 1);
	layout->addWidget(mm, 2, 0, 1, 1);

	layout->addWidget(hc, 0, 1, 1, 2);
	layout->addWidget(vc, 1, 1, 1, 2);
	layout->addWidget(mc, 2, 1, 1, 2);

	layout->addWidget(hUnit, 0, 3, 1, 1);
	layout->addWidget(vUnit, 1, 3, 1, 1);
	layout->addWidget(mUnit, 2, 3, 1, 1);



	basicsupport = new QButtonGroup(this);
	basicsupport->setExclusive(false);
	basicsupport->addButton(h, ColinSupport::X);
	basicsupport->addButton(v, ColinSupport::Z);
	basicsupport->addButton(m, ColinSupport::Phi);

	basicsupport2 = new QButtonGroup(this);
	basicsupport2->setExclusive(false);
	basicsupport2->addButton(hlock, ColinSupport::X);
	basicsupport2->addButton(vlock, ColinSupport::Z);
	basicsupport2->addButton(mlock, ColinSupport::Phi);

	springsupport = new QButtonGroup(this);
	springsupport->setExclusive(false);
	springsupport->addButton(hspring, ColinSupport::fx);
	springsupport->addButton(vspring, ColinSupport::fz);
	springsupport->addButton(mspring, ColinSupport::fphi);

	foreach(QPushButton *b, this->findChildren<QPushButton*>())
		b->setCheckable(true);




	this->setMinimumWidth(250);

	connect(morebox,			SIGNAL(clicked(bool)),
			this,				SLOT(extended(bool)));

	connect(basicsupport,		SIGNAL(buttonClicked(int)),
			this,				SLOT(setBasic()));

	connect(basicsupport2,		SIGNAL(buttonClicked(int)),
			this,				SLOT(setBasic()));

	connect(springsupport,		SIGNAL(buttonClicked(int)),
			this,				SLOT(setSpring()));

	connect(hc,					SIGNAL(editingFinished()),
			this,				SLOT(setSpringConstants()));

	connect(vc,					SIGNAL(editingFinished()),
			this,				SLOT(setSpringConstants()));

	connect(mc,					SIGNAL(editingFinished()),
			this,				SLOT(setSpringConstants()));

}

void supportExtended::extended(bool show)
{
	foreach(QWidget *w, this->morebox->findChildren<QWidget *>())
	{
		w->setShown(show);
	}
}

void supportExtended::setCurrentItem(const int &i)
{

	currentItem = i;
	ColinSupport::bearings sf;
	if(filelist::instance().currentFile()->node(i).hasbearing())
		sf = filelist::instance().currentFile()->node(i).bearing().form();
	QList<QButtonGroup*> groups;
	groups << basicsupport << basicsupport2 << springsupport;
	foreach(QButtonGroup *g, groups)
	{
		foreach(QAbstractButton *b, g->buttons())
		{
			if((g->id(b) & sf) != 0)
			{
				b->setChecked(true);
			}
			else
			{
				b->setChecked(false);
			}

		}
	}
	hc->setEnabled(sf & ColinSupport::fx);
	vc->setEnabled(sf & ColinSupport::fz);
	mc->setEnabled(sf & ColinSupport::fphi);


	if(sf != ColinSupport::NoBear)
	{
		hc->setText(QString::number(filelist::instance().currentFile()->node(i).bearing().c_x(), 'g', PRECISON));
		vc->setText(QString::number(filelist::instance().currentFile()->node(i).bearing().c_z(), 'g', PRECISON));
		mc->setText(QString::number(filelist::instance().currentFile()->node(i).bearing().c_phi(), 'g', PRECISON));
	}
	else
	{
		hc->setText("0");
		vc->setText("0");
		mc->setText("0");
	}
}


void supportExtended::setBasic()
{
	QButtonGroup *group = qobject_cast<QButtonGroup*>(sender());
	if(!group)
		return;

	ColinSupport su;
	if(filelist::instance().currentFile()->node(currentItem).hasbearing())
		su = filelist::instance().currentFile()->node(currentItem).bearing();

	foreach(QAbstractButton *b, group->buttons())
	{
		if(b->isChecked())
			su.setForm( ColinSupport::bearings(su.form() | group->id(b)));
		else
			su.setForm( su.form() & (~group->id(b)));
	}

	filelist::instance().currentFile()->setBearing(currentItem, su);
}

void supportExtended::setSpring()
{
	ColinSupport su;
	if(filelist::instance().currentFile()->node(currentItem).hasbearing())
		su = filelist::instance().currentFile()->node(currentItem).bearing();

	if(hspring->isChecked())
		su.setForm(ColinSupport::bearings((su.form() | ColinSupport::fx)& (~ColinSupport::X)));
	else{
		su.setForm( su.form() & (~ColinSupport::fx));
		su.setCx(0);
	}


	if(vspring->isChecked())
		su.setForm(ColinSupport::bearings((su.form() | ColinSupport::fz)& (~ColinSupport::Z)));
	else{
		su.setForm( su.form() & (~ColinSupport::fz));
		su.setCz(0);
	}


	if(mspring->isChecked())
		su.setForm(ColinSupport::bearings((su.form() | ColinSupport::fx)& (~ColinSupport::Phi)));
	else{
		su.setForm( su.form() & (~ColinSupport::fphi));
		su.setCphi(0);
	}

	filelist::instance().currentFile()->setBearing(currentItem, su);

}

void supportExtended::setSpringConstants()
{
	bool ok;
	double val = qobject_cast<QLineEdit*>(sender())->text().toDouble(&ok);
	if(!ok)
		return;
	if(sender() == hc)
		filelist::instance().currentFile()->setCx(currentItem, val);
	if(sender() == vc)
		filelist::instance().currentFile()->setCz(currentItem, val);
	if(sender() == mc)
		filelist::instance().currentFile()->setCm(currentItem, val);
}

nodeOverlay::nodeOverlay(QWidget *parent) :
	abstractOverlay(parent)
{

	item->setIcon(colinIcons::instance().icon(Colin::drawNode));
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

	QGroupBox *coords = new QGroupBox(this);
	vl1->addWidget(coords, Qt::AlignLeft);

	coords->setTitle("["+tr("coordinates")+"]");
	coords->setMinimumWidth(250);

	QGridLayout *cl = new QGridLayout(coords);
	coords->setLayout(cl);
	xlabel = new QLabel("x");
	xlabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	x = new QLineEdit();
	cl->addWidget(xlabel, 0, 0, 1, 1);
	cl->addWidget(x, 0, 1, 1, 1);
	cl->addWidget(new QLabel("[m]", coords), 0, 2, 1, 1);

	x->setWhatsThis(tr("<b>x</b> ")+
					tr("<a href=\"menu/node\">open manual</a><br /><br />")+
					tr("The horizontal coordinate of the node!"));

	zlabel = new QLabel("z");
	zlabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	z = new QLineEdit();
	cl->addWidget(zlabel, 1, 0, 1, 1);
	cl->addWidget(z, 1, 1, 1, 1);
	cl->addWidget(new QLabel("[m]", coords), 1, 2, 1, 1);

	z->setWhatsThis(tr("<b>z</b> ")+
					tr("<a href=\"menu/node\">open manual</a><br /><br />")+
					tr("The vertical coordinate of the node!"));

	philabel = new QLabel("phi");
	philabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	phi = new QLineEdit();
	cl->addWidget(philabel, 2, 0, 1, 1);
	cl->addWidget(phi, 2, 1, 1, 1);
	cl->addWidget(new QLabel("[grad]", coords), 2, 2, 1, 1);

	phi->setWhatsThis(tr("<b>phi</b> ")+
					  tr("<a href=\"menu/node\">open manual</a><br /><br />")+
					  tr("Use this to set the angle of supports attached to this node! <br />")+
					  tr("<b>This is only availible if a support is already set!</b>"));

	support = new supportExtended(this);
	vl1->addWidget(support, Qt::AlignLeft | Qt::AlignVCenter);


	QGroupBox *clipbox = new QGroupBox(this);
	vl1->addWidget(clipbox, Qt::AlignLeft);

	clipbox->setTitle("["+tr("clipboard")+"]");
	clipbox->setMinimumWidth(250);

	copyButton = new ColinPushButtonPart(colinIcons::instance().icon(Colin::Copy), tr("copy"));
	cutButton = new ColinPushButtonPart(colinIcons::instance().icon(Colin::Cut), tr("cut"));

	copyButton->setWhatsThis(tr("<b>copy<b> ")+
							 tr("<a href=\"menu/node\">open manual</a><br /><br />")+
							 tr("Press to put a copy of the node and the connected support to the clipboard!"));


	copyButton->setWhatsThis(tr("<b>cut<b> ")+
							 tr("<a href=\"menu/node\">open manual</a><br /><br />")+
							 tr("Press to put the node and the connected support to the clipboard and remove it from the current file!"));

	hl = new QHBoxLayout(clipbox);
	clipbox->setLayout(hl);
	ColinHMultiButton *clip = new ColinHMultiButton(clipbox);
	clip->addButton(copyButton);
	clip->addButton(cutButton);
	hl->addWidget(clip);


	vl1->addStretch(1000);

	QGroupBox *detailbox = new QGroupBox(this);

	detailbox->setTitle("["+tr("view")+"]");
	detailbox->setWhatsThis(tr("<b>view</b> ")+
						   tr("<a href=\"menu/node\">open manual</a><br /><br />")+
						   tr("shows the current node and all releated forces!"));


	QVBoxLayout *dl = new QVBoxLayout(detailbox);
	detailbox->setLayout(dl);
	vl2->addWidget(detailbox);

	clsButton = new ColinHMultiButton(this);
	cls = new QButtonGroup(this);
	cls->setExclusive(true);
	dl->addWidget(clsButton);
	clsButton->hide();

	detailWidget = new nodeDetail(this);
	detailWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	QSizePolicy sP = detailWidget->sizePolicy();
	sP.setHeightForWidth(true);
	detailWidget->setSizePolicy(sP);
	dl->addWidget(detailWidget);
	vl2->addStretch(10000);



	disp = new QGroupBox(this);
	vl3->addWidget(disp);

	disp->setTitle("["+tr("displacement")+"]");
	disp->setMinimumWidth(250);
	disp->setCheckable(true);

	QVBoxLayout *labels = new QVBoxLayout(disp);
	disp->setLayout(labels);
	displacement = new QLabel("N/A", disp);
	displacement->setTextInteractionFlags(Qt::TextBrowserInteraction | Qt::TextSelectableByKeyboard);
	disp->setWhatsThis(tr("<b>displacement</b> ")+
					   tr("<a href=\"menu/node\">open manual</a><br /><br />")+
					   tr("shows the displacement of the current node!"));
	labels->addWidget(displacement);


	reac = new QGroupBox(this);
	vl3->addWidget(reac);

	reac->setTitle("["+tr("reactions")+"]");
	reac->setMinimumWidth(250);
	reac->setCheckable(true);

	labels = new QVBoxLayout(reac);
	reac->setLayout(labels);
	reactions = new QLabel("N/A", reac);
	reactions->setTextInteractionFlags(Qt::TextBrowserInteraction | Qt::TextSelectableByKeyboard);
	reac->setWhatsThis(tr("<b>reaction forces</b> ")+
						tr("<a href=\"menu/node\">open manual</a><br /><br />")+
						tr("shows the reaction forces on the current node!"));
	labels->addWidget(reactions);

	beams = new QGroupBox(this);
	vl3->addWidget(beams);
	beams->setTitle("["+tr("beam forces")+"]");
	beams->setMinimumWidth(250);
	beams->setCheckable(true);
	beams->setWhatsThis(tr("<b>beam forces</b> ")+
						tr("<a href=\"menu/node\">open manual</a><br /><br />")+
						tr("shows the forces of all beams, conected to this node!"));



	hl = new QHBoxLayout(beams);
	beams->setLayout(hl);
	beamforces_min = new QLabel("N/A", beams);
	beamforces_min->setTextInteractionFlags(Qt::TextBrowserInteraction | Qt::TextSelectableByKeyboard);
	beamforces_max = new QLabel("", beams);
	beamforces_max->setTextInteractionFlags(Qt::TextBrowserInteraction | Qt::TextSelectableByKeyboard);
	hl->addWidget(beamforces_min);
	hl->addWidget(beamforces_max);

	vl3->addStretch(1000);

	connect(reac,			SIGNAL(clicked(bool)),
			this,			SLOT(hideMyChildren(bool)));

	connect(disp,			SIGNAL(clicked(bool)),
			this,			SLOT(hideMyChildren(bool)));

	connect(beams,			SIGNAL(clicked(bool)),
			this,			SLOT(hideMyChildren(bool)));

	connect(x,				SIGNAL(editingFinished()),
			this,			SLOT(changed()));

	connect(z,				SIGNAL(editingFinished()),
			this,			SLOT(changed()));

	connect(phi,			SIGNAL(editingFinished()),
			this,			SLOT(changed()));

	connect(cls,			SIGNAL(buttonClicked(int)),
			detailWidget,	SLOT(setCurrentCLS(int)));

	connect(copyButton,		SIGNAL(clicked()),
			this,			SLOT(copy()));

	connect(cutButton,		SIGNAL(clicked()),
			this,			SLOT(cut()));


	reac->setChecked(nodeOverlay::showReactions);
	disp->setChecked(nodeOverlay::showDisplacement);
	beams->setChecked(nodeOverlay::showBeamForces);
	support->morebox->setChecked(nodeOverlay::showSupportExtended);

	QList<QGroupBox*> boxes;
	boxes << reac << disp << beams << support->morebox;

	foreach(QGroupBox *gb, boxes)
	{
		if(!gb->isChecked())
		{
			foreach(QWidget *w, gb->findChildren<QWidget* >())
				w->setHidden(true);
		}
	}

	currentItem = -1;

}

nodeOverlay::~nodeOverlay()
{
	nodeOverlay::showReactions = reac->isChecked();
	nodeOverlay::showDisplacement = disp->isChecked();
	nodeOverlay::showBeamForces = beams->isChecked();
	nodeOverlay::showSupportExtended = support->morebox->isChecked();
}

void nodeOverlay::setCurrentItem(const int &i)
{
	this->setFocusPolicy(Qt::StrongFocus);
	currentItem = i;
	detailWidget->setCurrentItem(i);
	support->setCurrentItem(i);
	clsChanged();

	this->item->setText("#"+QString::number(i));

	ColinStruct &t = *filelist::instance().currentFile();

	if(filelist::instance().currentFile()){
		connect(&t,					SIGNAL(changedNode(int)),
				this,				SLOT(nodeChanged(int)));

		connect(&t,					SIGNAL(calculationFinished()),
				this,				SLOT(nodeChanged()));

		connect(&t,					SIGNAL(addedCLS(int)),
				this,				SLOT(clsChanged()));

		connect(&t,					SIGNAL(erasedCLS(int)),
				this,				SLOT(clsChanged()));

	}
	x->setText(QString::number(t.node(i).x(), 'f', PRECISON));
	z->setText(QString::number(t.node(i).z(), 'f', PRECISON));

	if(t.node(i).hasbearing()){
		phi->setText(QString::number(t.node(i).bearing().angle(), 'f', PRECISON));
		phi->setEnabled(true);
	}
	else{
		phi->setText(tr("only affects support"));
		phi->setDisabled(true);
	}


	if(t.isCalculated())
	{
		QString txt;
		txt.append("<b>u</b>");
		if(t.cls_n()==0){
			txt.append(QString(" = %1 m<br/>").arg(t.node(i).u(0)));
		}else{
			double max = 0;
			foreach(int cls, t.activeCLS()) max = qMax(fabs(t.node(i).u(cls)), max);
			txt.append(QString("<b><sub>max</sub> = %1 m</b><br/>").arg(max));
			if(max!=0){
				foreach(int cls, t.activeCLS()){
					txt.append(QString("u<sub>%1</sub> = %2 m<br/>").arg(cls).arg(t.node(i).u(cls)));
				}
			}
			txt.append("<hr/><br/>");
		}
		txt.append("<b>w</b>");
		if(t.cls_n()==0){
			txt.append(QString(" = %1 m<br/>").arg(t.node(i).w(0)));
		}else{
			double max = 0;
			foreach(int cls, t.activeCLS()) max = qMax(fabs(t.node(i).w(cls)), max);
			txt.append(QString("<b><sub>max</sub> = %1 m</b><br/>").arg(max));
			if(max!=0){
				foreach(int cls, t.activeCLS()){
					txt.append(QString("w<sub>%1</sub> = %2 m<br/>").arg(cls).arg(t.node(i).w(cls)));
				}
			}
			txt.append("<hr/><br/>");
		}
		txt.append("<b>phi</b>");
		if(t.cls_n()==0){
			txt.append(QString(" = %1 grad<br/>").arg(t.node(i).phi(0)*ANGLEPREFIX));
		}else{
			double max = 0;
			foreach(int cls, t.activeCLS()) max = qMax(fabs(t.node(i).phi(cls)), max);
			txt.append(QString("<b><sub>max</sub> = %1 grad</b><br/>").arg(max*ANGLEPREFIX));
			if(max!=0){
				foreach(int cls, t.activeCLS()){
					txt.append(QString("phi<sub>%1</sub> = %2 grad<br/>").arg(cls).arg(t.node(i).phi(cls)*ANGLEPREFIX));
				}
			}
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
		if(t.node(i).hasbearing())
		{
			if(t.node(i).bearing().x() || t.node(i).bearing().xf()){
				txt.append("<b>A<sub>x</sub></b>");
				if(t.cls_n()==0){
					txt.append(QString(" = %1 %2<br/>").arg(t.node(i).bearing().a_x(0)*PPREFIX).arg(unitSettings::instance().Peh()));
				}else{
					double max = 0;
					foreach(int cls, t.activeCLS()) max = qMax(fabs(t.node(i).bearing().a_x(cls)), max);
					txt.append(QString("<b><sub>max</sub> = %1 %2</b><br/>").arg(max*PPREFIX).arg(unitSettings::instance().Peh()));
					if(max!=0){
						foreach(int cls, t.activeCLS()){
							txt.append(QString("A<sub>x,%1</sub> = %2 %3<br/>").arg(cls).arg(t.node(i).bearing().a_x(cls)*PPREFIX).arg(unitSettings::instance().Peh()));
						}
					}
				}
			}

			if(t.node(i).bearing().z() || t.node(i).bearing().zf()){
				if(!txt.isEmpty())
					txt.append("<hr/><br/>");
				txt.append("<b>A<sub>z</sub></b>");
				if(t.cls_n()==0){
					txt.append(QString(" = %1 %2<br/>").arg(t.node(i).bearing().a_z(0)*PPREFIX).arg(unitSettings::instance().Peh()));
				}else{
					double max = 0;
					foreach(int cls, t.activeCLS()) max = qMax(fabs(t.node(i).bearing().a_z(cls)), max);
					txt.append(QString("<b><sub>max</sub> = %1 %2</b><br/>").arg(max*PPREFIX).arg(unitSettings::instance().Peh()));
					if(max!=0){
						foreach(int cls, t.activeCLS()){
							txt.append(QString("A<sub>z,%1</sub> = %2 %3<br/>").arg(cls).arg(t.node(i).bearing().a_z(cls)*PPREFIX).arg(unitSettings::instance().Peh()));
						}
					}
					txt.append("<br/>");
				}
			}

			if(t.node(i).bearing().phi() || t.node(i).bearing().phif()){
				if(!txt.isEmpty())
					txt.append("<hr/><br/>");
				txt.append("<b>M</b>");
				if(t.cls_n()==0){
					txt.append(QString(" = %1 m<br/>").arg(t.node(i).bearing().a_m(0)*MPREFIX).arg(unitSettings::instance().Meh()));
				}else{
					double max = 0;
					foreach(int cls, t.activeCLS()) max = qMax(fabs(t.node(i).bearing().a_m(cls)), max);
					txt.append(QString("<b><sub>max</sub> = %1 %2</b><br/>").arg(max*MPREFIX).arg(unitSettings::instance().Meh()));
					if(max!=0){
						foreach(int cls, t.activeCLS()){
							txt.append(QString("M<sub>%1</sub> = %2 %3<br/>").arg(cls).arg(t.node(i).bearing().a_m(cls)*MPREFIX).arg(unitSettings::instance().Meh()));
						}
					}
				}
			}
		}
		else
		{
			txt.append(tr("no bearing"));
		}
		reactions->setText(txt);
	}
	else
	{
		reactions->setText("N/A");
	}

	if(t.isCalculated())
	{
		QString txt_min;
		QString txt_max;
		for(int j = 0; j<t.beam_n(); j++){
			for(int m=0; m<2; m++)
			{
				if((m==0?t.beam(j).leftNodeI():t.beam(j).rightNodeI()) == i){
					if(!txt_min.isEmpty()){
						txt_max.append("<hr/><br/>");
						txt_min.append("<hr/><br/>");
					}
					double max = -std::numeric_limits<double>::max();
					//foreach(int cls, t.activeCLS())
					for(int cls = 0; cls<qMax(t.cls_n(), 1); cls++)
						max = qMax(t.beam(j).N(cls, (!m?0:t.beam(j).l())), max);
					//if(max*PPREFIX*PRECISON>1)
						txt_max.append(QString("<font color=\"%1\">N<sub>%2,%3,%4</sub> = %5 %6</font><br/>")
								   .arg(viewPortSettings::instance().color(Colin::C_Np).name())
								   .arg(!m?tr("l"):tr("r")).arg(t.cls_n()?"max":"").arg(j)
								   .arg(max*PPREFIX, 0, 'f', PRECISON).arg(unitSettings::instance().Peh()));
					double min = std::numeric_limits<double>::max();
					//foreach(int cls, t.activeCLS())
					for(int cls = 0; cls<qMax(t.cls_n(), 1); cls++)
						min = qMin(t.beam(j).N(cls, (!m?0:t.beam(j).l())), min);
					//if(min*PPREFIX*PRECISON<-1)
						txt_min.append(QString("<font color=\"%1\">N<sub>%2,%3,%4</sub> = %5 %6</font><br/>")
								   .arg(viewPortSettings::instance().color(Colin::C_Np).name())
								   .arg(!m?tr("l"):tr("r")).arg(t.cls_n()?"min":"").arg(j)
								   .arg(min*PPREFIX, 0, 'f', PRECISON).arg(unitSettings::instance().Peh()));


					max = -std::numeric_limits<double>::max();
					//foreach(int cls, t.activeCLS())
					for(int cls = 0; cls<qMax(t.cls_n(), 1); cls++)
						max = qMax((t.beam(j).Q(cls, (!m?0:t.beam(j).l()))), max);
					//if(max*PPREFIX*PRECISON>1)
						txt_max.append(QString("<font color=\"%1\">Q<sub>%2,%3,%4</sub> = %5 %6</font><br/>")
								   .arg(viewPortSettings::instance().color(Colin::C_Qp).name())
								   .arg(!m?tr("l"):tr("r")).arg(t.cls_n()?"max":"").arg(j)
								   .arg(max*PPREFIX, 0, 'f', PRECISON).arg(unitSettings::instance().Peh()));
					min = std::numeric_limits<double>::max();
					//foreach(int cls, t.activeCLS())
					for(int cls = 0; cls<qMax(t.cls_n(), 1); cls++)
						min = qMin(t.beam(j).Q(cls, (!m?0:t.beam(j).l())), min);
					//if(min*PPREFIX*PRECISON<-1)
						txt_min.append(QString("<font color=\"%1\">Q<sub>%2,%3,%4</sub> = %5 %6</font><br/>")
								   .arg(viewPortSettings::instance().color(Colin::C_Qp).name())
								   .arg(!m?tr("l"):tr("r")).arg(t.cls_n()?"min":"").arg(j)
								   .arg(min*PPREFIX, 0, 'f', PRECISON).arg(unitSettings::instance().Peh()));


					max = -std::numeric_limits<double>::max();
					//foreach(int cls, t.activeCLS())
					for(int cls = 0; cls<qMax(t.cls_n(), 1); cls++)
						max = qMax((t.beam(j).M(cls, (!m?0:t.beam(j).l()))), max);
					//if(max*MPREFIX*PRECISON>1)
						txt_max.append(QString("<font color=\"%1\">M<sub>%2,%3,%4</sub> = %5 %6</font><br/>")
								   .arg(viewPortSettings::instance().color(Colin::C_Mp).name())
								   .arg(!m?tr("l"):tr("r")).arg(t.cls_n()?"max":"").arg(j)
								   .arg(max*MPREFIX, 0, 'f', PRECISON).arg(unitSettings::instance().Meh()));
					min = std::numeric_limits<double>::max();
					//foreach(int cls, t.activeCLS())
					for(int cls = 0; cls<qMax(t.cls_n(), 1); cls++)
						min = qMin(t.beam(j).M(cls, (!m?0:t.beam(j).l())), min);
					//if(min*MPREFIX*PRECISON<-1)
						txt_min.append(QString("<font color=\"%1\">M<sub>%2,%3,%4</sub> = %5 %6</font><br/>")
								   .arg(viewPortSettings::instance().color(Colin::C_Mp).name())
								   .arg(!m?tr("l"):tr("r")).arg(t.cls_n()?"min":"").arg(j)
								   .arg(min*MPREFIX, 0, 'f', PRECISON).arg(unitSettings::instance().Meh()));



				}
			}
		}
		beamforces_min->setText(txt_min);
		if(t.cls_n())
			beamforces_max->setText(txt_max);
		else
			beamforces_max->setText("");
	}
	else
	{
		beamforces_min->setText("N/A");
		beamforces_max->setText("");
	}
}


void nodeOverlay::changed()
{
	if(sender() == x)
	{
		bool ok;
		double xval = x->text().toDouble(&ok);
		if(!ok)
			return;
		filelist::instance().currentFile()->setX(currentItem, xval);
	}
	if(sender() == z)
	{
		bool ok;
		double zval = z->text().toDouble(&ok);
		if(!ok)
			return;
		filelist::instance().currentFile()->setZ(currentItem, zval);
	}
	if(sender() == phi)
	{
		bool ok;
		double phival = phi->text().toDouble(&ok);
		if(!ok)
			return;
		filelist::instance().currentFile()->setAngle(currentItem, phival);
	}

}


void nodeOverlay::setTw(ColinStruct *t)
{
	this->deleteLater();
}

void nodeOverlay::nodeChanged()
{
	nodeChanged(currentItem);
}

void nodeOverlay::nodeChanged(int i)
{
	if(i==currentItem)
		this->setCurrentItem(i);
}

void nodeOverlay::nextItem()
{
	if(currentItem == filelist::instance().currentFile()->node_n()-1)
		setCurrentItem(0);
	else
		setCurrentItem(currentItem+1);
}

void nodeOverlay::previousItem()
{
	if(currentItem == 0)
		setCurrentItem(filelist::instance().currentFile()->node_n()-1);
	else
		setCurrentItem(currentItem-1);
}

void nodeOverlay::clsChanged()
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

void nodeOverlay::copy()
{
	toClipBoard();
	filelist::instance().currentFile()->selectNode(currentItem, false);
	this->deleteLater();

}
void nodeOverlay::cut()
{
	toClipBoard();
	filelist::instance().currentFile()->removeNode(currentItem);
	this->deleteLater();
}

void nodeOverlay::toClipBoard()
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
