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

#ifndef ABSTRACTOVERLAY_H
#define ABSTRACTOVERLAY_H

#include <QtGui/QWidget>

#include "colinhmultibutton.h"

class QLabel;
class QHBoxLayout;
class ColinStruct;

class quadWidget: public QWidget
{
	Q_OBJECT
public:
	explicit quadWidget(QWidget *parent = 0):QWidget(parent){};
	int heightForWidth(int w) const {return 0.99*w;}
};

class abstractOverlay : public QWidget
{
	Q_OBJECT
public:
	explicit abstractOverlay(QWidget *parent = 0);
	~abstractOverlay();


	void keyPressEvent(QKeyEvent *e);
	void paintEvent(QPaintEvent *e);

	bool eventFilter(QObject *o, QEvent *e);
signals:

public slots:
	void hideMyChildren(bool hide);
	virtual void nextItem();
	virtual void previousItem();
	virtual void setCurrentItem(const int &i);
	virtual void setTw(ColinStruct *tw);

private:

protected:
	ColinHMultiButton *header;
	ColinPushButtonPart *before,
						*item,
						*after;
	QLabel *quitmessage;
	QPushButton *quit;

	QHBoxLayout *headerlayout;

	static abstractOverlay *instance;



};

#endif // NODEOVERLAY_H
