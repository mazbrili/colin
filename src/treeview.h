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

#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QKeyEvent>
#include <QtGui/QDropEvent>

#include "filelist.h"
#include "colinicons.h"
#include "treedelegate.h"

#ifndef QT_NO_DEBUG
#include <QtCore/QTextStream>
#endif


class treeView : public QTreeWidget
{
    Q_OBJECT
public:
    explicit treeView(QWidget *parent = 0);




    void recount(QTreeWidgetItem *parent, const int &i);
    void fill();


	void keyPressEvent(QKeyEvent *e);
	void dropEvent(QDropEvent *event);


signals:

public slots:

    void dumpTw(QObject *o);
    void setTw(ColinStruct *t);
    void actualSelection();
    void actualEH();
    void selectionChanged();

    void popupEditor(const QModelIndex &index);
    void popupMenu(const QPoint &p);


    void addNode(const ColinNode &n, int i = -1);
    void addBeam(const ColinBeam &b, int i = -1);
	void addLoad(const ColinLoad &l, int i = -1);
	void addBLS(const ColinBLS &bls, int i = -1);
	void addCLS(const ColinCLS &cls, int i = -1);

    void addNode(const int &i){addNode(tw->node(i), i);}
	void addBeam(const int &i){addBeam(tw->beam(i), i);}
	void addLoad(const int &i){addLoad(tw->load(i), i);}
	void addBLS(const int &i){addBLS(tw->bls(i), i);}
	void addCLS(const int &i){addCLS(tw->cls(i), i);}

    void removeNode(const int &i);
	void removeBeam(const int &i);
	void removeLoad(const int &i);
	void removeBLS(const int &i);
	void removeCLS(const int &i);

    void setNode(const int &i, const ColinNode &n);
	void setBeam(const int &i, const ColinBeam &b);
	void setLoad(const int &i, const ColinLoad &l);
	void setBLS(const int &i, const ColinBLS &l);
	void setCLS(const int &i, const ColinCLS &l);

    void setNode(const int &i){setNode(i, tw->node(i));}
	void setBeam(const int &i){setBeam(i, tw->beam(i));}
	void setLoad(const int &i){setLoad(i, tw->load(i));}
	void setBLS(const int &i){setBLS(i, tw->bls(i));}
	void setCLS(const int &i){setCLS(i, tw->cls(i));}


    void writeData(QTreeWidgetItem *item, int column);

    void disableSelection(){selectionisUpToDate = true;}
    void enableSelection(){selectionisUpToDate = false; actualSelection();}

    //void editorClosed(QWidget *w, QAbstractItemDelegate::EndEditHint eh);

    void nextColumn();
    void previousColumn();
    void nextItem();
    void previousItem();

	void addVoidNode();
	void addVoidBeam();
	void addVoidLoad();
	void addVoidBLS();
	void addVoidCLS();
	void addVoidBLSCLS(int i);

	void checkAndCreateNode();
	void checkAndCreateBeam();
	void checkAndCreateLoad();
	void checkAndCreateBLS();
	void checkAndCreateCLS();
	void checkAndCreateCLSBLS();

private:

    void setHeader();
    QTreeWidgetItem *nodeParent;
	QTreeWidgetItem *beamParent;
	QTreeWidgetItem *loadParent;
	QTreeWidgetItem *blsParent;
	QTreeWidgetItem *clsParent;

    int opencolumn;
    QTreeWidgetItem *openitem;

    bool selectionisUpToDate;
    ColinStruct *tw;
    treeDelegate *delegate;
    bool isChanging;


};

#endif // TREEVIEW_H
