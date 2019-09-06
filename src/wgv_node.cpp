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

#include "wgv_node.h"




wgv_node::wgv_node()
{
    X=0;
    Z=0;

    bear = NULL;
}

wgv_node::wgv_node(double _x, double _z)
{
    X=_x;
    Z=_z;

    bear = NULL;
}


wgv_node::wgv_node(const wgv_node &n)
{
    X = n.x();
    Z = n.z();
    if(n.hasbearing())
    {
        bear = new wgv_bearing();
        *bear = n.bearing();
    }
    else
    {
        bear = NULL;
    }
}

wgv_node &wgv_node::operator=(const wgv_node &other)
{
    X = other.x();
    Z = other.z();
    if(other.hasbearing())
    {
        if(!hasbearing())
            bear = new wgv_bearing();
        *bear = other.bearing();
    }
    else if(hasbearing())
    {
        delete bear;
        bear = NULL;
    }
    return *this;

}

wgv_node::~wgv_node()
{
    if(bear != NULL)
        delete bear;
}

bool wgv_node::hasbearing() const
{
    if(bear == NULL)
        return false;
    else
        return true;
}

void wgv_node::setBearing(wgv_bearing bn)
{
    if(bear == NULL)
        bear = new wgv_bearing();
    *bear = bn;
}


