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

#ifndef WGV_LOAD_H
#define WGV_LOAD_H

#include <cmath>
#include <QtCore/QPointF>
#include <QtGui/QPolygonF>
#include "colinelement.h"



class ColinLoad : public ColinElement
{
public:
    enum form{
        nodeLoad                = 0,
        moment                  = 1,
        uniformlyDistibutedLoad = 2,
        increasingLinearLoad    = 3,
        decreasingLinearLoad    = 4,
        tempChange              = 5,
        tempDiffrence           = 6,
        doubleLoadLeft          = 7,
        doubleLoadRight         = 8,
//        defomation              = 9
    };

	ColinLoad(){ _Px = 0; _Pz = 0; _M = 0; _at = -1; _set = -1;}
	ColinLoad(const form& typn, const int& atn, const double& Pxn, const double& Pzn, const double& Mn, const int & set){
		_typ=typn; _at=atn; _Px=Pxn; _Pz=Pzn; _M=Mn; _set=set;}
	ColinLoad(const ColinLoad &other);

    const form &typ()const {return _typ;}
    const double &Px()const {return _Px;}
    const double &Pz()const {return _Pz;}
    const double &M()const {return _M;}
    const int &at()const {return _at;}
	const int &set()const {return _set;}
    const QPolygonF &shape()const {return _shape;}
    bool isOnBeam()const{return !(_typ==nodeLoad || _typ==moment);}

    void setPx(const double& Pxn) {_Px=Pxn;}
    void setPz(const double& Pzn) {_Pz=Pzn;}
    void setP(const QPointF& P){setPx(P.x()); setPz(P.y());}
    void setM(const double& Mn) {_M=Mn;}
    void setTyp(const form& typn) {_typ=typn;}
    void setBeam(const int& atn) {_at=atn;}
    void setShape(const QPolygonF &shapen) {_shape = shapen;}
	void setSet(const int& setn) {_set=setn;}

    double p(const double &xi) const{
        switch (_typ){
		case ColinLoad::uniformlyDistibutedLoad:         return (1);
		case ColinLoad::increasingLinearLoad:            return(xi);
		case ColinLoad::decreasingLinearLoad:            return(1-xi);
        default:                                        return (0);
        }}
    double angle() const {return atan2(_Px, _Pz);}

private:
    form _typ;
    double _Px;
    double _Pz;
    double _M;
    int _at;
    QPolygonF _shape;
	int _set;
};


#endif // WGV_LOAD_H
