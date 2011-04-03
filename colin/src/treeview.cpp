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
    this->setColumnCount(5);
    this->setRootIsDecorated(true);
    this->setRootIndex(QModelIndex());

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

    setHeader();

    this->addTopLevelItem(nodeParent);
    this->addTopLevelItem(beamParent);
    this->addTopLevelItem(loadParent);


    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    delegate = new treeDelegate(this);
    this->setItemDelegate(delegate);
    this->setIconSize(QSize(24, 24));
    this->setAllColumnsShowFocus(true);
    this->setAlternatingRowColors(true);
    this->setUniformRowHeights(true);
    //this->setDragDropMode(QAbstractItemView::DragDrop);

    QStringList hlab;
    for(int i =0; i<5; i++) hlab.append("");
    this->setHeaderLabels(hlab);

    for(int i=0; i<5; i++)
    {
        nodeParent->setData(i, 32, wgv_tw::uneditable);
        beamParent->setData(i, 32, wgv_tw::uneditable);
        loadParent->setData(i, 32, wgv_tw::uneditable);
    }

    tw = 0;
    selectionisUpToDate = false;
    openitem = 0;

    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(&unitSettings::instance(),          SIGNAL(settingschanged()),
            this,                               SLOT(actualEH()));

    connect(&filelist::instance(),              SIGNAL(currentChanged(wgv_tw*)),
            this,                               SLOT(setTw(wgv_tw*)));

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




void treeView::addNode(const wgv_node &n, int i)
{
    if(i==-1)
        i=nodeParent->childCount();
    QTreeWidgetItem *node = new QTreeWidgetItem();
    node->setIcon(0, colinIcons::instance().icon(Colin::drawNode));

    node->setData(0, 32, wgv_tw::ID_node);
    node->setData(1, 32, wgv_tw::x);
    node->setData(2, 32, wgv_tw::z);
    node->setData(3, 32, wgv_tw::uneditable);
    node->setData(4, 32, wgv_tw::uneditable);

    nodeParent->insertChild(i, node);
    setNode(i, n);
    recount(nodeParent, i);
}

void treeView::addBeam(const wgv_beam &b, int i)
{
    if(i==-1)
        i=beamParent->childCount();
    QTreeWidgetItem *beam = new QTreeWidgetItem();
    beam->setIcon(0, colinIcons::instance().icon(Colin::drawBeam));
    beam->setData(0, 32, wgv_tw::ID_beam);
    beam->setData(1, 32, wgv_tw::leftNode);
    beam->setData(2, 32, wgv_tw::rightNode);
    beam->setData(3, 32, wgv_tw::Mat);
    beam->setData(4, 32, wgv_tw::Profile);

    beamParent->insertChild(i, beam);

    QTreeWidgetItem *joints;

    for(int j=0; j<6; j++)
    {
        joints = new QTreeWidgetItem(beam);
        joints->setData(0, 32, wgv_tw::uneditable);
        joints->setData(1, 32, wgv_tw::Spring);
        joints->setData(2, 32, wgv_tw::uneditable);
        joints->setData(3, 32, wgv_tw::uneditable);
        joints->setData(4, 32, wgv_tw::uneditable);
    }

    setBeam(i, b);



    recount(beamParent, i);
}

