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

#ifndef COLINPASTEBUFFER_H
#define COLINPASTEBUFFER_H

#include <QtCore/QThread>
#include <QtGui/QClipboard>
#include <QtCore/QMutex>
#include <QtGui/QImage>
#include <QtCore/QList>
#include <QtCore/QWaitCondition>

class ColinStruct;


class ColinPasteBuffer : public QThread
{
    Q_OBJECT
public:
	static ColinPasteBuffer &instance(){
		if(!_instance) _instance = new ColinPasteBuffer();
		return *_instance;
	}

	int size() const;
	ColinStruct *structAt(int i);
	void renderPreview(QSize size, int i);
	void remove(int i);
	void copy(int i, ColinStruct *target);
	void run();

signals:
	void changedBuffer();
	void finishedRendering(QImage image, int i);
public slots:
	void changed(QClipboard::Mode mode);

private:
	ColinPasteBuffer();
	~ColinPasteBuffer();
	static ColinPasteBuffer *_instance;
	QClipboard *board;
	QList<ColinStruct*> structs;
	mutable QMutex lockStructs;
	QList<int> toDo_;
	QSize size_;
	QWaitCondition waitCondition;
	bool killYourSelf;

};

#endif // COLINPASTEBUFFER_H
