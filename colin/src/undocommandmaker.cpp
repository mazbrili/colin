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

#include "undocommandmaker.h"

void UndoCommander::setTw(wgv_tw *tw)
{

    if(t!=NULL)
    {
	t->disconnect(this,    SLOT(save_appended(wgv_node)));
	t->disconnect(this,    SLOT(save_appended(wgv_beam)));
	t->disconnect(this,    SLOT(save_appended(wgv_load)));

	t->disconnect(this,    SLOT(save_edited(int,wgv_node)));
	t->disconnect(this,    SLOT(save_edited(int,wgv_beam)));
	t->disconnect(this,    SLOT(save_edited(int,wgv_load)));

	t->disconnect(this,    SLOT(save_removed(int,wgv_node)));
	t->disconnect(this,    SLOT(save_removed(int,wgv_beam)));
	t->disconnect(this,    SLOT(save_removed(int,wgv_load)));

    }

    t=tw;


    connect(tw,         SIGNAL(appended(wgv_node)),
            this,       SLOT(save_appended(wgv_node)));

    connect(tw,         SIGNAL(appended(wgv_beam)),
            this,       SLOT(save_appended(wgv_beam)));

    connect(tw,         SIGNAL(appended(wgv_load)),
            this,       SLOT(save_appended(wgv_load)));



    connect(tw,         SIGNAL(edited(int,wgv_node)),
            this,       SLOT(save_edited(int,wgv_node)));

    connect(tw,         SIGNAL(edited(int,wgv_beam)),
            this,       SLOT(save_edited(int,wgv_beam)));

    connect(tw,         SIGNAL(edited(int,wgv_load)),
            this,       SLOT(save_edited(int,wgv_load)));



    connect(tw,         SIGNAL(removed(int,wgv_node)),
            this,       SLOT(save_removed(int,wgv_node)));

    connect(tw,         SIGNAL(removed(int,wgv_beam)),
            this,       SLOT(save_removed(int,wgv_beam)));

    connect(tw,         SIGNAL(removed(int,wgv_load)),
            this,       SLOT(save_removed(int,wgv_load)));

}

void UndoCommander::setStack(QUndoStack *us)
{
    stack=us;
}

void UndoCommander::save_edited(const int &id, const wgv_node &after)
{
    QUndoCommand *command = new editNodeUndoCommand(t, id, t->node(id), after);
    stack->push(command);
}

void UndoCommander::save_appended(const wgv_node &n)
{
    QUndoCommand *command = new newNodeUndoCommand(t, n);
    stack->push(command);
}

void UndoCommander::save_removed(const int &id, const wgv_node &r)
{
    QUndoCommand *command = new removeNodeUndoCommand(t, id, r);
    stack->push(command);
}

void UndoCommander::save_edited(const int &id, const wgv_beam &after)
{
    QUndoCommand *command = new editBeamUndoCommand(t, id, t->beam(id), after);
    stack->push(command);
}

void UndoCommander::save_appended(const wgv_beam &n)
{
    QUndoCommand *command = new newBeamUndoCommand(t, n);
    stack->push(command);
}

void UndoCommander::save_removed(const int &id, const wgv_beam &r)
{
    QUndoCommand *command = new removeBeamUndoCommand(t, id, r);
    stack->push(command);
}

void UndoCommander::save_edited(const int &id, const wgv_load &after)
{
    QUndoCommand *command = new editLoadUndoCommand(t, id, t->load(id), after);
    stack->push(command);
}

void UndoCommander::save_appended(const wgv_load &n)
{
    QUndoCommand *command = new newLoadUndoCommand(t, n);
    stack->push(command);
}

void UndoCommander::save_removed(const int &id, const wgv_load &r)
{
    QUndoCommand *command = new removeLoadUndoCommand(t, id, r);
    stack->push(command);
}