void treeView::addLoad(const wgv_load &l, int i)
{
    if(i==-1)
        i=loadParent->childCount();
    QTreeWidgetItem *load = new QTreeWidgetItem();
        load->setIcon(0, colinIcons::instance().icon(l.typ()));
    if(l.typ() == wgv_load::nodeLoad ||
       l.typ() == wgv_load::moment   )
        load->setIcon(1, colinIcons::instance().icon(Colin::drawNode));
    else
        load->setIcon(1, colinIcons::instance().icon(Colin::drawBeam));

    load->setData(0, 32, wgv_tw::ID_load);
    load->setData(1, 32, wgv_tw::theLoadsBeam);
    if(l.typ() == wgv_load::nodeLoad                ||
       l.typ() == wgv_load::uniformlyDistibutedLoad ||
       l.typ() == wgv_load::increasingLinearLoad    ||
       l.typ() == wgv_load::decreasingLinearLoad    )
    {
        load->setData(2, 32, wgv_tw::Px);
        load->setData(3, 32, wgv_tw::Pz);
        load->setData(4, 32, wgv_tw::uneditable);
    }
    else if( l.typ() == wgv_load::tempChange)
    {
	load->setData(2, 32, wgv_tw::Px);
	load->setData(3, 32, wgv_tw::uneditable);
	load->setData(4, 32, wgv_tw::uneditable);
    }
    else if( l.typ() == wgv_load::tempDiffrence)
    {
	load->setData(2, 32, wgv_tw::uneditable);
	load->setData(3, 32, wgv_tw::Pz);
	load->setData(4, 32, wgv_tw::uneditable);
    }
    else if(l.typ() == wgv_load::moment)
    {
	load->setData(2, 32, wgv_tw::uneditable);
	load->setData(3, 32, wgv_tw::uneditable);
	load->setData(4, 32, wgv_tw::M);
    }
    else if(l.typ() == wgv_load::doubleLoadLeft ||
	    l.typ() == wgv_load::doubleLoadRight)
    {
	load->setData(2, 32, wgv_tw::Px);
	load->setData(3, 32, wgv_tw::Pz);
	load->setData(4, 32, wgv_tw::M);
    }

    loadParent->insertChild(i, load);
    setLoad(i, l);
    recount(loadParent, i);

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

void treeView::setNode(const int &i, const wgv_node &n)
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
            bear->setData(0, 32, wgv_tw::uneditable);
            bear->setData(1, 32, wgv_tw::alpha_k);
            bear->setData(2, 32, wgv_tw::Bearing_h);
            bear->setData(3, 32, wgv_tw::Bearing_v);
            bear->setData(4, 32, wgv_tw::Bearing_m);
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

void treeView::setBeam(const int &i, const wgv_beam &b)
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

void treeView::setLoad(const int &i, const wgv_load &l)
{
    blockSignals(true);
    loadParent->child(i)->setText(1, QString("%1").arg(l.at()));
    if(l.typ() == wgv_load::nodeLoad                ||
       l.typ() == wgv_load::uniformlyDistibutedLoad ||
       l.typ() == wgv_load::increasingLinearLoad    ||
       l.typ() == wgv_load::decreasingLinearLoad    )
    {
        loadParent->child(i)->setText(2, QString("%1").arg(l.Px()*PPREFIX));
        loadParent->child(i)->setText(3, QString("%1").arg(l.Pz()*PPREFIX));
	loadParent->child(i)->setText(4, "-");
    }
    else if(l.typ() == wgv_load::tempChange)
    {
        loadParent->child(i)->setText(2, QString("%1").arg(l.Px()));
        loadParent->child(i)->setData(2, 32, wgv_tw::Px);
	loadParent->child(i)->setText(3, "-");
        loadParent->child(i)->setData(3, 32, wgv_tw::uneditable);
	loadParent->child(i)->setText(4, "-");
    }
    else if(l.typ() == wgv_load::tempDiffrence)
    {
	loadParent->child(i)->setText(2, "-");
        loadParent->child(i)->setData(2, 32, wgv_tw::uneditable);
        loadParent->child(i)->setText(3, QString("%1").arg(l.Pz()));
        loadParent->child(i)->setData(3, 32, wgv_tw::Pz);
	loadParent->child(i)->setText(4, "-");
    }
    else if(l.typ() == wgv_load::moment)
    {
	loadParent->child(i)->setText(2, "-");
	loadParent->child(i)->setText(3, "-");
	loadParent->child(i)->setText(4, QString("%1").arg(l.M()*MPREFIX));
    }
    else if(l.typ() == wgv_load::doubleLoadLeft ||
	    l.typ() == wgv_load::doubleLoadRight)
    {
	loadParent->child(i)->setText(2, QString("%1").arg(l.Px()*PPREFIX));
	loadParent->child(i)->setText(3, QString("%1").arg(l.Pz()*PPREFIX));
	loadParent->child(i)->setText(4, QString("%1").arg(l.M()*MPREFIX));
    }
    loadParent->child(i)->setIcon(0, colinIcons::instance().icon(l.typ()));
    blockSignals(false);
}

void treeView::recount(QTreeWidgetItem *parent, const int &i)
{
    for(int j=i; j<parent->childCount(); j++)
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

}

void treeView::dumpTw(QObject *o)
{
    if(o == tw)
        tw = 0;
}

void treeView::setTw(wgv_tw *t)
{

    selectionisUpToDate = true;
    openitem = 0;

    if(tw)
    {

	tw->disconnect(this,            SLOT(actualSelection()));
	tw->disconnect(this,            SLOT(addBeam(int)));
	tw->disconnect(this,            SLOT(addLoad(int)));
	tw->disconnect(this,            SLOT(addNode(int)));
	tw->disconnect(this,            SLOT(removeBeam(int)));
	tw->disconnect(this,            SLOT(removeLoad(int)));
	tw->disconnect(this,            SLOT(removeNode(int)));
	tw->disconnect(this,            SLOT(setBeam(int)));
	tw->disconnect(this,            SLOT(setLoad(int)));
	tw->disconnect(this,            SLOT(setNode(int)));
	tw->disconnect(this,            SLOT(disableSelection()));
	tw->disconnect(this,            SLOT(enableSelection()));

    }
    tw = t;
    QTreeWidgetItem *par = 0;
    for(int j=0; j<3; j++)
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

        connect(tw,		SIGNAL(beginS(QString)),
                this,		SLOT(disableSelection()));

        connect(tw,		SIGNAL(endS()),
                this,		SLOT(enableSelection()));

/**************
 *  COMMANDS  *
 **************/

        connect(tw,             SIGNAL(addedBeam(int)),
                this,           SLOT(addBeam(int)));

        connect(tw,             SIGNAL(addedLoad(int)),
                this,           SLOT(addLoad(int)));

        connect(tw,             SIGNAL(addedNode(int)),
                this,           SLOT(addNode(int)));


        connect(tw,             SIGNAL(changedBeam(int)),
                this,           SLOT(setBeam(int)));

        connect(tw,             SIGNAL(changedNode(int)),
                this,           SLOT(setNode(int)));

        connect(tw,             SIGNAL(changedLoad(int)),
                this,           SLOT(setLoad(int)));


        connect(tw,             SIGNAL(erasedBeam(int)),
                this,           SLOT(removeBeam(int)));

        connect(tw,             SIGNAL(erasedNode(int)),
                this,           SLOT(removeNode(int)));

        connect(tw,             SIGNAL(erasedLoad(int)),
                this,           SLOT(removeLoad(int)));


    }
    else
        selectionisUpToDate = false;

}

