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
#include "colinstruct.h"

class XmlWriter : public QXmlStreamWriter
{
public:
    XmlWriter(QIODevice *device);
    XmlWriter(QByteArray *array);
	void writeTw(const ColinStruct &tw, bool includeResults = false);
    void writeSelection(const ColinStruct &tw, QPointF translation = QPointF());
    void writeLoad(const ColinStruct &tw, const int &nr);
private:
    void writeRequredLibEntries(const ColinStruct &tw, bool onlySelection = false);
	void writeNode(const ColinNode &n, bool includeResults = false);
	void writeBeam(const ColinBeam &s, bool includeResults = false);
	void writeLoad(const ColinLoad &l, const ColinStruct &tw);
	void writeBearing(const ColinSupport &b);
	void writeBLS(const ColinBLS &b);
	void writeCLS(const ColinCLS &c, const ColinStruct &tw);
    void writeMat(const ColinMaterial &m);
	void writeProfile(const ColinCrossSection &p);
	void writeResult(const ColinNode &n, const int &clsindex);
	void writeResult(const ColinBeam &s, const int &clsindex);
};

#endif // XMLWRITER_H
