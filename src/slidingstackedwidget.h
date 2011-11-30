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
 * Author: This is taken from an example from www.developer.nokia.com:
 * Code Example for SlidingStackedWidget class in Qt:
 * quote from there: "This code example can be self-signed."
 *
 ***********************************************************/

#ifndef SLIDINGSTACKEDWIDGET_H
#define SLIDINGSTACKEDWIDGET_H

#include <QtGui/QStackedWidget>

#include <QtGui/QWidget>
#include <QtCore/QDebug>
#include <QtCore/QEasingCurve>
#include <QtCore/QPropertyAnimation>
#include <QtCore/QParallelAnimationGroup>

/*!
Description
		SlidingStackedWidget is a class that is derived from QtStackedWidget
		and allows smooth side shifting of widgets, in addition
		to the original hard switching from one to another as offered by
		QStackedWidget itself.
*/

class SlidingStackedWidget : public QStackedWidget
{
		Q_OBJECT

public:
		//! This enumeration is used to define the animation direction
		enum t_direction {
				LEFT2RIGHT,
				RIGHT2LEFT,
				TOP2BOTTOM,
				BOTTOM2TOP,
				AUTOMATIC
		};

		//! The Constructor and Destructor
		SlidingStackedWidget(QWidget *parent);
		~SlidingStackedWidget(void);


public slots:
		//! Some basic settings API
		void setSpeed(int speed);   //animation duration in milliseconds
		void setAnimation(enum QEasingCurve::Type animationtype); //check out the QEasingCurve documentation for different styles
		void setVerticalMode(bool vertical=true);
		void setWrap(bool wrap);    //wrapping is related to slideInNext/Prev;it defines the behaviour when reaching last/first page

		//! The Animation / Page Change API
		void slideInNext();
		void slideInPrev();
		void slideInIdx(int idx, enum t_direction direction=AUTOMATIC);


signals:
		//! this is used for internal purposes in the class engine
		void animationFinished(void);

protected slots:
		//! this is used for internal purposes in the class engine
		void animationDoneSlot(void);

protected:
		//! this is used for internal purposes in the class engine
		void slideInWgt(QWidget * widget, enum t_direction direction=AUTOMATIC);

		QWidget *m_mainwindow;

		int m_speed;
		enum QEasingCurve::Type m_animationtype;
		bool m_vertical;
		int m_now;
		int m_next;
		bool m_wrap;
		QPoint m_pnow;
		bool m_active;

		QList<QWidget*> blockedPageList;
};

#endif // SLIDINGSTACKEDWIDGET_H
