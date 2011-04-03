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


#include "wgv_tw.h"

wgv_tw::wgv_tw() :
wgv_abstract_tw()
{
    isCalc=false;
    //kglobmatrix=0;
    //pvektor=0;
    //s0vektor=0;
    //u_indexed=0;
    //uvektor=0;
    //k_index=0;

    lOBeam = -1;
    lONode = -1;
    lOLoad = -1;

    scale_m = 1./50;
    scale_p = 1./50;
    scale_u = 100;


    calculationThread = 0;

    LIB.setUp();

    connect(&LIB,           SIGNAL(matEdited(int)),
            this,           SLOT(resetCals()));

    connect(&LIB,           SIGNAL(profileEdited(int)),
            this,           SLOT(resetCals()));

}

bool wgv_tw::isCalculated()const
{
    return isCalc;
}

void wgv_tw::CommandEditNode(const int &k, const wgv_node &n)
{
    isCalc = false;
    calculationThread = 0;
    nodes[k]=n;
    for(int i=0; i<beams.size(); i++)
    {
        if(beam(i).leftNodeI() == k || beam(i).rightNodeI() == k)
            beam(i).setDirty();
    }
    emit changedNode(k);    
}

void wgv_tw::CommandRemoveNode(const int &k)
{
    isCalc = false;
    calculationThread = 0;
    for(int i=0; i<load_n(); )
    {
        if(!load(i).isOnBeam())
        {
            if(load(i).at()==k)
            {
                emit removed(i, load(i));
                continue;
            }
            else if(load(i).at()>k)
            {
                loads[i].setBeam(load(i).at()-1);
            }
        }
        i++;
    }
    for(int i=0; i<beam_n(); )
    {
        if(beam(i).leftNodeI()==k ||
           beam(i).rightNodeI()==k)
        {
            emit removed(i, beam(i));
            continue;
        }
        beams[i].setLeftNode(beam(i).leftNodeI()-((beam(i).leftNodeI()>k) ? 1: 0));
        beams[i].setRightNode(beam(i).rightNodeI()-((beam(i).rightNodeI()>k) ? 1: 0));
        i++;
    }
    nodes.removeAt(k);
    if(lONode == k)lONode--;
    emit erasedNode(k);
}

void wgv_tw::CommandInsertNode(const int &o, const wgv_node &n)
{
    isCalc = false;
    calculationThread = 0;
    nodes.insert(o, n);
    if(o!=node_n()-1)
    {
        for(int i=beam_n()-1; i>=0; i--)
        {
            if(!(beam(i).leftNodeI()<o))
                beams[i].setLeftNode(beam(i).leftNodeI()+1);
            if(!(beam(i).rightNodeI()<o))
                beams[i].setRightNode(beam(i).rightNodeI()+1);
        }
        for(int i=load_n()-1; i>=0; i--)
        {
            if(!load(i).isOnBeam())
            {
                if(!load(i).at()<o)
                    loads[i].setBeam(load(i).at()+1);
            }
        }
    }
    emit addedNode(o);
}

void wgv_tw::CommandEditBeam(const int &o, const wgv_beam &n)
{
    isCalc = false;
    calculationThread = 0;
    beams[o]=n;
    emit changedBeam(o);
#ifndef QT_NO_DEBUG
    QTextStream debugS(stdout);
    debugS << "beam " << o << " changed" << endl;
#endif
}

void wgv_tw::CommandRemoveBeam(const int &o)
{
    isCalc = false;
    calculationThread = 0;
    for(int i=0; i<load_n(); )
    {
        if(load(i).isOnBeam())
            {
            if(load(i).at()==o)
            {
                emit removed(i, load(i));
                continue;
            }
            else if(load(i).at()>o)
            {
                loads[i].setBeam(load(i).at()-1);
            }
        }
        i++;
    }

    beams.removeAt(o);
    if(lOBeam == o)
        lOBeam--;
    emit erasedBeam(o);
}

void wgv_tw::CommandInsertBeam(const int &o,const wgv_beam &n)
{
    isCalc = false;
    calculationThread = 0;
    for(int i=0; i<load_n(); )
    {
        if(load(i).isOnBeam())
            {
            if(!(load(i).at()<o))
            {
                loads[i].setBeam(load(i).at()+1);
            }
        }
        i++;
    }
    beams.insert(o, n);

    emit addedBeam(o);
}

void wgv_tw::CommandEditLoad(const int &o, const wgv_load &n)
{
    isCalc = false;
    calculationThread = 0;
    loads[o]=n;
    emit changedLoad(o);
}

void wgv_tw::CommandRemoveLoad(const int &o)
{
    isCalc = false;
    calculationThread = 0;
    loads.removeAt(o);
    emit erasedLoad(o);
}

void wgv_tw::CommandInsertLoad(const int &o, const wgv_load &n)
{
    isCalc = false;
    calculationThread = 0;
    loads.insert(o, n);
    emit addedLoad(o);
}

int wgv_tw::addNode(const wgv_node &n)
{
    isCalc = false;
    calculationThread = 0;
    emit appended(n);
    return node_n()-1;
}

void wgv_tw::editNode(const int &k, const  wgv_node &n)
{
    Q_ASSERT(!(k<0 || k>=node_n()));
    emit edited(k, n);

}

