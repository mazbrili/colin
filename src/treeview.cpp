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

#include <QtGui/QStandardItemModel>

#include "treeview.h"
#include "unitsettings.h"

#include "beammenu.h"
#include "loadmenu.h"
#include "nodemenu.h"
#include "tempmenu.h"
#include "momentmenu.h"
#include "dloadmenu.h"
#include "nloadmenu.h"


treeView::treeView(QWidget *parent) :
    QTreeWidget(parent)
{
	this->setColumnCount(6);
    this->setRootIsDecorated(true);
    this->setRootIndex(QModelIndex());
	this->setAnimated(true);

    isChanging = false;


    nodeParent = new QTreeWidgetItem(this);
    nodeParent->setExpanded(true);
    nodeParent->setIcon(0, colinIcons::instance().icon(Colin::drawNode));

    beamParent = new QTreeWidgetItem(this);
    beamParent->setExpanded(true);
    beamParent->setIcon(0, colinIcons::instance().icon(Colin::drawBeam));

	loadParent = new QTreeWidgetItem(this);
	loadParent->setExpanded(true);
	loadParent->setIcon(0, colinIcons::instance().icon(Colin::drawNLoad));

	blsParent = new QTreeWidgetItem(this);
	blsParent->setExpanded(false);
	blsParent->setIcon(0, colinIcons::instance().icon(Colin::BLS));

	clsParent = new QTreeWidgetItem(this);
	clsParent->setExpanded(false);
	clsParent->setIcon(0, colinIcons::instance().icon(Colin::CLS));

    setHeader();

    this->addTopLevelItem(nodeParent);
	this->addTopLevelItem(beamParent);
	this->addTopLevelItem(loadParent);
	this->addTopLevelItem(blsParent);
	this->addTopLevelItem(clsParent);


    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);

#ifdef COLIN_DRAG_AND_DROP
	this->setDragEnabled(true);
	this->setAcceptDrops(true);
	this->setDragDropMode(QAbstractItemView::InternalMove);
#endif

    delegate = new treeDelegate(this);
    this->setItemDelegate(delegate);
	this->setIconSize(QSize(16, 16));
    this->setAllColumnsShowFocus(true);
    this->setAlternatingRowColors(true);
	this->setUniformRowHeights(true);

    QStringList hlab;
	for(int i =0; i<6; i++) hlab.append("");
    this->setHeaderLabels(hlab);

	for(int i=0; i<6; i++)
    {
        nodeParent->setData(i, 32, ColinStruct::uneditable);
		beamParent->setData(i, 32, ColinStruct::uneditable);
		loadParent->setData(i, 32, ColinStruct::uneditable);
		blsParent->setData(i, 32, ColinStruct::uneditable);
		clsParent->setData(i, 32, ColinStruct::uneditable);
    }

    tw = 0;
    selectionisUpToDate = false;
    openitem = 0;

    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(&unitSettings::instance(),          SIGNAL(settingschanged()),
            this,                               SLOT(actualEH()));

    connect(&filelist::instance(),              SIGNAL(currentChanged(ColinStruct*)),
            this,                               SLOT(setTw(ColinStruct*)));

    connect(this,                               SIGNAL(itemSelectionChanged()),
            this,                               SLOT(selectionChanged()));

    connect(this,                               SIGNAL(clicked(QModelIndex)),
            this,                               SLOT(popupEditor(QModelIndex)));

    connect(this,                               SIGNAL(itemChanged(QTreeWidgetItem*,int)),
            this,                               SLOT(writeData(QTreeWidgetItem*,int)));

    connect(delegate,                           SIGNAL(openNext()),
            this,                               SLOT(nextColumn()));

    connect(delegate,                           SIGNAL(openPrevious()),
            this,                               SLOT(previousColumn()));

    connect(delegate,                           SIGNAL(openNextItem()),
            this,                               SLOT(nextItem()));

    connect(delegate,                           SIGNAL(openPreviousItem()),
            this,                               SLOT(previousItem()));

    connect(this,                               SIGNAL(customContextMenuRequested(QPoint)),
            this,                               SLOT(popupMenu(QPoint)));
    /*
    connect(delegate,                           SIGNAL(closeEditor(QWidget*,QAbstractItemDelegate::EndEditHint)),
            this,                               SLOT(editorClosed(QWidget*,QAbstractItemDelegate::EndEditHint)));
*/
}




void treeView::addNode(const ColinNode &n, int i)
{
    if(i==-1)
        i=nodeParent->childCount();
    QTreeWidgetItem *node = new QTreeWidgetItem();
    node->setIcon(0, colinIcons::instance().icon(Colin::drawNode));

    node->setData(0, 32, ColinStruct::ID_node);
    node->setData(1, 32, ColinStruct::x);
    node->setData(2, 32, ColinStruct::z);
    node->setData(3, 32, ColinStruct::uneditable);
    node->setData(4, 32, ColinStruct::uneditable);

    nodeParent->insertChild(i, node);
    setNode(i, n);
    recount(nodeParent, i);
	this->popupEditor(this->indexFromItem(nodeParent->child(tw->node_n()), 1));
}

void treeView::addBeam(const ColinBeam &b, int i)
{
    if(i==-1)
        i=beamParent->childCount();
    QTreeWidgetItem *beam = new QTreeWidgetItem();
    beam->setIcon(0, colinIcons::instance().icon(Colin::drawBeam));
    beam->setData(0, 32, ColinStruct::ID_beam);
    beam->setData(1, 32, ColinStruct::leftNode);
    beam->setData(2, 32, ColinStruct::rightNode);
    beam->setData(3, 32, ColinStruct::Mat);
	beam->setData(4, 32, ColinStruct::CrossSection);

    beamParent->insertChild(i, beam);

    QTreeWidgetItem *joints;

    for(int j=0; j<6; j++)
    {
        joints = new QTreeWidgetItem(beam);
        joints->setData(0, 32, ColinStruct::uneditable);
        joints->setData(1, 32, ColinStruct::Spring);
        joints->setData(2, 32, ColinStruct::uneditable);
        joints->setData(3, 32, ColinStruct::uneditable);
        joints->setData(4, 32, ColinStruct::uneditable);
    }

    setBeam(i, b);



    recount(beamParent, i);
}

void treeView::addLoad(const ColinLoad &l, int i)
{
    if(i==-1)
        i=loadParent->childCount();
    QTreeWidgetItem *load = new QTreeWidgetItem();
        load->setIcon(0, colinIcons::instance().icon(l.typ()));
    if(l.typ() == ColinLoad::nodeLoad ||
       l.typ() == ColinLoad::moment   )
        load->setIcon(1, colinIcons::instance().icon(Colin::drawNode));
    else
        load->setIcon(1, colinIcons::instance().icon(Colin::drawBeam));

    load->setData(0, 32, ColinStruct::ID_load);
	load->setData(1, 32, ColinStruct::Pos);
    if(l.typ() == ColinLoad::nodeLoad                ||
       l.typ() == ColinLoad::uniformlyDistibutedLoad ||
       l.typ() == ColinLoad::increasingLinearLoad    ||
       l.typ() == ColinLoad::decreasingLinearLoad    )
    {
        load->setData(2, 32, ColinStruct::Px);
        load->setData(3, 32, ColinStruct::Pz);
        load->setData(4, 32, ColinStruct::uneditable);
    }
    else if( l.typ() == ColinLoad::tempChange)
    {
	load->setData(2, 32, ColinStruct::Px);
	load->setData(3, 32, ColinStruct::uneditable);
	load->setData(4, 32, ColinStruct::uneditable);
    }
    else if( l.typ() == ColinLoad::tempDiffrence)
    {
	load->setData(2, 32, ColinStruct::uneditable);
	load->setData(3, 32, ColinStruct::Pz);
	load->setData(4, 32, ColinStruct::uneditable);
    }
    else if(l.typ() == ColinLoad::moment)
    {
	load->setData(2, 32, ColinStruct::uneditable);
	load->setData(3, 32, ColinStruct::uneditable);
	load->setData(4, 32, ColinStruct::M);
    }
    else if(l.typ() == ColinLoad::doubleLoadLeft ||
	    l.typ() == ColinLoad::doubleLoadRight)
    {
	load->setData(2, 32, ColinStruct::Px);
	load->setData(3, 32, ColinStruct::Pz);
	load->setData(4, 32, ColinStruct::M);
    }

	load->setData(5, 32, ColinStruct::LoadSet);

    loadParent->insertChild(i, load);
    setLoad(i, l);
    recount(loadParent, i);

}

