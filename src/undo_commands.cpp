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

#include "undo_commands.h"


editNodeUndoCommand::editNodeUndoCommand(ColinStruct* tw, int ind, ColinNode before, ColinNode after, QUndoCommand *parent) :
        with_index(tw, ind, parent)
{
    b=before;
    a=after;
    setText(tr("edit node %1").arg(ind));
}

void editNodeUndoCommand::undo()
{
    t->CommandEditNode(index, b);
}

void editNodeUndoCommand::redo()
{
    t->CommandEditNode(index, a);
}

bool editNodeUndoCommand::mergeWith(const QUndoCommand *other)
{
    Q_ASSERT(id()==other->id());


    const editNodeUndoCommand *oth = static_cast<const editNodeUndoCommand*>(other);
    if(oth->index != index)
        return false;

    //merge only move-commands
    //testing other
    if(oth->a.hasbearing() && oth->b.hasbearing()){
        if(oth->a.bearing() != oth->b.bearing())
            return false;}
    else if(!(!oth->a.hasbearing() && !oth->b.hasbearing()))
       return false;

    //testing this
    if(a.hasbearing() && b.hasbearing()){
        if(a.bearing() != b.bearing())
            return false;}
    else if(!(!a.hasbearing() && !b.hasbearing()))
       return false;

    //merge!
    a = oth->a;
    return true;
}

//-------------------------------------------------------------------------------------------------------------------------

newNodeUndoCommand::newNodeUndoCommand(ColinStruct *tw, ColinNode n, QUndoCommand *parent):
        with_tw(tw, parent)
{
    e=n;
    setText(tr("add node"));
}

void newNodeUndoCommand::undo()
{
    t->CommandRemoveNode(t->node_n()-1);
}

void newNodeUndoCommand::redo()
{
    t->CommandInsertNode(t->node_n(), e);
}

//-------------------------------------------------------------------------------------------------------------------------

removeNodeUndoCommand::removeNodeUndoCommand(ColinStruct* tw, int ind, ColinNode n, QUndoCommand *parent):
        with_index(tw, ind, parent)
{
    e=n;
    setText(tr("remove node %1").arg(ind));
}

void removeNodeUndoCommand::undo()
{
    t->CommandInsertNode(index, e);
}

void removeNodeUndoCommand::redo()
{
    t->CommandRemoveNode(index);
}

//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------

editBeamUndoCommand::editBeamUndoCommand(ColinStruct *tw, int ind, ColinBeam before, ColinBeam after, QUndoCommand *parent) :
        with_index(tw, ind, parent)
{
    b=before;
    a=after;
    setText(tr("edit beam %1").arg(ind));
}

void editBeamUndoCommand::undo()
{
    t->CommandEditBeam(index, b);
}

void editBeamUndoCommand::redo()
{
    t->CommandEditBeam(index, a);
}

//-------------------------------------------------------------------------------------------------------------------------

newBeamUndoCommand::newBeamUndoCommand(ColinStruct *tw, ColinBeam n, QUndoCommand *parent) :
        with_tw(tw, parent)
{
    e=n;
    setText(tr("add beam"));
}

void newBeamUndoCommand::undo()
{
    t->CommandRemoveBeam(t->beam_n()-1);
}

void newBeamUndoCommand::redo()
{
    t->CommandInsertBeam(t->beam_n(), e);
}

//-------------------------------------------------------------------------------------------------------------------------

removeBeamUndoCommand::removeBeamUndoCommand(ColinStruct* tw, int ind, ColinBeam n, QUndoCommand *parent):
        with_index(tw, ind, parent)
{
    e=n;
    setText(tr("remove beam %1").arg(ind));
}

void removeBeamUndoCommand::undo()
{
    t->CommandInsertBeam(index, e);
}

void removeBeamUndoCommand::redo()
{
    t->CommandRemoveBeam(index);
}

//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------

editLoadUndoCommand::editLoadUndoCommand(ColinStruct *tw, int ind, ColinLoad before, ColinLoad after, QUndoCommand *parent) :
		with_index(tw, ind, parent)
{
	b=before;
	a=after;
	setText(QString(tr("edit load %1")).arg(ind));
}

void editLoadUndoCommand::undo()
{
	t->CommandEditLoad(index, b);
}

void editLoadUndoCommand::redo()
{
	t->CommandEditLoad(index, a);
}

bool editLoadUndoCommand::mergeWith(const QUndoCommand *other)
{
	Q_ASSERT(id()==other->id());


	const editLoadUndoCommand *oth = static_cast<const editLoadUndoCommand*>(other);
	if(oth->index != index)
		return false;
	if(oth->a.typ() != oth->b.typ())
	return false;
	if(a.typ() != b.typ())
	return false;
	if(oth->a.at() != oth->b.at())
	{
	//MOVE ACTION!!!!
	if(a.at() != b.at())
	{
		//SECOND MOVE EVENT!!! MERGE!!!!
		a = oth->a;
		return true;
	}
	else
		return false;
	}
	if(a.at() != b.at())
	return false;

	//PX or PZ or M or deltaT changed MERGE!!!!

	a = oth->a;
	return true;
}

