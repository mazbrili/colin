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


#include <QtCore/QDate>

const int titleSize = 3;
const int alignLeft = 3;


structPrinter::structPrinter(ColinStruct *t, QPrinter *printer, const painterContent &content)
{
	this->tw = t;
	this->c = content;
	this->p = printer;
	painter = 0;
	device = 0;
}

void structPrinter::print(QPaintDevice *target, int pageNr)
{
	qDebug()  << "structPrinter::print(...)";
	this->device = target;
	if(!painter)
		painter = new QPainter(target);
	painter->scale(double(target->width())/double(p->paperRect(QPrinter::DevicePixel).width()),
				   double(target->height())/double(p->paperRect(QPrinter::DevicePixel).height()));

	//qDebug() << "painter matrix = " << painter->transform();

	int indizes[10];
	totalPageCount = requiredPages(p, c, indizes);



	if(pageNr==-1)
	{pageCount=0;
		tableContent(indizes);
		if(c.s.testFlag(painterContent::bls_in))
			blsPlot(&pageCount);
		if(c.s.testFlag(painterContent::cls_single))
			clsPlot(&pageCount);
		if(c.s.testFlag(painterContent::cls_all))
			clsPlotall(&pageCount);
		if(c.s.testFlag(painterContent::node_in))
			nodeIn(&pageCount);
		if(c.s.testFlag(painterContent::beam_in))
			beamIn(&pageCount);
		if(c.s.testFlag(painterContent::load_in))
			loadIn(&pageCount);
		if(c.s.testFlag(painterContent::node_res))
			nodeRes(&pageCount);
		if(c.s.testFlag(painterContent::beam_f))
			beamResF(&pageCount);
		if(c.s.testFlag(painterContent::beam_val))
			beamResVal(&pageCount);
	}
	else
	{
		tableContent(indizes);
	}
	this->device=0;

	delete painter;
	painter=0;
}

painterContent structPrinter::getContentOfPage(QPrinter *printer, painterContent content, int pageNr)
{

}

int structPrinter::getPageOfContent(QPrinter *printer, painterContent all, painterContent content)
{

}

int structPrinter::requiredPages(QPrinter *printer, painterContent content, int* indizes)
{
	qDebug()  << "structPrinter::requiredPages(...)";
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
		blsPlot(&pages, true);
	}
	if(content.s.testFlag(painterContent::cls_single)){
		if(indizes)
			indizes[1]=pages;
		clsPlot(&pages, true);
	}
	if(content.s.testFlag(painterContent::cls_all)){
		if(indizes)
			indizes[2]=pages;
		clsPlotall(&pages, true);
	}
	if(content.s.testFlag(painterContent::node_in)){
		if(indizes)
			indizes[3]=pages;
		nodeIn(&pages, true);
	}
	if(content.s.testFlag(painterContent::beam_in)){
		if(indizes)
			indizes[4]=pages;
		beamIn(&pages, true);
	}
	if(content.s.testFlag(painterContent::load_in)){
		if(indizes)
			indizes[5]=pages;
		loadIn(&pages, true);
	}
	if(content.s.testFlag(painterContent::node_res)){
		if(indizes)
			indizes[6]=pages;
		nodeRes(&pages, true);
	}
	if(content.s.testFlag(painterContent::beam_f)){
		if(indizes)
			indizes[7]=pages;
		beamResF(&pages, true);
	}
	if(content.s.testFlag(painterContent::beam_val)){
		if(indizes)
			indizes[8]=pages;
		beamResVal(&pages, true);
	}
	if(createdPainter){
		delete painter;
		painter = 0;
	}
	return pages;
}

void structPrinter::tableContent(int *indizes)
{
	qDebug()  << "structPrinter::tableContent(...)";
	QRectF bR;
	decoratePage();

	int i=1;

	QFont big = c.font;
	if(big.pointSize()>0)
		big.setPointSize(big.pointSize()*1.0);
	else
		big.setPixelSize(big.pixelSize()*1.0);
	painter->setFont(big);

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
}