void treeView::addBLS(const ColinBLS &bls, int i)
{
	if(i==-1)
		i=blsParent->childCount();
	QTreeWidgetItem *blsC = new QTreeWidgetItem();
	blsC->setIcon(0, colinIcons::instance().icon(Colin::BLS));
	blsC->setData(0, 32, ColinStruct::ID_bls);
	blsC->setData(1, 32, ColinStruct::BLSname);
	blsC->setData(2, 32, ColinStruct::BLScolor);

	blsParent->insertChild(i, blsC);
	setBLS(i);
	recount(blsParent, i);
}

void treeView::addCLS(const ColinCLS &cls, int i)
{
	if(i==-1)
		i=clsParent->childCount();
	QTreeWidgetItem *clsC = new QTreeWidgetItem();
	clsC->setIcon(0, colinIcons::instance().icon(Colin::CLS));
	clsC->setData(0, 32, ColinStruct::ID_cls);
	clsC->setData(1, 32, ColinStruct::CLSname);

	clsParent->insertChild(i, clsC);
	setCLS(i);
	recount(clsParent, i);
}

void treeView::removeNode(const int &i)
{
    if(openitem)
    {
        closePersistentEditor(openitem, opencolumn);
        openitem = 0;
    }
    delete nodeParent->child(i);
    recount(nodeParent, i);
}

void treeView::removeBeam(const int &i)
{
    if(openitem)
    {
        closePersistentEditor(openitem, opencolumn);
        openitem = 0;
    }
    delete beamParent->child(i);
    recount(beamParent, i);
}

void treeView::removeLoad(const int &i)
{
    if(openitem)
    {
        closePersistentEditor(openitem, opencolumn);
        openitem = 0;
    }
    delete loadParent->child(i);
    recount(loadParent, i);
}

void treeView::removeBLS(const int &i)
{
	if(openitem)
	{
		closePersistentEditor(openitem, opencolumn);
		openitem = 0;
	}
	delete blsParent->child(i);
	recount(blsParent, i);
}

void treeView::removeCLS(const int &i)
{
	if(openitem)
	{
		closePersistentEditor(openitem, opencolumn);
		openitem = 0;
	}
	delete clsParent->child(i);
	recount(clsParent, i);
}

void treeView::setNode(const int &i, const ColinNode &n)
{
    blockSignals(true);
    QTreeWidgetItem *node = nodeParent->child(i);
    QTreeWidgetItem *bear;
    node->setText(1, QString("%1").arg(n.x()));
    node->setText(2, QString("%1").arg(n.z()));
    if(n.hasbearing())
    {
        if(node->childCount()==0)
        {
            bear = new QTreeWidgetItem(node);
            bear->setData(0, 32, ColinStruct::uneditable);
            bear->setData(1, 32, ColinStruct::alpha_k);
            bear->setData(2, 32, ColinStruct::Bearing_h);
            bear->setData(3, 32, ColinStruct::Bearing_v);
            bear->setData(4, 32, ColinStruct::Bearing_m);
            bear->setIcon(0, colinIcons::instance().icon(n.bearing().simpleform()));
        }
        else
            bear = node->child(0);

        bear->setText(1, QString("%1").arg(-n.angle()*ANGLEPREFIX));

        if(n.bearing().x())
        {
            bear->setText(2, tr("locked"));
        }
        else if(n.bearing().xf())
        {
            bear->setText(2, QString("c = %1").arg(n.bearing().c_x()*FPREFIX));
        }
        else
        {
            bear->setText(2, tr("free"));
        }

        if(n.bearing().z())
        {
            bear->setText(3, tr("locked"));
        }
        else if(n.bearing().zf())
        {
            bear->setText(3, QString("c = %1").arg(n.bearing().c_z()*FPREFIX));
        }
        else
        {
            bear->setText(3, tr("free"));
        }

        if(n.bearing().phi())
        {
            bear->setText(4, tr("locked"));
        }
        else if(n.bearing().phif())
        {
            bear->setText(4, QString("c = %1").arg(n.bearing().c_phi()*FMPREFIX));
        }
        else
        {
            bear->setText(4, tr("free"));
        }



    }
    else
    {
        if(node->childCount() != 0)
            if(node->child(0) == openitem)
                openitem = 0;
            delete node->child(0);
	}
    blockSignals(false);
}

void treeView::setBeam(const int &i, const ColinBeam &b)
{
    blockSignals(true);
    QTreeWidgetItem *beam = beamParent->child(i);
    beam->setText(1, QString("%1").arg(b.leftNodeI()));
    beam->setText(2, QString("%1").arg(b.rightNodeI()));
    beam->setText(3, QString("%1").arg(b.Mat().name()));
    beam->setText(4, QString("%1").arg(b.Profile().name()));

    for(int j=0; j<6; j++)
    {
        beam->child(j)->setText(0, QString(tr("hinch")+" %1").arg(j));
        if(b.joint(j))
        {
            if(b.spring(j) == 0)
            {
                beam->child(j)->setText(1, tr("free"));
                beam->child(j)->setIcon(0, colinIcons::instance().icon(Colin::drawJoint));
            }
            else
            {
                if(j == 2 || j == 5) //momentMenu
                    beam->child(j)->setText(1, QString("c = %1").arg(b.spring(j)*FMPREFIX));
                else
                    beam->child(j)->setText(1, QString("c = %1").arg(b.spring(j)*FPREFIX));
                beam->child(j)->setIcon(0, colinIcons::instance().icon(Colin::Spring));
            }
        }
        else
        {
            beam->child(j)->setText(1, QString(tr("locked")));
            beam->child(j)->setIcon(0, QIcon());
        }
    }
    blockSignals(false);
}

