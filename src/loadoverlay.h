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

#ifndef LOADOVERLAY_H
#define LOADOVERLAY_H


#include "abstractoverlay.h"

class ColinStruct;
class QLineEdit;
class QLabel;
class QButtonGroup;
class QComboBox;



class loadOverlay : public abstractOverlay
{
	Q_OBJECT
public:
	explicit loadOverlay(QWidget *parent = 0);
signals:

public slots:
	void setCurrentItem(const int &i);
	void changed();
	void setTw(ColinStruct *t);
	void loadChanged();
	void loadChanged(int i);

	void copy();
	void cut();
	void toClipBoard();
	void setUnits();
	void setType(int t);
	void nextItem();
	void previousItem();
	void clsChanged();

private:
	QLabel *posLabel;
	QLineEdit *pos;

	QLabel *setLabel;
	QComboBox *set;

	QLabel *xL, *zL, *mL;
	QLineEdit *x, *z, *m;
	QLabel *xEH, *zEH, *mEH;

	QLabel *angleInfo;
	QLabel *absInfo;

	ColinPushButtonPart *force, *moment;
	ColinHMultiButton *nodeLoads;

	ColinPushButtonPart *uniform, *increasing, *decreasing;
	ColinHMultiButton *linearLoads;

	ColinPushButtonPart *leftSide, *rightSide;
	ColinHMultiButton *side;

	ColinPushButtonPart *tempChange, *tempDifference;
	ColinHMultiButton *temps;

	QButtonGroup *typeGroup;

	ColinPushButtonPart *projected, *fulllength;

	ColinPushButtonPart *copyButton, *cutButton;


	int currentItem;

};

#endif // LOADOVERLAY_H
