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

#include "structprinter.h"
#include "colinicons.h"
#include "unitsettings.h"
#include "colinversion.h"
#include "detailpainter.h"


#include <QtCore/QDate>


#define PRINTER_VERBOSE

const int titleSize = 3;
const int alignLeft = 3;

QColor structPrinter_gray = QColor(200, 200, 200);

structPrinter::structPrinter(ColinStruct *t, QPrinter *printer, const painterContent &content)
{
	this->tw = t;
	this->c = content;
	this->p = printer;
	painter = 0;
	device = 0;
	noNewPage = false;
}

void structPrinter::print(QPaintDevice *target, int pageNr)
{
#ifdef PRINTER_VERBOSE
	qDebug()  << "structPrinter::print(...)";
	qDebug() << "pageNr = " << pageNr;
#endif
	if(pageNr>-1){
		noNewPage=true;
		pageCount = pageNr;
	}
	else{
	}

	Q_ASSERT(pageNr >= -1);
	this->device = target;
	if(!painter)
		painter = new QPainter(target);
	painter->setFont(c.font);
	painter->scale(double(target->width())/double(p->paperRect(QPrinter::DevicePixel).width()),
				   double(target->height())/double(p->paperRect(QPrinter::DevicePixel).height()));

#ifdef PRINTER_VERBOSE
	qDebug() << "structPrinter::painter matrix = " << painter->transform();
#endif

	int indizes[10];
	totalPageCount = requiredPages(p, c, indizes);



	if(pageNr==-1)
	{
		pageCount=0;
		tableContent(indizes);
		if(c.s.testFlag(painterContent::bls_in))
			blsPlot(0, &pageCount);
		if(c.s.testFlag(painterContent::cls_single))
			clsPlot(0, &pageCount);
		if(c.s.testFlag(painterContent::cls_all))
			clsPlotall(0, &pageCount);
		if(c.s.testFlag(painterContent::node_in))
			nodeIn(0, &pageCount);
		if(c.s.testFlag(painterContent::beam_in))
			beamIn(0, &pageCount);
		if(c.s.testFlag(painterContent::load_in))
			loadIn(0, &pageCount);
		if(c.s.testFlag(painterContent::node_res))
			nodeRes(0, &pageCount);
		if(c.s.testFlag(painterContent::beam_f))
			beamResF(0, &pageCount);
//		if(c.s.testFlag(painterContent::beam_val))
//			beamResVal(0, &pageCount);
		noNewPage=true;
		decoratePage();
	}
	else
	{
#ifdef PRINTER_VERBOSE
		qDebug() << "structPrinter::painting page nr " << pageNr;
#endif
		painterContent pageContent = this->getContentOfPage(p, c, pageNr);
#ifdef PRINTER_VERBOSE
		qDebug() << "section of page is " << pageContent.s;
		qDebug() << "first index of page is " << pageContent.index;
#endif

		int *indizes = new int[9];
		requiredPages(p, c, indizes);
		pageCount = pageNr;
		switch (pageContent.s)
		{
		case painterContent::bls_in:
			blsPlot(pageContent.index, &pageNr);
			break;
		case painterContent::cls_single:
			clsPlot(pageContent.index, &pageNr);
			break;
		case painterContent::cls_all:
			clsPlotall(pageContent.index, &pageNr);
			break;
		case painterContent::node_in:
			nodeIn(pageContent.index, &pageNr);
			break;
		case painterContent::beam_in:
			beamIn(pageContent.index, &pageNr);
			break;
		case painterContent::load_in:
			loadIn(pageContent.index, &pageNr);
			break;
		case painterContent::node_res:
			nodeRes(pageContent.index, &pageNr);
			break;
		case painterContent::beam_f:
			beamResF(pageContent.index, &pageNr);
			break;
//		case painterContent::beam_val:
//			beamResVal(pageContent.index, &pageNr);
//			break;
		default:
			tableContent(indizes);
		}
		delete indizes;
	}
	this->device=0;

	if(pageNr>-1)
		noNewPage=false;
	delete painter;
	painter=0;
}

painterContent structPrinter::getContentOfPage(QPrinter *printer, painterContent content, int pageNr)
{
#ifdef PRINTER_VERBOSE
	qDebug() << "structPrinter::getContentOfPage(..., " << pageNr << ")";
#endif
	painterContent pageContent;
	if(pageNr==0){
		pageContent.s = painterContent::section(0);
		return pageContent;
	}
	int *indizes = new int[9];
	int *itemsPerPage = new int[9];
	requiredPages(printer, content, indizes, itemsPerPage);
	for(int i=1; i<9; i++){
		if(indizes[i]-1>=pageNr){
			pageContent.s = painterContent::section(1 << (i-1));
			pageContent.index = (pageNr-indizes[i-1])*itemsPerPage[i-1];

#ifdef PRINTER_VERBOSE
			qDebug() << "indizes: "
					 << indizes[0] << ", "
					 << indizes[1] << ", "
					 << indizes[2] << ", "
					 << indizes[3] << ", "
					 << indizes[4] << ", "
					 << indizes[5] << ", "
					 << indizes[6] << ", "
					 << indizes[7];
			qDebug() << "section of pageNr" << pageNr << " is section nr" << i;
			qDebug() << "first page of this section: " << indizes[i-1];
			qDebug() << "items per page:" << itemsPerPage[i-1];
			qDebug() << "first index on this page:" << pageContent.index;
#endif
			break;
		}
	}
	delete indizes;
	return pageContent;
}

int structPrinter::getPageOfContent(QPrinter *printer, painterContent all, painterContent content)
{
	return 1;
}

int structPrinter::requiredPages(QPrinter *printer, painterContent content, int* indizes, int *perPage)
{
#ifdef PRINTER_VERBOSE
	qDebug()  << "structPrinter::requiredPages(...)";
#endif
	pageCount = 0;
	int specPerPage;
	bool createdPainter = false;
	if(!painter){
		if(device)
			painter = new QPainter(device);
		else
			painter = new QPainter();
		createdPainter = true;
	}
	if(indizes){
		for(int i=0; i<9; i++)
			indizes[i]=0;
	}
	int pages = 1;
	if(content.s.testFlag(painterContent::bls_in)){
		if(indizes)
			indizes[0]=pages;
		blsPlot(0, &pages, true, &specPerPage);
		if(perPage)
			perPage[0]=specPerPage;
	}
	if(content.s.testFlag(painterContent::cls_single)){
		if(indizes)
			indizes[1]=pages;
		clsPlot(0, &pages, true, &specPerPage);
		if(perPage)
			perPage[1]=specPerPage;
	}
	if(content.s.testFlag(painterContent::cls_all)){
		if(indizes)
			indizes[2]=pages;
		clsPlotall(0, &pages, true, &specPerPage);
		if(perPage)
			perPage[2]=specPerPage;
	}
	if(content.s.testFlag(painterContent::node_in)){
		if(indizes)
			indizes[3]=pages;
		nodeIn(0, &pages, true, &specPerPage);
		if(perPage)
			perPage[3]=specPerPage;
	}
	if(content.s.testFlag(painterContent::beam_in)){
		if(indizes)
			indizes[4]=pages;
		beamIn(0, &pages, true, &specPerPage);
		if(perPage)
			perPage[4]=specPerPage;
	}
	if(content.s.testFlag(painterContent::load_in)){
		if(indizes)
			indizes[5]=pages;
		loadIn(0, &pages, true, &specPerPage);
		if(perPage)
			perPage[5]=specPerPage;
	}
	if(content.s.testFlag(painterContent::node_res)){
		if(indizes)
			indizes[6]=pages;
		nodeRes(0, &pages, true, &specPerPage);
		if(perPage)
			perPage[6]=specPerPage;
	}
	if(content.s.testFlag(painterContent::beam_f)){
		if(indizes)
			indizes[7]=pages;
		beamResF(0, &pages, true, &specPerPage);
		if(perPage)
			perPage[7]=specPerPage;
	}
//	if(content.s.testFlag(painterContent::beam_val)){
//		if(indizes)
//			indizes[8]=pages;
//		beamResVal(0, &pages, true, &specPerPage);
//		if(perPage)
//			perPage[8]=specPerPage;
//	}
	if(createdPainter){
		delete painter;
		painter = 0;
	}

#ifdef PRINTER_VERBOSE
	qDebug() << pages << " pages required!";
#endif
	return pages;
}

