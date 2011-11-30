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

void UndoCommander::setTw(ColinStruct *tw)
{

    if(t!=NULL)
    {
	t->disconnect(this,    SLOT(save_appended(ColinNode)));
	t->disconnect(this,    SLOT(save_appended(ColinBeam)));
	t->disconnect(this,    SLOT(save_appended(ColinLoad)));
	t->disconnect(this,    SLOT(save_appended(ColinBLS)));
	t->disconnect(this,    SLOT(save_appended(ColinCLS)));

	t->disconnect(this,    SLOT(save_edited(int,ColinNode)));
	t->disconnect(this,    SLOT(save_edited(int,ColinBeam)));
	t->disconnect(this,    SLOT(save_edited(int,ColinLoad)));
	t->disconnect(this,    SLOT(save_edited(int,ColinBLS)));
	t->disconnect(this,    SLOT(save_edited(int,ColinCLS)));

	t->disconnect(this,    SLOT(save_removed(int,ColinNode)));
	t->disconnect(this,    SLOT(save_removed(int,ColinBeam)));
	t->disconnect(this,    SLOT(save_removed(int,ColinLoad)));
	t->disconnect(this,    SLOT(save_removed(int,ColinBLS)));
	t->disconnect(this,    SLOT(save_removed(int,ColinCLS)));

    }

    t=tw;


    connect(tw,         SIGNAL(appended(ColinNode)),
            this,       SLOT(save_appended(ColinNode)));

	connect(tw,         SIGNAL(appended(ColinBeam)),
			this,       SLOT(save_appended(ColinBeam)));

	connect(tw,         SIGNAL(appended(ColinLoad)),
			this,       SLOT(save_appended(ColinLoad)));

	connect(tw,         SIGNAL(appended(ColinBLS)),
			this,       SLOT(save_appended(ColinBLS)));

	connect(tw,         SIGNAL(appended(ColinCLS)),
			this,       SLOT(save_appended(ColinCLS)));



    connect(tw,         SIGNAL(edited(int,ColinNode)),
            this,       SLOT(save_edited(int,ColinNode)));

    connect(tw,         SIGNAL(edited(int,ColinBeam)),
			this,       SLOT(save_edited(int,ColinBeam)));

	connect(tw,         SIGNAL(edited(int,ColinLoad)),
			this,       SLOT(save_edited(int,ColinLoad)));

	connect(tw,         SIGNAL(edited(int,ColinBLS)),
			this,       SLOT(save_edited(int,ColinBLS)));

	connect(tw,         SIGNAL(edited(int,ColinCLS)),
			this,       SLOT(save_edited(int,ColinCLS)));



	connect(tw,         SIGNAL(removed(int,ColinNode)),
			this,       SLOT(save_removed(int,ColinNode)));

	connect(tw,         SIGNAL(removed(int,ColinBeam)),
			this,       SLOT(save_removed(int,ColinBeam)));

	connect(tw,         SIGNAL(removed(int,ColinLoad)),
			this,       SLOT(save_removed(int,ColinLoad)));

	connect(tw,         SIGNAL(removed(int,ColinBLS)),
			this,       SLOT(save_removed(int,ColinBLS)));

	connect(tw,         SIGNAL(removed(int,ColinCLS)),
			this,       SLOT(save_removed(int,ColinCLS)));



}

void UndoCommander::setStack(QUndoStack *us)
{
    stack=us;
}

void UndoCommander::save_edited(const int &id, const ColinNode &after)
{
    QUndoCommand *command = new editNodeUndoCommand(t, id, t->node(id), after);
    stack->push(command);
}

void UndoCommander::save_appended(const ColinNode &n)
{
    QUndoCommand *command = new newNodeUndoCommand(t, n);
    stack->push(command);
}

void UndoCommander::save_removed(const int &id, const ColinNode &r)
{
    QUndoCommand *command = new removeNodeUndoCommand(t, id, r);
    stack->push(command);
}

void UndoCommander::save_edited(const int &id, const ColinBeam &after)
{
    QUndoCommand *command = new editBeamUndoCommand(t, id, t->beam(id), after);
    stack->push(command);
}

void UndoCommander::save_appended(const ColinBeam &n)
{
    QUndoCommand *command = new newBeamUndoCommand(t, n);
    stack->push(command);
}

void UndoCommander::save_removed(const int &id, const ColinBeam &r)
{
    QUndoCommand *command = new removeBeamUndoCommand(t, id, r);
    stack->push(command);
}

void UndoCommander::save_edited(const int &id, const ColinLoad &after)
{
	QUndoCommand *command = new editLoadUndoCommand(t, id, t->load(id), after);
	stack->push(command);
}

void UndoCommander::save_appended(const ColinLoad &n)
{
	QUndoCommand *command = new newLoadUndoCommand(t, n);
	stack->push(command);
}

void UndoCommander::save_removed(const int &id, const ColinLoad &r)
{
	QUndoCommand *command = new removeLoadUndoCommand(t, id, r);
	stack->push(command);
}

void UndoCommander::save_edited(const int &id, const ColinBLS &after)
{
	QUndoCommand *command = new editBLSUndoCommand(t, id, t->bls(id), after);
	stack->push(command);
}

void UndoCommander::save_appended(const ColinBLS &n)
{
	QUndoCommand *command = new newBLSUndoCommand(t, n);
	stack->push(command);
}

void UndoCommander::save_removed(const int &id, const ColinBLS &r)
{
	QUndoCommand *command = new removeBLSUndoCommand(t, id, r);
	stack->push(command);
}

void UndoCommander::save_edited(const int &id, const ColinCLS &after)
{
	QUndoCommand *command = new editCLSUndoCommand(t, id, t->cls(id), after);
	stack->push(command);
}

void UndoCommander::save_appended(const ColinCLS &n)
{
	QUndoCommand *command = new newCLSUndoCommand(t, n);
	stack->push(command);
}

void UndoCommander::save_removed(const int &id, const ColinCLS &r)
{
	QUndoCommand *command = new removeCLSUndoCommand(t, id, r);
	stack->push(command);
}

