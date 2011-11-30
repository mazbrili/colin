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

#ifndef CLSWIDGET_H
#define CLSWIDGET_H


#include <QtGui/QDialog>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QTreeWidget>
#include <QtGui/QButtonGroup>


#include "slidingstackedwidget.h"
#include "colinhmultibutton.h"
#include "colinstruct.h"


class clswidget : public QDialog
{
	Q_OBJECT
public:
	clswidget(QWidget *parent=0);

	ColinHMultiButton *header;
	ColinPushButtonPart *bls;
	ColinPushButtonPart *cls;

	QPushButton *addCls, *addBls1, *addBls2;
	QButtonGroup *headergroup;

	QVBoxLayout *layout;
	QGridLayout *blslayout, *clslayout;

	QTreeWidget *loadlist, *blslist1, *blslist2, *clslist;

	SlidingStackedWidget *slidingStacked;
	QWidget *blsWidget;
	QWidget *clsWidget;

public slots:

	void setTw(ColinStruct *t);

	void addBLS(const ColinBLS &bls, int i = -1);
	void addCLS(const ColinCLS &cls, int i = -1);

	void addBLS(const int &i){addBLS(tw->bls(i), i);}
	void addCLS(const int &i){addCLS(tw->cls(i), i);}

	void removeBLS(const int &i);
	void removeCLS(const int &i);

	void setBLS(const int &i, const ColinBLS &l);
	void setCLS(const int &i, const ColinCLS &l);

	void setBLS(const int &i){setBLS(i, tw->bls(i));}
	void setCLS(const int &i){setCLS(i, tw->cls(i));}


private:
	ColinStruct *tw;
};

#endif // CLSWIDGET_H
