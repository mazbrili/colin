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


class painterContent
{
public:
	enum section{
		bls_in = 0x1,
		node_in = 0x2,
		beam_in = 0x4,
		load_in = 0x8,
		cls_all = 0x10,
		cls_single = 0x20,
		node_res = 0x40,
		beam_f = 0x80,
		beam_val = 0x100,
		fourPerPage = 0x200,
		landscape = 0x400,
	};
	Q_DECLARE_FLAGS(sections, section)


	painterContent(sections s_, QFont f, int index_){s=s_; index_=index; font=f;}
	sections s;
	int index;
	QFont font;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(painterContent::sections)


class structPrinter
{
    Q_DECLARE_TR_FUNCTIONS(structPrinter)
public:
	structPrinter(ColinStruct *t);
	void print(QPrinter *printer, painterContent content, QPaintDevice *target=0, int pageNr=-1);

	painterContent getContentOfPage(QPrinter *printer, painterContent content, int pageNr);
	int getPageOfContent(QPrinter *printer, painterContent all, painterContent content);
	int requiredPages(QPrinter *printer, painterContent content);

	void tableContent();
	void blsPlot();
	void blsPlot(const int &i, const QRect &rect);

	void clsPlot();
	void clsPlotall(const QRect &rect);
	void clsPlot(const int &i, const QRect &rect);

	void nodeIn();
	void nodeIn(const int &i);

	void beamIn();
	void beamIn(const int &i);

	void loadIn();
	void loadIn(const int &i);

	void nodeRes();
	void nodeRes(const int &i);

	void beamResF();
	void beamResF(const int &i);

	void beamResVal();
	void beamResVal(const int &i);
private:
	ColinStruct *tw;
    int pageCount;
    int dx;




};

#endif // STRUCTPRINTER_H

