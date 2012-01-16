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

#ifndef GENERALOVERLAY_H
#define GENERALOVERLAY_H


#include "abstractoverlay.h"
#include <QtGui/QClipboard>


class ColinStruct;
class QLineEdit;
class QLabel;
class QButtonGroup;
class QComboBox;
class QGroupBox;
class QListWidget;
class QGridLayout;


class generalOverlay : public abstractOverlay
{
	Q_OBJECT
public:
	explicit generalOverlay(QWidget *parent = 0);
signals:

public slots:
	void clipBoardChanged();
	void clsChanged();
	void loadCLS(QModelIndex clsIndex);
	void loadCLS(int clsIndex);
private:


	QGroupBox *paste;
	QListWidget *clsView;
	QListWidget *blsView;

	QGridLayout *clsEditorLayout;
	QWidget *clsEditor;
};

#endif // GENERALOVERLAY_H
