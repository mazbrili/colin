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

#ifndef NODEOVERLAY_H
#define NODEOVERLAY_H

#include <QtGui/QGroupBox>


#include "abstractoverlay.h"

class ColinStruct;
class QLineEdit;
class QLabel;
class QButtonGroup;
class QComboBox;

class nodeDetail : public quadWidget
{
	Q_OBJECT
public:
	explicit nodeDetail(QWidget *parent);
	void paintEvent(QPaintEvent *e);
public slots:
	void setCurrentItem(const int &i);
	void setCurrentCLS(const int &i);
private:
	int currentItem;
	int currentCLS;
};

class supportExtended : public QGroupBox
{
	Q_OBJECT
public:
	explicit supportExtended(QWidget *parent);
	ColinPushButtonPart *hlock, *hspring;
	ColinPushButtonPart *vlock, *vspring;
	ColinPushButtonPart *mlock, *mspring;
	QLineEdit *hc, *vc, *mc;
	ColinPushButtonPart *h, *v, *m;
	QLabel *hUnit, *vUnit, *mUnit;
	QGroupBox *morebox;

	QButtonGroup *basicsupport;
	QButtonGroup *basicsupport2;
	QButtonGroup *springsupport;
public slots:
	void extended(bool show);
	void setCurrentItem(const int &i);
	void setBasic();
	void setSpring();
	void setSpringConstants();

private:
	int currentItem;
};


class nodeOverlay : public abstractOverlay
{
    Q_OBJECT
public:
	explicit nodeOverlay(QWidget *parent = 0);
signals:

public slots:
	void setCurrentItem(const int &i);
	void changed();
	void setTw(ColinStruct *t);
	void nodeChanged();
	void nodeChanged(int i);
	void nextItem();
	void previousItem();
	void clsChanged();

	void copy();
	void cut();
	void toClipBoard();

private:

	QLabel *xlabel, *zlabel, *philabel;
	QLineEdit *x, *z, *phi;
	QPushButton *more;
	supportExtended *support;


	ColinPushButtonPart *copyButton, *cutButton;
	ColinHMultiButton *clsButton;
	QButtonGroup *cls;
	nodeDetail *detailWidget;

	QLabel *reactions;
	QLabel *displacement;
	QLabel *beamforces_min;
	QLabel *beamforces_max;
	int currentItem;

};

#endif // NODEOVERLAY_H
