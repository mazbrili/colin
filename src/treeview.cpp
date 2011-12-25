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
#include <QtGui/QDropEvent>


#include "treeview.h"
#include "treemodel.h"

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
/*
	connect(this,                               SIGNAL(customContextMenuRequested(QPoint)),
			this,                               SLOT(popupMenu(QPoint)));
*/
/*
	connect(this->itemDelegate(),				SIGNAL(closeEditor(QWidget*,QAbstractItemDelegate::EndEditHint)),
			this,								SLOT(closeEditor(QWidget*,QAbstractItemDelegate::EndEditHint)));
*/
	connect(this,								SIGNAL(clicked(QModelIndex)),
			this,								SLOT(popupEditor(QModelIndex)));
}


void treeView::keyPressEvent(QKeyEvent *e)
{
	QTreeView::keyPressEvent(e);
}

void treeView::popupMenu(const QPoint &p)
{

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
		QModelIndex newIndex = index.parent().child(index.row(), model()->columnCount(index.parent())-1);
		if(newIndex.isValid() &&
		   model()->columnCount(newIndex.parent())>newIndex.column() &&
		   model()->flags(newIndex).testFlag(Qt::ItemIsEditable))
		{
			popupEditor(newIndex);
			return;
		}
		for(int i=1; i<2; i++){
			newIndex = index.parent().child(index.row()-i, model()->columnCount(index.parent())-1);
			if(newIndex.isValid() &&
			   model()->flags(newIndex).testFlag(Qt::ItemIsEditable))
			{
				popupEditor(newIndex);
			}
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