void wgv_tw::setBearing(const int &k, const wgv_bearing &b)
{
    Q_ASSERT(!(k<0 || k>=node_n()));
    wgv_node after=nodes[k];
    if(b.form() == wgv_bearing::NoBear)
        after.removeBearing();
    else
        after.setBearing(b);
    emit edited(k, after);

}

void wgv_tw::removeNode(const int &o)
{
    Q_ASSERT(!(o<0 || o>=node_n()));
    emit beginS(tr("remove node %1").arg(o));
    emit removed(o, nodes.at(o));
    emit endS();
}

void wgv_tw::setX(const int &o, const double &xn)
{
    Q_ASSERT(!(o<0 || o>=node_n()));
    wgv_node n(node(o));
    n.setX(xn);
    emit edited(o, n);
}

void wgv_tw::setZ(const int &o, const double &zn)
{
    Q_ASSERT(!(o<0 || o>=node_n()));
    wgv_node n(node(o));
    n.setZ(zn);
    emit edited(o, n);
}

void wgv_tw::setBx(const int &o, const bool &hn)
{
    Q_ASSERT(!(o<0 || o>=node_n()));
    wgv_node n(node(o));
    if(!n.hasbearing())
        n.setBearing(wgv_bearing());
    n.bearing_editable().setX(hn);
    if(n.bearing().form() == wgv_bearing::NoBear)
        n.removeBearing();
    emit edited(o, n);
}

void wgv_tw::setBz(const int &o, const bool &vn)
{
    Q_ASSERT(!(o<0 || o>=node_n()));
    wgv_node n(node(o));
    if(!n.hasbearing())
        n.setBearing(wgv_bearing());
    n.bearing_editable().setZ(vn);
    if(n.bearing().form() == wgv_bearing::NoBear)
        n.removeBearing();
    emit edited(o, n);
}

void wgv_tw::setBm(const int &o, const bool &mn)
{
    Q_ASSERT(!(o<0 || o>=node_n()));
    wgv_node n(node(o));
    if(!n.hasbearing())
        n.setBearing(wgv_bearing());
    n.bearing_editable().setPhi(mn);
    if(n.bearing().form() == wgv_bearing::NoBear)
        n.removeBearing();
    emit edited(o, n);
}

void wgv_tw::setCx(const int &o, const double &fn)
{
    Q_ASSERT(!(o<0 || o>=node_n()));
    wgv_node n(nodes[o]);
    n.bearing_editable().setCx(fn);
    if(n.bearing().form() == wgv_bearing::NoBear)
        n.removeBearing();
    emit edited(o, n);
}

void wgv_tw::setCz(const int &o, const double &fn)
{
    Q_ASSERT(!(o<0 || o>=node_n()));
    wgv_node n(nodes[o]);
    n.bearing_editable().setCz(fn);
    if(n.bearing().form() == wgv_bearing::NoBear)
        n.removeBearing();
    emit edited(o, n);
}

void wgv_tw::setCm(const int &o, const double &fn)
{
    Q_ASSERT(!(o<0 || o>=node_n()));
    wgv_node n(nodes[o]);
    n.bearing_editable().setCphi(fn);
    if(n.bearing().form() == wgv_bearing::NoBear)
        n.removeBearing();
    emit edited(o, n);
}

void wgv_tw::setAngle(const int &o, const double &alphan)
{
    Q_ASSERT(!(o<0 || o>=node_n()));
    wgv_node n(nodes[o]);
    n.bearing_editable().setAngle(alphan);
    emit edited(o, n);
}

void wgv_tw::setXZ(const int &o, const double &xn, const double &zn)
{
    Q_ASSERT(!(o<0 || o>=node_n()));
    wgv_node after=nodes[o];
    after.setX(xn);
    after.setZ(zn);
    emit edited(o, after);
}

int wgv_tw::knoten_von_auflager(const int &o) const
{
    int j=-1;
    int m;
    for(m=0; m<node_n(); m++)
    {
        if(nodes.at(m).hasbearing())
            j++;
        if(j==o)
            break;
    }
    return m;
}

int wgv_tw::addBeam(const wgv_beam &n)
{
    if(n.leftNodeI() == n.rightNodeI())
        return -1;
    emit appended(n);
    return beam_n()-1;
}

void wgv_tw::editBeam(const int &o, const wgv_beam &n)
{
    Q_ASSERT(!(o<0 || o>=beam_n()));
    emit edited(o, n);
}

void wgv_tw::removeBeam(const int &o)
{
    Q_ASSERT(!(o<0 || o>=beam_n()));
    emit beginS(tr("remove beam %1").arg(o));
    emit removed(o, beams.at(o));
    emit endS();
}

void wgv_tw::setLeft(const int &o, const int &Ln)
{
    Q_ASSERT(!(o<0 || o>=beam_n()));
    Q_ASSERT(!(Ln<0 || Ln>=node_n()));
    wgv_beam b(beam(o));
    b.setLeftNode(Ln);
    emit edited(o, b);
}

void wgv_tw::setRight(const int &o, const int &Rn)
{
    Q_ASSERT(!(o<0 || o>=beam_n()));
    Q_ASSERT(!(Rn<0 || Rn>=node_n()));
    wgv_beam b(beam(o));
    b.setRightNode(Rn);
    emit edited(o, b);
}