void treeView::fill()
{
    for(int i=0; i<tw->node_n(); i++)
        addNode(tw->node(i), i);
    for(int i=0; i<tw->beam_n(); i++)
        addBeam(tw->beam(i), i);
    for(int i=0; i<tw->load_n(); i++)
        addLoad(tw->load(i), i);
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
		if(selectedItems().at(0)->data(i, 32).toInt() != wgv_tw::uneditable)
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
        if(tw->load(i).typ() == wgv_load::tempChange ||
           tw->load(i).typ() == wgv_load::tempDiffrence)
        {
            tempMenu::instance().set(item->parent()->indexOfChild(item));
            tempMenu::instance().popup(p);
        }
        else if(tw->load(i).typ() == wgv_load::moment)
        {
            momentMenu::instance().set(item->parent()->indexOfChild(item));
            momentMenu::instance().popup(p);
        }
        else if(tw->load(i).typ() == wgv_load::doubleLoadLeft ||
                tw->load(i).typ() == wgv_load::doubleLoadRight)
        {
            dloadMenu::instance().set(item->parent()->indexOfChild(item));
            dloadMenu::instance().popup(p);
        }
        else if(tw->load(i).typ() == wgv_load::nodeLoad)
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
    if(index.data(32).toInt() == wgv_tw::uneditable)
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
    if(currentItem()->data(opencolumn+1, 32) == wgv_tw::uneditable)
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
        if(currentItem()->data(opencolumn, 32) == wgv_tw::uneditable)
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
        if(currentItem()->data(opencolumn, 32) == wgv_tw::uneditable)
            return;
        openPersistentEditor(currentItem(), opencolumn);
        openitem = currentItem();
    }
}