void structPrinter::tableContent(int *indizes)
{
#ifdef PRINTER_VERBOSE
	qDebug()  << "structPrinter::tableContent(...)";
#endif

	QRectF bR;

	int i=1;

	QFont big = c.font;
	if(big.pointSize()>0)
		big.setPointSize(big.pointSize()*1.0);
	else
		big.setPixelSize(big.pixelSize()*1.0);
	painter->setFont(big);

	this->dx = p->pageRect().top();

	painter->drawText(QRect(p->pageRect().left(), needLine(2)-1*lineHeight(), p->pageRect().width(), lineHeight()),
					  Qt::AlignVCenter | Qt::AlignHCenter,
					  tr("analysis results for ")+QString(" \"%1\"").arg(filelist::instance().currentFile()->objectName()), &bR);

	painter->setFont(c.font);
	painter->drawText(QRect(p->pageRect().left(), needLine(1), p->pageRect().width(), lineHeight()),
					  Qt::AlignVCenter | Qt::AlignHCenter,
					  tr("using Colin %1").arg(ColinVersion()), &bR);
	painter->drawText(QRect(p->pageRect().left(), needLine(1), p->pageRect().width(), lineHeight()),
					  Qt::AlignVCenter | Qt::AlignHCenter,
					  QDate::currentDate().toString(), &bR);
	painter->setFont(big);


	int n=4;
	for(int m=0; m<9; m++){
		if(indizes[m]>0)
			n++;
	}

	dx+= lineHeight()*2;
	int requiredSpace = n*lineHeight();

	int spaceLeft = usablePageHeight()-dx-requiredSpace;

	painter->drawRect( p->pageRect().left(), dx, p->pageRect().width(), spaceLeft);
	this->printStruct(QRect(p->pageRect().left(), dx, p->pageRect().width(), spaceLeft),
					  Colin::nload|
					  Colin::sload|
					  Colin::beam|
					  Colin::bearing|
					  Colin::joint,
					  QList<int>());

	dx+=spaceLeft;


	painter->drawText(QRect(p->pageRect().left(), needLine(2)-lineHeight(), p->pageRect().width(), lineHeight()),
					  Qt::AlignVCenter | Qt::AlignLeft,
					  tr("table of contents"), &bR);


	if(indizes[0]){
		painter->drawText(QRect(p->pageRect().left(), needLine(1), p->pageRect().width(), lineHeight()),
						  Qt::AlignVCenter | Qt::AlignLeft,
						  QString::number(i++) + ".   " + tr("basic load sets"), &bR);
		painter->drawText(QRect(p->pageRect().left(), dx, p->pageRect().width(), lineHeight()),
						  Qt::AlignVCenter | Qt::AlignRight,
						  QString::number(indizes[0]), &bR);
	}
	if(indizes[1]){
		painter->drawText(QRect(p->pageRect().left(), needLine(1), p->pageRect().width(), lineHeight()),
						  Qt::AlignVCenter | Qt::AlignLeft,
						  QString::number(i++) + ".   " + tr("combined load sets"));
		painter->drawText(QRect(p->pageRect().left(), dx, p->pageRect().width(), lineHeight()),
						  Qt::AlignVCenter | Qt::AlignRight,
						  QString::number(indizes[1]), &bR);
	}
	if(indizes[2]){
		painter->drawText(QRect(p->pageRect().left(), needLine(1), p->pageRect().width(), lineHeight()),
						  Qt::AlignVCenter | Qt::AlignLeft,
						  QString::number(i++) + ".   " + tr("combined load sets"));
		painter->drawText(QRect(p->pageRect().left(), dx, p->pageRect().width(), lineHeight()),
						  Qt::AlignVCenter | Qt::AlignRight,
						  QString::number(indizes[2]), &bR);
	}
	if(indizes[3]){
		painter->drawText(QRect(p->pageRect().left(), needLine(1), p->pageRect().width(), lineHeight()),
						  Qt::AlignVCenter | Qt::AlignLeft,
						  QString::number(i++) + ".   " + tr("node definitions"));
		painter->drawText(QRect(p->pageRect().left(), dx, p->pageRect().width(), lineHeight()),
						  Qt::AlignVCenter | Qt::AlignRight,
						  QString::number(indizes[3]), &bR);
	}
	if(indizes[4]){
		painter->drawText(QRect(p->pageRect().left(), needLine(1), p->pageRect().width(), lineHeight()),
						  Qt::AlignVCenter | Qt::AlignLeft,
						  QString::number(i++) + ".   " + tr("beam definitions"));
		painter->drawText(QRect(p->pageRect().left(), dx, p->pageRect().width(), lineHeight()),
						  Qt::AlignVCenter | Qt::AlignRight,
						  QString::number(indizes[4]), &bR);
	}
	if(indizes[5]){
		painter->drawText(QRect(p->pageRect().left(), needLine(1), p->pageRect().width(), lineHeight()),
						  Qt::AlignVCenter | Qt::AlignLeft,
						  QString::number(i++) + ".   " + tr("load definitions"));
		painter->drawText(QRect(p->pageRect().left(), dx, p->pageRect().width(), lineHeight()),
						  Qt::AlignVCenter | Qt::AlignRight,
						  QString::number(indizes[5]), &bR);
	}
	if(indizes[6]){
		painter->drawText(QRect(p->pageRect().left(), needLine(1), p->pageRect().width(), lineHeight()),
						  Qt::AlignVCenter | Qt::AlignLeft,
						  QString::number(i++) + ".   " + tr("node results"));
		painter->drawText(QRect(p->pageRect().left(), dx, p->pageRect().width(), lineHeight()),
						  Qt::AlignVCenter | Qt::AlignRight,
						  QString::number(indizes[6]), &bR);
	}
	if(indizes[7]){
		painter->drawText(QRect(p->pageRect().left(), needLine(1), p->pageRect().width(), lineHeight()),
						  Qt::AlignVCenter | Qt::AlignLeft,
						  QString::number(i++) + ".   " + tr("beam results (as functions)"));
		painter->drawText(QRect(p->pageRect().left(), dx, p->pageRect().width(), lineHeight()),
						  Qt::AlignVCenter | Qt::AlignRight,
						  QString::number(indizes[7]), &bR);
	}
	if(indizes[8]){
		painter->drawText(QRect(p->pageRect().left(), needLine(1), p->pageRect().width(), lineHeight()),
						  Qt::AlignVCenter | Qt::AlignLeft,
						  QString::number(i++) + ".   " + tr("beam results (as table)"));
		painter->drawText(QRect(p->pageRect().left(), dx, p->pageRect().width(), lineHeight()),
						  Qt::AlignVCenter | Qt::AlignRight,
						  QString::number(indizes[8]), &bR);
	}

	decoratePage();
}

