#include "undo_commands.h"


editNodeUndoCommand::editNodeUndoCommand(wgv_tw* tw, int ind, wgv_node before, wgv_node after, QUndoCommand *parent) :
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

newNodeUndoCommand::newNodeUndoCommand(wgv_tw *tw, wgv_node n, QUndoCommand *parent):
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

removeNodeUndoCommand::removeNodeUndoCommand(wgv_tw* tw, int ind, wgv_node n, QUndoCommand *parent):
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

editBeamUndoCommand::editBeamUndoCommand(wgv_tw *tw, int ind, wgv_beam before, wgv_beam after, QUndoCommand *parent) :
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

newBeamUndoCommand::newBeamUndoCommand(wgv_tw *tw, wgv_beam n, QUndoCommand *parent) :
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

removeBeamUndoCommand::removeBeamUndoCommand(wgv_tw* tw, int ind, wgv_beam n, QUndoCommand *parent):
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

editLoadUndoCommand::editLoadUndoCommand(wgv_tw *tw, int ind, wgv_load before, wgv_load after, QUndoCommand *parent) :
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

newLoadUndoCommand::newLoadUndoCommand(wgv_tw *tw,  wgv_load n, QUndoCommand *parent) :
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

removeLoadUndoCommand::removeLoadUndoCommand(wgv_tw* tw, int ind, wgv_load n, QUndoCommand *parent):
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
