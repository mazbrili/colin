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

#include "colinpastebuffer.h"

#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtCore/QDebug>
#include <QtCore/QMimeData>
#include <QtCore/QMutexLocker>
#include <QtCore/QThread>

#include "xmlreader.h"
#include "structpainter.h"

ColinPasteBuffer *ColinPasteBuffer::_instance = 0;
ColinPasteBuffer::ColinPasteBuffer() :
	QThread()
{
	qDebug() << "init PasteBuffer";
	board = QApplication::clipboard();
	killYourSelf = false;


	connect(board,					SIGNAL(changed(QClipboard::Mode)),
			this,					SLOT(changed(QClipboard::Mode)));

}

ColinPasteBuffer::~ColinPasteBuffer()
{
	lockStructs.lock();
	killYourSelf = true;
	waitCondition.wakeOne();
	lockStructs.unlock();
	wait();
}

void ColinPasteBuffer::changed(QClipboard::Mode mode)
{
	QMutexLocker lock(&lockStructs);
	qDebug() << "pasteBuffer changed!";
	if(mode != QClipboard::QClipboard::Clipboard){
		qDebug() << "not in clipboard mode";
		return;
	}
	if(board->mimeData()->data("text/colinfile").isEmpty()){
		qDebug() << "no colinfile";
		return;
	}

	ColinStruct *tw = new ColinStruct();
	tw->setParent(this);
	XmlReader reader(tw);
	if(!reader.read(board->mimeData()->data("text/colinfile"))
)
	{
		qDebug() << "no valid colinfile!";
		delete tw;
		return;
	}
	structs.push_front(tw);
	if(structs.size()>10)
		delete structs.takeLast();
	emit changedBuffer();
	qDebug() << "readed file from clipboard";
	qDebug() << tw->node_n() << " nodes";
	qDebug() << tw->beam_n() << " beams";
	qDebug() << tw->load_n() << " loads";

}


int ColinPasteBuffer::size() const
{
	QMutexLocker lock(&lockStructs);
	return structs.size();
}

ColinStruct *ColinPasteBuffer::structAt(int i)
{
	QMutexLocker lock(&lockStructs);
	return structs.at(i);
}

void ColinPasteBuffer::remove(int i)
{
	QMutexLocker lock(&lockStructs);
	delete structs.takeAt(i);
	emit changedBuffer();
}

void ColinPasteBuffer::copy(int i, ColinStruct *target)
{
	QMutexLocker lock(&lockStructs);
	target->copy(structs.at(i));
}

void ColinPasteBuffer::renderPreview(QSize size, int i)
{
	QMutexLocker lock(&lockStructs);
	qDebug() << "ColinPasteBuffer::render struct " << i;
	this->size_ = size;
	this->toDo_.append(i);
	lock.unlock();
	this->waitCondition.wakeAll();
}

void ColinPasteBuffer::run()
{
	int current;
	QSize size;
	qDebug() << "ColinPasteBuffer::run";

	forever
	{
		QMutexLocker lock(&lockStructs);
		if(toDo_.isEmpty())
			waitCondition.wait(&lockStructs);
		if(killYourSelf)
			return;
		current = toDo_.first();
		toDo_.removeFirst();
		size = size_;
		qDebug() << "ColinPasteBuffer::rendering struct" << current;


		ColinStruct tw;
		tw.copy(structs.at(current));

/*************************
 **     RENDERING       **
 *************************/

		QImage device(size, QImage::Format_ARGB32);
		device.fill(qRgba(255, 255, 255, 0));



		for(int i=0; i<tw.bls_n();i++)
			tw.setBLSColor(i, (i==0)?viewPortSettings::instance().firstStandardColor():
								 viewPortSettings::instance().nextStandardColor(tw.bls(i-1).color()));

		structPainter sP;

		QTransform t(1.0,  0.0,  0.0,
			 0.0,  1.0,  0.0,
			 0.0,  0.0,  1.0);


		//get a first idea about the global scale
		QRectF boundingRect = tw.boundingNodeRect();
		double scale = qMin((size.width()-30)*2/boundingRect.width(), (size.height()-30)*2/boundingRect.height());


		//scale M and P so they look good
		double mScale = tw.mMax();
		if(mScale != 0)
		{
			tw.setScaleM(120/mScale);
		}
		double pScale = tw.pMax();
		if(pScale != 0)
		{
			tw.setScaleP(size.height()/3/scale/pScale);
		}

		//calculate the
		boundingRect = tw.boundingRect();
		scale = qMin((size.width()-30)*2/boundingRect.width(), (size.height()-30)*2/boundingRect.height());


		t.translate(-boundingRect.x()*scale+size.width()-boundingRect.width()*scale/2,
			-boundingRect.y()*scale+size.height()-boundingRect.height()*scale/2);
		t.scale(scale, scale);


		QPainter p(&device);
		p.scale(0.5, 0.5);
		sP.drawStruct(tw, &p, &t,
			  Colin::beam    |
			  Colin::node    |
			  Colin::nload   |
			  Colin::bearing |
			  Colin::joint   );

		emit finishedRendering(device, current);
	}
}