void treeView::setLoad(const int &i, const ColinLoad &l)
{
    blockSignals(true);
    loadParent->child(i)->setText(1, QString("%1").arg(l.at()));
    if(l.typ() == ColinLoad::nodeLoad                ||
       l.typ() == ColinLoad::uniformlyDistibutedLoad ||
       l.typ() == ColinLoad::increasingLinearLoad    ||
       l.typ() == ColinLoad::decreasingLinearLoad    )
    {
        loadParent->child(i)->setText(2, QString("%1").arg(l.Px()*PPREFIX));
        loadParent->child(i)->setText(3, QString("%1").arg(l.Pz()*PPREFIX));
	loadParent->child(i)->setText(4, "-");
    }
    else if(l.typ() == ColinLoad::tempChange)
    {
        loadParent->child(i)->setText(2, QString("%1").arg(l.Px()));
        loadParent->child(i)->setData(2, 32, ColinStruct::Px);
	loadParent->child(i)->setText(3, "-");
        loadParent->child(i)->setData(3, 32, ColinStruct::uneditable);
	loadParent->child(i)->setText(4, "-");
    }
    else if(l.typ() == ColinLoad::tempDiffrence)
    {
	loadParent->child(i)->setText(2, "-");
        loadParent->child(i)->setData(2, 32, ColinStruct::uneditable);
        loadParent->child(i)->setText(3, QString("%1").arg(l.Pz()));
        loadParent->child(i)->setData(3, 32, ColinStruct::Pz);
	loadParent->child(i)->setText(4, "-");
    }
    else if(l.typ() == ColinLoad::moment)
    {
	loadParent->child(i)->setText(2, "-");
	loadParent->child(i)->setText(3, "-");
	loadParent->child(i)->setText(4, QString("%1").arg(l.M()*MPREFIX));
    }
    else if(l.typ() == ColinLoad::doubleLoadLeft ||
	    l.typ() == ColinLoad::doubleLoadRight)
    {
	loadParent->child(i)->setText(2, QString("%1").arg(l.Px()*PPREFIX));
	loadParent->child(i)->setText(3, QString("%1").arg(l.Pz()*PPREFIX));
	loadParent->child(i)->setText(4, QString("%1").arg(l.M()*MPREFIX));
    }
    loadParent->child(i)->setIcon(0, colinIcons::instance().icon(l.typ()));
	if(l.set() == -1)
		loadParent->child(i)->setText(5, tr("none"));
	else
		loadParent->child(i)->setText(5, tw->bls(l.set()).name());
    blockSignals(false);
}

void treeView::setBLS(const int &i, const ColinBLS &l)
{
	blockSignals(true);
	blsParent->child(i)->setText(1, l.name());
	blsParent->child(i)->setText(2, l.color().name());
	blsParent->child(i)->setIcon(2, colinIcons::instance().smallIcon(l.color()));
	blockSignals(false);
}

void treeView::setCLS(const int &i, const ColinCLS &l)
{
	blockSignals(true);
	clsParent->child(i)->setText(1, l.name());
	for(int j=tw->cls(i).count(); j<clsParent->child(i)->childCount();)
	{
		delete clsParent->child(i)->takeChild(j);
	}
	for(int j=clsParent->child(i)->childCount(); j<tw->cls(i).count(); j++){
		QTreeWidgetItem *bls = new QTreeWidgetItem(clsParent->child(i));
		clsParent->child(i)->addChild(bls);
	}
	for(int j=0; j<tw->cls(i).count(); j++){
		clsParent->child(i)->child(j)->setData(0, 32, ColinStruct::ID_clsbls);
		clsParent->child(i)->child(j)->setData(1, 32, ColinStruct::CLSBLS);
		clsParent->child(i)->child(j)->setData(2, 32, ColinStruct::CLSBLSfac);
		clsParent->child(i)->child(j)->setIcon(0, colinIcons::instance().icon(Colin::BLS));
		clsParent->child(i)->child(j)->setText(0, QString("#%1").arg(j));
		clsParent->child(i)->child(j)->setText(1, tw->bls(tw->cls(i).bls(j)).name());
		clsParent->child(i)->child(j)->setText(2, QString("%1").arg(tw->cls(i).fac(j)));
	}
	addVoidBLSCLS(i);
	blockSignals(false);
}

void treeView::recount(QTreeWidgetItem *parent, const int &i)
{
	for(int j=i; j<parent->childCount()-1; j++)
    {
        parent->child(j)->setText(0, QString("#%1").arg(j));
    }
}

void treeView::actualEH()
{
    setHeader();
    setTw(tw);
}

void treeView::setHeader()
{
    //nodeParent->setText(0, tr("nodes"));
    nodeParent->setText(1, "X[m]");
    nodeParent->setText(2, "Z[m]");
    //beamParent->setText(0, tr("beams"));
    beamParent->setText(1, tr("left node"));
    beamParent->setText(2, tr("right node"));
    beamParent->setText(3, tr("material"));
    beamParent->setText(4, tr("cross section"));
    //loadParent->setText(0, tr("load"));
    loadParent->setText(1, tr("position"));
    loadParent->setText(2, QString("Px[%1]/[%1/m]").arg(unitSettings::instance().Peh()));
    loadParent->setText(3, QString("Pz[%1]/[%1/m]").arg(unitSettings::instance().Peh()));
	loadParent->setText(4, QString("M[%1]").arg(unitSettings::instance().Meh()));
	loadParent->setText(5, tr("bls"));
	blsParent->setText(1, tr("basic sets"));
	blsParent->setText(1, tr("color"));
	clsParent->setText(1, tr("combined sets"));
	clsParent->setText(2, tr("factor"));

}

void treeView::dumpTw(QObject *o)
{
    if(o == tw)
        tw = 0;
}

void treeView::setTw(ColinStruct *t)
{

    selectionisUpToDate = true;
    openitem = 0;

    if(tw)
    {

	tw->disconnect(this,            SLOT(actualSelection()));
	tw->disconnect(this,            SLOT(addBeam(int)));
	tw->disconnect(this,            SLOT(addLoad(int)));
	tw->disconnect(this,            SLOT(addNode(int)));
	tw->disconnect(this,			SLOT(addBLS(int)));
	tw->disconnect(this,			SLOT(addCLS(int)));
	tw->disconnect(this,            SLOT(removeBeam(int)));
	tw->disconnect(this,            SLOT(removeLoad(int)));
	tw->disconnect(this,            SLOT(removeNode(int)));
	tw->disconnect(this,            SLOT(removeBLS(int)));
	tw->disconnect(this,            SLOT(removeCLS(int)));
	tw->disconnect(this,            SLOT(setBeam(int)));
	tw->disconnect(this,            SLOT(setLoad(int)));
	tw->disconnect(this,            SLOT(setNode(int)));
	tw->disconnect(this,            SLOT(setBLS(int)));
	tw->disconnect(this,            SLOT(setCLS(int)));
	tw->disconnect(this,            SLOT(disableSelection()));
	tw->disconnect(this,            SLOT(enableSelection()));

    }
    tw = t;
    QTreeWidgetItem *par = 0;
	for(int j=0; j<5; j++)
    {
        par = this->topLevelItem(j);
        while(0 < par->childCount())
        {
            delete par->child(0);
        }
    }

    if(t != 0)
    {
        fill();

        selectionisUpToDate = false;
        actualSelection();

        connect(tw,             SIGNAL(destroyed(QObject*)),
                this,           SLOT(dumpTw(QObject*)));

        connect(tw,             SIGNAL(selectionChanged()),
                this,           SLOT(actualSelection()));

		connect(tw,				SIGNAL(beginS(QString)),
				this,			SLOT(disableSelection()));

		connect(tw,				SIGNAL(endS()),
				this,			SLOT(enableSelection()));

/**************
 *  COMMANDS  *
 **************/

        connect(tw,             SIGNAL(addedBeam(int)),
                this,           SLOT(addBeam(int)));

        connect(tw,             SIGNAL(addedLoad(int)),
				this,           SLOT(addLoad(int)));

		connect(tw,             SIGNAL(addedNode(int)),
				this,           SLOT(addNode(int)));

		connect(tw,             SIGNAL(addedBLS(int)),
				this,           SLOT(addBLS(int)));

		connect(tw,             SIGNAL(addedCLS(int)),
				this,           SLOT(addCLS(int)));


        connect(tw,             SIGNAL(changedBeam(int)),
                this,           SLOT(setBeam(int)));

        connect(tw,             SIGNAL(changedNode(int)),
				this,           SLOT(setNode(int)));

		connect(tw,             SIGNAL(changedLoad(int)),
				this,           SLOT(setLoad(int)));

		connect(tw,             SIGNAL(changedBLS(int)),
				this,           SLOT(setBLS(int)));

		connect(tw,             SIGNAL(changedCLS(int)),
				this,           SLOT(setCLS(int)));


        connect(tw,             SIGNAL(erasedBeam(int)),
                this,           SLOT(removeBeam(int)));

        connect(tw,             SIGNAL(erasedNode(int)),
				this,           SLOT(removeNode(int)));

		connect(tw,             SIGNAL(erasedLoad(int)),
				this,           SLOT(removeLoad(int)));

		connect(tw,             SIGNAL(erasedBLS(int)),
				this,           SLOT(removeBLS(int)));

		connect(tw,             SIGNAL(erasedCLS(int)),
				this,           SLOT(removeCLS(int)));


    }
    else
        selectionisUpToDate = false;

}

