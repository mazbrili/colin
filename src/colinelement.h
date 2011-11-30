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

#ifndef WGV_ELEMENT_H
#define WGV_ELEMENT_H

class ColinStruct;

class ColinElement
{
public:
	ColinElement() {selec = false; par=0;}
	ColinElement(const ColinElement &other){selec = other.selec; par = other.par;}
    inline const bool &isSelected() const {return selec;}
    virtual void select(bool s = true){selec = s;}
	void inline setStruct(ColinStruct *parent){par=parent;}
	const inline ColinStruct *getStruct() const {return par;}
protected:
    bool selec;
	ColinStruct *par;

};

#endif // WGV_ELEMENT_H
