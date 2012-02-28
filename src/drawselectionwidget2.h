/***********************************************************
 * $Id: drawselectionwidget.h 36 2011-04-03 06:46:05Z colin $
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

#ifndef DRAWSELECTIONWIDGET2_H
#define DRAWSELECTIONWIDGET2_H

#include <QtGui/QWidget>
#include "viewportsettings.h"
#include "colinicons.h"
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>

#include "abstractdrawselectionwidget.h"

class drawSelectionWidget2 : public abstractDrawSelectionWidget
{
	Q_OBJECT
public:
	explicit drawSelectionWidget2(QWidget *parent = 0);
	~drawSelectionWidget2();

	void mouseMoveEvent(QMouseEvent *);
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);

	void paintEvent(QPaintEvent *);

	QSize sizeHint() const;


	void allowToShow(bool show);

private:
	void mouseOverPaint(QPainter *p);
	void notmouseOverPaint(QPainter *p);
	void drawText(QPainter *p, const QRect &rect, const QString &text, bool active);
	int getMaxWidth() const;
signals:

public slots:
	void setTw(ColinStruct *tw);
	void addedCLS();
	void removedCLS();
	void activeCLSChanged();
public:
private:
	bool mousePressed;
	bool allowedShowing;


};

#endif // DRAWSELECTIONWIDGET2_H
