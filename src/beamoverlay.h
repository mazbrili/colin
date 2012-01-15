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

#ifndef BEAMOVERLAY_H
#define BEAMOVERLAY_H

#include <QtGui/QGroupBox>


#include "abstractoverlay.h"

class ColinStruct;
class QLineEdit;
class QLabel;
class QButtonGroup;
class QComboBox;

class hingeExtended: public QGroupBox
{
	Q_OBJECT
public:
	explicit hingeExtended(QWidget *parent);
	~hingeExtended();
	ColinPushButtonPart *basicLeft, *basicRight;
	QLineEdit **springCs;


	QButtonGroup *freeButtons,
				 *springButtons,
				 *basicButtons;


	QGroupBox *moreBox;

public slots:
	void extended(bool show);
	void setCurrentItem(const int &i);
	void setBasic();
	void setSpring();
	void setSpringConstants();

private:
	int currentItem;
};

class beamDetail : public quadWidget
{
	Q_OBJECT
public:
	enum mode{
		externForces,
		internForces,
		functions
	};

	explicit beamDetail(QWidget *parent);
	void paintEvent(QPaintEvent *e);
public slots:
	void setCurrentItem(const int &i);
	void setCurrentCLS(const int &i);
	void setMode(const int &M);
	void setCutAt(const double X);
private:
	int currentItem;
	int currentCLS;
	double x;
	mode toDraw;
};

class beamOverlay : public abstractOverlay
{
	Q_OBJECT
public:
	explicit beamOverlay(QWidget *parent = 0);
signals:

public slots:
	void setCurrentItem(const int &i);
	void changed();
	void setTw(ColinStruct *t);
	void beamChanged();
	void beamChanged(int i);
	void nextItem();
	void previousItem();
	void clsChanged();

	void copy();
	void cut();
	void toClipBoard();

private:
	QLabel *leftNlabel, *rightNlabel;
	QLineEdit *leftN, *rightN;

	QLabel *angleInfo;
	QLabel *lengthInfo;

	QLabel *matlabel;
	QLabel *cslabel;

	QPushButton *mat;
	QPushButton *cs;
	hingeExtended *hinges;

	ColinPushButtonPart *copyButton, *cutButton;
	ColinHMultiButton *clsButton;
	QButtonGroup *cls;
	ColinPushButtonPart *modeExtern, *modeIntern;
	QButtonGroup *modeGroup;
	beamDetail *detailWidget;

	QLabel *displacement;
	QLabel *endforces;

	int currentItem;

};

#endif // BEAMOVERLAY_H