//-------------------------------------------------------------------------------------------------------------------------

newLoadUndoCommand::newLoadUndoCommand(ColinStruct *tw,  ColinLoad n, QUndoCommand *parent) :
		with_tw(tw, parent)
{
	e=n;
	setText(tr("add load"));
}

void newLoadUndoCommand::undo()
{
	t->CommandRemoveLoad(t->load_n()-1);
}

void newLoadUndoCommand::redo()
{
	t->CommandInsertLoad(t->load_n(), e);
}

//-------------------------------------------------------------------------------------------------------------------------

removeLoadUndoCommand::removeLoadUndoCommand(ColinStruct* tw, int ind, ColinLoad n, QUndoCommand *parent):
		with_index(tw, ind, parent)
{
	e=n;
	setText(tr("remove load %1").arg(ind));
}

void removeLoadUndoCommand::undo()
{
	t->CommandInsertLoad(index, e);
}

void removeLoadUndoCommand::redo()
{
	t->CommandRemoveLoad(index);
}

//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------

editBLSUndoCommand::editBLSUndoCommand(ColinStruct *tw, int ind, ColinBLS before, ColinBLS after, QUndoCommand *parent) :
		with_index(tw, ind, parent)
{
	b=before;
	a=after;
	setText(QString(tr("edit bls %1")).arg(ind));
}

void editBLSUndoCommand::undo()
{
	t->CommandEditBLS(index, b);
}

void editBLSUndoCommand::redo()
{
	t->CommandEditBLS(index, a);
}

bool editBLSUndoCommand::mergeWith(const QUndoCommand *other)
{
	Q_ASSERT(id()==other->id());

	return false;
}

//-------------------------------------------------------------------------------------------------------------------------

newBLSUndoCommand::newBLSUndoCommand(ColinStruct *tw,  ColinBLS n, QUndoCommand *parent) :
		with_tw(tw, parent)
{
	e=n;
	setText(tr("add bls"));
}

void newBLSUndoCommand::undo()
{
	t->CommandRemoveBLS(t->cls_n()-1);
}

void newBLSUndoCommand::redo()
{
	t->CommandInsertBLS(t->cls_n(), e);
}

//-------------------------------------------------------------------------------------------------------------------------

removeBLSUndoCommand::removeBLSUndoCommand(ColinStruct* tw, int ind, ColinBLS n, QUndoCommand *parent):
		with_index(tw, ind, parent)
{
	e=n;
	setText(tr("remove bls %1").arg(ind));
}

void removeBLSUndoCommand::undo()
{
	t->CommandInsertBLS(index, e);
}

void removeBLSUndoCommand::redo()
{
	t->CommandRemoveBLS(index);
}

//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------

editCLSUndoCommand::editCLSUndoCommand(ColinStruct *tw, int ind, ColinCLS before, ColinCLS after, QUndoCommand *parent) :
		with_index(tw, ind, parent)
{
	b=before;
	a=after;
	setText(QString(tr("edit cls %1")).arg(ind));
}

void editCLSUndoCommand::undo()
{
	t->CommandEditCLS(index, b);
}

void editCLSUndoCommand::redo()
{
	t->CommandEditCLS(index, a);
}

bool editCLSUndoCommand::mergeWith(const QUndoCommand *other)
{
	Q_ASSERT(id()==other->id());

	return false;
}

//-------------------------------------------------------------------------------------------------------------------------

newCLSUndoCommand::newCLSUndoCommand(ColinStruct *tw,  ColinCLS n, QUndoCommand *parent) :
		with_tw(tw, parent)
{
	e=n;
	setText(tr("add cls"));
}

void newCLSUndoCommand::undo()
{
	t->CommandRemoveCLS(t->cls_n()-1);
}

void newCLSUndoCommand::redo()
{
	t->CommandInsertCLS(t->cls_n(), e);
}

//-------------------------------------------------------------------------------------------------------------------------

removeCLSUndoCommand::removeCLSUndoCommand(ColinStruct* tw, int ind, ColinCLS n, QUndoCommand *parent):
		with_index(tw, ind, parent)
{
	e=n;
	setText(tr("remove cls %1").arg(ind));
}

void removeCLSUndoCommand::undo()
{
	t->CommandInsertCLS(index, e);
}

void removeCLSUndoCommand::redo()
{
	t->CommandRemoveCLS(index);
}

//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