void wgv_tw::setJointsandSprings(const int &beamnr, const bool *joi_array_6, const double *spr_array_6)
{
    Q_ASSERT(!(beamnr<0 || beamnr>=beam_n()));
    wgv_beam after = beams[beamnr];
    after.setJointsandSprings(joi_array_6, spr_array_6);
    emit edited(beamnr, after);
}

void wgv_tw::setJoint(const int &beamnr, const int &pos, const bool &thereIsAJoint)
{
    Q_ASSERT(!(beamnr<0 || beamnr>=beam_n()));
    wgv_beam b(beams[beamnr]);
    b.setJoint(pos, thereIsAJoint);
    emit edited(beamnr, b);
}

void wgv_tw::setSpring(const int &beamnr, const int &pos, const double &c_f)
{
    Q_ASSERT(!(beamnr<0 || beamnr>=beam_n()));
    wgv_beam b(beams[beamnr]);
    b.setSpring(pos, c_f);
    emit edited(beamnr, b);
}

void wgv_tw::setMat(const int &o, const int &matIndex)
{
    Q_ASSERT(!(o<0 || o>=beam_n()));
    wgv_beam b(beams[o]);
    b.setMat(matIndex);
    emit edited(o, b);
}

void wgv_tw::setProfile(const int &o, const int &QsIndex)
{
    Q_ASSERT(!(o<0 || o>=beam_n()));
    wgv_beam b(beams[o]);
    b.setQs(QsIndex);
    emit edited(o, b);
}

void wgv_tw::setMat(const int &o, const QString &matName)
{
    setMat(o, LIB.IndexFromMatByName(matName));
}

void wgv_tw::setProfile(const int &o, const QString &QsName)
{
    setProfile(o, LIB.IndexFromProfileByName(QsName));
}

int wgv_tw::addLoad(const wgv_load &n)
{
    emit appended(n);
    return load_n()-1;
}

void wgv_tw::editLoad(const int &o, const wgv_load &n)
{
    Q_ASSERT(!(o<0 || o>=load_n()));
    emit edited(o, n);
}

void wgv_tw::removeLoad(const int &o)
{
    Q_ASSERT(!(o<0 || o>=load_n()));
    emit removed(o, loads.at(o));
}

void wgv_tw::setPx(const int &o, const double &Pxn)
{
    Q_ASSERT(!(o<0 || o>=load_n()));
    wgv_load l(load(o));
    l.setPx(Pxn);
    emit edited(o, l);
}

void wgv_tw::setPz(const int &o, const double &Pzn)
{
    Q_ASSERT(!(o<0 || o>=load_n()));
    wgv_load l(load(o));
    l.setPz(Pzn);
    emit edited(o, l);
}

void wgv_tw::setM(const int &o, const double &Mn)
{
    Q_ASSERT(!(o<0 || o>=load_n()));
    wgv_load l(load(o));
    l.setM(Mn);
    emit edited(o, l);
}

void wgv_tw::setLoadTyp(const int &o, const wgv_load::form &Typn)
{
    Q_ASSERT(!(o<0 || o>=load_n()));
    /*
        puh
    */
    wgv_load l(load(o));
    l.setTyp(Typn);
    emit edited(o, l);
}

void wgv_tw::setPos(const int &o, const int &Pos)
{
    Q_ASSERT(!(o<0 || o>=load_n()));
    wgv_load l(load(o));
    l.setBeam(Pos);
    emit edited(o, l);
}

QTransform &wgv_tw::view(int i)
{
    for(int id=views.size()-1; id<i+1; id++)
    {
        views.append(QTransform(100, 0,     0,
                                0,   100,   0,
                                70,  70,    1));
    }
    return views[i];
}

void wgv_tw::setView(int i, const QTransform &t)
{
    for(int id=views.size()-1; id<i+1; id++)
    {
        views.append(QTransform(100, 0,     0,
                                0,   100,   0,
                                70,  70,    1));
    }
    views[i]=t;
}

int wgv_tw::nearestNode(const QPointF &p, const double &maxabs) const
{
    for(int i=0; i<node_n(); i++)
    {
        if((p-node(i).toQPointF()).manhattanLength() < maxabs)
            return i;
    }
    return -1;
}

int wgv_tw::nearestBeam(const QPointF &p, const double &maxabs, QPointF *n) const
{
    double m;
    QPointF nearest;
    for(int i=0; i<beam_n(); i++)
    {
        QRectF rect = beam(i).boundingRect().normalized();
        rect.adjust(-maxabs, -maxabs, maxabs, maxabs);
        if(rect.contains(p))
        {
            if(beam(i).leftNode().z() == beam(i).rightNode().z())
            {
                if(fabs(p.y()-beam(i).leftNode().z()) < maxabs)
                {
                    if(n!=0)
                    {
                        n->setX(p.x());
                        n->setY(beam(i).leftNode().z());
                    }
                    return i;
                }
            }
            if(beam(i).leftNode().x() == beam(i).rightNode().x())
            {
                if(fabs(p.x()-beam(i).leftNode().x()) < maxabs)
                {
                    if(n!=0)
                    {
                        n->setY(p.y());
                        n->setX(beam(i).leftNode().x());
                    }
                    return i;
                }
            }
            m=(beam(i).rightNode().z()-beam(i).leftNode().z())/
              (beam(i).rightNode().x()-beam(i).leftNode().x());
            nearest.setX((p.y()+p.x()/m-beam(i).leftNode().z()+m*beam(i).leftNode().x())/
                         (1/m+m));
            nearest.setY(m*nearest.x()+beam(i).leftNode().z()-m*beam(i).leftNode().x());
            if((nearest-p).manhattanLength()<maxabs)
            {
                if(n != 0)
                {
                    *n=nearest;
                }
                return i;
            }
        }
    }
    return -1;
}