void treeView::fill()
{
	this->blockSignals(true);
    for(int i=0; i<tw->node_n(); i++)
        addNode(tw->node(i), i);
	addVoidNode();
    for(int i=0; i<tw->beam_n(); i++)
        addBeam(tw->beam(i), i);
	addVoidBeam();
	for(int i=0; i<tw->load_n(); i++)
		addLoad(tw->load(i), i);
	addVoidLoad();
	for(int i=0; i<tw->bls_n(); i++)
		addBLS(tw->bls(i), i);
	addVoidBLS();
	for(int i=0; i<tw->cls_n(); i++)
		addCLS(tw->cls(i), i);
	addVoidCLS();
	recount(nodeParent, 0);
	recount(beamParent, 0);
	recount(loadParent, 0);
	recount(blsParent, 0);
	recount(clsParent, 0);
	this->blockSignals(false);
}

void treeView::actualSelection()
{
    if(selectionisUpToDate)
        return;
    selectionisUpToDate = true;
    for(int i=0; i<tw->node_n(); i++)
    {
        nodeParent->child(i)->setSelected(tw->node(i).isSelected());
        if(tw->node(i).hasbearing())
        {
            nodeParent->child(i)->child(0)->setSelected(tw->node(i).bearing().isSelected());
        }
    }
    for(int i=0; i<tw->beam_n(); i++)
        beamParent->child(i)->setSelected(tw->beam(i).isSelected());
    for(int i=0; i<tw->load_n(); i++)
	loadParent->child(i)->setSelected(tw->load(i).isSelected());
    selectionisUpToDate = false;
}

void treeView::selectionChanged()
{
    if(tw == 0)
        return;
    if(selectionisUpToDate)
        return;
    tw->blockSignals(true);
    for(int i=0; i<tw->node_n(); i++)
    {
	tw->selectNode(i, nodeParent->child(i)->isSelected());
        if(nodeParent->child(i)->childCount()>0)
            tw->selectBearing(i, nodeParent->child(i)->child(0)->isSelected());
    }
    for(int i=0; i<tw->beam_n(); i++)
	tw->selectBeam(i, beamParent->child(i)->isSelected());
    for(int i=0; i<tw->load_n(); i++)
	tw->selectLoad(i, loadParent->child(i)->isSelected());
    selectionisUpToDate = true;
    tw->blockSignals(false);
    tw->emitSelectionChanged();
    selectionisUpToDate = false;

}


void treeView::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_F2)
    {
	if(!selectedItems().isEmpty())
	{
	    for(int i=1; i<columnCount(); i++)
	    {
		if(selectedItems().at(0)->data(i, 32).toInt() != ColinStruct::uneditable)
		{
		    popupEditor(indexFromItem(selectedItems().at(0), i));
		    e->setAccepted(true);
		    break;
		}
	    }
	}
	return;
    }
    else if(e->key() == Qt::Key_Down)
    {
	if(selectedItems().count() == 1)
	{
	    if(indexBelow(indexFromItem(selectedItems().at(0), 0)).isValid())
	    {
		QTreeWidgetItem *sel = selectedItems().at(0);
		sel->setSelected(false);
		itemBelow(sel)->setSelected(true);
	    }
	}
	e->accept();
	return;
    }
    else if(e->key() == Qt::Key_Up)
    {
	if(selectedItems().count() == 1)
	{
	    if(indexAbove(indexFromItem(selectedItems().at(0), 0)).isValid())
	    {
		QTreeWidgetItem *sel = selectedItems().at(0);
		sel->setSelected(false);
		itemAbove(sel)->setSelected(true);
	    }
	}
	e->accept();
	return;
    }
    QTreeWidget::keyPressEvent(e);
}

void treeView::dropEvent(QDropEvent *e)
{
	QStandardItemModel model;
	model.dropMimeData(e->mimeData(), Qt::CopyAction, 0,0, QModelIndex());

	bool ok;
	QString s = model.item(0, 0)->text();
	s.remove("#");
	int new_node = s.toInt(&ok);

	if(!ok) return;

	QTreeWidgetItem *dropItem = itemAt(e->pos());
	int row = indexAt(e->pos()).row();
	int column = indexAt(e->pos()).column();

	if(model.item(0,0)->data(32) == ColinStruct::ID_node)
	{
		if(dropItem->data(0, 32) == ColinStruct::ID_beam)
		{
			if(column==1)
				tw->setLeft(row, new_node);
			if(column==2)
				tw->setLeft(row, new_node);
		}
		else if(dropItem->data(0, 32) == ColinStruct::ID_load)
		{
			if(tw->load(row).typ() == ColinLoad::nodeLoad)
				tw->setPos(row, new_node);
		}
	}
	if(model.item(0,0)->data(32) == ColinStruct::ID_beam)
	{
		if(dropItem->data(0, 32) == ColinStruct::ID_load)
		{
			if(tw->load(row).typ() != ColinLoad::nodeLoad)
				tw->setPos(row, new_node);
		}
	}
	if(model.item(0,0)->data(32) == ColinStruct::ID_load)
	{
		if(dropItem->data(0, 32) == ColinStruct::ID_beam && tw->load(new_node).typ()!=ColinLoad::nodeLoad)
			tw->setPos(new_node, row);
		if(dropItem->data(0, 32) == ColinStruct::ID_node && tw->load(new_node).typ()==ColinLoad::nodeLoad)
			tw->setPos(new_node, row);
	}
	if(model.item(0,0)->data(32) == ColinStruct::ID_bls)
	{
		if(dropItem->data(0, 32) == ColinStruct::ID_cls)
			tw->addBLStoCLS(row, new_node, 1.0);
		else if(dropItem->data(0, 32) == ColinStruct::ID_bls && dropItem->parent()->data(0, 32) == ColinStruct::ID_cls)
			tw->addBLStoCLS(this->indexFromItem(dropItem->parent()).row(), new_node, 1.0);
	}
}

/*
void treeView::dropEvent(QDropEvent *e)
{
    QString text;
    if(e->mimeData()->hasText())
        text = e->mimeData()->text();
    else
        return;

    if(text.contains("node"))
    {
        text.remove("node id ");
        bool ok;
        int nr = text.toInt(&ok);
        if(ok)
        {
            QModelIndex index = indexAt(mapFromGlobal(e->pos()));
            if(index.data(32).toInt() == wgv_tw::leftNode)
            {
                tw->setLeft(index.row(), nr);
            }
        }
    }
}
*/