bool structPrinter::decoratePage()
{
#ifdef PRINTER_VERBOSE
	qDebug()  << "structPrinter::decoratePage(...)" << " pageNr: " << pageCount;
#endif

	painter->save();
	painter->resetTransform();
	painter->setFont(c.font);
	painter->scale(double(painter->device()->width())/double(p->paperRect(QPrinter::DevicePixel).width()),
				   double(painter->device()->height())/double(p->paperRect(QPrinter::DevicePixel).height()));


	painter->setPen(QPen(Qt::black, 0));
	QRect pr = p->pageRect(QPrinter::DevicePixel).toRect();
	QRect bR;

	dx = pr.top();

	painter->setOpacity(0.3);
	painter->drawPixmap(pr.left(), pr.bottom()-64, 64, 64, QPixmap(colinIcons::icondir_+"tooltip/node_dark_64.png"));
	painter->drawPixmap(pr.right()-64, pr.bottom()-64, 64, 64, QPixmap(colinIcons::icondir_+"clazzes-logo-alpha_64x64.png"));
	painter->setOpacity(1);
	painter->drawText(QRect(pr.bottomLeft()-QPoint(0, 20), QSize(300, 20)),
					  Qt::AlignBottom | Qt::AlignLeft,
					  "Colin - clazzes.org");

#ifdef PRINTER_VERBOSE
	qDebug() << "pageCount = " << pageCount;
#endif

	if(pageCount!=0)
	{
		painter->drawText(QRect(pr.bottomRight()-QPoint(100, 20), QSize(100, 20)),
						  Qt::AlignBottom | Qt::AlignRight,
						  QString("%1/%2").arg(pageCount).arg(totalPageCount-1), &bR);
	}
	/*else
	{
		painter->drawText(QRect(pr.bottomRight()-QPoint(100, 20), QSize(100, 20)),
						  Qt::AlignBottom | Qt::AlignRight,
						  QString(tr("contents")), &bR);
	}*/
	painter->drawLine(QPoint(pr.left(), pr.bottom()-painter->fontMetrics().height()-2),
					  QPoint(pr.right(), pr.bottom()-painter->fontMetrics().height()-2));

	pageCount++;


	if(!noNewPage)
		p->newPage();

	painter->restore();
	return !noNewPage;
}

int structPrinter::usablePageHeight()
{
	return p->pageRect(QPrinter::DevicePixel).toRect().height()-lineHeight();
}

int structPrinter::lineHeight()
{
	return ((c.font.pixelSize()>0)?c.font.pixelSize():c.font.pointSizeF())*1.5;
}

int structPrinter::needLine(int i, bool *ok)
{
	bool newPage = true;
	if(dx+lineHeight()*i > p->pageRect().height())
	{
		newPage = decoratePage();
		dx = p->pageRect().top();
	}
	else
	{
		dx += i*lineHeight();
	}
	if(ok) *ok = newPage;

	return dx;
}

void structPrinter::blsPlot(int startAt, int *pages, bool test, int *itemsPerPage)
{
#ifdef PRINTER_VERBOSE
	qDebug()  << "structPrinter::blsPlot(...)" << " pageNr: " << pageCount;
#endif


	if(test)
	{
		int required = 0;
		if(c.s.testFlag(painterContent::fourPerPage))
			required = ceil(double(tw->bls_n())/4.);
		else
			required = tw->bls_n();
		(*pages)+=required;
		if(itemsPerPage)
			*itemsPerPage = c.s.testFlag(painterContent::fourPerPage)?4:1;
		return;
	}


	QRect rect = p->pageRect();

	rect.setHeight(usablePageHeight());

	if(c.s.testFlag(painterContent::fourPerPage))
	{
		rect.setWidth(rect.width()/2);
		rect.setHeight(rect.height()/2);
		rect.translate(rect.width(), rect.height());
	}
	for(int i=0; i<tw->bls_n()-startAt; i++)
	{
		if(c.s.testFlag(painterContent::fourPerPage))
		{
			if(!(i%4))
			{
				rect.translate(-rect.width(), -rect.height());
				painter->drawLine(rect.topRight(), rect.bottomRight()+QPointF(0, rect.height()));
				painter->drawLine(rect.bottomLeft(), rect.bottomRight()+QPointF(rect.width(), 0));
			}
			else if(!(i%2))
				rect.translate(-rect.width(), rect.height());
			else
				rect.translate(rect.width(), 0);
		}

		painter->drawText(rect.adjusted(10, 0, 0, 0), Qt::AlignTop | Qt::AlignLeft, QString(tr("BLS #%1: ")).arg(i)+tw->bls(i).name());
		QList<int> bls;
		bls << i+startAt;
		printStructBLS(rect,
					   Colin::nload|
					   Colin::sload|
					   Colin::beam|
					   Colin::bearing|
					   Colin::joint,
					   bls);

		if((i%4)==3 || !c.s.testFlag(painterContent::fourPerPage)){
			if(!decoratePage())
				return;
		}
	}
}


void structPrinter::clsPlot(int startAt, int *pages, bool test, int *itemsPerPage)
{
#ifdef PRINTER_VERBOSE
	qDebug()  << "structPrinter::clsPlot(...)" << " pageNr: " << pageCount;
#endif

	if(test)
	{
		int required;
		if(c.s.testFlag(painterContent::fourPerPage)){
			required = tw->cls_n();
			if(itemsPerPage) *itemsPerPage = 4;
		}
		else{
			required = tw->cls_n()*4;
			if(itemsPerPage) *itemsPerPage = 1;
		}
		(*pages)+=required;
		return;
	}


	decoratePage();
	QRect rect = p->pageRect();
	rect.setHeight(usablePageHeight());
	if(c.s.testFlag(painterContent::fourPerPage))
	{
		rect.setWidth(rect.width()/2);
		rect.setHeight(rect.height()/2);
		rect.translate(rect.width(), rect.height());
	}
	for(int m=startAt/4; m<tw->cls_n(); m++)
	{
		for(int j=startAt%4; j<4; j++)
		{
			int i = m*4+j;
			Colin::Elements toDraw;
			if(j==0)				toDraw = Colin::nload | Colin::u;
			if(j==1)				toDraw = Colin::N;
			if(j==2)				toDraw = Colin::Q;
			if(j==3)				toDraw = Colin::M;


			if(c.s.testFlag(painterContent::fourPerPage))
			{
				if(!(i%4)){
					rect.translate(-rect.width(), -rect.height());
					painter->drawLine(rect.topRight(), rect.bottomRight()+QPointF(0, rect.height()));
					painter->drawLine(rect.bottomLeft(), rect.bottomRight()+QPointF(rect.width(), 0));
				}
				else if(!(i%2))
					rect.translate(-rect.width(), rect.height());
				else
					rect.translate(rect.width(), 0);
			}

			painter->drawText(rect.adjusted(10, 0, 0, 0), Qt::AlignTop | Qt::AlignLeft, QString(tr("CLS #%1: ")).arg(i)+tw->cls(m).name());
			QList<int> cls;
			cls << m;

#ifdef PRINTER_VERBOSE
			qDebug() << "cls = " << cls;
#endif

			printStruct(rect,
						 Colin::beam    |
						 Colin::bearing |
						 Colin::joint   |
						 toDraw,
						 cls);

			if((i%4)==3 || !c.s.testFlag(painterContent::fourPerPage)){
				if(!decoratePage())
					return;
			}
		}
	}
}

