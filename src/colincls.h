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


#ifndef WGV_CLS_H
#define WGV_CLS_H

#include <QtCore/QString>
#include <QtCore/QList>

#include "colinbls.h"

class ColinCLS : public ColinElement
{
public:
	ColinCLS(){};
	explicit ColinCLS(QString name):name_(name){};
	inline int count()const {return bls_.size();}
	const inline int &bls(const int &i)const{return bls_.at(i).bls;}
	const inline double &fac(const int &i)const{return bls_.at(i).fac;}
	inline double facOfBLS(const int &o)const{ for(int i=0; i<count(); i++){if(bls_.at(i).bls==o) return bls_.at(i).fac;} return 0;}
	const inline QString &name()const{return name_;}
	void setName(const QString &name){this->name_=name;}
	void removeBLS(const int &i){bls_.removeAt(i);}
	void addBLS(const int &blsi, const double &fac){bls_.append(bls_fac(blsi, fac));}
	void setFactorbyIndex(const int &blsi, const double &fac){bls_[blsi].fac=fac;}
	bool contains(const int &blsi) const { for(int i=0; i<count(); i++){if(bls_.at(i).bls==blsi) return true;} return false;}

private:
	QString name_;
	QList<bls_fac> bls_;
};

#endif // WGV_CLS_H
