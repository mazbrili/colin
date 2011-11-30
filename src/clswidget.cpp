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

#include "clswidget.h"

#include "colinicons.h"
#include "colinstruct.h"


clswidget::clswidget(QWidget *parent)
	:QDialog(parent)
{
	this->setMinimumSize(800, 400);

	layout = new QVBoxLayout(this);

	headergroup = new QButtonGroup(this);
	header = new ColinHMultiButton(this);
	bls = new ColinPushButtonPart(colinIcons::instance().icon(Colin::BLS), tr("basic load sets"));
	cls = new ColinPushButtonPart(colinIcons::instance().icon(Colin::CLS), tr("combined load sets"));
	bls->setCheckable(true);
	cls->setCheckable(true);
	bls->setChecked(true);
	headergroup->setExclusive(true);
	headergroup->addButton(bls, 0);
	headergroup->addButton(cls, 1);
	header->addButton(bls);
	header->addButton(cls);
	header->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	header->setFixedHeight(32);

	layout->addWidget(header);

	slidingStacked= new SlidingStackedWidget(this);

	layout->addWidget(slidingStacked);


	blsWidget = new QWidget(this);
	clsWidget = new QWidget(this);
	slidingStacked->addWidget(blsWidget);
	slidingStacked->addWidget(clsWidget);
	slidingStacked->setSpeed(500);
	slidingStacked->setAnimation(QEasingCurve::InOutExpo);


	blslayout = new QGridLayout(blsWidget);
	clslayout = new QGridLayout(clsWidget);

	loadlist = new QTreeWidget(blsWidget);
	blslist1 = new QTreeWidget(clsWidget);

	blslist2 = new QTreeWidget(clsWidget);
	clslist = new QTreeWidget(clsWidget);

	clslist->setColumnCount(2);

	addCls = new QPushButton(colinIcons::instance().icon(Colin::New), tr("new CLS"));
	addBls1 = new QPushButton(colinIcons::instance().icon(Colin::New), tr("new BLS"));
	addBls2 = new QPushButton(colinIcons::instance().icon(Colin::New), tr("new BLS"));

	blslayout->addWidget(loadlist, 0, 0, 2, 1);
	blslayout->addWidget(blslist1, 1, 1, 1, 1);
	blslayout->addWidget(addBls1, 0, 1, 1, 1);


	clslayout->addWidget(addBls2, 0, 0, 1, 1);
	clslayout->addWidget(addCls, 0, 1, 1, 1);
	clslayout->addWidget(blslist2, 1, 0, 1, 1);
	clslayout->addWidget(clslist, 1, 1, 1, 1);

	tw = filelist::instance().currentFile();

	QTreeWidgetItem *listwidget;
	if(tw!=0){
		for(int i=0; i<tw->load_n(); i++)
		{
			listwidget = new QTreeWidgetItem();
			listwidget->setIcon(0, colinIcons::instance().icon(tw->load(i).typ()));
			listwidget->setText(0, QString(tr("load")+" %1").arg(i));
			loadlist->addTopLevelItem(listwidget);
		}
		for(int i=0; i<tw->bls_n(); i++)
		{
			listwidget = new QTreeWidgetItem();
			listwidget->setIcon(0, colinIcons::instance().icon(Colin::BLS));
			listwidget->setText(0, tw->bls(i).name());
			listwidget = new QTreeWidgetItem();
			listwidget->setIcon(0, colinIcons::instance().icon(Colin::BLS));
			listwidget->setText(0, tw->bls(i).name());
		}
		for(int i=0; i<tw->cls_n(); i++)
		{
			listwidget = new QTreeWidgetItem();
			listwidget->setIcon(0, colinIcons::instance().icon(Colin::CLS));
			listwidget->setText(0, tw->cls(i).name());
		}
	}

	connect(headergroup,			SIGNAL(buttonClicked(int)),
			slidingStacked,			SLOT(slideInIdx(int)));
}

void clswidget::setTw(ColinStruct *t)
{
	this->tw = t;
}

void clswidget::addBLS(const ColinBLS &bls, int i)
{

}

void clswidget::addCLS(const ColinCLS &cls, int i)
{

}

void clswidget::removeBLS(const int &i)
{

}

void clswidget::removeCLS(const int &i)
{

}

void clswidget::setBLS(const int &i, const ColinBLS &l)
{

}

void clswidget::setCLS(const int &i, const ColinCLS &l)
{

}
