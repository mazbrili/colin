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

#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QMenu>

#include "treeview.h"
#include "treemodel.h"

delMenu::delMenu(int clsId, int blsId, QWidget *parent) : QMenu(parent){
	addAction(colinIcons::instance().icon(Colin::Close), tr("remove"), this, SLOT(delBLS()));
	this->clsId = clsId;
	this->blsId = blsId;
}

void delMenu::delBLS(){
	if(clsId<0)
		filelist::instance().currentFile()->removeBLS(blsId);
	else if(blsId<0)
		filelist::instance().currentFile()->removeCLS(clsId);
	else
		filelist::instance().currentFile()->removeBLSbyIndex(clsId, blsId);
		deleteLater();
}

treeView::treeView(QWidget *parent) :
	QTreeView(parent)
{
//	this->setHeaderHidden(true);
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
//	this->setAllColumnsShowFocus(true);
//	this->setAlternatingRowColors(true);
	this->setUniformRowHeights(true);
	setContextMenuPolicy(Qt::CustomContextMenu);

	setModel(new treeModel(this));


	setWhatsThis(tr("<b>tree</b> ")+
				 tr("<a href=\"tree\">open manual</a><br /><br />")+
				 tr("This is a tree like representation of your structure. ")+
				 tr("You can use it to edit and create elements in an table like enviroment. ")+
				 tr("Click on any property of any element to edit it. Columns marked with a triangle can be expanded to show more information.<br />")+
				 tr("Click on the triangles on the left side of the columns to expand it and show more information!<br />")+
				 tr("Items marked with a  green plus can be used to add new elements to your structure!<br /><br />")+
				 treeModel::treeNavigation());

	connect(delegate,                           SIGNAL(openNext(QModelIndex)),
			this,                               SLOT(nextColumn(QModelIndex)));

	connect(delegate,                           SIGNAL(openPrevious(QModelIndex)),
			this,                               SLOT(previousColumn(QModelIndex)));

	connect(delegate,                           SIGNAL(openNextItem(QModelIndex)),
			this,                               SLOT(nextItem(QModelIndex)));

	connect(delegate,                           SIGNAL(openPreviousItem(QModelIndex)),
			this,                               SLOT(previousItem(QModelIndex)));

	connect(delegate,                           SIGNAL(openFirstColumn(QModelIndex)),
			this,                               SLOT(firstColumn(QModelIndex)));

	connect(this,								SIGNAL(clicked(QModelIndex)),
			this,								SLOT(popupEditor(QModelIndex)));
}


void treeView::keyPressEvent(QKeyEvent *e)
{
	QTreeView::keyPressEvent(e);
}

void treeView::mousePressEvent(QMouseEvent *e)
{
	if(e->buttons().testFlag(Qt::RightButton))
	{
		QModelIndex index = indexAt(e->pos());
#ifdef TREEVIEW_VERBOSE
		qDebug() << "treeView::rightClick on " << index;
		qDebug() << "internal id " << int(treeModel::Id(index.internalId()));
#endif
		switch(treeModel::Id(index.internalId())){
		case treeModel::node:
			emit rightClick(catcher::CatchedNode, index.row());
			return;
		case treeModel::support:
			emit rightClick(catcher::CatchedNode, index.parent().row());
			return;
		case treeModel::beam:
			emit rightClick(catcher::CatchedBeam, index.row());
			return;
		case treeModel::hinge:
			emit rightClick(catcher::CatchedBeam, index.parent().row());
			return;
		case treeModel::load:
			emit rightClick(catcher::CatchedLoad, index.row());
			return;
		case treeModel::cls:
			popupDelMenu(index.row(), -1);
			return;
		case treeModel::bls:
			popupDelMenu(-1, index.row());
			return;
		case treeModel::clsbls:
			popupDelMenu(index.parent().row(), index.row());
			return;
		default:
			return;
		}
	}

	if(indexAt(e->pos()).flags().testFlag(Qt::ItemIsUserCheckable))
	{
		QModelIndex mI = indexAt(e->pos());

		if(static_cast<Qt::CheckState>(model()->itemData(mI)[Qt::CheckStateRole].toUInt()) == Qt::Unchecked)
		{
			model()->setData(mI, 1, Qt::CheckStateRole);
		}
		else
		{
			model()->setData(mI, 0, Qt::CheckStateRole);
		}
		return;
	}

	QTreeView::mousePressEvent(e);
}

void treeView::mouseReleaseEvent(QMouseEvent *e)
{
	if(indexAt(e->pos()).flags().testFlag(Qt::ItemIsUserCheckable))
	{
		return;
	}
	QTreeView::mouseReleaseEvent(e);
}

void treeView::popupEditor(const QModelIndex &index)
{
	if(this->model()->flags(index) & Qt::ItemIsEditable)
	openPersistentEditor(index);
}

