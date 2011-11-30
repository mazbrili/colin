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

#ifndef CWIDGET_H
#define CWIDGET_H

#include <QtGui/QSplitter>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtCore/QByteArray>
#include <QtCore/QStateMachine>
#include <QtCore/QPropertyAnimation>
#include <QtCore/QParallelAnimationGroup>

#include "colinsidebar.h"
#include "viewport.h"

class cWidget : public QSplitter
{
    Q_OBJECT
public:
    explicit cWidget(QWidget *parent = 0);
    void repaintyourChildren();

    inline void zoomRect(const int &i, const QRectF &r) {if(views.at(i)->isVisible())
        views[i]->zoomRect(filelist::instance().currentFile()->view(i).mapRect(r), true);}

	void hideToolTips();
	void resizeEvent(QResizeEvent *e);

signals:
    void viewChanged();
	void basePoint(QPointF p);

public slots:
    void idrequest(int* id);
    void setViewCount(int i);
    void setVisibleViews(const QByteArray &vis);
    void setTw(ColinStruct* tw);
    void maximizeView();
    void hideView();
    void showAll();
	void showSideBar(bool show);
    void emitBasePoint(QPointF p){emit basePoint(p);}

    inline int viewCount() const {return views.size();}
    inline int rowmax() const {return rowmax_;}
    inline bool isVisible(const int &i) const {return views.at(i)->isVisible();}

private:
	QWidget *viewContainer;
	QHBoxLayout *layout;
	ColinSidebar *sidebar;
	QList<viewport*> views;
	QParallelAnimationGroup *viewportAnimations;
    int rowmax_;
    int hasFullArea;
    int shown;

};


#endif // CWIDGET_H
