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

#ifndef UNDO_COMMANDS_H
#define UNDO_COMMANDS_H

#include <QtGui/QUndoCommand>
#include <QtCore/QObject>


#include "wgv_tw.h"

class with_tw : public QUndoCommand, public QObject
{
    Q_DECLARE_TR_FUNCTIONS(with_tw)
public:
    with_tw(wgv_tw *tw, QUndoCommand *parent=0): QUndoCommand(parent){t=tw;}
protected:
    wgv_tw* t;
};

class with_index  : public with_tw
{
public:
    with_index(wgv_tw *tw, int ind,  QUndoCommand *parent=0):with_tw(tw, parent){ index=ind;}
protected:
    int index;
};

//-----------------------------------------------------------------------------------------------------

class editNodeUndoCommand : public with_index
{
public:
    editNodeUndoCommand(wgv_tw* tw, int ind, wgv_node before, wgv_node after, QUndoCommand *parent=0);
    void undo();
    void redo();
    virtual int id() const {return 1;}
    bool mergeWith(const QUndoCommand *other);
private:
    wgv_node b, a;
};
class newNodeUndoCommand : public with_tw
{
public:
    newNodeUndoCommand(wgv_tw *tw, wgv_node n, QUndoCommand *parent=0);
    void undo();
    void redo();
private:
    wgv_node e;
};
class removeNodeUndoCommand : public with_index
{
public:
    removeNodeUndoCommand(wgv_tw* tw, int ind,  wgv_node n, QUndoCommand *parent=0);
    void undo();
    void redo();
private:
    wgv_node e;
};

//-----------------------------------------------------------------------------------------------------

class editBeamUndoCommand : public with_index
{
public:
    editBeamUndoCommand(wgv_tw* tw, int ind, wgv_beam before, wgv_beam after, QUndoCommand *parent=0) ;
    void undo();
    void redo();
private:
    wgv_beam b, a;
};
class newBeamUndoCommand : public with_tw
{
public:
    newBeamUndoCommand(wgv_tw *tw, wgv_beam n, QUndoCommand *parent=0);
    void undo();
    void redo();
private:
    wgv_beam e;
};
class removeBeamUndoCommand : public with_index
{
public:
    removeBeamUndoCommand(wgv_tw* tw, int ind, wgv_beam n, QUndoCommand *parent=0);
    void undo();
    void redo();
private:
    wgv_beam e;
};

//-----------------------------------------------------------------------------------------------------

class editLoadUndoCommand : public with_index
{
public:
    editLoadUndoCommand(wgv_tw* tw, int ind,  wgv_load before, wgv_load after, QUndoCommand *parent=0);
    void undo();
    void redo();
    virtual int id() const {return 3;}
    bool mergeWith(const QUndoCommand *other);
private:
    wgv_load b, a;
};
class newLoadUndoCommand : public with_tw
{
public:
    newLoadUndoCommand(wgv_tw *tw, wgv_load n, QUndoCommand *parent=0);
    void undo();
    void redo();
private:
    wgv_load e;
};
class removeLoadUndoCommand : public with_index
{
public:
    removeLoadUndoCommand(wgv_tw* tw, int ind, wgv_load n, QUndoCommand *parent=0);
    void undo();
    void redo();
private:
    wgv_load e;
};


#endif // UNDO_COMMANDS_H
