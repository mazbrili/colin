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

#ifndef XMLREADER_H
#define XMLREADER_H

#include <QtCore/QXmlStreamReader>
#include "colinstruct.h"


class XmlReader : public QXmlStreamReader
{
	friend class ColinStruct;
public:
	XmlReader(ColinStruct *t);
    bool read(QIODevice *device);
    bool read(const QByteArray &array);
    void readTw();
    void readNode();
    void readBeam();
    void readLoad();
	void readBLS();
	void readCLS();
    void readMat();
    void readProfile();
	ColinSupport readBearing();
    QString errors() const;

private:
	ColinStruct *tw;

    QMap<QString, int> materialMap;
    QMap<QString, int> profileMap;
};

#endif // XMLREADER_H