void wgv_tw::crossingBeams(const QPointF &p, const double &maxabs, int *first, int *second, QPointF *n) const
{
/*
#ifndef QT_NO_DEBUG
    QTextStream debugS(stdout);
    debugS << "snap: crossings beams" << endl;
    debugS << "&&&&&&&&&&&&&&&&&&&&&" << endl;
    debugS << "MousePos = " << p.x() << ", " << p.y() << endl;
#endif
*/

    QList<int> c;
    for(int i=0; i<beam_n(); i++)
    {
	QRectF rect = beam(i).boundingRect().normalized();
/*
#ifndef QT_NO_DEBUG
        debugS << "rect" << i << " = "  << rect.x() <<  ", "<< rect.y() << ", " << rect.width() << ", " << rect.height() << endl;
#endif
*/
        rect.adjust(-maxabs, -maxabs, maxabs, maxabs);
/*
#ifndef QT_NO_DEBUG
        debugS << "adjR" << i << " = "  << rect.x() <<  ", "<< rect.y() << ", " << rect.width() << ", " << rect.height() << endl;
#endif
*/
        if(rect.contains(p))
	    c.append(i);
    }
/*
#ifndef QT_NO_DEBUG
    debugS << "candidates" << endl;
    foreach(int i, c)
        debugS << "beam " << i << endl;
#endif
*/
    double m[c.size()];
    double b[c.size()];

    for(int i=0; i<c.size(); i++)
    {
	m[i] = (beam(c.at(i)).rightNode().z()-beam(c.at(i)).leftNode().z())/
	       (beam(c.at(i)).rightNode().x()-beam(c.at(i)).leftNode().x());


	b[i] = beam(c.at(i)).leftNode().z()-beam(c.at(i)).leftNode().x()*m[i];
    }
/*
#ifndef QT_NO_DEBUG
    debugS << "vars:" << endl;
    int j=0;
    foreach(int i, c){
        debugS << "m_" << i << " = " << m[j] << " , b_" << i << " = " << b[j] << endl;
        j++;}
#endif
*/
    for(int i=0; i<c.size(); i++)
    {
	for(int j=i+1; j<c.size(); j++)
	{
	    if(qFuzzyCompare(m[i],m[j]))			continue;

	    double x_min = qMin(beam(c.at(i)).leftNode().x(),
				qMin(beam(c.at(i)).rightNode().x(),
				     qMin(beam(c.at(j)).leftNode().x(),
					  beam(c.at(j)).rightNode().x()
					  )
				     )
				);

	    double x_max = qMax(beam(c.at(i)).leftNode().x(),
				qMax(beam(c.at(i)).rightNode().x(),
				     qMax(beam(c.at(j)).leftNode().x(),
					  beam(c.at(j)).rightNode().x()
					  )
				     )
				);

            QPointF cross;

            //savety for vertical beams
            if(qFuzzyCompare(beam(c.at(i)).leftNode().x(), beam(c.at(i)).rightNode().x()))
            {
                double x = beam(c.at(i)).leftNode().x();
                cross = QPointF(x, m[j]*x+b[j]);
            }
            else if(qFuzzyCompare(beam(c.at(j)).leftNode().x(), beam(c.at(j)).rightNode().x()))
            {
                double x = beam(c.at(j)).leftNode().x();
                cross = QPointF(x, m[i]*x+b[i]);
            }
            else
            {
                double x = (b[j]-b[i])/(m[i]-m[j]);              //every angle != +-pi/2
                cross = QPointF(x, m[i]*x+b[i]);
            }

            if(cross.x()>x_min && cross.x()<x_max)
	    {

		if((p-cross).manhattanLength()<maxabs)
		{
		    *first = c.at(i);
		    *second = c.at(j);
		    if(n != 0)
		    {
			*n = cross;
		    }
		    return;
		}

	    }

	}
    }
    *first = -1;
    *second =  -1;
}