void treeView::writeData(QTreeWidgetItem *item, int column)
{
#ifndef QT_NO_DEBUG
    QTextStream debugS(stdout);
#endif

    //if(isChanging)
    //    return;
    //isChanging = true;

    this->closePersistentEditor(item, column);

    bool ok;
    switch(item->data(column, 32).toInt())
    {
/****************
 *   X-KOORD    *
 ****************/
    case wgv_tw::x:
        item->text(column).toDouble(&ok);
        if(ok)
                tw->setX(item->parent()->indexOfChild(item), item->text(column).toDouble());
        else
            item->setText(column, QString("%1").arg(tw->node(item->parent()->indexOfChild(item)).x()));
#ifndef QT_NO_DEBUG
        debugS << "treeView: handled: " << item->data(column, 32).toInt()
               <<  (ok ? " value accepted" : " value not accepted") << endl;
#endif
        break;
/****************
 *   Z-KOORD    *
 ****************/
    case wgv_tw::z:
        item->text(column).toDouble(&ok);
        if(ok)
            tw->setZ(item->parent()->indexOfChild(item), item->data(column, 0).toDouble());
        else
            item->setText(column, QString("%1").arg(tw->node(item->parent()->indexOfChild(item)).z()));
#ifndef QT_NO_DEBUG
        debugS << "treeView: handled: " << item->data(column, 32).toInt()
               <<  (ok ? " value accepted" : " value not accepted") << endl;
#endif
        break;
/****************
 *  NODEANGLE   *
 ****************/
    case wgv_tw::alpha_k:
        item->text(column).toDouble(&ok);
        if(ok)
            tw->setAngle(indexFromItem(item->parent()).row(), -item->data(column, 0).toDouble()/ANGLEPREFIX);
        else
            item->setText(column, QString::number(tw->node(item->parent()->indexOfChild(item)).angle()*ANGLEPREFIX));
#ifndef QT_NO_DEBUG
        debugS << "treeView: handled: " << item->data(column, 32).toInt()
               <<  (ok ? " value accepted" : " value not accepted") << endl;
#endif
        break;
/****************
 *  LEFT NODE   *
 ****************/
    case wgv_tw::leftNode:
        if(item->text(column).toInt(&ok)< 0 ||item->text(column).toInt(&ok)>=tw->node_n())
            ok = false;
        if(ok)
            tw->setLeft(item->parent()->indexOfChild(item), item->data(column, 0).toInt());
        else
            item->setText(column, QString("%1").arg(tw->beam(item->parent()->indexOfChild(item)).leftNodeI()));
#ifndef QT_NO_DEBUG
        debugS << "treeView: handled: " << item->data(column, 32).toInt()
               <<  (ok ? " value accepted" : " value not accepted") << endl;
#endif
        break;
/****************
 *  RIGHT NODE  *
 ****************/
    case wgv_tw::rightNode:
        if(item->text(column).toInt(&ok)< 0 ||item->text(column).toInt(&ok)>=tw->node_n())
            ok = false;
        if(ok)
            tw->setRight(item->parent()->indexOfChild(item), item->data(column, 0).toInt());
        else
            item->setText(column, QString("%1").arg(tw->beam(item->parent()->indexOfChild(item)).leftNodeI()));
#ifndef QT_NO_DEBUG
        debugS << "treeView: handled: " << item->data(column, 32).toInt()
               <<  (ok ? " value accepted" : " value not accepted") << endl;
#endif
        break;
/*******************
 *     MATERIAL    *
 *******************/
    case wgv_tw::Mat:
        if(item->text(column)!="")
            tw->setMat(item->parent()->indexOfChild(item), item->text(column));
#ifndef QT_NO_DEBUG
        debugS << "treeView: handled: " << item->data(column, 32).toInt() << endl;
#endif
        break;
/*******************
 *     PROFILE     *
 *******************/
    case wgv_tw::Profile:
        if(item->text(column)!="")
            tw->setProfile(item->parent()->indexOfChild(item), item->text(column));
#ifndef QT_NO_DEBUG
        debugS << "treeView: handled: " << item->data(column, 32).toInt() << endl;
#endif
        break;
/*******************
 *  JOINT/SPRING   *
 *******************/
    case wgv_tw::Spring:
        item->text(column).toDouble(&ok);
        if(item->text(column) == tr("free"))
        {
            tw->setJoint(indexFromItem(item->parent()).row(), item->parent()->indexOfChild(item), true);
#ifndef QT_NO_DEBUG
        debugS << "treeView: handled: " << item->data(column, 32).toInt()
               <<  " value accepted ( free )" << endl;
#endif
        }
        else if(item->text(column) == tr("locked"))
        {
            tw->setJoint(indexFromItem(item->parent()).row(), item->parent()->indexOfChild(item), false);
#ifndef QT_NO_DEBUG
        debugS << "treeView: handled: " << item->data(column, 32).toInt()
               <<  " value accepted ( locked )" << endl;
#endif
        }
        else if(ok)
        {
            if(item->parent()->indexOfChild(item) == 2 || item->parent()->indexOfChild(item) == 5) //moment
                tw->setSpring(indexFromItem(item->parent()).row(), item->parent()->indexOfChild(item), item->text(column).toDouble()/FMPREFIX);
            else
                tw->setSpring(indexFromItem(item->parent()).row(), item->parent()->indexOfChild(item), item->text(column).toDouble()/FPREFIX);
#ifndef QT_NO_DEBUG
        debugS << "treeView: handled: " << item->data(column, 32).toInt()
               <<  (ok ? " value accepted" : " value not accepted") << endl;
#endif
        }
        break;
/****************
 *  BEARING X   *
 ****************/
    case wgv_tw::Bearing_h:
        item->text(column).toDouble(&ok);
        if(item->text(column) == tr("free"))
        {
            tw->setBx(indexFromItem(item->parent()).row(), false);
//#ifndef QT_NO_DEBUG
//        debugS << "treeView: handled: " << item->data(column, 32).toInt()
//               <<  " value accepted ( free )" << endl;
//#endif
        }
        else if(item->text(column) == tr("locked"))
        {
            tw->setBx(indexFromItem(item->parent()).row(), true);
#ifndef QT_NO_DEBUG
        debugS << "treeView: handled: " << item->data(column, 32).toInt()
               <<  " value accepted ( locked )" << endl;
#endif
        }
        else if(ok)
        {
            tw->setCx(indexFromItem(item->parent()).row(), item->text(column).toDouble()/FPREFIX);
//            item->setText(column, "c = "+ QString::number(tw->node(indexFromItem(item->parent()).row()).bearing().xf()*FPREFIX));
#ifndef QT_NO_DEBUG
        debugS << "treeView: handled: " << item->data(column, 32).toInt()
               <<  (ok ? " value accepted" : " value not accepted") << endl;
#endif
        }
        break;
/****************
 *  BEARING Z   *
 ****************/
    case wgv_tw::Bearing_v:
        item->text(column).toDouble(&ok);
        if(item->text(column) == tr("free"))
        {
            tw->setBz(indexFromItem(item->parent()).row(), false);
//#ifndef QT_NO_DEBUG
//        debugS << "treeView: handled: " << item->data(column, 32).toInt()
//               <<  " value accepted ( free )" << endl;
//#endif
        }
        else if(item->text(column) == tr("locked"))
        {
            tw->setBz(indexFromItem(item->parent()).row(), true);
#ifndef QT_NO_DEBUG
        debugS << "treeView: handled: " << item->data(column, 32).toInt()
               <<  " value accepted ( locked )" << endl;
#endif
        }
        else if(ok)
        {
            tw->setCz(indexFromItem(item->parent()).row(), item->text(column).toDouble()/FPREFIX);
//          item->setText(column, "c = "+ QString::number(tw->node(indexFromItem(item->parent()).row()).bearing().zf()*FPREFIX));
#ifndef QT_NO_DEBUG
        debugS << "treeView: handled: " << item->data(column, 32).toInt()
               <<  (ok ? " value accepted" : " value not accepted") << endl;
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
    case wgv_tw::Bearing_m:
        item->text(column).toDouble(&ok);
        if(item->text(column) == tr("free"))
        {
            tw->setBm(indexFromItem(item->parent()).row(), false);
//#ifndef QT_NO_DEBUG
//        debugS << "treeView: handled: " << item->data(column, 32).toInt()
//               <<  " value accepted ( free )" << endl;
//#endif
        }
        else if(item->text(column) == tr("locked"))
        {
            tw->setBm(indexFromItem(item->parent()).row(), true);
#ifndef QT_NO_DEBUG
        debugS << "treeView: handled: " << item->data(column, 32).toInt()
               <<  " value accepted ( locked )" << endl;
#endif
        }
        else if(ok)
        {
            tw->setCm(indexFromItem(item->parent()).row(), item->text(column).toDouble()/FMPREFIX);
//            item->setText(column, "c = "+ QString::number(tw->node(indexFromItem(item->parent()).row()).bearing().phif()*FPREFIX));
#ifndef QT_NO_DEBUG
        debugS << "treeView: handled: " << item->data(column, 32).toInt()
               <<  (ok ? " value accepted" : " value not accepted") << endl;
#endif
        }
        break;
/****************
 *   LOAD POS   *
 ****************/
    case wgv_tw::theLoadsBeam:
        if(item->text(column).toInt(&ok) < 0 ||
           item->text(column).toInt(&ok) >= ((tw->load(item->parent()->indexOfChild(item)).typ()==wgv_load::nodeLoad  ||
                                                      tw->load(item->parent()->indexOfChild(item)).typ()==wgv_load::moment)   ?
                                                                         tw->node_n()              :
                                                                         tw->beam_n()))
            ok = false;
        if(ok)
            tw->setPos(item->parent()->indexOfChild(item), item->text(column).toInt());
        else
	    item->setText(column, QString::number(tw->load(item->parent()->indexOfChild(item)).at()));
#ifndef QT_NO_DEBUG
        debugS << "treeView: handled: " << item->data(column, 32).toInt()
               <<  (ok ? " value accepted" : " value not accepted") << endl;
#endif
        break;
/****************
 *    LOAD X    *
 ****************/
    case wgv_tw::Px:
        item->text(column).toDouble(&ok);
        if(ok)
            tw->setPx(item->parent()->indexOfChild(item), item->text(column).toDouble()/PPREFIX);
        else
            item->setText(column, QString::number(tw->load(item->parent()->indexOfChild(item)).Px()*PPREFIX));
#ifndef QT_NO_DEBUG
        debugS << "treeView: handled: " << item->data(column, 32).toInt()
               <<  (ok ? " value accepted" : " value not accepted") << endl;
#endif
        break;
/****************
 *    LOAD Z    *
 ****************/
    case wgv_tw::Pz:
        item->text(column).toDouble(&ok);
        if(ok)
            tw->setPz(item->parent()->indexOfChild(item), item->text(column).toDouble()/PPREFIX);
        else
            item->setText(column, QString::number(tw->load(item->parent()->indexOfChild(item)).Pz()*PPREFIX));
#ifndef QT_NO_DEBUG
        debugS << "treeView: handled: " << item->data(column, 32).toInt()
               <<  (ok ? " value accepted" : " value not accepted") << endl;
#endif
        break;
/****************
 *    Moment    *
 ****************/
    case wgv_tw::M:
        item->text(column).toDouble(&ok);
        if(ok)
            tw->setM(item->parent()->indexOfChild(item), item->text(column).toDouble()/MPREFIX);
        else
            item->setText(column, QString::number(tw->load(item->parent()->indexOfChild(item)).M()*MPREFIX));
#ifndef QT_NO_DEBUG
        debugS << "treeView: handled: " << item->data(column, 32).toInt()
               <<  (ok ? " value accepted" : " value not accepted") << endl;
#endif
        break;
    default:
#ifndef QT_NO_DEBUG
        debugS << "treeView: not handled:" << item->data(column, 32).toInt() << endl;
#endif
        break;
    }
    isChanging = false;

}

