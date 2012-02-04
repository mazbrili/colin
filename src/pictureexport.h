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

#ifndef PICTUREEXPORT_H
#define PICTUREEXPORT_H

#include <QtGui/QPixmap>

#include "colin_namespace.h"

class ColinStruct;

class pictureExport
{
public:
	pictureExport(){}
	pictureExport(ColinStruct *tw, const Colin::Elements &toDraw, const QRectF &boundingRect, double prescale, const QSizeF &resolution);
	void save(const QString &filename);
	QPixmap preview();
	void setElements(Colin::Elements toDraw);
	void setStruct(ColinStruct *tw);
	void setBoundingRect(const QRectF &boundingRect);
	void setPreScale(double prescale);
	void setResolution(const QSizeF &resolution);
private:
	void draw(QPixmap *p, bool backG = false);
	ColinStruct *tw;
	Colin::Elements toDraw;
	double prescale;
	QRectF boundingRect;
	QSizeF resolution;

};

#endif // PICTUREEXPORT_H
