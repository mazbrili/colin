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

#ifndef TREEDELEGATE_H
#define TREEDELEGATE_H

#include <QtGui/QItemDelegate>
#include <QtGui/QComboBox>
#include <QtGui/QKeyEvent>
#include <QtGui/QColorDialog>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>


#include "filelist.h"


class treeDelegate : public QItemDelegate
{
	Q_OBJECT
public:
	explicit treeDelegate(QObject *parent = 0 );
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
	void setEditorData(QWidget *editor, const QModelIndex &index ) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const;

protected:
	bool eventFilter(QObject *obj, QEvent *event);

	mutable QModelIndex lastIndex;
	mutable bool commitOnFocusLoss;

signals:
	void openPrevious(const QModelIndex&);
	void openNext(const QModelIndex&);
	void openPreviousItem(const QModelIndex&);
	void openNextItem(const QModelIndex&);
	void openFirstColumn(const QModelIndex&);


public slots:

private slots:
	void emitCommitData();
};

#endif // TREEDELEGATE_H
