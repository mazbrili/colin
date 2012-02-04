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

#include "pastepreviewwidget.h"

#include <QtGui/QPainter>

#include "colinicons.h"
#include "colinpushbuttonpart.h"
#include "colinhmultibutton.h"
#include "colinpastebuffer.h"
#include "viewport.h"


pastePreviewWidget::pastePreviewWidget(int nr, QWidget *parent) :
	QWidget(parent)
{

	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	QSizePolicy sPolicy = this->sizePolicy();
	sPolicy.setHeightForWidth(true);
	setSizePolicy(sPolicy);


	removeButton = new QPushButton(QIcon(colinIcons::instance().icon(Colin::Close).pixmap(12, QIcon::Normal, QIcon::On)), "", this);
	removeButton->setFlat(true);
	removeButton->setFixedSize(12, 12);
	removeButton->move(width()-16, 4);

	index = nr;
	image = QImage(QSize(1, 1), QImage::Format_RGB32);


	connect(removeButton,								SIGNAL(pressed()),
			this,										SLOT(remove()));

	connect(&ColinPasteBuffer::instance(),				SIGNAL(finishedRendering(QImage,int)),
			this,										SLOT(giefPix(QImage, int)));

}

int pastePreviewWidget::heightForWidth(int w) const
{
	return 0.75*w;
}

void pastePreviewWidget::mousePressEvent(QMouseEvent *e)
{
	if(e->buttons().testFlag(Qt::LeftButton))
		paste();
}

void pastePreviewWidget::resizeEvent(QResizeEvent *e)
{
	removeButton->move(width()-16, 4);
	QWidget::resizeEvent(e);
}

void pastePreviewWidget::paintEvent(QPaintEvent *e)
{
	QPainter p(this);
	if(image.size()!=this->size())
	{
		ColinPasteBuffer::instance().renderPreview(size(), index);
		p.drawText(rect(), tr("rendering in process..."), Qt::AlignVCenter | Qt::AlignHCenter);
	}
	else
	{
		p.setBrush(QColor(255, 255, 255, 100));
		p.setPen(palette().color(QPalette::Dark));
		p.drawRoundedRect(QRect(0, 0, width()-1, height()-1), 4, 4);
		p.drawImage(0, 0, image);
	}
}


void pastePreviewWidget::giefPix(QImage image, int nr)
{
	if(nr == index && image.size() == size())
	{
		this->image = image;
		update();
	}
}

void pastePreviewWidget::paste()
{
	ColinStruct *tw = new ColinStruct();

	ColinPasteBuffer::instance().copy(index, tw);

	if(tw->node_n()==0 && tw->load_n()>0)
	{
		if(tw->load(0).typ() == ColinLoad::nodeLoad ||
		   tw->load(0).typ() == ColinLoad::moment   )
			viewPortSettings::instance().setClipBoard(Colin::pasteLoadRequestNode);
		else
			viewPortSettings::instance().setClipBoard(Colin::pasteLoadRequestBeam);
		viewport::setToPaste(tw);
	}
	else if(tw->node_n()==0)
	{
		viewPortSettings::instance().setClipBoard(Colin::noRequest);
		delete tw;
	}
	else
	{
		viewPortSettings::instance().setClipBoard(Colin::pasteRequest);
		viewport::setToPaste(tw);
	}

	emit pasteRequest();
}

void pastePreviewWidget::remove()
{
	ColinPasteBuffer::instance().remove(index);
}
