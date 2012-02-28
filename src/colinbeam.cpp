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

#include "colinbeam.h"
#include "colinstruct.h"

#include <QtCore/QDebug>


ColinBeam::ColinBeam(ColinStruct *parent):
	ColinElement()
{
	par=parent;
	node_l = 0;
	node_r = 0;
	hinges = NoHinge;
	for(int i=0; i<6; i++){spr[i]=0;}
	mt = 0;
	qs = 0;
	dirty = true;
	res=0;
}

ColinBeam::ColinBeam(ColinStruct *parent, int leftN, int rightN, int Mat, int qS):
	ColinElement()
{
	par=parent;
	hinges = NoHinge;
	for(int i=0; i<6; i++){spr[i]=0;}
	node_l=leftN;
	node_r=rightN;
	mt=Mat;
	qs=qS;
	res=0;
	dirty = true;
}

ColinBeam::ColinBeam(ColinStruct *parent, int leftN, int rightN, QString Mat, QString qS):
	ColinElement()
{
	par=parent;
	hinges = NoHinge;
	for(int i=0; i<6; i++){spr[i]=0;}
	node_l=leftN;
	node_r=rightN;
	mt=LIB.IndexFromMatByName(Mat);
	qs=LIB.IndexFromProfileByName(qS);
	res=0;
	dirty = true;
}

ColinBeam::ColinBeam(const ColinBeam &other)
	:ColinElement(other)
{
	hinges = other.hinges;
	for(int i=0; i<6; i++)
	{
		spr[i]=other.spring(i);
	}
	node_l=other.leftNodeI();
	node_r=other.rightNodeI();
	mt=other.mt;
	qs=other.qs;
	if(!other.dirty){
		len = other.l();
		alp = other.angle();
		dirty = false;
	}
	else
	{
		len = 0;
		alp = 0;
		dirty = true;
	}
	res=other.res;
}

ColinBeam &ColinBeam::operator=(const ColinBeam &other)
{
	hinges = other.hinges;
	par=other.par;
	for(int i=0; i<6; i++)
	{
		spr[i]=other.spring(i);
	}
	node_l=other.leftNodeI();
	node_r=other.rightNodeI();
	mt=other.mt;
	qs=other.qs;
	if(!other.dirty){
		len = other.l();
		alp = other.angle();
		dirty = false;
	}
	else
	{
		len = 0;
		alp = 0;
		dirty = true;
	}
	res=other.res;
	return *this;
}



QTransform ColinBeam::transform() const
{
	QTransform t;
	t.translate(leftNode().x(), leftNode().z());
	t.rotate(angle()*180/M_PI);
	return t;
}
const ColinNode &ColinBeam::leftNode() const
{
	return (par->node(node_l));
}

const ColinNode &ColinBeam::rightNode() const
{
	return (par->node(node_r));
}

QRectF ColinBeam::boundingRect() const
{
	return QRectF(leftNode().toQPointF(), rightNode().toQPointF());
}
QLineF ColinBeam::toQLineF() const
{
	return QLineF(leftNode().toQPointF(), rightNode().toQPointF());
}


void ColinBeam::calcLen() const
{
	if(leftNode().x()<rightNode().x())
		alp = atan((leftNode().z()-rightNode().z())/(leftNode().x()-rightNode().x()));
	else
		alp = atan((leftNode().z()-rightNode().z())/(leftNode().x()-rightNode().x()))+M_PI;
	if (alp>=M_PI)
		alp= alp - 2*M_PI;

	len = sqrt((leftNode().x()-rightNode().x())*(leftNode().x()-rightNode().x())+
		  (leftNode().z()-rightNode().z())*(leftNode().z()-rightNode().z()));
	dirty = false;
}

const double &ColinBeam::angle() const
{
	if(dirty)
		calcLen();

	return(alp);
}

const double &ColinBeam::l() const
{
	if(dirty)
		calcLen();
	return len;
}


void ColinBeam::setJoints(hingeForms h)
{
	hinges = h;
}

void ColinBeam::setSprings(const double *array)
{
	for(int o=0; o<6; o++)
	{
		spr[o]=array[o];
	}
}

void ColinBeam::setJoint(const int &pos, const bool &thereIsAJoint)
{
	hinges &= (~springfromPos(position(pos)));
	if(thereIsAJoint)
		hinges |= hingefromPos(position(pos));

#ifdef HINGES_VERBOSE
	qDebug() << hinges
			 << (((hinges&Xleft)!=0)?((hinges&fxleft)!=0?"S":"H"):"-")
			 << (((hinges&Zleft)!=0)?((hinges&fzleft)!=0?"S":"H"):"-")
			 << (((hinges&Phileft)!=0)?((hinges&fphileft)!=0?"S":"H"):"-")
			 << (((hinges&Xright)!=0)?((hinges&fxright)!=0?"S":"H"):"-")
			 << (((hinges&Zright)!=0)?((hinges&fzright)!=0?"S":"H"):"-")
			 << (((hinges&Phiright)!=0)?((hinges&fphiright)!=0?"S":"H"):"-");
#endif
}

void ColinBeam::setHasSpring(const int& pos, const bool& thereIsAJoint)
{
	hinges &= (~springfromPos(position(pos)));
	if(thereIsAJoint)
		hinges |= springfromPos(position(pos));

#ifdef HINGES_VERBOSE
	qDebug() << hinges
			 << (((hinges&Xleft)!=0)?((hinges&fxleft)!=0?"S":"H"):"-")
			 << (((hinges&Zleft)!=0)?((hinges&fzleft)!=0?"S":"H"):"-")
			 << (((hinges&Phileft)!=0)?((hinges&fphileft)!=0?"S":"H"):"-")
			 << (((hinges&Xright)!=0)?((hinges&fxright)!=0?"S":"H"):"-")
			 << (((hinges&Zright)!=0)?((hinges&fzright)!=0?"S":"H"):"-")
			 << (((hinges&Phiright)!=0)?((hinges&fphiright)!=0?"S":"H"):"-");
#endif
}

void ColinBeam::setSpring(const int &pos, const double &c_f)
{
	hinges |= springfromPos(position(pos));
	spr[pos]=c_f;
}

void ColinBeam::springs(double *double_array_6)
{
	for(int o=0; o<6; o++)
		double_array_6[o]=spr[o];
}

double ColinBeam::s(const int &i, const int& pos) const
{
	switch(pos)
	{
	case 0:
		return res[i].N(0);
	case 1:
		return res[i].Q(0);
	case 2:
		return res[i].M(0);
	case 3:
		return res[i].N(l());
	case 4:
		return res[i].Q(l());
	case 5:
		return res[i].M(l());
	default:
		Q_ASSERT(true);
		return(0);
	}
}

double ColinBeam::v(const int &i, const int& pos) const
{
	switch(pos)
	{
	case 0:
		return res[i].u(0);
	case 1:
		return res[i].w(0);
	case 2:
		return res[i].p(0);
	case 3:
		return res[i].u(l());
	case 4:
		return res[i].w(l());
	case 5:
		return res[i].p(l());
	default:
		Q_ASSERT(true);
		return(0);
	}
}
