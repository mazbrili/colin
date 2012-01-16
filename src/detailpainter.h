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

#ifndef DETAILPAINTER_H
#define DETAILPAINTER_H

#include <QtCore/QList>

#include "colinstruct.h"

class detailPainter
{
public:
	detailPainter();
	void drawNode(QPainter *p, const ColinStruct &t, int i, const QList<int> &cls);
	void drawBeamExtern(QPainter *p, const ColinStruct &t, int i, const QList<int> &cls);
	void drawBeamIntern(QPainter *p, const ColinStruct &t, int i, const double &x, const QList<int> &cls);
	void drawLoads(QPainter *p, double N, double Q, double M);
	void setFunctionGradient(QPainter *p, QColor c1, QColor c2);
private:
	double validAngle(double angle) const;
};

#endif // DETAILPAINTER_H
