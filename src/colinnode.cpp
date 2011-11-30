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

#include "colinnode.h"

#include <QtCore/QDebug>


ColinNode::ColinNode():
	ColinElement()
{
    X=0;
    Z=0;

    bear = NULL;

	res = NULL;

	qDebug() << "node without parent";
}

ColinNode::ColinNode(double _x, double _z):
	ColinElement()
{
    X=_x;
    Z=_z;

    bear = NULL;

	res = NULL;

}


ColinNode::ColinNode(const ColinNode &n):
	ColinElement(n)
{
    X = n.x();
    Z = n.z();
    if(n.hasbearing())
    {
        bear = new ColinSupport();
        *bear = n.bearing();
    }
    else
    {
        bear = NULL;
    }
	par=n.par;
	res = n.res;

}

ColinNode &ColinNode::operator=(const ColinNode &other)
{
    X = other.x();
    Z = other.z();
    if(other.hasbearing())
    {
        if(!hasbearing())
            bear = new ColinSupport();
        *bear = other.bearing();
    }
    else if(hasbearing())
    {
        delete bear;
        bear = NULL;
    }
	par=other.par;
	res = other.res;

	return *this;

}

ColinNode::~ColinNode()
{
    if(bear != NULL)
        delete bear;
}

bool ColinNode::hasbearing() const
{
    if(bear == NULL)
        return false;
    else
        return true;
}

void ColinNode::setBearing(ColinSupport bn)
{
    if(bear == NULL)
        bear = new ColinSupport();
    *bear = bn;
}