int wgv_tw::nearestLoad(const QPointF &p, const double &maxabs) const
{
    for(int i=0; i<load_n(); i++)
    {
	if(load(i).typ() == wgv_load::nodeLoad)
	{
            if(load(i).Px() == 0)
            {
                if(fabs(p.x()-node(load(i).at()).x()) < maxabs)
                {
                    if(p.y()>load(i).shape()[0].y() && p.y()<load(i).shape()[1].y())
                        return i;
                    if(p.y()<load(i).shape()[0].y() && p.y()>load(i).shape()[1].y())
                        return i;
                }
            }
            if(load(i).Pz() == 0)
            {
                if(p.x()>load(i).shape()[0].x() && p.x()<load(i).shape()[1].x())
                    return i;
                if(p.x()<load(i).shape()[0].x() && p.x()>load(i).shape()[1].x())
                    return i;
            }
	    QRectF rect;
            rect.setTopLeft(node(load(i).at()).toQPointF());
            rect.setWidth(-load(i).Px());
            rect.setHeight(-load(i).Pz());
            rect.normalized();
	    rect.adjust(-maxabs, -maxabs, maxabs, maxabs);
	    if(rect.contains(p))
	    {

		double m= (load(i).Pz())/
			  (load(i).Px());
		QPointF nearest;
		nearest.setX((p.y()+p.x()/m-node(load(i).at()).z()+m*node(load(i).at()).x())/
			     (1/m+m));
		nearest.setY(m*nearest.x()+node(load(i).at()).z()-m*node(load(i).at()).x());
		if((nearest-p).manhattanLength()<maxabs)
		{
		    return i;
		}
	    }
        }
        else if(load(i).typ() == wgv_load::increasingLinearLoad	||
                load(i).typ() == wgv_load::decreasingLinearLoad	||
                load(i).typ() == wgv_load::uniformlyDistibutedLoad )
        {

            if(load(i).shape().containsPoint(p, Qt::OddEvenFill))
                return i;
        }
    }

    return -1;
}

void wgv_tw::removeViewPort(int i)
{
    views.removeAt(i);
}

int wgv_tw::bestimmt()
{
    int aa=0;
    int gg=0;
    for(int u=0; u<node_n(); u++)
    {
        if(node(u).hasbearing())
        {
            if(node(u).bearing().x() || node(u).bearing().c_x()!=0)
                aa++;
            if(node(u).bearing().z() || node(u).bearing().c_z()!=0)
                aa++;
            if(node(u).bearing().phi() || node(u).bearing().c_phi()!=0)
                aa++;
        }
    }
    for(int u=0; u<beam_n(); u++)
    {
        for(int i=0; i<6; i++)
            if(beam(u).joint(i) && beam(u).spring(i)==0)
                gg++;
    }
    return((aa+beam_n()*3)-(node_n()*3+gg));

}

int wgv_tw::bestimmt(int *aa, int *ss, int *kk, int *gg, int *ff)
{
    (*aa)=0;
    (*gg)=0;
    (*ff)=0;
    (*ss)=beam_n();
    (*kk)=node_n();
    for(int u=0; u<node_n(); u++)
    {
        if(node(u).bearing().x() || node(u).bearing().c_x()!=0)
            *aa++;
        if(node(u).bearing().z() || node(u).bearing().c_z()!=0)
            *aa++;
        if(node(u).bearing().phi() || node(u).bearing().c_phi()!=0)
            *aa++;
    }
    for(int u=0; u<beam_n()*6; u++)
    {
        for(int m=0; m<6; m++)
        {
            if(beam(u).joint(m))
                (*gg)++;
            if(beam(u).spring(m)!=0)
            {
                (*ff)++;
            }
        }
    }
    return(((*aa)+(*ff)+(*ss)*3)-((*kk)*3+(*gg)));

}

void wgv_tw::selectAll()
{
    for(int i=0; i<nodes.size(); i++)
    {
        nodes[i].select();
    }
    for(int i=0; i<beams.size(); i++)
    {
        beams[i].select();
    }
    for(int i=0; i<loads.size(); i++)
    {
        loads[i].select();
    }
    emit selectionChanged();
}

void wgv_tw::deselectAll()
{
    {
        for(int i=0; i<nodes.size(); i++)
        {
            nodes[i].select(false);
        }
        for(int i=0; i<beams.size(); i++)
        {
            beams[i].select(false);
        }
        for(int i=0; i<loads.size(); i++)
        {
            loads[i].select(false);
        }
    }
    emit selectionChanged();
}

void wgv_tw::invertSelection()
{
    {
        for(int i=0; i<nodes.size(); i++)
        {
            nodes[i].select(!nodes.at(i).isSelected());
        }
        for(int i=0; i<beams.size(); i++)
        {
            beams[i].select(!beams.at(i).isSelected());
        }
        for(int i=0; i<loads.size(); i++)
        {
            loads[i].select(!loads.at(i).isSelected());
        }
    }
    emit selectionChanged();
}