void treeView::popupMenu(const QPoint &p)
{
    QTreeWidgetItem *item = itemAt(p);
    if(!item)
        return;
    if(item == nodeParent)
        return;
    if(item == beamParent)
        return;
    if(item == loadParent)
        return;
    if(item->parent() == nodeParent)
    {
        nodeMenu::instance().set(item->parent()->indexOfChild(item));
        nodeMenu::instance().popup(p);
    }
    else if(item->parent() == beamParent)
    {
        beamMenu::instance().set(item->parent()->indexOfChild(item));
        beamMenu::instance().popup(p);
    }
    else if(item->parent() == loadParent)
    {
        int i = item->parent()->indexOfChild(item);
        if(tw->load(i).typ() == ColinLoad::tempChange ||
           tw->load(i).typ() == ColinLoad::tempDiffrence)
        {
            tempMenu::instance().set(item->parent()->indexOfChild(item));
            tempMenu::instance().popup(p);
        }
        else if(tw->load(i).typ() == ColinLoad::moment)
        {
            momentMenu::instance().set(item->parent()->indexOfChild(item));
            momentMenu::instance().popup(p);
        }
        else if(tw->load(i).typ() == ColinLoad::doubleLoadLeft ||
                tw->load(i).typ() == ColinLoad::doubleLoadRight)
        {
            dloadMenu::instance().set(item->parent()->indexOfChild(item));
            dloadMenu::instance().popup(p);
        }
        else if(tw->load(i).typ() == ColinLoad::nodeLoad)
        {
            nloadMenu::instance().set(item->parent()->indexOfChild(item));
            nloadMenu::instance().popup(p);
        }
        else
        {
            loadMenu::instance().set(item->parent()->indexOfChild(item));
            loadMenu::instance().popup(p);
        }

    }

}

void treeView::popupEditor(const QModelIndex &index)
{
    if(index.data(32).toInt() == ColinStruct::uneditable)
        return;
	if(index.column() == 0)
        return;
    if(openitem)
    {
        closePersistentEditor(openitem, opencolumn);
        openitem = 0;
    }
    openPersistentEditor(itemFromIndex(index), index.column());
    opencolumn = index.column();
    openitem = itemFromIndex(index);
}

/*
void treeView::editorClosed(QWidget *w, QAbstractItemDelegate::EndEditHint eh)
{
    (void)w;

    if(eh == QAbstractItemDelegate::EditNextItem)
    {
        if(currentItem()->data(opencolumn+1, 32) == wgv_tw::uneditable)
            return;
        if(openitem)
            closePersistentEditor(currentItem(), opencolumn);
        opencolumn++;
        openPersistentEditor(currentItem(), opencolumn);
    }
    else if(eh == QAbstractItemDelegate::EditPreviousItem)
    {
        if(opencolumn<1)
            return;
        if(openitem)
            closePersistentEditor(currentItem(), opencolumn);
        opencolumn--;
        openPersistentEditor(currentItem(), opencolumn);
    }
}
*/

void treeView::nextColumn()
{
    if(opencolumn == 4)
        return;
    if(currentItem()->data(opencolumn+1, 32) == ColinStruct::uneditable)
        return;

    if(openitem)
        closePersistentEditor(openitem, opencolumn);
    opencolumn++;
    openPersistentEditor(currentItem(), opencolumn);
    openitem = currentItem();
}

void treeView::previousColumn()
{
    if(opencolumn<2)
        return;
    if(openitem)
    closePersistentEditor(openitem, opencolumn);
    opencolumn--;
    openPersistentEditor(currentItem(), opencolumn);
    openitem = currentItem();
}

void treeView::nextItem()
{
    if(openitem)
    {
        closePersistentEditor(openitem, opencolumn);
        openitem = 0;
    }
    if(indexBelow(currentIndex()).isValid())
    {
        setCurrentIndex(indexBelow(indexFromItem(currentItem(), currentIndex().column())));
        if(currentItem()->data(opencolumn, 32) == ColinStruct::uneditable)
            return;
        openPersistentEditor(currentItem(), opencolumn);
        openitem = currentItem();
    }
}

void treeView::previousItem()
{
    if(openitem)
    {
        closePersistentEditor(openitem, opencolumn);
        openitem = 0;
    }
    if(indexAbove(currentIndex()).isValid())
    {
        setCurrentIndex(indexAbove(indexFromItem(currentItem(), currentIndex().column())));
        if(currentItem()->data(opencolumn, 32) == ColinStruct::uneditable)
            return;
        openPersistentEditor(currentItem(), opencolumn);
        openitem = currentItem();
    }
}

