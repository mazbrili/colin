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

#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <QtGui/QTreeView>

#include "filelist.h"
#include "colinicons.h"
#include "treedelegate.h"

#ifndef QT_NO_DEBUG
#include <QtCore/QTextStream>
#endif


class treeView : public QTreeView
{
    Q_OBJECT
public:
    explicit treeView(QWidget *parent = 0);

	void keyPressEvent(QKeyEvent *e);


signals:

public slots:


    void popupEditor(const QModelIndex &index);
    void popupMenu(const QPoint &p);




	void nextColumn(const QModelIndex &index);
	void previousColumn(const QModelIndex &index);
	void nextItem(const QModelIndex &index);
	void previousItem(const QModelIndex &index);
	void firstColumn(const QModelIndex &index);

private:


    int opencolumn;

    treeDelegate *delegate;


};

#endif // TREEVIEW_H