void structPrinter::clsPlotall(int startAt, int *pages, bool test, int *itemsPerPage)
{
#ifdef PRINTER_VERBOSE
	qDebug()  << "structPrinter::clsPlotall(...)" << " pageNr: " << pageCount;
#endif

	if(test)
	{
		int required;
		if(c.s.testFlag(painterContent::fourPerPage))
			required = 1;
		else
			required = 4;
		(*pages)+=required;
		if(itemsPerPage)
			*itemsPerPage = c.s.testFlag(painterContent::fourPerPage)?4:1;
		return;
	}

	QRect rect = p->pageRect();
	rect.setHeight(usablePageHeight());
	if(c.s.testFlag(painterContent::fourPerPage))
	{
		rect.setWidth(rect.width()/2);
		rect.setHeight(rect.height()/2);
		rect.translate(rect.width(), rect.height());
	}
	for(int j=startAt; j<4; j++)
	{
		int i = j;
		Colin::Elements toDraw;
		if(j==0)				toDraw = Colin::nload | Colin::u;
		if(j==1)				toDraw = Colin::N;
		if(j==2)				toDraw = Colin::Q;
		if(j==3)				toDraw = Colin::M;


		if(c.s.testFlag(painterContent::fourPerPage))
		{
			if(!(i%4)){
				rect.translate(-rect.width(), -rect.height());
				painter->drawLine(rect.topRight(), rect.bottomRight()+QPointF(0, rect.height()));
				painter->drawLine(rect.bottomLeft(), rect.bottomRight()+QPointF(rect.width(), 0));
			}
			else if(!(i%2))
				rect.translate(-rect.width(), rect.height());
			else
				rect.translate(rect.width(), 0);
		}

		QList<int> cls;
		for(int m=0; m<tw->cls_n(); m++) cls << m;
		printStruct(rect,
					 Colin::beam    |
					 Colin::bearing |
					 Colin::joint   |
					 toDraw,
					 cls);

		if((i%4)==3 || !c.s.testFlag(painterContent::fourPerPage)){
			if(!decoratePage())
				return;
		}
	}
}


void structPrinter::headerNodeIn(double *dh, double dw, double width)
{

	QRectF bRect;
	painter->drawText(QRectF(dw+width*0, *dh, p->pageRect().width(), width), Qt::AlignLeft | Qt::AlignTop, tr("nodes"), &bRect);
	painter->drawText(QRectF(dw+width*4, *dh, p->pageRect().width(), width), Qt::AlignLeft | Qt::AlignTop, tr("supports"), &bRect);
	*dh += lineHeight()+4;
	painter->drawLine(p->pageRect().left(), *dh-2, p->pageRect().right(), *dh-2);

	int j=1;
	painter->drawText(QRectF(dw+width*j++, *dh, p->pageRect().width(), width), Qt::AlignLeft | Qt::AlignTop, tr("x[m]"), &bRect);
	painter->drawText(QRectF(dw+width*j++, *dh, p->pageRect().width(), width), Qt::AlignLeft | Qt::AlignTop, tr("z[m]"), &bRect);
	painter->drawText(QRectF(dw+width*j++, *dh, p->pageRect().width(), width), Qt::AlignLeft | Qt::AlignTop, tr("phi[grad]"), &bRect);
	painter->drawText(QRectF(dw+width*j++, *dh, p->pageRect().width(), width), Qt::AlignLeft | Qt::AlignTop, QString(tr("x[%1]")).arg(unitSettings::instance().Feh()), &bRect);
	painter->drawText(QRectF(dw+width*j++, *dh, p->pageRect().width(), width), Qt::AlignLeft | Qt::AlignTop, QString(tr("z[%1]")).arg(unitSettings::instance().Feh()), &bRect);
	painter->drawText(QRectF(dw+width*j++, *dh, p->pageRect().width(), width), Qt::AlignLeft | Qt::AlignTop, QString(tr("phi[%1]")).arg(unitSettings::instance().FMeh()), &bRect);

	*dh += lineHeight()+4;

	painter->drawLine(p->pageRect().left(), *dh-2, p->pageRect().right(), *dh-2);

}

void structPrinter::headerBeamIn(double *dh, double dw, double width)
{

	QRectF bRect;
	painter->drawText(QRectF(dw+width*0, *dh, p->pageRect().width(), width), Qt::AlignLeft | Qt::AlignTop, tr("beams"), &bRect);
	painter->drawText(QRectF(dw+width*5, *dh, p->pageRect().width(), width), Qt::AlignLeft | Qt::AlignTop, tr("hinges"), &bRect);
	*dh += lineHeight()+4;
	painter->drawLine(p->pageRect().left(), *dh-2, p->pageRect().right(), *dh-2);

	double j=1;
	painter->drawText(QRectF(dw+width*j++, *dh, p->pageRect().width(), width), Qt::AlignLeft | Qt::AlignTop, tr("left Node"), &bRect);
	painter->drawText(QRectF(dw+width*j++, *dh, p->pageRect().width(), width), Qt::AlignLeft | Qt::AlignTop, tr("right Node"), &bRect);
	painter->drawText(QRectF(dw+width*j++, *dh, p->pageRect().width(), width), Qt::AlignLeft | Qt::AlignTop, tr("material"), &bRect);
	painter->drawText(QRectF(dw+width*j++, *dh, p->pageRect().width(), width), Qt::AlignLeft | Qt::AlignTop, tr("cross section"), &bRect);

	painter->drawText(QRectF(dw+width*j, *dh, p->pageRect().width(), width), Qt::AlignLeft | Qt::AlignTop, tr("u_l"), &bRect);j+=0.5;
	painter->drawText(QRectF(dw+width*j, *dh, p->pageRect().width(), width), Qt::AlignLeft | Qt::AlignTop, tr("w_l"), &bRect);j+=0.5;
	painter->drawText(QRectF(dw+width*j, *dh, p->pageRect().width(), width), Qt::AlignLeft | Qt::AlignTop, tr("phi_l"), &bRect);j+=0.5;
	painter->drawText(QRectF(dw+width*j, *dh, p->pageRect().width(), width), Qt::AlignLeft | Qt::AlignTop, tr("u_r"), &bRect);j+=0.5;
	painter->drawText(QRectF(dw+width*j, *dh, p->pageRect().width(), width), Qt::AlignLeft | Qt::AlignTop, tr("w_r"), &bRect);j+=0.5;
	painter->drawText(QRectF(dw+width*j, *dh, p->pageRect().width(), width), Qt::AlignLeft | Qt::AlignTop, tr("phi_r"), &bRect);j+=0.5;

	*dh += lineHeight()+4;

	painter->drawLine(p->pageRect().left(), *dh-2, p->pageRect().right(), *dh-2);

}