void treeView::writeData(QTreeWidgetItem *item, int column)
{

    //if(isChanging)
    //    return;
    //isChanging = true;

    this->closePersistentEditor(item, column);

    bool ok;

	ColinStruct::VarNames toChange = static_cast<ColinStruct::VarName>(item->data(column, 32).toInt());

	if((toChange & ColinStruct::creation) == ColinStruct::creation)
	{
		toChange &= ~ColinStruct::creation;

		switch(toChange)
		{
		case ColinStruct::x:
		case ColinStruct::z:
			checkAndCreateNode();
			return;
		case ColinStruct::leftNode:
		case ColinStruct::rightNode:
		case ColinStruct::Mat:
		case ColinStruct::CrossSection:
			checkAndCreateBeam();
			return;
		case ColinStruct::Px:
		case ColinStruct::Pz:
		case ColinStruct::M:
		case ColinStruct::Pos:
			checkAndCreateLoad();
			return;
		case ColinStruct::BLSname:
			checkAndCreateBLS();
			return;
		case ColinStruct::CLSname:
			checkAndCreateCLS();
			return;
		case ColinStruct::CLSBLS:
		case ColinStruct::CLSBLSfac:
			checkAndCreateCLSBLS();
			return;
		default:
			return;
		}
	}
	switch(toChange)
    {
	case ColinStruct::uneditable:
	case ColinStruct::ID_node:
	case ColinStruct::ID_beam:
	case ColinStruct::ID_bearing:
	case ColinStruct::ID_bls:
	case ColinStruct::ID_cls:
	case ColinStruct::ID_load:
		break;
/****************
 *   X-KOORD    *
 ****************/
    case ColinStruct::x:
        item->text(column).toDouble(&ok);
        if(ok)
                tw->setX(item->parent()->indexOfChild(item), item->text(column).toDouble());
        else
            item->setText(column, QString("%1").arg(tw->node(item->parent()->indexOfChild(item)).x()));
#ifndef QT_NO_DEBUG
		qDebug() << "treeView: handled: " << item->data(column, 32).toInt()
			   <<  (ok ? " value accepted" : " value not accepted");
#endif
        break;
/****************
 *   Z-KOORD    *
 ****************/
    case ColinStruct::z:
        item->text(column).toDouble(&ok);
        if(ok)
            tw->setZ(item->parent()->indexOfChild(item), item->data(column, 0).toDouble());
        else
            item->setText(column, QString("%1").arg(tw->node(item->parent()->indexOfChild(item)).z()));
#ifndef QT_NO_DEBUG
		qDebug() << "treeView: handled: " << item->data(column, 32).toInt()
			   <<  (ok ? " value accepted" : " value not accepted");
#endif
        break;
/****************
 *  NODEANGLE   *
 ****************/
    case ColinStruct::alpha_k:
        item->text(column).toDouble(&ok);
        if(ok)
            tw->setAngle(indexFromItem(item->parent()).row(), -item->data(column, 0).toDouble()/ANGLEPREFIX);
        else
            item->setText(column, QString::number(tw->node(item->parent()->indexOfChild(item)).angle()*ANGLEPREFIX));
#ifndef QT_NO_DEBUG
		qDebug() << "treeView: handled: " << item->data(column, 32).toInt()
			   <<  (ok ? " value accepted" : " value not accepted");
#endif
        break;
/****************
 *  LEFT NODE   *
 ****************/
    case ColinStruct::leftNode:
        if(item->text(column).toInt(&ok)< 0 ||item->text(column).toInt(&ok)>=tw->node_n())
            ok = false;
        if(ok)
            tw->setLeft(item->parent()->indexOfChild(item), item->data(column, 0).toInt());
        else
            item->setText(column, QString("%1").arg(tw->beam(item->parent()->indexOfChild(item)).leftNodeI()));
#ifndef QT_NO_DEBUG
		qDebug() << "treeView: handled: " << item->data(column, 32).toInt()
			   <<  (ok ? " value accepted" : " value not accepted");
#endif
        break;
/****************
 *  RIGHT NODE  *
 ****************/
    case ColinStruct::rightNode:
        if(item->text(column).toInt(&ok)< 0 ||item->text(column).toInt(&ok)>=tw->node_n())
            ok = false;
        if(ok)
            tw->setRight(item->parent()->indexOfChild(item), item->data(column, 0).toInt());
        else
            item->setText(column, QString("%1").arg(tw->beam(item->parent()->indexOfChild(item)).leftNodeI()));
#ifndef QT_NO_DEBUG
		qDebug() << "treeView: handled: " << item->data(column, 32).toInt()
			   <<  (ok ? " value accepted" : " value not accepted");
#endif
        break;
/*******************
 *     MATERIAL    *
 *******************/
    case ColinStruct::Mat:
        if(item->text(column)!="")
            tw->setMat(item->parent()->indexOfChild(item), item->text(column));
#ifndef QT_NO_DEBUG
		qDebug() << "treeView: handled: " << item->data(column, 32).toInt();
#endif
        break;
/*******************
 *     PROFILE     *
 *******************/
	case ColinStruct::CrossSection:
        if(item->text(column)!="")
            tw->setProfile(item->parent()->indexOfChild(item), item->text(column));
#ifndef QT_NO_DEBUG
		qDebug() << "treeView: handled: " << item->data(column, 32).toInt();
#endif
        break;
/*******************
 *  JOINT/SPRING   *
 *******************/
    case ColinStruct::Spring:
        item->text(column).toDouble(&ok);
        if(item->text(column) == tr("free"))
        {
            tw->setJoint(indexFromItem(item->parent()).row(), item->parent()->indexOfChild(item), true);
#ifndef QT_NO_DEBUG
		qDebug() << "treeView: handled: " << item->data(column, 32).toInt()
			   <<  " value accepted ( free )";
#endif
        }
        else if(item->text(column) == tr("locked"))
        {
            tw->setJoint(indexFromItem(item->parent()).row(), item->parent()->indexOfChild(item), false);
#ifndef QT_NO_DEBUG
		qDebug() << "treeView: handled: " << item->data(column, 32).toInt()
			   <<  " value accepted ( locked )";
#endif
        }
        else if(ok)
        {
            if(item->parent()->indexOfChild(item) == 2 || item->parent()->indexOfChild(item) == 5) //moment
                tw->setSpring(indexFromItem(item->parent()).row(), item->parent()->indexOfChild(item), item->text(column).toDouble()/FMPREFIX);
            else
                tw->setSpring(indexFromItem(item->parent()).row(), item->parent()->indexOfChild(item), item->text(column).toDouble()/FPREFIX);
#ifndef QT_NO_DEBUG
		qDebug() << "treeView: handled: " << item->data(column, 32).toInt()
			   <<  (ok ? " value accepted" : " value not accepted");
#endif
        }
        break;
/****************
 *  BEARING X   *
 ****************/
    case ColinStruct::Bearing_h:
        item->text(column).toDouble(&ok);
        if(item->text(column) == tr("free"))
        {
            tw->setBx(indexFromItem(item->parent()).row(), false);
//#ifndef QT_NO_DEBUG
//        qDebug() << "treeView: handled: " << item->data(column, 32).toInt()
//               <<  " value accepted ( free )";
//#endif
        }
        else if(item->text(column) == tr("locked"))
        {
            tw->setBx(indexFromItem(item->parent()).row(), true);
#ifndef QT_NO_DEBUG
		qDebug() << "treeView: handled: " << item->data(column, 32).toInt()
			   <<  " value accepted ( locked )";
#endif
        }
        else if(ok)
        {
            tw->setCx(indexFromItem(item->parent()).row(), item->text(column).toDouble()/FPREFIX);
//            item->setText(column, "c = "+ QString::number(tw->node(indexFromItem(item->parent()).row()).bearing().xf()*FPREFIX));
#ifndef QT_NO_DEBUG
		qDebug() << "treeView: handled: " << item->data(column, 32).toInt()
			   <<  (ok ? " value accepted" : " value not accepted");
#endif
        }
        break;
/****************
 *  BEARING Z   *
 ****************/
    case ColinStruct::Bearing_v:
        item->text(column).toDouble(&ok);
        if(item->text(column) == tr("free"))
        {
            tw->setBz(indexFromItem(item->parent()).row(), false);
//#ifndef QT_NO_DEBUG
//        qDebug() << "treeView: handled: " << item->data(column, 32).toInt()
//               <<  " value accepted ( free )";
//#endif
        }
        else if(item->text(column) == tr("locked"))
        {
            tw->setBz(indexFromItem(item->parent()).row(), true);
#ifndef QT_NO_DEBUG
		qDebug() << "treeView: handled: " << item->data(column, 32).toInt()
			   <<  " value accepted ( locked )";
#endif
        }
        else if(ok)
        {
            tw->setCz(indexFromItem(item->parent()).row(), item->text(column).toDouble()/FPREFIX);
//          item->setText(column, "c = "+ QString::number(tw->node(indexFromItem(item->parent()).row()).bearing().zf()*FPREFIX));
#ifndef QT_NO_DEBUG
		qDebug() << "treeView: handled: " << item->data(column, 32).toInt()
			   <<  (ok ? " value accepted" : " value not accepted");
#endif
        }/*
        if(!tw->node(indexFromItem(item->parent()).row()).hasbearing())
        {
            item->parent()->removeChild(item);
            delete item;
	}*/
	break;
/****************
 *  BEARING M   *
 ****************/
    case ColinStruct::Bearing_m:
        item->text(column).toDouble(&ok);
        if(item->text(column) == tr("free"))
        {
            tw->setBm(indexFromItem(item->parent()).row(), false);
//#ifndef QT_NO_DEBUG
//        qDebug() << "treeView: handled: " << item->data(column, 32).toInt()
//               <<  " value accepted ( free )";
//#endif
        }
        else if(item->text(column) == tr("locked"))
        {
            tw->setBm(indexFromItem(item->parent()).row(), true);
#ifndef QT_NO_DEBUG
		qDebug() << "treeView: handled: " << item->data(column, 32).toInt()
			   <<  " value accepted ( locked )";
#endif
        }
        else if(ok)
        {
            tw->setCm(indexFromItem(item->parent()).row(), item->text(column).toDouble()/FMPREFIX);
//            item->setText(column, "c = "+ QString::number(tw->node(indexFromItem(item->parent()).row()).bearing().phif()*FPREFIX));
#ifndef QT_NO_DEBUG
		qDebug() << "treeView: handled: " << item->data(column, 32).toInt()
			   <<  (ok ? " value accepted" : " value not accepted");
#endif
        }
        break;
/****************
 *   LOAD POS   *
 ****************/
	case ColinStruct::Pos:
        if(item->text(column).toInt(&ok) < 0 ||
           item->text(column).toInt(&ok) >= ((tw->load(item->parent()->indexOfChild(item)).typ()==ColinLoad::nodeLoad  ||
                                                      tw->load(item->parent()->indexOfChild(item)).typ()==ColinLoad::moment)   ?
                                                                         tw->node_n()              :
                                                                         tw->beam_n()))
            ok = false;
        if(ok)
            tw->setPos(item->parent()->indexOfChild(item), item->text(column).toInt());
        else
	    item->setText(column, QString::number(tw->load(item->parent()->indexOfChild(item)).at()));
#ifndef QT_NO_DEBUG
		qDebug() << "treeView: handled: " << item->data(column, 32).toInt()
			   <<  (ok ? " value accepted" : " value not accepted");
#endif
        break;
/****************
 *    LOAD X    *
 ****************/
    case ColinStruct::Px:
        item->text(column).toDouble(&ok);
        if(ok)
            tw->setPx(item->parent()->indexOfChild(item), item->text(column).toDouble()/PPREFIX);
        else
            item->setText(column, QString::number(tw->load(item->parent()->indexOfChild(item)).Px()*PPREFIX));
#ifndef QT_NO_DEBUG
		qDebug() << "treeView: handled: " << item->data(column, 32).toInt()
			   <<  (ok ? " value accepted" : " value not accepted");
#endif
        break;
/****************
 *    LOAD Z    *
 ****************/
    case ColinStruct::Pz:
        item->text(column).toDouble(&ok);
        if(ok)
            tw->setPz(item->parent()->indexOfChild(item), item->text(column).toDouble()/PPREFIX);
        else
            item->setText(column, QString::number(tw->load(item->parent()->indexOfChild(item)).Pz()*PPREFIX));
#ifndef QT_NO_DEBUG
		qDebug() << "treeView: handled: " << item->data(column, 32).toInt()
			   <<  (ok ? " value accepted" : " value not accepted");
#endif
		break;
/****************
 *    Moment    *
 ****************/
	case ColinStruct::M:
		item->text(column).toDouble(&ok);
		if(ok)
			tw->setM(item->parent()->indexOfChild(item), item->text(column).toDouble()/MPREFIX);
		else
			item->setText(column, QString::number(tw->load(item->parent()->indexOfChild(item)).M()*MPREFIX));
#ifndef QT_NO_DEBUG
		qDebug() << "treeView: handled: " << item->data(column, 32).toInt()
			   <<  (ok ? " value accepted" : " value not accepted");
#endif
		break;
/****************
 *      Set     *
 ****************/
	case ColinStruct::LoadSet:
		//item->text(column).toDouble(&ok);

		tw->setLoadSet(item->parent()->indexOfChild(item), tw->getBLSIDbyName(item->text(column)));
#ifndef QT_NO_DEBUG
		qDebug() << "treeView: handled: " << item->data(column, 32).toInt()
			   <<  (ok ? " value accepted" : " value not accepted");
#endif
		break;
/*****************
 *    BLSNAME    *
 *****************/
	case ColinStruct::BLSname:
		if(tw->getBLSIDbyName(item->text(column))==-1)
			tw->setBLSName(item->parent()->indexOfChild(item),item->text(column));
		else
			item->setText(column, tw->bls(item->parent()->indexOfChild(item)).name());
#ifndef QT_NO_DEBUG
		qDebug() << "treeView: handled: " << item->data(column, 32).toInt()
			   <<  (ok ? " value accepted" : " value not accepted");
#endif
		break;
/*****************
 *   BLSCOLOR    *
 *****************/
	case ColinStruct::BLScolor:
		ok=true;
		tw->setBLSColor(item->parent()->indexOfChild(item), QColor(item->text(column)));
#ifndef QT_NO_DEBUG
		qDebug() << "treeView: handled: " << item->data(column, 32).toInt()
			   <<  (ok ? " value accepted" : " value not accepted");
#endif
		break;
/*****************
 *    CLSNAME    *
 *****************/
	case ColinStruct::CLSname:
		if(tw->getCLSIDbyName(item->text(column))==-1)
			tw->setCLSName(item->parent()->indexOfChild(item),item->text(column));
		else
			item->setText(column, tw->cls(item->parent()->indexOfChild(item)).name());
#ifndef QT_NO_DEBUG
		qDebug() << "treeView: handled: " << item->data(column, 32).toInt()
			   <<  (ok ? " value accepted" : " value not accepted");
#endif
		break;
/*****************
 *  BLSCLSNAME   *
 *****************/
	case ColinStruct::CLSBLS:
		ok=true;
#warning unhandled
		//tw->addBLStoCLS(item->parent()->parent()->indexOfChild(item->parent()),tw->getCLSIDbyName(item->text(column)), 1);
#ifndef QT_NO_DEBUG
		qDebug() << "treeView: handled: " << item->data(column, 32).toInt()
			   <<  (ok ? " value accepted" : " value not accepted");
#endif
		break;
/*****************
 *   BLSFACTOR   *
 *****************/
	case ColinStruct::CLSBLSfac:
		item->text(column).toDouble(&ok);
		if(ok)
			tw->setFacbyIndex(item->parent()->parent()->indexOfChild(item->parent()), item->parent()->indexOfChild(item), item->text(column).toDouble());
		else
			item->setText(column, QString::number(tw->cls(item->parent()->parent()->indexOfChild(item->parent())).fac(item->parent()->indexOfChild(item))));
#ifndef QT_NO_DEBUG
		qDebug() << "treeView: handled: " << item->data(column, 32).toInt()
			   <<  (ok ? " value accepted" : " value not accepted");
#endif
		break;
    default:
#ifndef QT_NO_DEBUG
		qDebug() << "treeView: not handled:" << item->data(column, 32).toInt();
#endif
        break;
    }
    isChanging = false;

}


