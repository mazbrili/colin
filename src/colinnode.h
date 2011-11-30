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

#ifndef WGV_NODE_H
#define WGV_NODE_H

#include <QtCore/QMetaType>
#include <QtCore/QPointF>
#include "colinsupport.h"


class node_result
{
public:
	node_result(double u_, double w_, double phi_):
		u(u_), w(w_), phi(phi_){}
	node_result(){}
	double u, w, phi;
};

class ColinNode : public ColinElement
{
public:

	ColinNode();
	ColinNode(double _x, double _z);
	ColinNode(const ColinNode &n);
	~ColinNode();

    void select(bool s = true){selec = s; if(bear) bear->select(s);}

    const inline double &x()const {return X;}
    const inline double &z()const {return Z;}

	const inline double &u(int i)const {return res[i].u;}
	const inline double &w(int i)const {return res[i].w;}
	const inline double &phi(int i)const {return res[i].phi;}

	const inline double &Ax(int i)const {return bear->a_x(i);}
	const inline double &Az(int i)const {return bear->a_z(i);}
	const inline double &Am(int i)const {return bear->a_m(i);}

	inline double angle()const {return bear?bear->angle():0;}

    bool hasbearing() const;

	const inline ColinSupport &bearing() const {return *bear;}

	inline ColinSupport &bearing_editable() {return *bear;}

    void inline setX(double Xn){ X=Xn;}
    void inline setZ(double Zn){ Z=Zn;}

	void setBearing(ColinSupport bn);
    void inline removeBearing(){if(bear != NULL) delete bear; bear = NULL;}

	void setU(node_result *res) {this->res=res;}
	void setR(bearing_result *res) {bear->setR(res);}
	node_result *results() {return res;}
	bearing_result *bearing_results() {return bear->results();}

	friend bool operator!=(ColinNode const& one, ColinNode const& sec)
    {
        if(one.x()!=sec.x()) return true;
        if(one.z()!=sec.z()) return true;
        if(one.bearing()!=sec.bearing()) return true;
        return false;
    }


    inline QPointF toQPointF() const{ return QPointF(X, Z);}

	ColinNode &operator=(const ColinNode &other);


private:


    double X;
    double Z;

	ColinSupport *bear;

	node_result *res;
};

#endif // WGV_NODE_H
