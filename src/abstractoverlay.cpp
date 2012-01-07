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

#include "abstractoverlay.h"
#include <QtGui/QPainter>
#include <QtGui/QLinearGradient>
#include <QtGui/QRadialGradient>
#include <QtGui/QGridLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QKeyEvent>

#include "viewportsettings.h"

#include "colinicons.h"

abstractOverlay::abstractOverlay(QWidget *parent) :
	QWidget(parent)
{
	QVBoxLayout *layout = new QVBoxLayout(this);


	header = new ColinHMultiButton(this);
	before = new ColinPushButtonPart("<", header);
	item = new ColinPushButtonPart("", header);
	after = new ColinPushButtonPart(">", header);

	header->addButton(before);
	header->addButton(item);
	header->addButton(after);

	header->setFixedHeight(item->sizeHint().height());
	QHBoxLayout *headerlayout = new QHBoxLayout();
	layout->addLayout(headerlayout);
	headerlayout->addSpacing(1000);
	headerlayout->addWidget(header, Qt::AlignHCenter | Qt::AlignVCenter);
	headerlayout->addSpacing(1000);

	quitmessage = new QLabel(tr("Press [Esc] to quit"), this);
	quit = new QPushButton(QIcon(colinIcons::instance().icon(Colin::Close).pixmap(32, QIcon::Normal, QIcon::On)), "", this);
	quit->setFlat(true);

	headerlayout->addWidget(quitmessage);
	headerlayout->addWidget(quit);


	QPalette pal = this->palette();
	QColor bc = QColor(viewPortSettings::instance().color(Colin::C_Background));
#ifdef OVERLAY_LINEAR
	QLinearGradient grad(0, 0, 150, 500);
	bc.setAlpha(180);
	grad.setColorAt(0.4, bc);
	bc.setAlpha(80);
	grad.setColorAt(1, bc);
#else
	QRadialGradient grad(1000, 1500, 1500);
	bc.setAlpha(0);
	grad.setColorAt(0.9, bc);
	bc.setAlpha(0);
	grad.setColorAt(0.7, bc);
#endif
	pal.setBrush(QPalette::Window, QBrush(grad));

	this->setPalette(pal);

	this->setAutoFillBackground(true);

	connect(before,				SIGNAL(clicked()),
			this,				SLOT(previousItem()));

	connect(after,				SIGNAL(clicked()),
			this,				SLOT(nextItem()));

	connect(quit,				SIGNAL(clicked()),
			this,				SLOT(deleteLater()));

}


bool abstractOverlay::eventFilter(QObject *o, QEvent *e)
{
	if(e->type()==QEvent::Resize)
	{
		QResizeEvent *event = static_cast<QResizeEvent*>(e);

		this->setFixedSize(event->size());
		return false;
	}
	//return QWidget::eventFilter(o, e);
}

void abstractOverlay::keyPressEvent(QKeyEvent *e)
{
	switch(e->key())
	{
	case Qt::Key_Escape:
		this->deleteLater();
		e->setAccepted(true);
		return;
	case Qt::Key_Left:
	case Qt::Key_Tab:
		e->setAccepted(true);
		nextItem();
		return;
	case Qt::Key_Right:
	case Qt::Key_Backtab:
		e->setAccepted(true);
		previousItem();
		return;
	}
	QWidget::keyPressEvent(e);
}
;
void abstractOverlay::paintEvent(QPaintEvent *)
{

}

void abstractOverlay::hideMyChildren(bool hide)
{
	QWidget *w = qobject_cast<QWidget*>(sender());
	if(!w)
		return;

	foreach(QWidget *c, w->findChildren<QWidget*>())
	{
		if(!hide)
			c->hide();
		else
			c->show();
	}
}

void abstractOverlay::nextItem()
{

}

void abstractOverlay::previousItem()
{

}

void abstractOverlay::setCurrentItem(const int &i)
{

}