void treeView::nextColumn(const QModelIndex &index)
{
	if(index.isValid())
	{
		closePersistentEditor(index);
		QModelIndex newIndex = index.parent().child(index.row(), index.column()+1);
		if(newIndex.isValid() &&
		   model()->columnCount(newIndex.parent())>newIndex.column() &&
		   model()->rowCount(newIndex.parent())>newIndex.row() &&
		   model()->flags(newIndex).testFlag(Qt::ItemIsEditable))
		{
			popupEditor(newIndex);
			return;
		}
		newIndex =index.parent().child(index.row()+1, 1);
		if(newIndex.isValid() &&
		   model()->columnCount(newIndex.parent())>newIndex.column() &&
		   model()->rowCount(newIndex.parent())>newIndex.row() &&
		   model()->flags(newIndex).testFlag(Qt::ItemIsEditable))
		{
			popupEditor(newIndex);
			return;
		}
		newIndex = index.parent().child(0, 1);
		if(newIndex.isValid() &&
		   model()->columnCount(newIndex.parent())>newIndex.column() &&
		   model()->rowCount(newIndex.parent())>newIndex.row() &&
		   model()->flags(newIndex).testFlag(Qt::ItemIsEditable))
		{
			popupEditor(newIndex);
			return;
		}
	}
}

void treeView::previousColumn(const QModelIndex &index)
{
	if(index.isValid())
	{
		closePersistentEditor(index);
		QModelIndex newIndex = index.parent().child(index.row(), index.column()-1);
		if(newIndex.isValid() &&
		   newIndex.column()>0 &&
		   newIndex.row()>-1 &&
		   model()->flags(newIndex).testFlag(Qt::ItemIsEditable))
		{
			popupEditor(newIndex);
			return;
		}
		newIndex =index.parent().child(index.row()-1, model()->columnCount(index.parent())-1);
		if(newIndex.isValid() &&
		   newIndex.column()>0 &&
		   newIndex.row()>-1 &&
		   model()->flags(newIndex).testFlag(Qt::ItemIsEditable))
		{
			popupEditor(newIndex);
			return;
		}
		newIndex = index.parent().child(model()->rowCount(index.parent())-1, model()->columnCount(index.parent())-1);
		if(newIndex.isValid() &&
		   newIndex.column()>0 &&
		   newIndex.row()>-1 &&
		   model()->flags(newIndex).testFlag(Qt::ItemIsEditable))
		{
			popupEditor(newIndex);
			return;
		}
	}
}

void treeView::nextItem(const QModelIndex &index)
{
	if(index.isValid())
	{
		closePersistentEditor(index);
		QModelIndex newIndex = index.parent().child(index.row()+1, index.column());
		if(newIndex.isValid() &&
		   model()->columnCount(newIndex.parent())>newIndex.column() &&
		   model()->rowCount(newIndex.parent())>newIndex.row() &&
		   model()->flags(newIndex).testFlag(Qt::ItemIsEditable))
		{
			popupEditor(newIndex);
			return;
		}
		newIndex = index.parent().child(0, index.column());
		if(newIndex.isValid() &&
		   model()->columnCount(newIndex.parent())>newIndex.column() &&
		   model()->rowCount(newIndex.parent())>newIndex.row() &&
		   model()->flags(newIndex).testFlag(Qt::ItemIsEditable))
		{
			popupEditor(newIndex);
			return;
		}
	}
}

void treeView::previousItem(const QModelIndex &index)
{
	if(index.isValid())
	{
		closePersistentEditor(index);
		QModelIndex newIndex = index.parent().child(index.row()-1, index.column());
		if(newIndex.isValid() &&
		   model()->columnCount(newIndex.parent())>newIndex.column() &&
		   newIndex.row()>=0 &&
		   model()->flags(newIndex).testFlag(Qt::ItemIsEditable))
		{
			popupEditor(newIndex);
			return;
		}
		newIndex = index.parent().child(model()->rowCount(index.parent())-1, index.column());
		if(newIndex.isValid() &&
		   model()->columnCount(newIndex.parent())>newIndex.column() &&
		   model()->rowCount(newIndex.parent())>newIndex.row() &&
		   model()->flags(newIndex).testFlag(Qt::ItemIsEditable))
		{
			popupEditor(newIndex);
			return;
		}
	}
}

void treeView::firstColumn(const QModelIndex &index)
{
	if(index.isValid())
	{
		closePersistentEditor(index);
		QModelIndex newIndex = model()->index(index.row()+1, 1, index.parent());
		if(newIndex.isValid() &&
		   model()->columnCount(newIndex.parent())>newIndex.column() &&
		   model()->rowCount(newIndex)>newIndex.row() &&
		   model()->flags(newIndex).testFlag(Qt::ItemIsEditable))
		{
			popupEditor(newIndex);
			return;
		}
	}
}

void treeView::popupDelMenu(int clsId, int blsId)
{
#ifdef TREEVIEW_VERBOSE
	qDebug() << "popup";
#endif
	delMenu *Menu = new delMenu(clsId, blsId);
	Menu->popup(QCursor::pos());
}