void structPrinter::decoratePage()
{

	qDebug()  << "structPrinter::decoratePage(...)" << " pageNr: " << pageCount;
	QRect pr = p->pageRect(QPrinter::DevicePixel).toRect();
	QRect bR;

	dx = pr.top();

	painter->setOpacity(0.3);
	painter->drawPixmap(pr.left(), pr.bottom()-64, 64, 64, QPixmap(colinIcons::icondir_+"tooltip/node_dark_64.png"));
	painter->drawPixmap(pr.right()-64, pr.bottom()-64, 64, 64, QPixmap(colinIcons::icondir_+"clazzes-logo-alpha_64x64.png"));
	painter->setOpacity(1);
	painter->drawText(QRect(pr.bottomLeft()-QPoint(0, 20), QSize(100, 20)),
					  Qt::AlignBottom | Qt::AlignLeft,
					  "Colin - clazzes.org");
	if(pageCount)
	{
		painter->drawText(QRect(pr.bottomRight()-QPoint(100, 20), QSize(100, 20)),
						  Qt::AlignBottom | Qt::AlignRight,
						  QString("%1/%2").arg(pageCount).arg(totalPageCount), &bR);
	}
	else
	{
		painter->drawText(QRect(pr.bottomRight()-QPoint(100, 20), QSize(100, 20)),
						  Qt::AlignBottom | Qt::AlignRight,
						  QString(tr("contents")), &bR);
	}
	painter->drawLine(QPoint(pr.left(), pr.bottom()-painter->fontMetrics().height()-2),
					  QPoint(pr.right(), pr.bottom()-painter->fontMetrics().height()-2));

	pageCount++;
}

int structPrinter::usablePageHeight()
{
	return p->pageRect(QPrinter::DevicePixel).toRect().height()-lineHeight();
}

int structPrinter::lineHeight()
{
	return painter->fontMetrics().height()*1.5;
}

int structPrinter::needLine(int i)
{
	dx += i*lineHeight();
	return dx;
}

int structPrinter::blsPlot(int *pages, bool test)
{
	qDebug()  << "structPrinter::blsPlot(...)" << " pageNr: " << pageCount;
	if(test)
	{
		int required;
		if(c.s.testFlag(painterContent::fourPerPage))
			required = tw->bls_n()/4 + (required%4)?1:0;
		else
			required = tw->bls_n();
		*pages+=required;
		return usablePageHeight();
	}

	QRect rect = p->pageRect();
	rect.setHeight(usablePageHeight());

	if(c.s.testFlag(painterContent::fourPerPage))
	{
		rect.setWidth(rect.width()/2);
		rect.setHeight(rect.height()/2);
		rect.translate(rect.width(), rect.height());
	}
	for(int i=0; i<tw->bls_n(); i++)
	{
		if(!(i%4) || !c.s.testFlag(painterContent::fourPerPage)){
			*pages++;
			p->newPage();
			decoratePage();
			if(c.s.testFlag(painterContent::fourPerPage)){
				rect.translate(-rect.width(), -rect.height());
				painter->drawLine(rect.topRight(), rect.bottomRight()+QPointF(0, rect.height()));
				painter->drawLine(rect.bottomLeft(), rect.bottomRight()+QPointF(rect.width(), 0));
			}
		}
		else if(!(i%2))
			rect.translate(-rect.width(), rect.height());
		else
			rect.translate(rect.width(), 0);

		painter->drawText(rect.adjusted(10, 0, 0, 0), Qt::AlignTop | Qt::AlignLeft, tw->bls(i).name());
		QList<int> bls;
		bls << i;
		printStructBLS(rect,
					   Colin::nload|
					   Colin::sload|
					   Colin::beam|
					   Colin::bearing|
					   Colin::joint,
					   bls);

	}
}


int structPrinter::clsPlot(int *pages, bool test)
{
	qDebug()  << "structPrinter::clsPlot(...)" << " pageNr: " << pageCount;

	if(test)
	{
		int required;
		if(c.s.testFlag(painterContent::fourPerPage))
			required = tw->cls_n();
		else
			required = tw->cls_n()*4;
		*pages+=required;
		return usablePageHeight();
	}


	QRect rect = p->pageRect();
	rect.setHeight(usablePageHeight());
	if(c.s.testFlag(painterContent::fourPerPage))
	{
		rect.setWidth(rect.width()/2);
		rect.setHeight(rect.height()/2);
		rect.translate(rect.width(), rect.height());
	}
	for(int m=0; m<tw->cls_n(); m++)
	{
		for(int j=0; j<4; j++)
		{
			int i = m*4+j;
			Colin::Elements toDraw;
			if(j==0)				toDraw = Colin::nload | Colin::u;
			if(j==1)				toDraw = Colin::N;
			if(j==2)				toDraw = Colin::Q;
			if(j==3)				toDraw = Colin::M;

			if(!(i%4) || !c.s.testFlag(painterContent::fourPerPage)){
				*pages++;
				p->newPage();
				decoratePage();
				if(c.s.testFlag(painterContent::fourPerPage)){
					rect.translate(-rect.width(), -rect.height());
					painter->drawLine(rect.topRight(), rect.bottomRight()+QPointF(0, rect.height()));
					painter->drawLine(rect.bottomLeft(), rect.bottomRight()+QPointF(rect.width(), 0));
				}
			}
			else if(!(i%2))
				rect.translate(-rect.width(), rect.height());
			else
				rect.translate(rect.width(), 0);

			painter->drawText(rect.adjusted(10, 0, 0, 0), Qt::AlignTop | Qt::AlignLeft, tw->cls(m).name());
			QList<int> cls;
			cls << m;
			printStruct(rect,
						 Colin::beam    |
						 Colin::bearing |
						 Colin::joint   |
						 toDraw,
						 cls);
		}
	}
}

