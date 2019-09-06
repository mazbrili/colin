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

#include <QtCore/QPointF>
#include "wgv_bearing.h"


class wgv_node : public wgv_element
{
public:

    wgv_node();
    wgv_node(double _x, double _z);
    wgv_node(const wgv_node &n);
    ~wgv_node();

    void select(bool s = true){selec = s; if(bear) bear->select(s);}

    const inline double &x()const {return X;}
    const inline double &z()const {return Z;}

    const inline double &u()const {return _u;}
    const inline double &w()const {return _w;}
    const inline double &phi()const {return _phi;}

    const inline double &Ax()const {return bear->a_x();}
    const inline double &Az()const {return bear->a_z();}
    const inline double &Am()const {return bear->a_m();}

    const inline double &angle()const {return bear->angle();}

    bool hasbearing() const;

    const inline wgv_bearing &bearing() const {return *bear;}

    inline wgv_bearing &bearing_editable() {return *bear;}

    void inline setX(double Xn){ X=Xn;}
    void inline setZ(double Zn){ Z=Zn;}

    void setBearing(wgv_bearing bn);
    void inline removeBearing(){if(bear != NULL) delete bear; bear = NULL;}

    void setU(const double& un, const double& wn, const double &phin) {_u=un; _w=wn; _phi=phin;}


    friend bool operator!=(wgv_node const& one, wgv_node const& sec)
    {
        if(one.x()!=sec.x()) return true;
        if(one.z()!=sec.z()) return true;
        if(one.bearing()!=sec.bearing()) return true;
        return false;
    }


    inline QPointF toQPointF() const{ return QPointF(X, Z);}

    wgv_node &operator=(const wgv_node &other);


private:


    double X;
    double Z;

    double _u;
    double _w;
    double _phi;


    wgv_bearing *bear;
};


#endif // WGV_NODE_H
