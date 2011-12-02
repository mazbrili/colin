#include "viewportlayout.h"
#include <math.h>
#include <QtGui/QWidget>
#include <QtCore/QDebug>

ViewPortLayout::ViewPortLayout(QWidget *parent) :
    QLayout(parent)
{

}

ViewPortLayout::~ViewPortLayout()
{
	QLayoutItem *item;
	while ((item = takeAt(0)))
		delete item;
}

void ViewPortLayout::addItem(QLayoutItem *item)
{
	itemList.append(item);
}

int ViewPortLayout::count() const
{
	return itemList.size();
}

QLayoutItem *ViewPortLayout::itemAt(int index) const
{
	return itemList.value(index);
}

QLayoutItem *ViewPortLayout::takeAt(int index)
{
	if (index >= 0 && index < itemList.size())
		return itemList.takeAt(index);
	else
		return 0;
}

Qt::Orientations ViewPortLayout::expandingDirections() const
{
	return 0;
}

bool ViewPortLayout::hasHeightForWidth() const
{
	return false;
}

int ViewPortLayout::heightForWidth(int width) const
{
	return 0;
}

void ViewPortLayout::setGeometry(const QRect &rect)
{
	QLayout::setGeometry(rect);
	doLayout(rect);
}

QSize ViewPortLayout::sizeHint() const
{
	return minimumSize();
}

QSize ViewPortLayout::minimumSize() const
{
	QSize size;
	QLayoutItem *item;
	foreach (item, itemList)
		size = size.expandedTo(item->minimumSize());

	size += QSize(2*margin(), 2*margin());
	return size;
}

void ViewPortLayout::doLayout(const QRect &rect) const
{
//	qDebug() << "----------------";
//	qDebug() << "doing Layout";
	int left, top, right, bottom;
	getContentsMargins(&left, &top, &right, &bottom);
	QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
	int x = effectiveRect.x();

	int rowCount = sqrt(itemList.size());
	if(rowCount*rowCount< itemList.size())
		rowCount++;

	/*
		------j----->
	  |     line
	  |  r
	  i  o
	  |  w
	  v

	  i = row -> height[i] / verticalStrech[i]
	  j = line -> width[i] / horizontalStrech[j]
	  */

	int verticalPxlWish[rowCount];
	int horizontalPxlWish[rowCount];
	int verticalPxlWishHelper[rowCount];
	for(int i=0; i<rowCount; i++)
	{
		verticalPxlWish[i]=0;
		horizontalPxlWish[i]=0;
	}

	for(int j=0; j<rowCount; j++)
	{
		for(int i=0; i<rowCount; i++)
		{
			if(j+i*rowCount>=itemList.size())
				break;
			if(!itemList.at(j+i*rowCount)->isEmpty()){
			//	verticalPxlWish[j]+=itemList.at(j+i*rowCount)->widget()->sizePolicy().verticalStretch();
				horizontalPxlWish[i]+=itemList.at(j+i*rowCount)->widget()->sizePolicy().horizontalStretch();
			}
			//else{
			int vpw=0;
			for(int m=0; m<rowCount; m++){
				if(!itemList.at(m+i*rowCount)->isEmpty())
					vpw=qMax(vpw, itemList.at(m+i*rowCount)->widget()->sizePolicy().verticalStretch());
			}
			verticalPxlWishHelper[i] = vpw;
			verticalPxlWish[j]+=vpw;
			//}
		}
	}
	double verticalPxlPerWish[rowCount];
	double horizontalPxlPerWish[rowCount];
	for(int i=0; i<rowCount; i++)
	{
		if(verticalPxlWish[i])
			verticalPxlPerWish[i]=effectiveRect.height()/double(verticalPxlWish[i]);
		else
			verticalPxlPerWish[i]=0;
		if(horizontalPxlWish[i])
			horizontalPxlPerWish[i]=effectiveRect.width()/double(horizontalPxlWish[i]);
		else
			horizontalPxlPerWish[i]=0;
	}


//	for(int j=0; j<rowCount; j++)
//	{
//		qDebug( )<< "row " << j << ")vertical stretch " << verticalPxlWish[j] << "    vertical pxl per strech " << verticalPxlPerWish[j];
//	}
//
//	for(int j=0; j<rowCount; j++)
//	{
//		qDebug() << "line " << j << ")horizontal stretch " << horizontalPxlWish[j] << "    horizontal pxl per strech " << horizontalPxlPerWish[j];
//	}


	/*
		------j----->
	  |     line
	  |  r
	  i  o
	  |  w
	  v

	  i = row -> height[i] / verticalStrech[i]
	  j = line -> width[i] / horizontalStrech[j]
	  */

	QRect childRect;
	int y[rowCount];
	for(int j=0; j<rowCount; j++)
		y[j]=effectiveRect.y();
	for(int i=0; i<rowCount; i++)
	{
		x = effectiveRect.x();
		for(int j=0; j<rowCount; j++)
		{
			if(j+i*rowCount>=itemList.size())
				break;
			if(!itemList.at(j+i*rowCount)->isEmpty()){
				double h, w;
				if(i==rowCount-1)
					h = effectiveRect.height()-y[j];
				else
					h = double(verticalPxlWishHelper[i])*verticalPxlPerWish[j];
				if(j==rowCount-1)
					w = effectiveRect.width()-x;
				else
					w = double(itemList.at(j+i*rowCount)->widget()->sizePolicy().horizontalStretch())*horizontalPxlPerWish[i];
				childRect = QRect(x, y[j], w, h);
				itemList[j+i*rowCount]->setGeometry(childRect.adjusted(j?1:0, i?1:0, 0, 0));
//				qDebug() << "rect(row " << i << "/line" << j << ") = " << childRect;
				x=childRect.right();
				y[j]=childRect.bottom();
			}
			else{
				y[j]=-1;
//				qDebug() << "rect(row " << i << "/line" << j << ") = hidden";
			}
		}
		int y_max = 0;
		for(int m=0; m<rowCount; m++)
			y_max = qMax(y_max, y[m]);
		for(int m=0; m<rowCount; m++)
			y[m] = y_max;
	}
}