void structPrinter::headerLoadIn(double *dh, double dw, double width)
{
	QRectF bRect;
	painter->drawText(QRectF(dw+width*0, *dh, p->pageRect().width(), width), Qt::AlignLeft | Qt::AlignTop, tr("loads"), &bRect);
	*dh = bRect.bottom()+4;
	painter->drawLine(p->pageRect().left(), *dh-2, p->pageRect().right(), *dh-2);

	double j=1;
	painter->drawText(QRectF(dw+width*j++, *dh, p->pageRect().width(), width), Qt::AlignLeft | Qt::AlignTop, tr("Beam/Node"), &bRect);
	painter->drawText(QRectF(dw+width*j++, *dh, p->pageRect().width(), width), Qt::AlignLeft | Qt::AlignTop, QString(tr("Px[%1]/[%1/m]")).arg(unitSettings::instance().Peh()), &bRect);
	painter->drawText(QRectF(dw+width*j++, *dh, p->pageRect().width(), width), Qt::AlignLeft | Qt::AlignTop, QString(tr("Pz[%1]/[%1/m]")).arg(unitSettings::instance().Peh()), &bRect);
	painter->drawText(QRectF(dw+width*j++, *dh, p->pageRect().width(), width), Qt::AlignLeft | Qt::AlignTop, QString(tr("M[%1]")).arg(unitSettings::instance().Meh()), &bRect);
	painter->drawText(QRectF(dw+width*j++, *dh, p->pageRect().width(), width), Qt::AlignLeft | Qt::AlignTop, QString(tr("dT[K]")), &bRect);
	painter->drawText(QRectF(dw+width*j++, *dh, p->pageRect().width(), width), Qt::AlignLeft | Qt::AlignTop, QString(tr("dTh[K]")), &bRect);
	painter->drawText(QRectF(dw+width*j++, *dh, p->pageRect().width(), width), Qt::AlignLeft | Qt::AlignTop, tr("Set"), &bRect);


	*dh = bRect.bottom()+4;

	painter->drawLine(p->pageRect().left(), *dh-2, p->pageRect().right(), *dh-2);

}

void structPrinter::nodeIn(int startAt, int *pages, bool test, int *itemsPerPage)
{
#ifdef PRINTER_VERBOSE
	qDebug()  << "structPrinter::nodeIn(...)" << " pageNr: " << pageCount;
#endif

	if(test)
	{
		int requiredHeight = lineHeight()*(tw->node_n());
		int required = requiredHeight/usablePageHeight()+1;
		(*pages)+=required;
		if(itemsPerPage) *itemsPerPage = (double)(p->pageRect().height()-10)/(double)lineHeight()-2;
		return;
	}


	double dh = p->pageRect().y();
	double dw = p->pageRect().x();

	QRectF bRect;

	int width = p->pageRect().width()/7.;

	headerNodeIn(&dh, dw, width);

	for(int i=startAt; i<tw->node_n(); i++)
	{
		int j=0;
		painter->drawText(QRectF(dw+width*j++, dh, p->pageRect().width(), width),
						  Qt::AlignLeft | Qt::AlignTop, QString(tr("node #%1").arg(i)), &bRect);
		painter->drawText(QRectF(dw+width*j++, dh, p->pageRect().width(), width),
						  Qt::AlignLeft | Qt::AlignTop, QString::number(tw->node(i).x(), 'f', PRECISON), &bRect);
		painter->drawText(QRectF(dw+width*j++, dh, p->pageRect().width(), width),
						  Qt::AlignLeft | Qt::AlignTop, QString::number(tw->node(i).z(), 'f', PRECISON), &bRect);

		if(tw->node(i).hasbearing())
		{
			painter->drawText(QRectF(dw+width*j++, dh, p->pageRect().width(), width),
							  Qt::AlignLeft | Qt::AlignTop, QString::number(tw->node(i).bearing().phi(), 'f', PRECISON), &bRect);

			if(tw->node(i).bearing().x())
				painter->drawText(QRectF(dw+width*j++, dh, p->pageRect().width(), width),
								  Qt::AlignLeft | Qt::AlignTop, tr("locked"), &bRect);
			else if(tw->node(i).bearing().xf())
				painter->drawText(QRectF(dw+width*j++, dh, p->pageRect().width(), width),
								  Qt::AlignLeft | Qt::AlignTop, QString::number(tw->node(i).bearing().c_x()), &bRect);
			else
				painter->drawText(QRectF(dw+width*j++, dh, p->pageRect().width(), width),
								  Qt::AlignLeft | Qt::AlignTop, tr("free"), &bRect);

			if(tw->node(i).bearing().z())
				painter->drawText(QRectF(dw+width*j++, dh, p->pageRect().width(), width),
								  Qt::AlignLeft | Qt::AlignTop, tr("locked"), &bRect);
			else if(tw->node(i).bearing().zf())
				painter->drawText(QRectF(dw+width*j++, dh, p->pageRect().width(), width),
								  Qt::AlignLeft | Qt::AlignTop, QString::number(tw->node(i).bearing().c_z()), &bRect);
			else
				painter->drawText(QRectF(dw+width*j++, dh, p->pageRect().width(), width),
								  Qt::AlignLeft | Qt::AlignTop, tr("free"), &bRect);

			if(tw->node(i).bearing().phi())
				painter->drawText(QRectF(dw+width*j++, dh, p->pageRect().width(), width),
								  Qt::AlignLeft | Qt::AlignTop, tr("locked"), &bRect);
			else if(tw->node(i).bearing().phif())
				painter->drawText(QRectF(dw+width*j++, dh, p->pageRect().width(), width),
								  Qt::AlignLeft | Qt::AlignTop, QString::number(tw->node(i).bearing().c_phi()), &bRect);
			else
				painter->drawText(QRectF(dw+width*j++, dh, p->pageRect().width(), width),
								  Qt::AlignLeft | Qt::AlignTop, tr("free"), &bRect);


		}
		else
		{
			painter->drawText(QRectF(dw+width*j++, dh, p->pageRect().width(), width),
							  Qt::AlignLeft | Qt::AlignTop, QString::number(0, 'f', PRECISON), &bRect);
		}

		if((dh+2*lineHeight() > p->pageRect().bottom()) || i == tw->node_n()-1){
			if(!decoratePage())
				return;
			dh = p->pageRect().top();
			if(i!=tw->node_n()-1)
				headerNodeIn(&dh, dw, width);
		}
		else
			dh += lineHeight();

	}


}


void structPrinter::beamIn(int startAt, int *pages, bool test, int *itemsPerPage)
{
	if(test)
	{
		int requiredHeight = lineHeight()*(tw->beam_n());
		int required = requiredHeight/usablePageHeight()+1;
		requiredHeight = requiredHeight%usablePageHeight();
		(*pages)+=required;
		if(itemsPerPage) *itemsPerPage =  (double)(p->pageRect().height()-10)/(double)lineHeight()-2;
		return;
	}

	double dh = p->pageRect().y();
	double dw = p->pageRect().x();

	QRectF bRect;

	int width = p->pageRect().width()/8.;

	headerBeamIn(&dh, dw, width);

	for(int i=startAt; i<tw->beam_n(); i++)
	{
		int j=0;
		painter->drawText(QRectF(dw+width*j++, dh, p->pageRect().width(), width),
						  Qt::AlignLeft | Qt::AlignTop, QString(tr("beam #%1").arg(i)), &bRect);
		painter->drawText(QRectF(dw+width*j++, dh, p->pageRect().width(), width),
						  Qt::AlignLeft | Qt::AlignTop, QString::number(tw->beam(i).leftNodeI()), &bRect);
		painter->drawText(QRectF(dw+width*j++, dh, p->pageRect().width(), width),
						  Qt::AlignLeft | Qt::AlignTop, QString::number(tw->beam(i).rightNodeI()), &bRect);
		painter->drawText(QRectF(dw+width*j++, dh, p->pageRect().width(), width),
						  Qt::AlignLeft | Qt::AlignTop, tw->beam(i).Mat().name(), &bRect);
		painter->drawText(QRectF(dw+width*j++, dh, p->pageRect().width(), width),
						  Qt::AlignLeft | Qt::AlignTop, tw->beam(i).Profile().name(), &bRect);


		if((dh+2*bRect.height() > p->pageRect().bottom()) || i == tw->beam_n()-1){
			if(!decoratePage())
				return;
			dh = p->pageRect().top();
			if(i!=tw->beam_n()-1)
				headerBeamIn(&dh, dw, width);
		}
		else
			dh += lineHeight();

	}
}