void treeView::addVoidNode()
{
	if(!tw) return;
	if(tw->node_n()!=this->nodeParent->childCount())
		delete nodeParent->takeChild(nodeParent->childCount()-1);

	QTreeWidgetItem *node = new QTreeWidgetItem(nodeParent);

	node->setIcon(0, colinIcons::instance().icon(Colin::addNode));
	node->setText(0, tr("new"));
	node->setText(1, tr("x"));
	node->setText(2, tr("z"));

	node->setData(0, 32, static_cast<int>(ColinStruct::ID_node));
	node->setData(1, 32, static_cast<int>(ColinStruct::x | ColinStruct::creation));
	node->setData(2, 32, static_cast<int>(ColinStruct::z | ColinStruct::creation));
	node->setData(3, 32, static_cast<int>(ColinStruct::uneditable));
	node->setData(4, 32, static_cast<int>(ColinStruct::uneditable));

	this->nodeParent->addChild(node);
}

void treeView::addVoidBeam()
{
	if(!tw) return;
	if(tw->beam_n()!=this->beamParent->childCount())
		delete beamParent->takeChild(beamParent->childCount()-1);

	QTreeWidgetItem *beam = new QTreeWidgetItem(beamParent);

	beam->setIcon(0, colinIcons::instance().icon(Colin::addBeam));
	beam->setText(0, tr("new"));
	beam->setText(1, tr("left"));
	beam->setText(2, tr("right"));
	beam->setText(3, tr("material"));
	beam->setText(4, tr("cross section"));

	beam->setData(0, 32, static_cast<int>(ColinStruct::ID_beam | ColinStruct::creation));
	beam->setData(1, 32, static_cast<int>(ColinStruct::leftNode | ColinStruct::creation));
	beam->setData(2, 32, static_cast<int>(ColinStruct::rightNode | ColinStruct::creation));
	beam->setData(3, 32, static_cast<int>(ColinStruct::Mat | ColinStruct::creation));
	beam->setData(4, 32, static_cast<int>(ColinStruct::CrossSection | ColinStruct::creation));

	this->beamParent->addChild(beam);
}