int structPrinter::clsPlotall(int *pages, bool test)
{
	qDebug()  << "structPrinter::clsPlotall(...)" << " pageNr: " << pageCount;

	if(test)
	{
		int required;
		if(c.s.testFlag(painterContent::fourPerPage))
			required = 1;
		else
			required = 4;
		*pages+=required;
		return usablePageHeight();
	}

	QRect rect = p->pageRect();
	rect.setHeight(usablePageHeight());
	if(c.s.testFlag(painterContent::fourPerPage))
	{
		rect.setWidth(rect.width()/2);
		rect.setHeight(rect.height()/2);
		rect.translate(rect.width(), rect.height());
	}
	for(int j=0; j<4; j++)
	{
		int i = j;
		Colin::Elements toDraw;
		if(j==0)				toDraw = Colin::nload | Colin::u;
		if(j==1)				toDraw = Colin::N;
		if(j==2)				toDraw = Colin::Q;
		if(j==3)				toDraw = Colin::M;

		if(!(i%4) || !c.s.testFlag(painterContent::fourPerPage)){
			*pages++;
			p->newPage();
			decoratePage();
			if(c.s.testFlag(painterContent::fourPerPage)){
				rect.translate(-rect.width(), -rect.height());
				painter->drawLine(rect.topRight(), rect.bottomRight()+QPointF(0, rect.height()));
				painter->drawLine(rect.bottomLeft(), rect.bottomRight()+QPointF(rect.width(), 0));
			}
		}
		else if(!(i%2))
			rect.translate(-rect.width(), rect.height());
		else
			rect.translate(rect.width(), 0);

		QList<int> cls;
		for(int m=0; m<tw->cls_n(); m++) cls << m;
		printStruct(rect,
					 Colin::beam    |
					 Colin::bearing |
					 Colin::joint   |
					 toDraw,
					 cls);
	}
}

int structPrinter::nodeIn(int *pages, bool test)
{
	qDebug()  << "structPrinter::nodeIn(...)" << " pageNr: " << pageCount;

	if(test)
	{
		int requiredHeight = lineHeight()*(tw->node_n()+titleSize);
		int required = requiredHeight/usablePageHeight()+1;
		requiredHeight = requiredHeight%usablePageHeight();
		*pages+=required;
		return requiredHeight;
	}
}


int structPrinter::beamIn(int *pages, bool test)
{
	if(test)
	{
		int requiredHeight = lineHeight()*(tw->beam_n()+titleSize);
		int required = requiredHeight/usablePageHeight()+1;
		requiredHeight = requiredHeight%usablePageHeight();
		*pages+=required;
		return requiredHeight;
	}
}

int structPrinter::loadIn(int *pages, bool test)
{
	if(test)
	{
		int requiredHeight = lineHeight()*(tw->load_n()+tw->bls_n()+tw->cls_n()+titleSize*3);
		int required = requiredHeight/usablePageHeight()+1;
		requiredHeight = requiredHeight%usablePageHeight();
		*pages+=required;
		return requiredHeight;
	}
}

int structPrinter::nodeRes(int *pages, bool test)
{
	if(test)
	{
		int requiredHeight = lineHeight()*(2*tw->node_n()*tw->cls_n()+titleSize);
		int required = requiredHeight/usablePageHeight()+1;
		requiredHeight = requiredHeight%usablePageHeight();
		*pages+=required;
		return requiredHeight;
	}
}

int structPrinter::beamResF(int *pages, bool test)
{
	if(test)
	{
		int requiredHeight = lineHeight()*(8*tw->beam_n()*tw->cls_n()+titleSize);
		int required = requiredHeight/usablePageHeight()+1;
		requiredHeight = requiredHeight%usablePageHeight();
		*pages+=required;
		return requiredHeight;
	}
}

int structPrinter::beamResVal(int *pages, bool test)
{
	if(test)
	{
		int requiredHeight = lineHeight()*(8*tw->beam_n()*tw->cls_n()+titleSize);
		int required = requiredHeight/usablePageHeight()+1;
		requiredHeight = requiredHeight%usablePageHeight();
		*pages+=required;
		return requiredHeight;
	}
}

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
