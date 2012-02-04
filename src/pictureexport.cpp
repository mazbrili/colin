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

#include "pictureexport.h"

#include "structpainter.h"
#include "unitsettings.h"

pictureExport::pictureExport(ColinStruct *tw, const Colin::Elements &toDraw, const QRectF &boundingRect, double prescale,  const QSizeF &resolution)
{
	this->tw = tw;
	this->toDraw = toDraw;
	this->prescale = prescale;
	this->boundingRect = boundingRect;
	this->resolution = resolution;

}

void pictureExport::draw(QPixmap *pixmap, bool backG)
{
	pixmap->fill(QColor(255, 255, 255, 0));

	QPainter p(pixmap);

	if(backG){
		p.setPen(Qt::NoPen);
		p.setBrush(QColor(255, 255, 255, 100));
		p.drawRoundedRect(pixmap->rect().adjusted(0, 0, -2, -2), 4, 4);
	}


	structPainter sP;

	QTransform t;

	double scale = qMin((resolution.width()-30)/prescale/boundingRect.width(), (resolution.height()-30)/prescale/boundingRect.height());

	t.translate(-boundingRect.x()*scale+resolution.width()/prescale/2-boundingRect.width()*scale/2,
				-boundingRect.y()*scale+resolution.height()/prescale/2-boundingRect.height()*scale/2);
	t.scale(scale, scale);

	p.scale(prescale, prescale);
	p.save();
	sP.ignoreSelection(true);
	sP.ignoreHotSpots(true);
	sP.drawStruct(*filelist::instance().currentFile(), &p, &t, toDraw);
	p.restore();

	QRectF bR(0, 0, 10, 1);
	if(toDraw.testFlag(Colin::N)){
		p.setPen(viewPortSettings::instance().color(Colin::C_Np));
		p.drawText(QRectF(10, bR.bottom(), 100, 100), Qt::AlignTop | Qt::AlignTop, QString("N [%1]").arg(unitSettings::instance().Peh()), &bR);
		bR.moveTo(0, bR.bottom());}

	if(toDraw.testFlag(Colin::Q)){
		p.setPen(viewPortSettings::instance().color(Colin::C_Qp));
		p.drawText(QRectF(10, bR.bottom(), 100, 100), Qt::AlignTop | Qt::AlignTop, QString("Q [%1]").arg(unitSettings::instance().Peh()), &bR);
		bR.moveTo(0, bR.bottom());}

	if(toDraw.testFlag(Colin::M)){
		p.setPen(viewPortSettings::instance().color(Colin::C_Mp));
		p.drawText(QRectF(10, bR.bottom(), 100, 100), Qt::AlignTop | Qt::AlignTop, QString("M [%1]").arg(unitSettings::instance().Meh()), &bR);
		bR.moveTo(0, bR.bottom());}

}

void pictureExport::save(const QString &filename)
{
	QPixmap pixmap(resolution.width(), resolution.height());

	draw(&pixmap);

	pixmap.save(filename);
}

QPixmap pictureExport::preview()
{
	QPixmap pixmap(resolution.width(), resolution.height());

	draw(&pixmap, true);

	return pixmap;
}

void pictureExport::setElements(Colin::Elements toDraw)
{
	this->toDraw = toDraw;
}

void pictureExport::setStruct(ColinStruct *tw)
{
	this->tw = tw;
}

void pictureExport::setBoundingRect(const QRectF &boundingRect)
{
	this->boundingRect = boundingRect;
}

void pictureExport::setPreScale(double prescale)
{
	this->prescale = prescale;
}

void pictureExport::setResolution(const QSizeF &resolution)
{
	this->resolution = resolution;
}