void structPrinter::loadIn(int startAt, int *pages, bool test, int *itemsPerPage)
{
	if(test)
	{
		int requiredHeight = lineHeight()*(tw->load_n());
		int required = requiredHeight/usablePageHeight()+1;
		requiredHeight = requiredHeight%usablePageHeight();
		if(itemsPerPage) *itemsPerPage =  (double)(p->pageRect().height()-10)/(double)lineHeight()-2;
		(*pages)+=required;
		return;
	}


	QRectF bRect;

	double dh = p->pageRect().y();
	double dw = p->pageRect().x();

	int width = p->pageRect().width()/8.;

	headerLoadIn(&dh, dw, width);

	for(int i=startAt; i<tw->load_n(); i++)
	{
		int j=0;
		painter->drawText(QRectF(dw+width*j++, dh, p->pageRect().width(), width),
						  Qt::AlignLeft | Qt::AlignTop, QString(tr("load #%1").arg(i)), &bRect);
		painter->drawText(QRectF(dw+width*j++, dh, p->pageRect().width(), width),
						  Qt::AlignLeft | Qt::AlignTop, QString::number(tw->load(i).at()), &bRect);
		switch(tw->load(i).typ())
		{
		case ColinLoad::nodeLoad:
		case ColinLoad::moment:
		case ColinLoad::doubleLoadLeft:
		case ColinLoad::doubleLoadRight:
			painter->drawText(QRectF(dw+width*j++, dh, p->pageRect().width(), width),
							  Qt::AlignLeft | Qt::AlignTop, QString::number(tw->load(i).Px()*PPREFIX), &bRect);
			painter->drawText(QRectF(dw+width*j++, dh, p->pageRect().width(), width),
							  Qt::AlignLeft | Qt::AlignTop, QString::number(tw->load(i).Pz()*PPREFIX), &bRect);
			painter->drawText(QRectF(dw+width*j++, dh, p->pageRect().width(), width),
							  Qt::AlignLeft | Qt::AlignTop, QString::number(tw->load(i).M()*MPREFIX), &bRect);
			break;
		case ColinLoad::decreasingLinearLoad:
		case ColinLoad::increasingLinearLoad:
		case ColinLoad::uniformlyDistibutedLoad:
			painter->drawText(QRectF(dw+width*j++, dh, p->pageRect().width(), width),
							  Qt::AlignLeft | Qt::AlignTop, QString::number(tw->load(i).Px()*PPREFIX), &bRect);
			painter->drawText(QRectF(dw+width*j++, dh, p->pageRect().width(), width),
							  Qt::AlignLeft | Qt::AlignTop, QString::number(tw->load(i).Pz()*PPREFIX), &bRect);
			painter->drawText(QRectF(dw+width*j++, dh, p->pageRect().width(), width),
							  Qt::AlignLeft | Qt::AlignTop, QString::number(tw->load(i).M()*MPREFIX), &bRect);
			break;
		case ColinLoad::tempChange:
			painter->drawText(QRectF(dw+width*5, dh, p->pageRect().width(), width),
							  Qt::AlignLeft | Qt::AlignTop, QString::number(tw->load(i).Px()), &bRect);
			break;
		case ColinLoad::tempDiffrence:
			painter->drawText(QRectF(dw+width*6, dh, p->pageRect().width(), width),
							  Qt::AlignLeft | Qt::AlignTop, QString::number(tw->load(i).Pz()), &bRect);
			break;
		}

		if(tw->load(i).set()>-1)
			painter->drawText(QRectF(dw+width*7, dh, p->pageRect().width(), width),
							  Qt::AlignLeft | Qt::AlignTop, tw->bls(tw->load(i).set()).name(), &bRect);

		if((dh+2*bRect.height() > p->pageRect().bottom()) || i == tw->load_n()-1){
			if(!decoratePage())
				return;
			dh = p->pageRect().top();
			if(i!=tw->load_n()-1)
				headerLoadIn(&dh, dw, width);
		}
		else
			dh+=lineHeight();

	}

}

