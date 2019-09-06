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
    UndoCommander(wgv_tw *tw, QUndoStack *us, QObject *parent =0) : QObject(parent){t=0; stack=us; setTw(tw);}
    void setTw(wgv_tw *tw);
    void setStack(QUndoStack *us);

public slots:
    void save_edited(const int &id, const wgv_node &after);
    void save_appended(const wgv_node &n);
    void save_removed(const int &id, const wgv_node &r);

    void save_edited(const int &id, const wgv_beam &after);
    void save_appended(const wgv_beam &n);
    void save_removed(const int &id, const wgv_beam &r);

    void save_edited(const int &id, const wgv_load &after);
    void save_appended(const wgv_load &n);
    void save_removed(const int &id, const wgv_load &r);



private:
    wgv_tw *t;
    QUndoStack *stack;

};

#endif // UNDOCOMMANDER_H