void treeView::addVoidLoad()
{
	if(!tw) return;
	if(tw->load_n()!=this->loadParent->childCount())
		delete loadParent->takeChild(loadParent->childCount()-1);

	QTreeWidgetItem *load = new QTreeWidgetItem(loadParent);

	load->setIcon(0, colinIcons::instance().icon(Colin::addLoad));
	load->setText(0, tr("new"));
	load->setText(1, tr("position"));
	load->setText(2, tr("Px"));
	load->setText(3, tr("Pz"));
	load->setText(4, tr("M"));

	load->setData(0, 32, static_cast<int>(ColinStruct::ID_load | ColinStruct::creation));
	load->setData(1, 32, static_cast<int>(ColinStruct::Pos | ColinStruct::creation));
	load->setData(2, 32, static_cast<int>(ColinStruct::Px | ColinStruct::creation));
	load->setData(3, 32, static_cast<int>(ColinStruct::Pz | ColinStruct::creation));
	load->setData(4, 32, static_cast<int>(ColinStruct::M | ColinStruct::creation));

	loadParent->addChild(load);
}

void treeView::addVoidBLS()
{
	if(!tw) return;
	if(tw->bls_n()!=this->blsParent->childCount())
		delete blsParent->takeChild(blsParent->childCount()-1);

	QTreeWidgetItem *bls = new QTreeWidgetItem(blsParent);

	bls->setIcon(0, colinIcons::instance().icon(Colin::addBLS));
	bls->setText(0, tr("new"));
	bls->setText(1, tr("name"));

	bls->setData(0, 32, static_cast<int>(ColinStruct::ID_bls | ColinStruct::creation));
	bls->setData(1, 32, static_cast<int>(ColinStruct::BLSname | ColinStruct::creation));
	bls->setData(2, 32, ColinStruct::uneditable);
	bls->setData(3, 32, ColinStruct::uneditable);
	bls->setData(4, 32, ColinStruct::uneditable);

	blsParent->addChild(bls);
}

void treeView::addVoidCLS()
{
	if(!tw) return;
	if(tw->cls_n()!=this->clsParent->childCount())
		delete clsParent->takeChild(clsParent->childCount()-1);

	QTreeWidgetItem *cls = new QTreeWidgetItem(clsParent);

	cls->setIcon(0, colinIcons::instance().icon(Colin::addCLS));
	cls->setText(0, tr("new"));
	cls->setText(1, tr("name"));

	cls->setData(0, 32, static_cast<int>(ColinStruct::ID_cls | ColinStruct::creation));
	cls->setData(1, 32, static_cast<int>(ColinStruct::CLSname | ColinStruct::creation));
	cls->setData(2, 32, ColinStruct::uneditable);
	cls->setData(3, 32, ColinStruct::uneditable);
	cls->setData(4, 32, ColinStruct::uneditable);

	cls->addChild(cls);
}

void  treeView::addVoidBLSCLS(int i)
{
	if(!tw) return;

	QTreeWidgetItem *cls = clsParent->child(i);

	if(tw->cls(i).count()!=cls->childCount())
		delete cls->takeChild(clsParent->childCount()-1);

	QTreeWidgetItem *blscls = new QTreeWidgetItem(cls);
	blscls->setIcon(0, colinIcons::instance().icon(Colin::addBLS));
	blscls->setText(0, tr("add"));
	blscls->setText(1, tr("bls"));
	blscls->setText(2, tr("factor"));


	cls->setData(0, 32, static_cast<int>(ColinStruct::ID_clsbls | ColinStruct::creation));
	blscls->setData(1, 32, static_cast<int>(ColinStruct::CLSBLS | ColinStruct::creation));
	blscls->setData(2, 32, static_cast<int>(ColinStruct::CLSBLSfac | ColinStruct::creation));
	blscls->setData(3, 32, ColinStruct::uneditable);
	blscls->setData(4, 32, ColinStruct::uneditable);

	cls->addChild(blscls);

}


void treeView::checkAndCreateNode()
{
	bool ok;
	double x, y;
	x = nodeParent->child(tw->node_n())->text(1).toDouble(&ok);
	if(!ok) return;
	y = nodeParent->child(tw->node_n())->text(2).toDouble(&ok);
	if(!ok) return;
	tw->addNode(ColinNode(x,y));
	addVoidNode();
	this->setCurrentIndex(this->indexFromItem(nodeParent->child(tw->node_n()), 1));
	this->popupEditor(currentIndex());
}

void treeView::checkAndCreateBeam()
{
	bool ok;
	int l, r, mat, q;
	l = beamParent->child(tw->beam_n())->text(1).toInt(&ok);
	if(!ok) return;
	r = beamParent->child(tw->beam_n())->text(2).toInt(&ok);
	if(!ok) return;
	if(tw->beam_n())
	{
		mat = tw->beam(tw->beam_n()-1).MatI();
		q = tw->beam(tw->beam_n()-1).ProfileI();
	}
	else
	{
		mat = 0;
		q = 0;
	}
	tw->addBeam(ColinBeam(tw, l, r, mat, q));
	addVoidBeam();
}

void treeView::checkAndCreateLoad()
{

}

void treeView::checkAndCreateBLS()
{
	if(blsParent->child(tw->bls_n())->text(1)==tr("name")) return;
	if(blsParent->child(tw->bls_n())->text(1)==tr("")) return;
	tw->addBLS(ColinBLS(blsParent->child(tw->bls_n())->text(1)));
	addVoidBLS();
}

void treeView::checkAndCreateCLS()
{
	if(clsParent->child(tw->cls_n())->text(1)==tr("name")) return;
	if(clsParent->child(tw->cls_n())->text(1)==tr("")) return;
	tw->addCLS(ColinCLS(clsParent->child(tw->cls_n())->text(1)));
	addVoidCLS();
}

void treeView::checkAndCreateCLSBLS()
{
	QString clsnr = currentItem()->parent()->text(0);
	clsnr.remove("#");
	bool ok;
	int cls;
	int bls;
	cls = clsnr.toInt(&ok);
	if(!ok)
		return;
	QString blsname = currentItem()->text(1);
	ColinStruct *tw = filelist::instance().currentFile();
	if(tw==0)
		return;
	ok=false;
	for(bls=0; bls<tw->bls_n(); bls++)
	{
		if(tw->bls(bls).name()==blsname)
		{
			ok=true;
			break;
		}
	}
	if(ok)
		tw->addBLStoCLS(cls, bls, 1);
}
