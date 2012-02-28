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

#ifndef WGV_BLS_H
#define WGV_BLS_H

#include <QtCore/QString>
#include <QtGui/QColor>

#include "colinelement.h"

class ColinBLS : public ColinElement
{
public:
	ColinBLS(){}
	explicit ColinBLS(const QString &name):_name(name){}
	const QString &name()const {return _name;}
	const QColor &color() const {return _color;}
	void setName(const QString &namen){_name=namen;}
	void setColor(const QColor &color){_color=color;}
private:
	QString _name;
	QColor _color;
};

class bls_fac
{
public:
	bls_fac(const int &bls_, const double &fac_):bls(bls_),fac(fac_){};
	int bls;
	double fac;
};

#endif // WGV_BLS_H
