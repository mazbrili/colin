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

#ifndef WGV_MAT_H
#define WGV_MAT_H

#include <QtCore/QString>
#include <QtGui/QIcon>

class ColinMaterial
{
public:
	ColinMaterial(){}
	ColinMaterial(const double &_E, /*const double &_v, const double &_roh,*/ const double &_alphaT, const QString &_name)
    {e=_E; /*Dingsda=_v; r=_roh; */at=_alphaT; n=_name;}

    const double &E() const {return e;}
    //const double &v() const {return Dingsda;}
    //const double &roh() const {return r;}
    const double &alphaT() const {return at;}
    const QString &name() const {return n;}
    //const QString &iconname() const {return in;}
    //QIcon icon() const {return(QIcon(iconname()));}


    void setE(const double &_E){e=_E;}
    //void setV(const double &_v){Dingsda=_v;}
    //void setRoh(const double &_roh){r=_roh;}
    void setAlphaT(const double &_alphaT){at=_alphaT;}
    void setName(const QString &_name){n=_name;}
    //void setIconName(const QString &_name){in=_name;}


	friend bool operator!=(ColinMaterial const& one, ColinMaterial const& sec)
    {
        if(one.E() != sec.E()) return true;
        //if(one.v() != sec.v()) return true;
        //if(one.roh() != sec.roh()) return true;
        if(one.alphaT() != sec.alphaT()) return true;
        return false;
    }
	friend bool operator==(ColinMaterial const& one, ColinMaterial const& sec)
    {
        return !(one!=sec);
    }

private:
    double e;
//    double r;
    double at;
//    double Dingsda;
    QString n;
//    QString in;
};

#endif // WGV_MAT_H
