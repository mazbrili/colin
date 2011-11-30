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

#ifndef UNDOCOMMANDER_H
#define UNDOCOMMANDER_H

#include <QtCore/QObject>
#include <QtGui/QUndoStack>

#include "undo_commands.h"

class UndoCommander : public QObject
{
    Q_OBJECT
public:
    UndoCommander(QObject *parent=0) : QObject(parent){t=0; stack=0;}
    UndoCommander(ColinStruct *tw, QUndoStack *us, QObject *parent =0) : QObject(parent){t=0; stack=us; setTw(tw);}
    void setTw(ColinStruct *tw);
    void setStack(QUndoStack *us);

public slots:
    void save_edited(const int &id, const ColinNode &after);
    void save_appended(const ColinNode &n);
    void save_removed(const int &id, const ColinNode &r);

    void save_edited(const int &id, const ColinBeam &after);
    void save_appended(const ColinBeam &n);
    void save_removed(const int &id, const ColinBeam &r);

	void save_edited(const int &id, const ColinLoad &after);
	void save_appended(const ColinLoad &n);
	void save_removed(const int &id, const ColinLoad &r);

	void save_edited(const int &id, const ColinBLS &after);
	void save_appended(const ColinBLS &n);
	void save_removed(const int &id, const ColinBLS &r);

	void save_edited(const int &id, const ColinCLS &after);
	void save_appended(const ColinCLS &n);
	void save_removed(const int &id, const ColinCLS &r);




private:
    ColinStruct *t;
    QUndoStack *stack;

};

#endif // UNDOCOMMANDER_H
