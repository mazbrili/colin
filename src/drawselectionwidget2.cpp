/***********************************************************
 * $Id: $
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

#include "drawselectionwidget2.h"

#include "filelist.h"

const double fontscale = 1.75;
const int maxW=150;
const int maxCases = 3;

drawSelectionWidget2::drawSelectionWidget2(QWidget *parent) :
	abstractDrawSelectionWidget(parent)
{

	setMouseTracking(true);
	mouseOver = false;
	setMinimumHeight(20);

	connect(&filelist::instance(),					SIGNAL(currentChanged(ColinStruct*)),
			this,									SLOT(setTw(ColinStruct*)));




	setWhatsThis(tr("<b>visible CLS</b> <a href=\"view/cls\">open manual</a><br /> <br />")+
				 tr("Use this widget to show or hide combinations of loads!"));


}
drawSelectionWidget2::~drawSelectionWidget2()
{
}

void drawSelectionWidget2::mouseMoveEvent(QMouseEvent *)
{
	repaint();
}

void drawSelectionWidget2::mousePressEvent(QMouseEvent *e)
{
	if(e->button() == Qt::LeftButton)
		mousePressed = true;
	repaint();
}

void drawSelectionWidget2::mouseReleaseEvent(QMouseEvent *e)
{
	ColinStruct *tw = filelist::instance().currentFile();
	int y = e->pos().y();
	int clicked = y/(font().pointSize()*1.5*fontscale+offset());
	if(clicked>=0 && clicked <= tw->cls_n())
	{
		tw->setActiveCLS(clicked, !tw->isActiveCLS(clicked));
	}
	mousePressed = false;
	repaint();
}

void drawSelectionWidget2::paintEvent(QPaintEvent *)
{
	QPainter p(this);

	drawBackground(&p);

	if(mouseOver)
		mouseOverPaint(&p);
	else
		notmouseOverPaint(&p);
}

QSize drawSelectionWidget2::sizeHint() const
{
	if(mouseOver)
	{
		if(filelist::instance().currentFile()){
			return QSize(getMaxWidth(),
						(font().pointSize()*1.5*fontscale+offset())*qMax(filelist::instance().currentFile()->cls_n(),1)+offset());
		}
		else
			return QSize(maxW+offset()*2, font().pointSize()*1.5*fontscale+2*offset());
	}
	else
	{
		if(filelist::instance().currentFile()){
			return QSize(getMaxWidth(),
						 (font().pointSize()*1.5*fontscale+offset())*qMin(qMax(filelist::instance().currentFile()->activeCLS_n(),1),maxCases)+offset());
		}
		else
			return QSize(getMaxWidth(), font().pointSize()*1.5*fontscale+2*offset());
	}
}

void drawSelectionWidget2::mouseOverPaint(QPainter *p)
{
	ColinStruct *tw = filelist::instance().currentFile();
	if(!tw)
		return;

	QRect rect = QRect(offset(), offset(), width()-offset()*2, font().pointSize()*1.5*fontscale);
	for(int i=0; i<tw->cls_n(); i++){
		p->setPen(palette().color(QPalette::Highlight));

		drawRect(p, rect, (mousePressed && rect.contains(mapFromGlobal(QCursor::pos())))?true:false, tw->activeCLS().contains(i), rect.contains(mapFromGlobal(QCursor::pos())));
		drawText(p, rect, tw->cls(i).name(), tw->isActiveCLS(i));
		rect.moveTop(rect.bottom()+offset()+1);
	}
}

void drawSelectionWidget2::notmouseOverPaint(QPainter *p)
{
	ColinStruct *tw = filelist::instance().currentFile();
	if(!tw)
		return;

	QRect rect = QRect(offset(), offset(), width()-offset()*2, font().pointSize()*1.5*fontscale);
	if(tw->activeCLS_n()==0){
		drawRect(p, rect, false, true);
		drawText(p, rect, tr("cls"), true);
	}
	int rects=0;
	foreach(int i, tw->activeCLS()){
		p->setPen(palette().color(QPalette::Highlight));

		drawRect(p, rect, false, true);
		drawText(p, rect, tw->cls(i).name(), true);
		rect.moveTop(rect.bottom()+offset()+1);
		rects++;
		if(rects>=maxCases-1 && tw->activeCLS_n()>maxCases)
			break;
	}
	if(rects<tw->activeCLS_n())
	{
		drawRect(p, rect, false, true);
		drawText(p, rect, QString(tr("and %1 other!")).arg(tw->activeCLS_n()-rects), true);
	}
}

void drawSelectionWidget2::drawText(QPainter *p, const QRect &rect, const QString &text, bool active)
{
	if(active)
		p->setPen(Qt::black);
	else
		p->setPen(Qt::darkGray);
	colinIcons::instance().icon(Colin::CLS).paint(p, rect.left()+offset(), rect.top()+offset(), rect.height()-2*offset(), rect.height()-2*offset());
	p->drawText(rect.adjusted(offset()*2+rect.height(), 0, 0, 0), Qt::AlignLeft | Qt::AlignVCenter, text);

}

int drawSelectionWidget2::getMaxWidth() const
{
	ColinStruct *tw = filelist::instance().currentFile();
	if(!tw)
		return 10;
	QRect rect = QRect(0, 0, 200, font().pointSize()*1.5*fontscale);
	int wMax = 0;
	if(mouseOver)
	{
		for(int i=0; i<tw->cls_n(); i++)
		{
			wMax = qMax(static_cast<int>(this->fontMetrics().boundingRect(rect, Qt::AlignLeft | Qt::AlignVCenter, tw->cls(i).name()).width()), wMax);
		}
	}
	else
	{
		if(tw->activeCLS_n()>maxCases)
		{
			wMax = qMax(static_cast<int>(this->fontMetrics().boundingRect(rect, Qt::AlignLeft | Qt::AlignVCenter, tr("and 99 other!")).width()), wMax);
		}
		foreach(int i, tw->activeCLS())
		{
			wMax = qMax(static_cast<int>(this->fontMetrics().boundingRect(rect, Qt::AlignLeft | Qt::AlignVCenter, tw->cls(i).name()).width()), wMax);
		}
	}
	wMax = qMax(static_cast<int>(this->fontMetrics().boundingRect(rect, Qt::AlignLeft | Qt::AlignVCenter, tr("cls")).width()), wMax);
	wMax+= offset()*6+rect.height();
	return wMax;
}


void drawSelectionWidget2::setTw(ColinStruct *tw)
{
	disconnect(this,				SLOT(addedCLS()));
	disconnect(this,				SLOT(removedCLS()));

	if(tw){
		if(tw->cls_n()){
			if(allowedShowing)
				show();
		}

		connect(tw,					SIGNAL(addedCLS(int)),
				this,				SLOT(addedCLS()));
		connect(tw,					SIGNAL(removed(int,ColinCLS)),
				this,				SLOT(removedCLS()));
	}
	else
		hide();

	this->requestResize(sizeHint());

}

void drawSelectionWidget2::addedCLS()
{
	if(allowedShowing) show();
}

void drawSelectionWidget2::removedCLS()
{
	if(!filelist::instance().currentFile()->cls_n())
		hide();
}


void drawSelectionWidget2::allowToShow(bool show)
{
	allowedShowing = show;
	if(!show)
		hide();
	if(show){
		if(filelist::instance().currentFile())
		{
			if(filelist::instance().currentFile()->cls_n())
				this->show();
			else
				this->hide();
		}
	}
}
