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

#ifndef STRUCTPRINTER_H
#define STRUCTPRINTER_H

#include <QtGui/QPrinter>
#include "structpainter.h"
#include "viewportsettings.h"
#include "polynom.h"

class structPrinter: public QPrinter
{
    Q_DECLARE_TR_FUNCTIONS(structPrinter)
public:
    structPrinter(wgv_tw *t);
    void print();

private:
    wgv_tw *tw;
    int pageCount;
    int dx;

    void addHeader(QPainter *p);
    void addPage(QPainter *p);

    void drawElementInfo(QPainter *p, const Colin::Elements &e, const QPointF &dp_);
    void writeMats(QPainter *p);
    void writeNodes(QPainter *p);
    void writeBeams(QPainter *p);
    void writeLoads(QPainter *p);
    void writeResultsNodes(QPainter *p);
    void writeResultsBeams(QPainter *p);
    void usedSpace(int h, QPainter *p);

};

#endif // STRUCTPRINTER_H
