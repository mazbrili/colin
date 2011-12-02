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

ColinBeam::ColinBeam(ColinStruct *parent):
	ColinElement()
{
	par=parent;
	for(int i=0; i<6; i++){gel[i]=0; spr[i]=0;}
	mt = 0;
	qs = 0;
	dirty = true;
	res=0;
}

ColinBeam::ColinBeam(ColinStruct *parent, int leftN, int rightN, int Mat, int qS):
	ColinElement()
{
	par=parent;
	for(int i=0; i<6; i++){gel[i]=0; spr[i]=0;}
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
	for(int i=0; i<6; i++){gel[i]=0; spr[i]=0;}
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
	for(int i=0; i<6; i++)
	{
		gel[i]=other.joint(i);
		spr[i]=other.spring(i);
	}
	node_l=other.leftNodeI();
	node_r=other.rightNodeI();
	mt=other.mt;
	qs=other.qs;
	len = other.l();
	alp = other.angle();
	res=other.res;
	dirty = false;
}

ColinBeam &ColinBeam::operator=(const ColinBeam &other)
{
	par=other.par;
	for(int i=0; i<6; i++)
	{
		gel[i]=other.joint(i);
		spr[i]=other.spring(i);
	}
	node_l=other.leftNodeI();
	node_r=other.rightNodeI();
	mt=other.mt;
	qs=other.qs;
	len = other.l();
	alp = other.angle();
	dirty = false;
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


void ColinBeam::setJoints(const bool *array)
{
	for(int o=0;o<6; o++)
	{
		gel[o]=array[o];
	}
}

void ColinBeam::setSprings(const double *array)
{
	for(int o=0; o<6; o++)
	{
		spr[o]=array[o];
	}
}


void ColinBeam::setJointsandSprings(const bool *barray, const double *darray)
{
	 setJoints(barray);
	 setSprings(darray);
}


void ColinBeam::setJoint(const int &pos, const bool &thereIsAJoint)
{
	gel[pos]=thereIsAJoint;
	if(!thereIsAJoint)
		spr[pos]=0;
}

void ColinBeam::setSpring(const int &pos, const double &c_f)
{
	spr[pos]=c_f;
	gel[pos]=true;
}



void ColinBeam::joints(bool *bool_array_6)
{
	for(int o=0; o<6; o++)
		bool_array_6[o]=gel[o];
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
		return res[i].N(1);
	case 4:
		return res[i].Q(1);
	case 5:
		return res[i].M(1);
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
		return res[i].u(1);
	case 4:
		return res[i].w(1);
	case 5:
		return res[i].p(1);
	default:
		Q_ASSERT(true);
		return(0);
	}
}
