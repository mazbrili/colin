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

#include "colinhmultibutton.h"

#include <QtCore/QDebug>
#include <QtGui/QHBoxLayout>


ColinHMultiButton::ColinHMultiButton(QWidget *parent)
    : QWidget(parent)
{
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	QHBoxLayout *l = new QHBoxLayout(this);
	l->setMargin(0);
	l->setSpacing(1);
}


ColinHMultiButton::~ColinHMultiButton()
{

}



void ColinHMultiButton::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	QStyleOptionButton option;
	option.initFrom(this);
	option.rect = QRect(0, 0, width(), height());
	painter.translate(0, height());
	painter.scale(1, -1);
	style()->drawControl(QStyle::CE_PushButton, &option, &painter, this);
}

void ColinHMultiButton::addButton(ColinPushButtonPart *but)
{
	QHBoxLayout *l = static_cast<QHBoxLayout*>(layout());
	if(l->count())
	{
		QWidget *last = l->itemAt(l->count()-1)->widget();
		if(last){
			if(qobject_cast<ColinPushButtonPart*>(last))
			{
				qobject_cast<ColinPushButtonPart*>(last)->setCutted(ColinPushButtonPart::Right, true);
				but->setCutted(ColinPushButtonPart::Left, true);
			}
		}
	}


	but->setParent(this);
	layout()->addWidget(but);

}

void ColinHMultiButton::removeButton(QAbstractButton *but)
{
	delete but;
}

void ColinHMultiButton::clear()
{
	foreach(QAbstractButton *b, this->findChildren<ColinPushButtonPart*>())
		delete b;
}