void structPrinter::nodeRes(int startAt, int *pages, bool test, int *itemsPerPage)
{
	if(test)
	{
		int requiredHeight = lineHeight()*(qMax(tw->cls_n()+2, 11));
		int perPage = floor(usablePageHeight()/requiredHeight);
		int required = ceil(double(tw->node_n())/double(perPage));
		if(itemsPerPage) *itemsPerPage = perPage;
		(*pages)+=required;
		return;
	}


	double dh = p->pageRect().y();
	double dw = p->pageRect().x();

	QRectF bRect;

	int width = (p->pageRect().width()-lineHeight()*10-40)/6;



	QList<int> cls;
	for(int i=0; i<qMax(1, tw->cls_n()); i++)
	{
		cls << i;
	}
	QFont f = painter->font();
	f.setBold(true);
	f.setPointSize(f.pointSize()*2);
	for(int i=startAt; i<tw->node_n(); i++)
	{
		int beamCount =0;
		painter->save();
		painter->translate(dw-40, dh-40);

		painter->save();
		painter->setFont(f);
		painter->setPen(structPrinter_gray);
		painter->drawText(40, 70, QString(tr("#%1")).arg(i));
		painter->restore();

		detailPainter dP;
		dP.drawNode(painter, *tw, i, cls, QSize(lineHeight()*10+80, lineHeight()*10+80));
		painter->restore();

		double dth = dh;

		int j=0;
		painter->drawText(QRectF(dw+40+lineHeight()*10+width*j++, dh-2, p->pageRect().width(), width),
						  Qt::AlignLeft | Qt::AlignTop, QString(tr("Ax[%1]")).arg(unitSettings::instance().Peh()), &bRect);
		painter->drawText(QRectF(dw+40+lineHeight()*10+width*j++, dh-2, p->pageRect().width(), width),
						  Qt::AlignLeft | Qt::AlignTop, QString(tr("Az[%1]")).arg(unitSettings::instance().Peh()), &bRect);
		painter->drawText(QRectF(dw+40+lineHeight()*10+width*j++, dh-2, p->pageRect().width(), width),
						  Qt::AlignLeft | Qt::AlignTop, QString(tr("M[%1]")).arg(unitSettings::instance().Meh()), &bRect);
		painter->drawText(QRectF(dw+40+lineHeight()*10+width*j++, dh-2, p->pageRect().width(), width),
						  Qt::AlignLeft | Qt::AlignTop, QString(tr("u[m]")), &bRect);
		painter->drawText(QRectF(dw+40+lineHeight()*10+width*j++, dh-2, p->pageRect().width(), width),
						  Qt::AlignLeft | Qt::AlignTop, QString(tr("w[m]")), &bRect);
		painter->drawText(QRectF(dw+40+lineHeight()*10+width*j++, dh-2, p->pageRect().width(), width),
						  Qt::AlignLeft | Qt::AlignTop, QString(tr("phi[grad]")), &bRect);

		dh+=lineHeight();

		if(tw->isCalculated()){
			for(int c=0; c<qMax(1, tw->cls_n()); c++)
			{
				j=0;
				if(tw->node(i).hasbearing()){
					painter->drawText(QRectF(dw+40+lineHeight()*10+width*j++, dh, p->pageRect().width(), width),
									  Qt::AlignLeft | Qt::AlignTop, QString::number(tw->node(i).bearing().a_x(c)*PPREFIX, 'f', PRECISON), &bRect);
					painter->drawText(QRectF(dw+40+lineHeight()*10+width*j++, dh, p->pageRect().width(), width),
									  Qt::AlignLeft | Qt::AlignTop, QString::number(tw->node(i).bearing().a_z(c)*PPREFIX, 'f', PRECISON), &bRect);
					painter->drawText(QRectF(dw+40+lineHeight()*10+width*j++, dh, p->pageRect().width(), width),
									  Qt::AlignLeft | Qt::AlignTop, QString::number(tw->node(i).bearing().a_m(c)*MPREFIX, 'f', PRECISON), &bRect);
				}
				else
					j+=3;
				painter->drawText(QRectF(dw+40+lineHeight()*10+width*j++, dh, p->pageRect().width(), width),
								  Qt::AlignLeft | Qt::AlignTop, QString::number(tw->node(i).u(c)), &bRect);
				painter->drawText(QRectF(dw+40+lineHeight()*10+width*j++, dh, p->pageRect().width(), width),
								  Qt::AlignLeft | Qt::AlignTop, QString::number(tw->node(i).w(c)), &bRect);
				painter->drawText(QRectF(dw+40+lineHeight()*10+width*j++, dh, p->pageRect().width(), width),
								  Qt::AlignLeft | Qt::AlignTop, QString::number(tw->node(i).phi(c)*ANGLEPREFIX), &bRect);
				dh+=lineHeight();
			}
			if(tw->cls_n()==0)
			{
				painter->drawText(QRectF(dw+40+lineHeight()*10+width*0, dh, p->pageRect().width(), width),
								  Qt::AlignLeft | Qt::AlignTop, tr("beam forces:"), &bRect);
				dh+=lineHeight();
				for(int b=0; b<tw->beam_n(); b++)
				{
					if(tw->beam(b).leftNodeI() == i)
					{
						painter->save();
						painter->setPen(viewPortSettings::instance().color(Colin::C_Np));
						painter->drawText(QRectF(dw+40+lineHeight()*10+width*0, dh, p->pageRect().width(), width),
										  Qt::AlignLeft | Qt::AlignTop,
										  QString("N_l,%1 = %2 %3")
										  .arg(b)
										  .arg(tw->beam(b).N(0, 0)*PPREFIX, 0, 'f', PRECISON)
										  .arg(unitSettings::instance().Peh()), &bRect);
						painter->setPen(viewPortSettings::instance().color(Colin::C_Qp));
						painter->drawText(QRectF(dw+40+lineHeight()*10+width*2, dh, p->pageRect().width(), width),
										  Qt::AlignLeft | Qt::AlignTop,
										  QString("Q_l,%1 = %2 %3")
										  .arg(b)
										  .arg(tw->beam(b).Q(0, 0)*PPREFIX, 0, 'f', PRECISON)
										  .arg(unitSettings::instance().Peh()), &bRect);
						painter->setPen(viewPortSettings::instance().color(Colin::C_Mp));
						painter->drawText(QRectF(dw+40+lineHeight()*10+width*4, dh, p->pageRect().width(), width),
										  Qt::AlignLeft | Qt::AlignTop,
										  QString("M_l,%1 = %2 %3")
										  .arg(b)
										  .arg(tw->beam(b).M(0, 0)*MPREFIX, 0, 'f', PRECISON)
										  .arg(unitSettings::instance().Meh()), &bRect);
						beamCount++;
						dh+=lineHeight();
						painter->restore();
					}
					if(tw->beam(b).rightNodeI() == i)
					{
						painter->save();
						painter->setPen(viewPortSettings::instance().color(Colin::C_Np));
						painter->drawText(QRectF(dw+40+lineHeight()*10+width*0, dh, p->pageRect().width(), width),
										  Qt::AlignLeft | Qt::AlignTop,
										  QString("N_r,%1 = %2 %3")
										  .arg(b)
										  .arg(tw->beam(b).N(0, tw->beam(b).l())*PPREFIX, 0, 'f', PRECISON)
										  .arg(unitSettings::instance().Peh()), &bRect);
						painter->setPen(viewPortSettings::instance().color(Colin::C_Qp));
						painter->drawText(QRectF(dw+40+lineHeight()*10+width*2, dh, p->pageRect().width(), width),
										  Qt::AlignLeft | Qt::AlignTop,
										  QString("Q_r,%1 = %2 %3")
										  .arg(b)
										  .arg(tw->beam(b).N(0, tw->beam(b).l())*PPREFIX, 0, 'f', PRECISON)
										  .arg(unitSettings::instance().Peh()), &bRect);
						painter->setPen(viewPortSettings::instance().color(Colin::C_Mp));
						painter->drawText(QRectF(dw+40+lineHeight()*10+width*4, dh, p->pageRect().width(), width),
										  Qt::AlignLeft | Qt::AlignTop,
										  QString("M_r,%1 = %2 %3")
										  .arg(b)
										  .arg(tw->beam(b).M(0, tw->beam(b).l())*MPREFIX, 0, 'f', PRECISON)
										  .arg(unitSettings::instance().Meh()), &bRect);
						beamCount++;
						dh+=lineHeight();
						painter->restore();
					}
				}
			}
			//if(tw->cls_n()<10)
			//{
			//	dh+=lineHeight()*(10-tw->cls_n());
			//}
		}
		//else{
		//	dh+=lineHeight()*qMax(10, tw->cls_n());
		//}
		dh = dth;

		dh +=lineHeight()*qMax(11, tw->cls_n());
		painter->drawLine(dw, dh+2-lineHeight(), dw+p->pageRect().width(), dh+2-lineHeight());

		if(dh+lineHeight()*qMax(11, tw->cls_n())>p->pageRect().bottom() || i == tw->node_n()-1){
			if(!decoratePage())
				return;
			dh = p->pageRect().top();
		}
	}


}

