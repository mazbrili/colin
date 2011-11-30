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

#ifndef WGV_BEARING_H
#define WGV_BEARING_H

#include "colin_namespace.h"
#include "colinelement.h"


class bearing_result
{
public:
	bearing_result(double ax_, double az_, double am_):
		x(ax_), z(az_), m(am_){}
	bearing_result(){}
	double x, z, m;
};

class ColinSupport : public ColinElement
{
public:
    enum bearing{
        NoBear =    0x0,
        X =         0x1,
        Z =         0x2,
        Phi =       0x4,

        XZ =        X | Z,
        XPhi =      X | Phi,
        ZPhi =      Z | Phi,
        XZPhi =     X | Z | Phi,

        fx =        0x8,
        fz =        0x10,
        fphi =      0x20,

        nX =        0xFFFFFFFF & ~X,
        nZ =        0xFFFFFFFF & ~Z,
        nPhi =      0xFFFFFFFF & ~Phi,
        nFX =       (0xFFFFFFFF & ~X) & ~fx,
        nFZ =       (0xFFFFFFFF & ~Z) & ~fz,
        nFPhi =     (0xFFFFFFFF & ~Phi) & ~fphi,
        nF =        ((0xFFFFFFFF & ~fx) & ~fz) & ~fphi
    };
    Q_DECLARE_FLAGS(bearings, bearing);

	static Colin::Actions enumToEnum(const ColinSupport::bearings &b){
        Colin::Actions a;
        if((b&X)==X) a |= Colin::drawBearingH;
        if((b&Z)==Z) a |= Colin::drawBearingV;
        if((b&Phi)==Phi) a |= Colin::drawBearingM;
        return a;
    }


	static Colin::Actions formToDrawEnum(const ColinSupport::bearings &b){
        Colin::Actions a = Colin::drawBearing;
		if(b.testFlag(ColinSupport::X)) a |= Colin::drawBearingH;
		if(b.testFlag(ColinSupport::Z)) a |= Colin::drawBearingV;
		if(b.testFlag(ColinSupport::Phi)) a |= Colin::drawBearingM;
        return a;
    }


	ColinSupport();
	ColinSupport(Colin::Actions a);
	ColinSupport(const bool &x, const bool &z, const bool &phi);


    inline bool x() const {return (b&X)==X;}
    inline bool z() const {return (b&Z)==Z;}
    inline bool phi() const {return (b&Phi)==Phi;}
    inline bool xf() const {return (b&fx)==fx;}
    inline bool zf() const {return (b&fz)==fz;}
    inline bool phif() const {return (b&fphi)==fphi;}
	inline const ColinSupport::bearings &form() const {return b;}
	inline ColinSupport::bearings simpleform() const {return b & nF;}

    const inline double &c_x() const {return f_x;}
    const inline double &c_z() const {return f_z;}
    const inline double &c_phi() const {return f_phi;}

	const inline double &a_x(int i) const {return res[i].x;}
	const inline double &a_z(int i) const {return res[i].z;}
	const inline double &a_m(int i) const {return res[i].m;}

    const inline double &angle() const {return alp;}

	inline void setForm(const ColinSupport::bearings b_){b=b_;}
    inline void setX(const bool& Xn) {(Xn)?b|=X:b&=nFX; setCx(0);}
    inline void setZ(const bool& Zn) {(Zn)?b|=Z:b&=nFZ; setCz(0);}
    inline void setPhi(const bool& Phin) {(Phin)?b|=Phi:b&=nFPhi; setCphi(0);}

    inline void setCx(const double& c) {f_x=c; if(c==0) b=b&~fx; else b=(b&nX)|fx;}
    inline void setCz(const double& c) {f_z=c; if(c==0) b=b&~fz; else b=(b&nZ)|fz;}
    inline void setCphi(const double& c) {f_phi=c; if(c==0) b=b&~fphi; else b=(b&nPhi)|fphi;}

    inline void setAngle(const double& alpha) {alp=alpha;}

	inline void setR(bearing_result *res){this->res=res;}
	bearing_result *results(){return res;}


	friend bool operator!=(ColinSupport const& one, ColinSupport const& sec)
    {
        if(one.form() != sec.form()) return true;
        if(one.c_x()!=sec.c_x()) return true;
        if(one.c_z()!=sec.c_z()) return true;
        if(one.c_phi()!=sec.c_phi()) return true;
        if(one.angle()!=sec.angle()) return true;
        return false;
    }


private:

    bearings b;

    double f_x;
    double f_z;
    double f_phi;

    double alp;


	bearing_result *res;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ColinSupport::bearings);

#endif // WGV_BEARING_H
