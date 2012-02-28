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

#include "measuremaker.h"
#include "math.h"
#include <QtGui/QPainterPath>
#include <QtCore/QDebug>

measureMaker::measureMaker()
{
}

void measureMaker::paint(QPainter *p, bool light, bool shadow)
{
	QSize size = QSize(p->device()->width(), p->device()->height());

	p->scale(double(size.height())/32., double(size.width())/32.);
	QLinearGradient grad(0, 0, 0, 32);
	if(light)
		p->setBrush(iconMaker::gradLight(grad));
	else
		p->setBrush(iconMaker::gradDark(grad));
	QPen pen(p->brush(), iconMaker::lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	pen.setCosmetic(true);
	p->setPen(pen);
	p->setBrush(Qt::NoBrush);



	QRect ruler(2, 16-5, 32-4,10);
	p->drawRoundedRect(ruler, 1, 1);

	for(int i=4; i<32; i+=4)
	{
		p->drawLine(i, 16+1, i, 16+4);
	}


	qDebug() << "paintEvent " << (shadow?"shadow":"foreground");
}

