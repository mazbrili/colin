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

#ifndef WGV_ABSTRACT_TW_H
#define WGV_ABSTRACT_TW_H

#include <QtCore/QObject>
#include "wgv_node.h"
#include "wgv_lib.h"



class wgv_abstract_tw : public QObject
{
public:
    wgv_abstract_tw(){}

    virtual const wgv_node &node(const int &i) const {return nodes.at(i);}
    virtual int node_n() const {return(nodes.size());}

protected:

    QList<wgv_node> nodes;

};


#endif // WGV_ABSTRACT_TW_H
