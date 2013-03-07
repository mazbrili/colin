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
#include "colin_namespace.h"

class painterContent
{
public:
	enum section{
		bls_in = 0x1,
		cls_single = 0x2,
		cls_all = 0x4,
		node_in = 0x8,
		beam_in = 0x10,
		load_in = 0x20,
		node_res = 0x40,
		beam_f = 0x80,
		//beam_val = 0x100,
		fourPerPage = 0x200,
		landscape = 0x400
	};
	Q_DECLARE_FLAGS(sections, section)


	painterContent(){}
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
	structPrinter(ColinStruct *t,QPrinter *printer, const painterContent &content);
	void print(QPaintDevice *target=0, int pageNr=-1);

	painterContent getContentOfPage(QPrinter *printer, painterContent content, int pageNr);
	int getPageOfContent(QPrinter *printer, painterContent all, painterContent content);
	int requiredPages(QPrinter *printer, painterContent content, int* indizes = 0, int *perPage = 0);

	void tableContent(int *indizes);
	bool decoratePage();
	void blsPlot(int startAt, int *pages, bool test = false, int *itemsPerPage = 0);
	void blsPlot(int startAt, const int &i, const QRect &rect, bool test = false);

	void clsPlot(int startAt, int *pages, bool test = false, int *itemsPerPage = 0);
	void clsPlotall(int startAt, int *pages, bool test = false, int *itemsPerPage = 0);

	void headerNodeIn(double *dh, double dw, double width);
	void headerBeamIn(double *dh, double dw, double width);
	void headerLoadIn(double *dh, double dw, double width);

	void nodeIn(int startAt, int *pages, bool test = false, int *itemsPerPage = 0);
	void beamIn(int startAt, int *pages, bool test = false, int *itemsPerPage = 0);
	void loadIn(int startAt, int *pages, bool test = false, int *itemsPerPage = 0);
	void nodeRes(int startAt, int *pages, bool test = false, int *itemsPerPage = 0);
	void beamResF(int startAt, int *pages, bool test = false, int *itemsPerPage = 0);
	//void beamResVal(int startAt, int *pages, bool test = false, int *itemsPerPage = 0);


	int usablePageHeight();
	int lineHeight();
	int needLine(int i, bool *ok = 0);

	void printStruct(const QRect &rect, Colin::Elements, QList<int> cls);
	void printStructBLS(const QRect &rect, Colin::Elements, QList<int> bls);
	void printStruct(const QRect &rect, const QRectF &tRect, Colin::Elements, QList<int> cls);
	void printStructBLS(const QRect &rect, const QRectF &tRect, Colin::Elements, QList<int> bls);
	QTransform getScale(const QRectF &rect, const QRectF &tRect);
private:
	ColinStruct *tw;
	QPrinter *p;
	painterContent c;
	QPainter *painter;
	QPaintDevice *device;
    int pageCount;
	int totalPageCount;
    int dx;
	bool noNewPage;




};

#endif // STRUCTPRINTER_H