void wgv_tw::selectRect(const QRectF &r, bool keepSelect)
{

#ifndef QT_NO_DEBUG
    QTextStream debugS(stdout);
    debugS << "selection" << endl << "rect(l, r, t, b) = " << r.left() << ", " << r.y() << ", " << r.top() << ", " << r.bottom() << endl;
#endif

    for(int i=0; i<node_n(); i++)
    {
        if(r.contains(node(i).toQPointF()))
        {
            nodes[i].select();
            if(node(i).hasbearing())
                nodes[i].bearing_editable().select();
        }
        else if(node(i).isSelected())
        {
            nodes[i].select(keepSelect);
            if(node(i).hasbearing())
                nodes[i].bearing_editable().select(keepSelect);
        }
    }
/*
#ifndef QT_NO_DEBUG
            debugS << "selecting beams" << endl;
#endif
*/
    for(int i=0; i<beam_n(); i++)
    {


        if(r.contains(beam(i).leftNode().toQPointF()) ||
           r.contains(beam(i).rightNode().toQPointF()))
        {
            beams[i].select(true);
            continue;
        }

        QRectF bR = QRectF(beam(i).leftNode().toQPointF(),
                           beam(i).rightNode().toQPointF());
        bR = bR.normalized();
        bR.adjust(-0.01, -0.01, 0.01, 0.01);


        if(r.intersects(bR))
        {
            if(qFuzzyCompare(beam(i).leftNode().x(), beam(i).rightNode().x()) ||
               qFuzzyCompare(beam(i).leftNode().z(), beam(i).rightNode().z()) )
            {
                beams[i].select(true);
                continue;
            }

            double m = (beams[i].rightNode().z()-beams[i].leftNode().z())/
                       (beams[i].rightNode().x()-beams[i].leftNode().x());
            double b =beams[i].leftNode().z()-beams[i].leftNode().x()*m;
            double y;
/*
#ifndef QT_NO_DEBUG
            debugS << "beam " << i << "intersects with bounding rect" << endl;
            debugS << "m = " << m << endl;
            debugS << "b = " << b << endl;
            debugS << "y1(left) = " << m*r.left()+b << endl;
            debugS << "y2(right) = " << m*r.right()+b << endl;
            debugS << "x1(top) = " << (r.top()-b)/m << endl;
            debugS << "x2(bottm) = " << (r.bottom()-b)/m << endl;
#endif
*/
            //testing for crossing a side of the rectangle
            y = m*r.left()+b; //y=m*x+b
            if(y <= r.bottom() && y >= r.top()){
                beams[i].select(true);
                continue;}

            y = m*r.right()+b; //y=m*x+b
            if(y <= r.bottom() && y >= r.top()){
                beams[i].select(true);
                continue;}


            double &x = y; //namechanging!  \ο/
            x = (r.top()-b)/m; //x = (y-b)/m
            if(x <= r.right() && x >= r.left()){
                beams[i].select(true);
                continue;}

            //last side is unnecesary...


        }

        if(beam(i).isSelected())
	    beams[i].select(keepSelect);
    }
    for(int i=0; i<load_n(); i++)
    {
        if(load(i).typ() == wgv_load::tempChange ||
           load(i).typ() == wgv_load::tempDiffrence)
        {
            if(r.contains(beam(load(i).at()).middleP()))
                loads[i].select(true);
            else if(load(i).isSelected())
                loads[i].select(keepSelect);
            continue;
        }
        else if(load(i).typ() == wgv_load::doubleLoadLeft)
        {
            if(r.contains(beam(load(i).at()).leftNode().toQPointF()))
                loads[i].select(true);
            else if(load(i).isSelected())
                loads[i].select(keepSelect);
        }
        else if(load(i).typ() == wgv_load::doubleLoadRight)
        {
            if(r.contains(beam(load(i).at()).rightNode().toQPointF()))
                loads[i].select(true);
            else if(load(i).isSelected())
                loads[i].select(keepSelect);
        }
        else if(load(i).typ() == wgv_load::moment)
        {
            if(r.contains(node(load(i).at()).toQPointF()))
                loads[i].select(true);
            else if(load(i).isSelected())
                loads[i].select(keepSelect);
        }
        else if(load(i).shape().boundingRect().normalized().adjusted(-0.01, -0.01, 0.01, 0.01).intersects(r) ||
           load(i).shape().boundingRect().normalized().adjusted(-0.01, -0.01, 0.01, 0.01).contains(r)	||
           r.contains(load(i).shape().boundingRect().normalized().adjusted(-0.01, -0.01, 0.01, 0.01))   )
        {
            loads[i].select();
        }
        else if(load(i).isSelected())
            loads[i].select(keepSelect);
    }

    emit selectionChanged();
}

QRectF wgv_tw::boundingRect(const bool &alsoLoads) const
{
    calculateShapes();
    if(node_n() < 1)
        return QRectF(0, 0, 0, 0);
    QRectF bound(node(0).toQPointF(), QSizeF(0.0001, 0.0001));
    foreach(wgv_node n, nodes)
    {
        bound.setLeft(      qMin(      bound.left(),    n.x()));
        bound.setRight(     qMax(      bound.right(),   n.x()));
        bound.setTop(       qMin(      bound.top(),     n.z()));
        bound.setBottom(    qMax(      bound.bottom(),  n.z()));
    }
    if(alsoLoads)
    {
        for(int i=0; i<load_n(); i++)
        {
            if(load(i).typ() == wgv_load::decreasingLinearLoad    ||
               load(i).typ() == wgv_load::increasingLinearLoad    ||
               load(i).typ() == wgv_load::uniformlyDistibutedLoad )
            {
                for(int n=1; n<3; n++)
                {
                    bound.setLeft(      qMin(      bound.left(),    load(i).shape()[n].x()));
                    bound.setRight(     qMax(      bound.right(),   load(i).shape()[n].x()));
                    bound.setTop(       qMin(      bound.top(),     load(i).shape()[n].y()));
                    bound.setBottom(    qMax(      bound.bottom(),  load(i).shape()[n].y()));
                }
            }
            if(load(i).typ() == wgv_load::nodeLoad)
            {
                bound.setLeft(      qMin(      bound.left(),    load(i).shape()[1].x()));
                bound.setRight(     qMax(      bound.right(),   load(i).shape()[1].x()));
                bound.setTop(       qMin(      bound.top(),     load(i).shape()[1].y()));
                bound.setBottom(    qMax(      bound.bottom(),  load(i).shape()[1].y()));
            }
        }
    }
    return bound.normalized();
}

