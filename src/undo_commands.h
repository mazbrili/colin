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


#include "colinstruct.h"

class with_tw : public QUndoCommand, public QObject
{
    Q_DECLARE_TR_FUNCTIONS(with_tw)
public:
    with_tw(ColinStruct *tw, QUndoCommand *parent=0): QUndoCommand(parent){t=tw;}
protected:
    ColinStruct* t;
};

class with_index  : public with_tw
{
public:
    with_index(ColinStruct *tw, int ind,  QUndoCommand *parent=0):with_tw(tw, parent){ index=ind;}
protected:
    int index;
};

//-----------------------------------------------------------------------------------------------------

class editNodeUndoCommand : public with_index
{
public:
    editNodeUndoCommand(ColinStruct* tw, int ind, ColinNode before, ColinNode after, QUndoCommand *parent=0);
    void undo();
    void redo();
    virtual int id() const {return 1;}
    bool mergeWith(const QUndoCommand *other);
private:
    ColinNode b, a;
};
class newNodeUndoCommand : public with_tw
{
public:
    newNodeUndoCommand(ColinStruct *tw, ColinNode n, QUndoCommand *parent=0);
    void undo();
    void redo();
private:
    ColinNode e;
};
class removeNodeUndoCommand : public with_index
{
public:
    removeNodeUndoCommand(ColinStruct* tw, int ind,  ColinNode n, QUndoCommand *parent=0);
    void undo();
    void redo();
private:
    ColinNode e;
};

//-----------------------------------------------------------------------------------------------------

class editBeamUndoCommand : public with_index
{
public:
    editBeamUndoCommand(ColinStruct* tw, int ind, ColinBeam before, ColinBeam after, QUndoCommand *parent=0) ;
    void undo();
    void redo();
private:
    ColinBeam b, a;
};
class newBeamUndoCommand : public with_tw
{
public:
    newBeamUndoCommand(ColinStruct *tw, ColinBeam n, QUndoCommand *parent=0);
    void undo();
    void redo();
private:
    ColinBeam e;
};
class removeBeamUndoCommand : public with_index
{
public:
    removeBeamUndoCommand(ColinStruct* tw, int ind, ColinBeam n, QUndoCommand *parent=0);
    void undo();
    void redo();
private:
    ColinBeam e;
};

//-----------------------------------------------------------------------------------------------------

class editLoadUndoCommand : public with_index
{
public:
	editLoadUndoCommand(ColinStruct* tw, int ind,  ColinLoad before, ColinLoad after, QUndoCommand *parent=0);
	void undo();
	void redo();
	virtual int id() const {return 3;}
	bool mergeWith(const QUndoCommand *other);
private:
	ColinLoad b, a;
};
class newLoadUndoCommand : public with_tw
{
public:
	newLoadUndoCommand(ColinStruct *tw, ColinLoad n, QUndoCommand *parent=0);
	void undo();
	void redo();
private:
	ColinLoad e;
};
class removeLoadUndoCommand : public with_index
{
public:
	removeLoadUndoCommand(ColinStruct* tw, int ind, ColinLoad n, QUndoCommand *parent=0);
	void undo();
	void redo();
private:
	ColinLoad e;
};

//-----------------------------------------------------------------------------------------------------

class editCLSUndoCommand : public with_index
{
public:
	editCLSUndoCommand(ColinStruct* tw, int ind,  ColinCLS before, ColinCLS after, QUndoCommand *parent=0);
	void undo();
	void redo();
	virtual int id() const {return 3;}
	bool mergeWith(const QUndoCommand *other);
private:
	ColinCLS b, a;
};
class newCLSUndoCommand : public with_tw
{
public:
	newCLSUndoCommand(ColinStruct *tw, ColinCLS n, QUndoCommand *parent=0);
	void undo();
	void redo();
private:
	ColinCLS e;
};
class removeCLSUndoCommand : public with_index
{
public:
	removeCLSUndoCommand(ColinStruct* tw, int ind, ColinCLS n, QUndoCommand *parent=0);
	void undo();
	void redo();
private:
	ColinCLS e;
};

//-----------------------------------------------------------------------------------------------------

class editBLSUndoCommand : public with_index
{
public:
	editBLSUndoCommand(ColinStruct* tw, int ind,  ColinBLS before, ColinBLS after, QUndoCommand *parent=0);
	void undo();
	void redo();
	virtual int id() const {return 3;}
	bool mergeWith(const QUndoCommand *other);
private:
	ColinBLS b, a;
};
class newBLSUndoCommand : public with_tw
{
public:
	newBLSUndoCommand(ColinStruct *tw, ColinBLS n, QUndoCommand *parent=0);
	void undo();
	void redo();
private:
	ColinBLS e;
};
class removeBLSUndoCommand : public with_index
{
public:
	removeBLSUndoCommand(ColinStruct* tw, int ind, ColinBLS n, QUndoCommand *parent=0);
	void undo();
	void redo();
private:
	ColinBLS e;
};



#endif // UNDO_COMMANDS_H
