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

#ifndef XMLWRITER_H
#define XMLWRITER_H

#include <QtCore/QXmlStreamWriter>
#include "wgv_tw.h"

class XmlWriter : public QXmlStreamWriter
{
public:
    XmlWriter(QIODevice *device);
    XmlWriter(QByteArray *array);
    void writeTw(const wgv_tw &tw);
    void writeSelection(const wgv_tw &tw, QPointF translation = QPointF());
    void writeLoad(const wgv_tw &tw, const int &nr);
private:
    void writeRequredLibEntries(const wgv_tw &tw, bool onlySelection = false);
    void writeNode(const wgv_node &n);
    void writeBeam(const wgv_beam &s);
    void writeLoad(const wgv_load &l);
    void writeBearing(const wgv_bearing &b);
    void writeMat(const wgv_mat &m);
    void writeProfile(const wgv_profile &p);
};

#endif // XMLWRITER_H