double wgv_tw::mMax() const
{
    double m = 0;
    foreach(wgv_load l, loads)
    {
	if(l.typ() == wgv_load::moment			||
	   l.typ() == wgv_load::doubleLoadLeft		||
	   l.typ() == wgv_load::doubleLoadRight	        )
            m = qMax(m, fabs(l.M()));
    }
    if(isCalculated())
        m = qMax(m, globalMMax);
    return qMax(m, 1.);
}

double wgv_tw::pMax() const
{
    double p = 0;
    foreach(wgv_load l, loads)
    {
        if(l.typ() == wgv_load::nodeLoad                ||
           l.typ() == wgv_load::uniformlyDistibutedLoad ||
           l.typ() == wgv_load::increasingLinearLoad    ||
	   l.typ() == wgv_load::decreasingLinearLoad    ||
	   l.typ() == wgv_load::doubleLoadLeft		||
	   l.typ() == wgv_load::doubleLoadRight	        )
            p = qMax(p, sqrt(pow(l.Px(), 2)+pow(l.Pz(), 2)));
    }
    if(isCalculated())
        p = qMax(p, globalPMax);
    return qMax(p, 1.);
}

double wgv_tw::uMax() const
{
    return globalUMax;
}

bool wgv_tw::isSelectionEmpty()
{
    for(int i=0; i<node_n(); i++)
    {
        if(node(i).isSelected())
            return false;
    }
    for(int i=0; i<beam_n(); i++)
    {
        if(beam(i).isSelected())
            return false;
    }
    for(int i=0; i<load_n(); i++)
    {
        if(load(i).isSelected())
            return false;
    }
    return true;
}

void wgv_tw::deleteSelection()
{
    beginS(tr("delete"));
    for(int i=0; i<load_n();)
    {
#ifndef QT_NO_DEBUG
     QTextStream debugg(stdout);
     debugg <<"load[" <<  i << (load(i).isSelected() ? "] is selected" : "] is not selected") << endl;
 #endif

	if(load(i).isSelected())
	{
	    removeLoad(i);
#ifndef QT_NO_DEBUG
	    debugg <<"removed load[" << i << "]" << endl;
#endif
	}
	else
	{
	    i++;
	}
    }

    for(int i=0; i<beam_n();)
    {
#ifndef QT_NO_DEBUG
     QTextStream debugg(stdout);
     debugg <<"beam[" <<  i << (beam(i).isSelected() ? "] is selected" : "] is not selected") << endl;
 #endif

	if(beam(i).isSelected())
	{
	    removeBeam(i);
#ifndef QT_NO_DEBUG
	    debugg <<"removed beam[" << i << "]" << endl;
#endif
	}
	else
	{
	    i++;
	}
#ifndef QT_NO_DEBUG
		    debugg <<"next beam = beam[" <<  i << "]" << endl;
#endif
    }

    for(int i=0; i<node_n();)
    {
#ifndef QT_NO_DEBUG
     QTextStream debugg(stdout);
     debugg <<"node[" <<  i << (node(i).isSelected() ? "] is selected" : "] is not selected") << endl;
 #endif

	if(node(i).isSelected())
	{
	    removeNode(i);
#ifndef QT_NO_DEBUG
	    debugg <<"removed node[" << i << "]" << endl;
#endif
	}
	else
	{
	    i++;
	}
#ifndef QT_NO_DEBUG
		    debugg <<"next node = node[" <<  i << "]" << endl;
#endif
    }
    endS();
}

void wgv_tw::setScaleM(const double &s)
{
    if(this->mMax()*s>5000)
        return;
    scale_m=s;
    emit changedMScale(scale_m);
}

void wgv_tw::setScaleP(const double &s)
{
    if(this->pMax()*s>5000)
        return;
    scale_p=s;
    emit changedPScale(scale_p);
}

void wgv_tw::setScaleU(const double &s)
{
    if(this->uMax()*s>5000)
        return;
    scale_u=s;
    emit changedUScale(scale_u);
}