void structPrinter::beamResF(int startAt, int *pages, bool test, int *itemsPerPage)
{
	int resultCount = qMax(1, tw->cls_n());
	if(test)
	{
		int requiredLines = tw->beam_n()*(6*resultCount+6);
		int required = ceil(double(requiredLines*lineHeight())/double(usablePageHeight()));
		(*pages)+=required;
		if(itemsPerPage) *itemsPerPage = floor(usablePageHeight()*(resultCount)/(resultCount+1)/lineHeight());
		return;
	}

	dx = p->pageRect().y();
	double dw = p->pageRect().x();


	int width = (p->pageRect().width()-lineHeight()*10-40)/6;

	QFont f = painter->font();
	f.setBold(true);
	f.setPointSize(f.pointSize()*2);

/*
  startAt:  beam		-> 0-beam_n
			function	-> 0-5
			cls			-> 0-max(cls_n, 1)

*/

	int startAtBeam = floor(startAt/(6.*resultCount));
	int startAtFunction = floor(startAt-startAtBeam*(6*resultCount))/resultCount;
	int startAtClS = startAt-startAtBeam*(6*resultCount)-startAtFunction*(resultCount);

	for(int i=startAtBeam; i<tw->beam_n(); i++)
	{
		painter->save();


		painter->translate(dw, dx+lineHeight()*5);

		painter->save();
		painter->setFont(f);
		painter->setPen(structPrinter_gray);
		painter->drawText(0, -lineHeight()*5+30, QString(tr("#%1")).arg(i));
		painter->restore();

		painter->drawLine(0, 0, lineHeight()*10, 0);
		structPainter sP;

		painter->setRenderHint(QPainter::Antialiasing, true);

		QTransform tran = QTransform(double(lineHeight()*10)/tw->beam(i).l(), 0, 0,
									 0, double(lineHeight()*10)/tw->beam(i).l(), 0,
									 0, 0, 1);
		sP.trm = &tran;


		bool keepWriting;
		if(tw->isCalculated())
		{

			/*draw function on the left side of paper*/


			QList<const function*> funs;

			for(int j=0; j<qMax(1, tw->cls_n()); j++)
				funs << &tw->beam(i).Nconst(j);
			painter->setPen(viewPortSettings::instance().color(Colin::C_Np));
			sP.drawFunction(painter, funs, tw->beam(i).l(), tw->scaleP()/2, 0);

			funs.clear();

			for(int j=0; j<qMax(1, tw->cls_n()); j++)
				funs << &tw->beam(i).Qconst(j);
			painter->setPen(viewPortSettings::instance().color(Colin::C_Qp));
			sP.drawFunction(painter, funs, tw->beam(i).l(), tw->scaleP()/2, 0);

			funs.clear();

			for(int j=0; j<qMax(1, tw->cls_n()); j++)
				funs << &tw->beam(i).Mconst(j);
			painter->setPen(viewPortSettings::instance().color(Colin::C_Mp));
			sP.drawFunction(painter, funs, tw->beam(i).l(), tw->scaleM()/2, 0);

			painter->restore();


			/*write functions on the right side of paper*/

			painter->setPen(Qt::black);
			for(int j=0; j<qMax(1, tw->cls_n()); j++){
				painter->drawText(QRectF(dw+40+lineHeight()*10, dx, p->pageRect().width(), width),
								  Qt::AlignLeft | Qt::AlignTop,
								  QString(tr("u(x)[m] = %1")
										  .arg(tw->beam(i).uconst(j).toString(1, 'g', PRECISON))));
				this->needLine(1, &keepWriting); if(!keepWriting) return;;
			}
			dx+=lineHeight();
			for(int j=0; j<qMax(1, tw->cls_n()); j++){
				painter->drawText(QRectF(dw+40+lineHeight()*10, dx, p->pageRect().width(), width),
								  Qt::AlignLeft | Qt::AlignTop,
								  QString(tr("w(x)[m] = %1")
										  .arg(tw->beam(i).wconst(j).toString(1, 'g', PRECISON))));
				this->needLine(1, &keepWriting); if(!keepWriting) return;;
			}
			dx+=lineHeight();
			for(int j=0; j<qMax(1, tw->cls_n()); j++){
				painter->drawText(QRectF(dw+40+lineHeight()*10, dx, p->pageRect().width(), width),
								  Qt::AlignLeft | Qt::AlignTop,
								  tr("phi(x)[grad] = %1")
								  .arg(tw->beam(i).phiconst(j).toString(ANGLEPREFIX, 'g', PRECISON)));
				this->needLine(1, &keepWriting); if(!keepWriting) return;;
			}
			dx+=lineHeight();
			painter->setPen(viewPortSettings::instance().color(Colin::C_Np));
			for(int j=0; j<qMax(1, tw->cls_n()); j++){
				painter->drawText(QRectF(dw+40+lineHeight()*10, dx, p->pageRect().width(), width),
								  Qt::AlignLeft | Qt::AlignTop,
								  tr("N(x)[%1] = %2")
								  .arg(unitSettings::instance().Peh())
								  .arg(tw->beam(i).Nconst(j).toString(PPREFIX, 'f', PRECISON)));
				this->needLine(1, &keepWriting); if(!keepWriting) return;;
			}
			dx+=lineHeight();
			painter->setPen(viewPortSettings::instance().color(Colin::C_Qp));
			for(int j=0; j<qMax(1, tw->cls_n()); j++){
				painter->drawText(QRectF(dw+40+lineHeight()*10, dx, p->pageRect().width(), width),
								  Qt::AlignLeft | Qt::AlignTop,
								  tr("Q(x)[%1] = %2")
								  .arg(unitSettings::instance().Peh())
								  .arg(tw->beam(i).Qconst(j).toString(PPREFIX, 'f', PRECISON)));
				this->needLine(1, &keepWriting); if(!keepWriting) return;;
			}
			dx+=lineHeight();
			painter->setPen(viewPortSettings::instance().color(Colin::C_Mp));
			for(int j=0; j<qMax(1, tw->cls_n()); j++){
				painter->drawText(QRectF(dw+40+lineHeight()*10, dx, p->pageRect().width(), width),
								  Qt::AlignLeft | Qt::AlignTop,
								  tr("M(x)[%1] = %2")
								  .arg(unitSettings::instance().Meh())
								  .arg(tw->beam(i).Mconst(j).toString(MPREFIX, 'f', PRECISON)));
				this->needLine(1, &keepWriting); if(!keepWriting) return;;
			}
			this->needLine(1, &keepWriting); if(!keepWriting) return;;


		}
		else
			painter->restore();
		painter->setPen(Qt::black);
		painter->drawLine(dw, dx-lineHeight()/2, p->pageRect().width(), dx-lineHeight()/2);
	}
}

//void structPrinter::beamResVal(int startAt, int *pages, bool test, int *itemsPerPage)
//{
//	if(test)
//	{
//		int requiredHeight = lineHeight()*(8*tw->beam_n()*tw->cls_n()+titleSize);
//		int required = requiredHeight/usablePageHeight()+1;
//		requiredHeight = requiredHeight%usablePageHeight();
//		(*pages)+=required;
//		return;
//	}
//}

void structPrinter::printStruct(const QRect &rect, Colin::Elements e, QList<int> cls)
{
	printStruct(rect, tw->boundingRect(), e, cls);
}

void structPrinter::printStructBLS(const QRect &rect, Colin::Elements e, QList<int> bls)
{
	printStructBLS(rect, tw->boundingRect(), e, bls);
}

void structPrinter::printStruct(const QRect &rect, const QRectF &tRect, Colin::Elements e, QList<int> cls)
{
	painter->save();
	QTransform transform = getScale(QRectF(rect), tRect);
	structPainter sP;
	painter->setClipRect(rect);
	sP.ignoreHotSpots(true);
	sP.setCLS(cls);
	sP.setBLS(QList<int>());
	sP.drawStruct(*filelist::instance().currentFile(), painter, &transform, e);
	painter->restore();
}

void structPrinter::printStructBLS(const QRect &rect, const QRectF &tRect, Colin::Elements e, QList<int> bls)
{
	painter->save();
	QTransform transform = getScale(QRectF(rect), tRect);
	structPainter sP;
	painter->setClipRect(rect);
	sP.setBLS(bls);
	sP.setCLS(QList<int>());
	sP.ignoreHotSpots(true);
	sP.drawStruct(*filelist::instance().currentFile(), painter, &transform, e);
	painter->restore();
}

QTransform structPrinter::getScale(const QRectF &rect, const QRectF &boundingRect){

	QTransform t;
	t.translate(rect.x(), rect.y());
	QSizeF size = rect.size();
	double scale;
	scale = qMin((size.width()-60)/boundingRect.width(), (size.height()-60)/boundingRect.height());


	t.translate(-boundingRect.x()*scale+size.width()/2.-boundingRect.width()*scale/2.,
				-boundingRect.y()*scale+size.height()/2.-boundingRect.height()*scale/2.);
	t.scale(scale, scale);

	return t;
}