void wgv_tw::calculateShapes() const
{
    wgv_tw *t = const_cast<wgv_tw*>(this);
    QMap<int, QLineF> line;
    for(int i=0; i<load_n(); i++)
    {
        if(load(i).typ() == wgv_load::decreasingLinearLoad    ||
           load(i).typ() == wgv_load::uniformlyDistibutedLoad ||
           load(i).typ() == wgv_load::increasingLinearLoad    )
	{
	    if(!line.contains(load(i).at()))
	    {
		line.insert(load(i).at(),
			    (beam(load(i).at()).toQLineF()));
	    }
	    QPolygonF points(4);

	    points[0] = line[load(i).at()].p1();
	    points[3] = line[load(i).at()].p2();


	    if(load(i).typ() == wgv_load::uniformlyDistibutedLoad)
	    {
		points[1] = points[0]-QPointF(load(i).Px(), load(i).Pz())*scaleP();
		points[2] = points[3]-QPointF(load(i).Px(), load(i).Pz())*scaleP();
	    }
	    else if(load(i).typ() == wgv_load::increasingLinearLoad)
	    {
		points[1]=points[0];
		points[2]= points[3]-QPointF(load(i).Px(), load(i).Pz())*scaleP();
	    }
	    else if(load(i).typ() == wgv_load::decreasingLinearLoad)
	    {
		points[1] = points[0]-QPointF(load(i).Px(), load(i).Pz())*scaleP();
		points[2] = points[3];
            }

	    t->loads[i].setShape(points);


	    line[load(i).at()].setP1(points[1]);
	    line[load(i).at()].setP2(points[2]);
	}
        else if(load(i).typ() == wgv_load::tempChange ||
                load(i).typ() == wgv_load::tempDiffrence)
        {
            QRectF rect = QRectF(beam(load(i).at()).middleP()-QPointF(10.*70, 100.*70), QSizeF(20.*70, 100.*70));

            QPolygonF points(4);
            points[0] = rect.topLeft();
            points[1] = rect.topRight();
            points[2] = rect.bottomRight();
            points[3] = rect.bottomLeft();
        }
        else if(load(i).typ() == wgv_load::nodeLoad)
        {
            QPolygonF points(2);
            points[0] = node(load(i).at()).toQPointF();
            points[1] = node(load(i).at()).toQPointF()-QPointF(load(i).Px(), load(i).Pz())*scaleP();
            t->loads[i].setShape(points);
        }
    }

}

void wgv_tw::mergeWith(wgv_tw *tw, QPointF dp)
{
    emit beginS(tr("paste"));
    QMap<int, int> n;
    int beamCount = beam_n();

    for(int i=0; i<tw->node_n(); i++)
    {
        tw->nodes[i].setX(tw->node(i).x()+dp.x());
        tw->nodes[i].setZ(tw->node(i).z()+dp.y());
    }


    if(node_n()==0)
    {
        for(int i=0; i<tw->node_n(); i++)
        {
            n.insert(i, addNode(tw->node(i)));
        }
    }
    else
    {
        for(int i=0; i<tw->node_n(); i++)
        {
            for(int j=0; j<node_n(); j++)
            {
                if(qFuzzyCompare(tw->node(i).x(), node(j).x()) &&
                   qFuzzyCompare(tw->node(i).z(), node(j).z()) )
                {
                    if(tw->node(i).hasbearing())
                    {
                        if(node(j).hasbearing())
                        {
                            wgv_bearing bear;
                            if(tw->node(i).bearing().x() || node(j).bearing().x())
                                bear.setX(true);
                            else if(tw->node(i).bearing().xf() || node(j).bearing().xf())
                                bear.setCx(tw->node(i).bearing().xf() + node(j).bearing().xf());
                            else
                                bear.setX(false);

                            if(tw->node(i).bearing().z() || node(j).bearing().z())
                                bear.setZ(true);
                            else if(tw->node(i).bearing().zf() || node(j).bearing().zf())
                                bear.setCz(tw->node(i).bearing().zf() + node(j).bearing().zf());
                            else
                                bear.setZ(false);

                            if(tw->node(i).bearing().phi() || node(j).bearing().phi())
                                bear.setPhi(true);
                            else if(tw->node(i).bearing().phif() || node(j).bearing().phif())
                                bear.setCphi(tw->node(i).bearing().phif() + node(j).bearing().phif());
                            else
                                bear.setPhi(false);
                        }
                        else
                            setBearing(j, tw->node(i).bearing());
                    }
                    n.insert(i, j);
                    break;
                }
                else if(j == node_n()-1)
                {
                    n.insert(i, addNode(tw->node(i)));
                }
            }
        }
    }

    for(int i=0; i<tw->beam_n(); i++)
    {
        /*
        wgv_beam b(this,
                   n.value(tw->beam(i).leftNodeI(),-1),
                   n.value(tw->beam(i).rightNodeI(),-1),
                   tw->beam(i).MatI(),
                   tw->beam(i).ProfileI());
        */
        wgv_beam b(tw->beam(i));
        b.setLeftNode(n.value(b.leftNodeI(), -1));
        b.setRightNode(n.value(b.rightNodeI(), -1));

        Q_ASSERT(tw->beam(i).leftNodeI() != -1);
        Q_ASSERT(tw->beam(i).rightNodeI() != -1);

        b.setParent(this);
        addBeam(b);
    }
    for(int i=0; i<tw->load_n(); i++)
    {
        if(tw->load(i).typ() == wgv_load::decreasingLinearLoad    ||
           tw->load(i).typ() == wgv_load::increasingLinearLoad    ||
           tw->load(i).typ() == wgv_load::uniformlyDistibutedLoad ||
           tw->load(i).typ() == wgv_load::tempChange              ||
           tw->load(i).typ() == wgv_load::tempDiffrence           ||
           tw->load(i).typ() == wgv_load::doubleLoadLeft          ||
           tw->load(i).typ() == wgv_load::doubleLoadRight         )
        {
            wgv_load l = tw->load(i);
            l.setBeam(tw->load(i).at()+beamCount);
            Q_ASSERT(l.at()<beam_n() && l.at()>-1);
            addLoad(l);
        }
        else
        {
            tw->loads[i].setBeam(n.value(tw->load(i).at()));
            addLoad(tw->load(i));
        }
    }



    emit endS();
}
