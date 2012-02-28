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


#include "colinstruct.h"

ColinStruct::ColinStruct() :
QObject()
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
	n_res = 0;
	a_res = 0;
	b_res = 0;

    LIB.setUp();

    connect(&LIB,           SIGNAL(matEdited(int)),
            this,           SLOT(resetCals()));

    connect(&LIB,           SIGNAL(profileEdited(int)),
            this,           SLOT(resetCals()));

}

bool ColinStruct::isCalculated()const
{
    return isCalc;
}

void ColinStruct::CommandEditNode(const int &k, const ColinNode &n)
{

	resetResults();
    nodes[k]=n;
	nodes[k].setStruct(this);
    for(int i=0; i<beams.size(); i++)
    {
        if(beam(i).leftNodeI() == k || beam(i).rightNodeI() == k)
            beam(i).setDirty();
    }
    emit changedNode(k);    

#ifdef CHANGES_VERBOSE
	qDebug() << "node " << k << " changed" ;
#endif
}

void ColinStruct::CommandRemoveNode(const int &k)
{
	resetResults();
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
#ifdef CHANGES_VERBOSE
	qDebug() << "node " << k << " removed" ;
#endif
}

void ColinStruct::CommandInsertNode(const int &o, const ColinNode &n)
{
	resetResults();
    nodes.insert(o, n);
	nodes[o].setStruct(this);
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
#ifdef CHANGES_VERBOSE
	qDebug() << "beam " << o << " inserted" ;
#endif
}

void ColinStruct::CommandEditBeam(const int &o, const ColinBeam &n)
{
	resetResults();
    beams[o]=n;
	beams[o].setStruct(this);
    emit changedBeam(o);
#ifdef CHANGES_VERBOSE
	qDebug() << "beam " << o << " changed" ;
#endif
}

void ColinStruct::CommandRemoveBeam(const int &o)
{
	resetResults();
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

#ifdef CHANGES_VERBOSE
	qDebug() << "beam " << o << " removed" ;
#endif
}

void ColinStruct::CommandInsertBeam(const int &o,const ColinBeam &n)
{
	resetResults();
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
	beams[o].setStruct(this);

    emit addedBeam(o);

#ifdef CHANGES_VERBOSE
	qDebug() << "beam " << o << " changed" ;
#endif
}

void ColinStruct::CommandEditLoad(const int &o, const ColinLoad &n)
{
	resetResults();
	loads[o]=n;
	loads[o].setStruct(this);
	emit changedLoad(o);

#ifdef CHANGES_VERBOSE
	qDebug() << "load " << o << " changed" ;
#endif
}

void ColinStruct::CommandRemoveLoad(const int &o)
{
	resetResults();
	loads.removeAt(o);
	emit erasedLoad(o);

#ifdef CHANGES_VERBOSE
	qDebug() << "load " << o << " removed" ;
#endif
}

void ColinStruct::CommandInsertLoad(const int &o, const ColinLoad &n)
{
	resetResults();
	loads.insert(o, n);
	loads[o].setStruct(this);
	emit addedLoad(o);

#ifdef CHANGES_VERBOSE
	qDebug() << "load " << o << " inserted" ;
#endif
}

void ColinStruct::CommandEditBLS(const int &o, const ColinBLS &n)
{
	resetResults();
	basicloadsets[o]=n;
	basicloadsets[o].setStruct(this);
	emit changedBLS(o);

#ifdef CHANGES_VERBOSE
	qDebug() << "bls " << o << " edited" ;
#endif
}

void ColinStruct::CommandRemoveBLS(const int &o)
{
	resetResults();
	for(int i=0; i<load_n(); i++)
	{
		if(load(i).set()==o)
		{
			ColinLoad l = load(i);
			l.setSet(-1);
			emit edited(i, l);
		}
		else if(load(i).set()>o)
		{
			loads[i].setSet(load(i).set()-1);
		}
	}
	for(int i=0; i<cls_n(); i++)
	{
		if(cls(i).contains(o))
		{
			ColinCLS c = cls(i);
			c.removeBLS(o);
			emit edited(i, c);
		}
		for(int j=0; j<cls(i).count(); j++)
		{
			if(cls(i).bls(j)>o)
				combinedloadsets[i].setBLSbyIndex(j, cls(i).bls(j)-1);
		}
	}
	basicloadsets.removeAt(o);
	emit erasedBLS(o);

#ifdef CHANGES_VERBOSE
	qDebug() << "bls " << o << " removed" ;
#endif
}

void ColinStruct::CommandInsertBLS(const int &o, const ColinBLS &n)
{
	resetResults();
	basicloadsets.insert(o, n);
	basicloadsets[o].setStruct(this);
	for(int i=0; i<load_n(); i++)
	{
		if(!(load(i).set()<o))
		{
			loads[i].setSet(load(i).set()+1);
		}
	}
	for(int i=0; i<cls_n(); i++)
	{
		for(int j=0; j<cls(i).count(); j++)
		{
			if(!(cls(i).bls(j)<o))
				combinedloadsets[i].setBLSbyIndex(j, cls(i).bls(j)+1);
		}
	}
	emit addedBLS(o);


#ifdef CHANGES_VERBOSE
	qDebug() << "bls " << o << " inserted" ;
#endif
}

void ColinStruct::CommandEditCLS(const int &o, const ColinCLS &n)
{
	resetResults();
	combinedloadsets[o]=n;
	combinedloadsets[o].setStruct(this);
	emit changedCLS(o);

#ifdef CHANGES_VERBOSE
	qDebug() << "cls " << o << " edited" ;
#endif
}

void ColinStruct::CommandRemoveCLS(const int &o)
{
	resetResults();
	combinedloadsets.removeAt(o);
	emit erasedCLS(o);

#ifdef CHANGES_VERBOSE
	qDebug() << "cls " << o << " removed" ;
#endif
}

void ColinStruct::CommandInsertCLS(const int &o, const ColinCLS &n)
{
	resetResults();
	combinedloadsets.insert(o, n);
	combinedloadsets[o].select();
	combinedloadsets[o].setStruct(this);
	emit addedCLS(o);

#ifdef CHANGES_VERBOSE
	qDebug() << "cls " << o << " inserted" ;
#endif
}

void ColinStruct::resetResults()
{
	isCalc = false;
	calculationThread = 0;
	if(n_res) delete[] n_res;
	if(n_res) delete[] a_res;
	if(n_res) delete[] b_res;
	n_res = 0;
	a_res = 0;
	b_res = 0;
}

ColinNode ColinStruct::getNode(const int &i)
{
	if(i<0 || i>=nodes.size())
		return ColinNode();
	return nodes[i];
}

int ColinStruct::addNode(double x, double y)
{
	return addNode(ColinNode(x, y));
}

int ColinStruct::addNode(const ColinNode &n)
{
    isCalc = false;
    calculationThread = 0;
    emit appended(n);
    return node_n()-1;
}

void ColinStruct::setNode(const int &k, const ColinNode &n)
{
	if(k>=0 && k<node_n())
		emit edited(k, n);
}

void ColinStruct::editNode(const int &k, const  ColinNode &n)
{
	if(k<0 || k>=node_n())
		return;
	emit edited(k, n);
}

void ColinStruct::removeNode(const int &o)
{
	if(o<0 || o>=node_n())
		return;
	emit beginS(tr("remove node %1").arg(o));
	emit removed(o, nodes.at(o));
	emit endS();
}

void ColinStruct::setSupport(const int &k, const ColinSupport &b)
{
	if(k<0 || k>=node_n())
		return;
	ColinNode after=nodes[k];
	if(b.form() == ColinSupport::NoBear)
		after.removeBearing();
	else
		after.setBearing(b);
	emit edited(k, after);

}

ColinSupport ColinStruct::getSupport(const int &i)
{
	if(i<0 || i>=node_n())
		return ColinSupport();
	return nodes.at(i).bearing();
}

ColinBeam ColinStruct::getBeam(const int &i)
{
	if(i<0 || i>=nodes.size())
		return ColinBeam();
	return beams[i];
}

void ColinStruct::setBeam(const int &k, const ColinBeam &b)
{
	if(k>=0 && k<beam_n())
		emit edited(k, b);
}


int ColinStruct::addBeam(const ColinBeam &n)
{
	if(n.leftNodeI() == n.rightNodeI())
		return -1;
	emit appended(n);
	return beam_n()-1;
}

int ColinStruct::addBeam(int left, int right)
{
	if(left<0 || left>=node_n())
		return -1;
	if(right<0 || right>=node_n())
		return -1;
	if(beam_n()>0)
		addBeam(ColinBeam(this, left, right, lastbeam().MatI(), lastbeam().ProfileI()));
	else
		addBeam(ColinBeam(this, left, right, 0, 0));
	return beam_n()-1;
}

void ColinStruct::removeBeam(const int &o)
{
	if(o<0 || o>=beam_n())
		return;
	emit beginS(tr("remove beam %1").arg(o));
	emit removed(o, beams.at(o));
	emit endS();
}

ColinLoad ColinStruct::getLoad(const int &i)
{
	if(i<0 || i>=load_n())
		return ColinLoad();
	return loads.at(i);
}

void ColinStruct::setLoad(const int &i, const ColinLoad &l)
{
	if(i<0 || i>=load_n())
		return;
	if(l.typ()<0 || l.typ()>9)
		return;
	if(l.isOnBeam() && (l.at()>=beam_n() || l.at()<0))
		return;
	else if(l.at()>=node_n() || l.at()<0)
		return;
	editLoad(i, l);
}

int ColinStruct::addLoad(const ColinLoad &n)
{
	if(n.typ()<0 || n.typ()>9)
		return -1;
	if(n.isOnBeam() && (n.at()>=beam_n() || n.at()<0))
		return -1;
	else if(n.at()>=node_n() || n.at()<0)
		return -1;
	emit appended(n);
	return load_n()-1;
}

void ColinStruct::removeLoad(const int &o)
{
	if(o<0 || o>=load_n())
		return;
	emit removed(o, loads.at(o));
}

ColinBLS ColinStruct::getBLS(const int &i)
{
	if(i<0 || i>=bls_n())
		return ColinBLS();
	return basicloadsets.at(i);
}

void ColinStruct::setBLS(const int &i, const ColinBLS &n)
{
	if(i>0 && i<bls_n())
		editBLS(i, n);
}

int ColinStruct::addBLS(const ColinBLS &n)
{
	QString blsname = n.name();
	int nr = 1;
	ColinBLS nn = n;
	while(getBLSIDbyName(nn.name())!=-1){
		nn.setName(blsname + " ("+QString::number(nr++)+")");
	}

	emit appended(nn);
	return bls_n()-1;
}

void ColinStruct::removeBLS(const int &i)
{
	if(i<0 || i>=bls_n())
		return;
	beginScript(tr("remove bls %1").arg(i));
	emit removed(i, bls(i));
	endScript();
}

ColinCLS ColinStruct::getCLS(const int &i)
{
	if(i<0 || i>=cls_n())
		return ColinCLS();
	return combinedloadsets.at(i);
}

int ColinStruct::addCLS(const ColinCLS &n)
{
	QString clsname = n.name();
	int nr = 1;
	ColinCLS nn = n;
	while(getCLSIDbyName(nn.name())!=-1){
		nn.setName(clsname + " ("+QString::number(nr++)+")");
	}
	emit appended(nn);
	return cls_n()-1;
}

void ColinStruct::setCLS(const int &i, const ColinCLS &n)
{
	if(i>0 && i<cls_n())
		editCLS(i, n);
}

void ColinStruct::removeCLS(const int &i)
{
	Q_ASSERT(!(i<0 || i>=cls_n()));
	emit removed(i, cls(i));
}

void ColinStruct::setX(const int &o, const double &xn)
{
    Q_ASSERT(!(o<0 || o>=node_n()));
	ColinNode n(node(o));
    n.setX(xn);
    emit edited(o, n);
}

void ColinStruct::setZ(const int &o, const double &zn)
{
    Q_ASSERT(!(o<0 || o>=node_n()));
	ColinNode n(node(o));
    n.setZ(zn);
    emit edited(o, n);
}

void ColinStruct::setBx(const int &o, const bool &hn)
{
    Q_ASSERT(!(o<0 || o>=node_n()));
	ColinNode n(node(o));
    if(!n.hasbearing())
		n.setBearing(ColinSupport());
    n.bearing_editable().setX(hn);
	if(n.bearing().form() == ColinSupport::NoBear)
        n.removeBearing();
    emit edited(o, n);
}

void ColinStruct::setBz(const int &o, const bool &vn)
{
    Q_ASSERT(!(o<0 || o>=node_n()));
	ColinNode n(node(o));
    if(!n.hasbearing())
		n.setBearing(ColinSupport());
    n.bearing_editable().setZ(vn);
	if(n.bearing().form() == ColinSupport::NoBear)
        n.removeBearing();
    emit edited(o, n);
}

void ColinStruct::setBm(const int &o, const bool &mn)
{
    Q_ASSERT(!(o<0 || o>=node_n()));
	ColinNode n(node(o));
    if(!n.hasbearing())
		n.setBearing(ColinSupport());
    n.bearing_editable().setPhi(mn);
	if(n.bearing().form() == ColinSupport::NoBear)
        n.removeBearing();
    emit edited(o, n);
}

void ColinStruct::setCx(const int &o, const double &fn)
{
    Q_ASSERT(!(o<0 || o>=node_n()));
	ColinNode n(nodes[o]);
    n.bearing_editable().setCx(fn);
	if(n.bearing().form() == ColinSupport::NoBear)
        n.removeBearing();
    emit edited(o, n);
}

void ColinStruct::setCz(const int &o, const double &fn)
{
    Q_ASSERT(!(o<0 || o>=node_n()));
	ColinNode n(nodes[o]);
    n.bearing_editable().setCz(fn);
	if(n.bearing().form() == ColinSupport::NoBear)
        n.removeBearing();
    emit edited(o, n);
}

void ColinStruct::setCm(const int &o, const double &fn)
{
    Q_ASSERT(!(o<0 || o>=node_n()));
	ColinNode n(nodes[o]);
    n.bearing_editable().setCphi(fn);
	if(n.bearing().form() == ColinSupport::NoBear)
        n.removeBearing();
    emit edited(o, n);
}

void ColinStruct::setAngle(const int &o, const double &alphan)
{
    Q_ASSERT(!(o<0 || o>=node_n()));
	ColinNode n(nodes[o]);
    n.bearing_editable().setAngle(alphan);
    emit edited(o, n);
}

void ColinStruct::setXZ(const int &o, const double &xn, const double &zn)
{
    Q_ASSERT(!(o<0 || o>=node_n()));
	ColinNode after=nodes[o];
    after.setX(xn);
    after.setZ(zn);
    emit edited(o, after);
}

int ColinStruct::knoten_von_auflager(const int &o) const
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

void ColinStruct::editBeam(const int &o, const ColinBeam &n)
{
    Q_ASSERT(!(o<0 || o>=beam_n()));
    emit edited(o, n);
}

void ColinStruct::setLeft(const int &o, const int &Ln)
{
    Q_ASSERT(!(o<0 || o>=beam_n()));
    Q_ASSERT(!(Ln<0 || Ln>=node_n()));
	ColinBeam b(beam(o));
    b.setLeftNode(Ln);
    emit edited(o, b);
}

void ColinStruct::setRight(const int &o, const int &Rn)
{
    Q_ASSERT(!(o<0 || o>=beam_n()));
    Q_ASSERT(!(Rn<0 || Rn>=node_n()));
	ColinBeam b(beam(o));
    b.setRightNode(Rn);
    emit edited(o, b);
}

void ColinStruct::setJointsandSprings(const int &beamnr, const bool *joi_array_6, const double *spr_array_6)
{
	Q_ASSERT(true);
}

void ColinStruct::setJoint(const int &beamnr, const int &pos, const bool &thereIsAJoint)
{
    Q_ASSERT(!(beamnr<0 || beamnr>=beam_n()));
	ColinBeam b(beams[beamnr]);
    b.setJoint(pos, thereIsAJoint);
    emit edited(beamnr, b);
}

void ColinStruct::setSpring(const int &beamnr, const int &pos, const double &c_f)
{
    Q_ASSERT(!(beamnr<0 || beamnr>=beam_n()));
	ColinBeam b(beams[beamnr]);
    b.setSpring(pos, c_f);
    emit edited(beamnr, b);
}

void ColinStruct::setMat(const int &o, const int &matIndex)
{
    Q_ASSERT(!(o<0 || o>=beam_n()));
	ColinBeam b(beams[o]);
    b.setMat(matIndex);
    emit edited(o, b);
}

void ColinStruct::setProfile(const int &o, const int &QsIndex)
{
    Q_ASSERT(!(o<0 || o>=beam_n()));
	ColinBeam b(beams[o]);
    b.setQs(QsIndex);
    emit edited(o, b);
}

void ColinStruct::setMat(const int &o, const QString &matName)
{
    setMat(o, LIB.IndexFromMatByName(matName));
}

void ColinStruct::setProfile(const int &o, const QString &QsName)
{
    setProfile(o, LIB.IndexFromProfileByName(QsName));
}

void ColinStruct::editLoad(const int &o, const ColinLoad &n)
{
    Q_ASSERT(!(o<0 || o>=load_n()));
    emit edited(o, n);
}


void ColinStruct::setPx(const int &o, const double &Pxn)
{
    Q_ASSERT(!(o<0 || o>=load_n()));
	ColinLoad l(load(o));
    l.setPx(Pxn);
    emit edited(o, l);
}

void ColinStruct::setPz(const int &o, const double &Pzn)
{
    Q_ASSERT(!(o<0 || o>=load_n()));
	ColinLoad l(load(o));
    l.setPz(Pzn);
    emit edited(o, l);
}

void ColinStruct::setM(const int &o, const double &Mn)
{
    Q_ASSERT(!(o<0 || o>=load_n()));
	ColinLoad l(load(o));
    l.setM(Mn);
    emit edited(o, l);
}

void ColinStruct::setLoadTyp(const int &o, const ColinLoad::form &Typn)
{
    Q_ASSERT(!(o<0 || o>=load_n()));
    /*
        puh
    */
	ColinLoad l(load(o));
	if(l.typ() == ColinLoad::tempChange &&
	   Typn == ColinLoad::tempDiffrence )
		l.setPz(l.Px());
	else if(l.typ() == ColinLoad::tempDiffrence &&
			Typn == ColinLoad::tempChange)
		l.setPx(l.Pz());
    l.setTyp(Typn);
    emit edited(o, l);
}

void ColinStruct::setPos(const int &o, const int &Pos)
{
    Q_ASSERT(!(o<0 || o>=load_n()));
	ColinLoad l(load(o));
    l.setBeam(Pos);
    emit edited(o, l);
}

void ColinStruct::setLoadSet(const int &o, const int &ls)
{
	Q_ASSERT(!(o<0 || o>=load_n()));
	Q_ASSERT(!(ls<-1||ls>=bls_n()));
	ColinLoad l(load(o));
	l.setSet(ls);
	emit edited(o, l);
}

void ColinStruct::editBLS(const int &o, const ColinBLS &n)
{
	Q_ASSERT(!(o<0 || o>=bls_n()));
	emit edited(o, n);
}

void ColinStruct::setBLSName(const int &i, const QString &name)
{
	Q_ASSERT(!(i<0 || i>=bls_n()));
	ColinBLS b = bls(i);
	b.setName(name);
	emit edited(i, b);
}

void ColinStruct::setBLSColor(const int &i, const QColor &color)
{
	Q_ASSERT(!(i<0 || i>=bls_n()));
	basicloadsets[i].setColor(color);
	emit changedBLS(i);
}

int ColinStruct::getBLSIDbyName(const QString &name) const
{
	for(int i=0; i<bls_n(); i++)
	{
		if(bls(i).name()==name)
			return i;
	}
	return -1;
}

void ColinStruct::editCLS(const int &o, const ColinCLS &n)
{
	Q_ASSERT(!(o<0 || o>=cls_n()));
	emit edited(o, n);
}

void ColinStruct::setCLSName(const int &i, const QString &name)
{
	Q_ASSERT(!(i<0 || i>=cls_n()));
	ColinCLS n(cls(i));
	n.setName(name);
	emit edited(i, n);
}

void ColinStruct::addBLStoCLS(const int &clsi, const int &blsi, const double &fac)
{
	if(clsi<0 || clsi>=cls_n())
		return;
	if(blsi<0 || blsi>=bls_n())
		return;
	ColinCLS c =cls(clsi);
	c.addBLS(blsi, fac);
	emit edited(clsi, c);
}

void ColinStruct::removeBLSbyIndex(const int &clsi, const int &i)
{
	Q_ASSERT(!(clsi<0 || clsi>=cls_n()));
	ColinCLS c = cls(clsi);
	c.removeBLS(i);
	emit edited(clsi, c);
}

void ColinStruct::setFacbyIndex(const int &clsi, const int &i, const double &fac)
{
	Q_ASSERT(!(clsi<0 || clsi>=cls_n()));
	ColinCLS c = cls(clsi);
	c.setFactorbyIndex(i, fac);
	emit edited(clsi, c);
}

void ColinStruct::setBLSbyIndex(const int &clsi, const int &i, const int newBLS)
{
	Q_ASSERT(!(clsi<0 || clsi>=cls_n()));
	ColinCLS c = cls(clsi);
	c.setBLSbyIndex(i, newBLS);
	emit edited(clsi, c);
}

int ColinStruct::getCLSIDbyName(const QString &name) const
{
	for(int i=0; i<cls_n(); i++)
	{
		if(cls(i).name()==name)
			return i;
	}
	return -1;
}

void ColinStruct::setActiveCLS(int clsI, bool active)
{
	if(combinedloadsets[clsI].isSelected() == active)
		return;
	combinedloadsets[clsI].select(active);
	emit changedCLS(clsI);
	emit changedActiveCLS();
}

bool ColinStruct::isActiveCLS(int clsI) const
{
	return cls(clsI).isSelected();
}

QSet<int> ColinStruct::activeCLS()const
{
	QSet<int> activeLS;
	for(int i=0; i<cls_n(); i++){
		if(cls(i).isSelected())
			activeLS.insert(i);
	}
	return activeLS;
}

int ColinStruct::activeCLS_n() const
{
	return activeCLS().count();
}


QTransform &ColinStruct::view(int i)
{
    for(int id=views.size()-1; id<i+1; id++)
    {
        views.append(QTransform(100, 0,     0,
                                0,   100,   0,
                                70,  70,    1));
    }
    return views[i];
}

void ColinStruct::setView(int i, const QTransform &t)
{
    for(int id=views.size()-1; id<i+1; id++)
    {
        views.append(QTransform(100, 0,     0,
                                0,   100,   0,
                                70,  70,    1));
    }
    views[i]=t;
}

int ColinStruct::nearestNode(const QPointF &p, const double &maxabs) const
{
    for(int i=0; i<node_n(); i++)
    {
        if((p-node(i).toQPointF()).manhattanLength() < maxabs)
            return i;
    }
    return -1;
}

int ColinStruct::nearestBeam(const QPointF &p, const double &maxabs, QPointF *n) const
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

double ColinStruct::nearestPointOnBeam(const QPointF &p, const int &i) const
{
	double m;
	QPointF nearest;

	if(beam(i).leftNode().z() == beam(i).rightNode().z())
	{
		if(beam(i).leftNode().x() < beam(i).rightNode().x())
			return p.x()-beam(i).leftNode().x();
		else
			return -p.x()+beam(i).leftNode().x();
	}
	if(beam(i).leftNode().x() == beam(i).rightNode().x())
	{
		if(beam(i).leftNode().z() < beam(i).rightNode().z())
			return p.y()-beam(i).leftNode().z();
		else
			return -p.y()+beam(i).leftNode().z();
	}


	m=(beam(i).rightNode().z()-beam(i).leftNode().z())/
	  (beam(i).rightNode().x()-beam(i).leftNode().x());
	nearest.setX((p.y()+p.x()/m-beam(i).leftNode().z()+m*beam(i).leftNode().x())/
				 (1/m+m));
	nearest.setY(m*nearest.x()+beam(i).leftNode().z()-m*beam(i).leftNode().x());

#ifdef NEAREST_VERBOSE
	qDebug() << "nearest point =" << nearest;
	qDebug() << "nearest x = " << hypot(beam(i).leftNode().x()-nearest.x(), beam(i).leftNode().z()-nearest.y());
#endif

	return hypot(beam(i).leftNode().x()-nearest.x(), beam(i).leftNode().z()-nearest.y());
}

void ColinStruct::crossingBeams(const QPointF &p, const double &maxabs, int *first, int *second, QPointF *n) const
{
#ifdef NEAREST_VERBOSE
	qDebug() << "snap: crossings beams" ;
	qDebug() << "&&&&&&&&&&&&&&&&&&&&&" ;
	qDebug() << "MousePos = " << p.x() << ", " << p.y() ;
#endif

	QList<int> c;
	for(int i=0; i<beam_n(); i++)
	{
		QRectF rect = beam(i).boundingRect().normalized();

#ifdef NEAREST_VERBOSE
		qDebug() << "rect" << i << " = "  << rect.x() <<  ", "<< rect.y() << ", " << rect.width() << ", " << rect.height() ;
#endif

		rect.adjust(-maxabs, -maxabs, maxabs, maxabs);

#ifdef NEAREST_VERBOSE
		qDebug() << "adjR" << i << " = "  << rect.x() <<  ", "<< rect.y() << ", " << rect.width() << ", " << rect.height() ;
#endif

		if(rect.contains(p))
		c.append(i);
    }

#ifdef NEAREST_VERBOSE

	qDebug() << "candidates" ;
    foreach(int i, c)
		qDebug() << "beam " << i ;
#endif

    double m[c.size()];
    double b[c.size()];

    for(int i=0; i<c.size(); i++)
    {
	m[i] = (beam(c.at(i)).rightNode().z()-beam(c.at(i)).leftNode().z())/
	       (beam(c.at(i)).rightNode().x()-beam(c.at(i)).leftNode().x());


	b[i] = beam(c.at(i)).leftNode().z()-beam(c.at(i)).leftNode().x()*m[i];
    }

#ifdef NEAREST_VERBOSE
	qDebug() << "vars:" ;
    int j=0;
    foreach(int i, c){
		qDebug() << "m_" << i << " = " << m[j] << " , b_" << i << " = " << b[j] ;
        j++;}
#endif

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

int ColinStruct::nearestLoad(const QPointF &p, const double &maxabs) const
{
    for(int i=0; i<load_n(); i++)
    {
	if(load(i).typ() == ColinLoad::nodeLoad)
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
		else if(load(i).typ() == ColinLoad::increasingLinearLoad	||
				load(i).typ() == ColinLoad::decreasingLinearLoad	||
				load(i).typ() == ColinLoad::uniformlyDistibutedLoad )
        {

            if(load(i).shape().containsPoint(p, Qt::OddEvenFill))
                return i;
        }
    }

    return -1;
}

void ColinStruct::removeViewPort(int i)
{
    views.removeAt(i);
}

int ColinStruct::bestimmt()
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

int ColinStruct::bestimmt(int *aa, int *ss, int *kk, int *gg, int *ff)
{
    (*aa)=0;
    (*gg)=0;
    (*ff)=0;
    (*ss)=beam_n();
    (*kk)=node_n();
    for(int u=0; u<node_n(); u++)
    {
        if(node(u).bearing().x() || node(u).bearing().c_x()!=0)
            (*aa)++;
        if(node(u).bearing().z() || node(u).bearing().c_z()!=0)
            (*aa)++;
        if(node(u).bearing().phi() || node(u).bearing().c_phi()!=0)
            (*aa)++;
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

void ColinStruct::selectAll()
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

void ColinStruct::deselectAll()
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

void ColinStruct::invertSelection()
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

void ColinStruct::selectRect(const QRectF &r, bool keepSelect)
{

#ifndef NEAREST_VERBOSE
	qDebug() << "selection" << endl << "rect(l, r, t, b) = " << r.left() << ", " << r.y() << ", " << r.top() << ", " << r.bottom() ;
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

#ifndef NEAREST_VERBOSE
			qDebug() << "selecting beams" ;
#endif

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

#ifndef NEAREST_VERBOSE
			qDebug() << "beam " << i << "intersects with bounding rect" ;
			qDebug() << "m = " << m ;
			qDebug() << "b = " << b ;
			qDebug() << "y1(left) = " << m*r.left()+b ;
			qDebug() << "y2(right) = " << m*r.right()+b ;
			qDebug() << "x1(top) = " << (r.top()-b)/m ;
			qDebug() << "x2(bottm) = " << (r.bottom()-b)/m ;
#endif

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
		if(load(i).typ() == ColinLoad::tempChange ||
		   load(i).typ() == ColinLoad::tempDiffrence)
        {
            if(r.contains(beam(load(i).at()).middleP()))
                loads[i].select(true);
            else if(load(i).isSelected())
                loads[i].select(keepSelect);
            continue;
        }
		else if(load(i).typ() == ColinLoad::doubleLoadLeft)
        {
            if(r.contains(beam(load(i).at()).leftNode().toQPointF()))
                loads[i].select(true);
            else if(load(i).isSelected())
                loads[i].select(keepSelect);
        }
		else if(load(i).typ() == ColinLoad::doubleLoadRight)
        {
            if(r.contains(beam(load(i).at()).rightNode().toQPointF()))
                loads[i].select(true);
            else if(load(i).isSelected())
                loads[i].select(keepSelect);
        }
		else if(load(i).typ() == ColinLoad::moment)
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

QRectF ColinStruct::boundingRect(const bool &alsoLoads) const
{
    calculateShapes();
    if(node_n() < 1)
        return QRectF(0, 0, 0, 0);
    QRectF bound(node(0).toQPointF(), QSizeF(0.0001, 0.0001));
	foreach(ColinNode n, nodes)
	{
		bound = bound.united(QRectF(n.x()-0.00001, n.z()-0.00001, 0.00002, 0.00002));
    }
    if(alsoLoads)
    {
        for(int i=0; i<load_n(); i++)
        {
			if(load(i).typ() == ColinLoad::decreasingLinearLoad    ||
			   load(i).typ() == ColinLoad::increasingLinearLoad    ||
			   load(i).typ() == ColinLoad::uniformlyDistibutedLoad )
            {
                for(int n=1; n<3; n++)
                {
                    bound.setLeft(      qMin(      bound.left(),    load(i).shape()[n].x()));
                    bound.setRight(     qMax(      bound.right(),   load(i).shape()[n].x()));
                    bound.setTop(       qMin(      bound.top(),     load(i).shape()[n].y()));
                    bound.setBottom(    qMax(      bound.bottom(),  load(i).shape()[n].y()));
                }
            }
			if(load(i).typ() == ColinLoad::nodeLoad)
            {
                bound.setLeft(      qMin(      bound.left(),    load(i).shape()[1].x()));
                bound.setRight(     qMax(      bound.right(),   load(i).shape()[1].x()));
                bound.setTop(       qMin(      bound.top(),     load(i).shape()[1].y()));
                bound.setBottom(    qMax(      bound.bottom(),  load(i).shape()[1].y()));
            }
        }
		if(isCalculated())
		{
			foreach(ColinNode n, nodes)
			{
				for(int j = 0; j<qMax(1, cls_n()); j++){
					bound = bound.united(QRectF(n.x()+n.u(j)*scaleU()-0.00001, n.z()+n.w(j)*scaleU()-0.00001, 0.00002, 0.00002));
				}
			}
			for(int i=0; i<beam_n(); i++)
			{
				QTransform t = beam(i).transform();

				for(int j = 0; j<qMax(1, cls_n()); j++){
					for(int m=0; m<2; m++){
						QPointF maxP = t.map(QPointF(beam(i).l()*m, beam(i).M(j, beam(i).l()*m)*scaleM()));
						bound = bound.united(QRectF(maxP.x()-0.00001, maxP.y()-0.00001, 0.00002, 0.00002));
					}
					for(int m=0; m<2; m++){
						QPointF maxP = t.map(QPointF(beam(i).Mconst(j).max(m), beam(i).M(j, beam(i).Mconst(j).max(m))*scaleM()));
						bound = bound.united(QRectF(maxP.x()-0.00001, maxP.y()-0.00001, 0.00002, 0.00002));
					}
					for(int m=0; m<2; m++){
						QPointF maxP = t.map(QPointF(beam(i).l()*m, beam(i).N(j, beam(i).l()*m)*scaleP()));
						bound = bound.united(QRectF(maxP.x()-0.00001, maxP.y()-0.00001, 0.00002, 0.00002));
					}
					for(int m=0; m<1; m++){
						QPointF maxP = t.map(QPointF(beam(i).Nconst(j).max(m), beam(i).N(j, beam(i).Nconst(j).max(m))*scaleP()));
						bound = bound.united(QRectF(maxP.x()-0.00001, maxP.y()-0.00001, 0.00002, 0.00002));
					}
					for(int m=0; m<2; m++){
						QPointF maxP = t.map(QPointF(beam(i).l()*m, beam(i).Q(j, beam(i).l()*m)*scaleP()));
						bound = bound.united(QRectF(maxP.x()-0.00001, maxP.y()-0.00001, 0.00002, 0.00002));
					}
					for(int m=0; m<1; m++){
						QPointF maxP = t.map(QPointF(beam(i).Qconst(j).max(m), beam(i).Q(j, beam(i).Qconst(j).max(m))*scaleP()));
						bound = bound.united(QRectF(maxP.x()-0.00001, maxP.y()-0.00001, 0.00002, 0.00002));
					}
				}
			}
		}
    }
    return bound.normalized();
}

double ColinStruct::mMax() const
{
    double m = 0;
	foreach(ColinLoad l, loads)
    {
	if(l.typ() == ColinLoad::moment			||
	   l.typ() == ColinLoad::doubleLoadLeft		||
	   l.typ() == ColinLoad::doubleLoadRight	        )
            m = qMax(m, fabs(l.M()));
    }
    if(isCalculated())
        m = qMax(m, globalMMax);
    return qMax(m, 1.);
}

double ColinStruct::pMax() const
{
    double p = 0;
	foreach(ColinLoad l, loads)
    {
		if(l.typ() == ColinLoad::nodeLoad                ||
		   l.typ() == ColinLoad::uniformlyDistibutedLoad ||
		   l.typ() == ColinLoad::increasingLinearLoad    ||
	   l.typ() == ColinLoad::decreasingLinearLoad    ||
	   l.typ() == ColinLoad::doubleLoadLeft		||
	   l.typ() == ColinLoad::doubleLoadRight	        )
            p = qMax(p, sqrt(pow(l.Px(), 2)+pow(l.Pz(), 2)));
    }
    if(isCalculated())
        p = qMax(p, globalPMax);
    return qMax(p, 1.);
}

double ColinStruct::uMax() const
{
    return globalUMax;
}

bool ColinStruct::isSelectionEmpty()
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

void ColinStruct::deleteSelection()
{
    beginS(tr("delete"));
    for(int i=0; i<load_n();)
    {
#ifndef QT_NO_DEBUG
     QTextStream debugg(stdout);
	 debugg <<"load[" <<  i << (load(i).isSelected() ? "] is selected" : "] is not selected") ;
 #endif

	if(load(i).isSelected())
	{
	    removeLoad(i);
#ifndef QT_NO_DEBUG
		debugg <<"removed load[" << i << "]" ;
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
	 debugg <<"beam[" <<  i << (beam(i).isSelected() ? "] is selected" : "] is not selected") ;
 #endif

	if(beam(i).isSelected())
	{
	    removeBeam(i);
#ifndef QT_NO_DEBUG
		debugg <<"removed beam[" << i << "]" ;
#endif
	}
	else
	{
	    i++;
	}
#ifndef QT_NO_DEBUG
			debugg <<"next beam = beam[" <<  i << "]" ;
#endif
    }

    for(int i=0; i<node_n();)
    {
#ifndef QT_NO_DEBUG
     QTextStream debugg(stdout);
	 debugg <<"node[" <<  i << (node(i).isSelected() ? "] is selected" : "] is not selected") ;
 #endif

	if(node(i).isSelected())
	{
	    removeNode(i);
#ifndef QT_NO_DEBUG
		debugg <<"removed node[" << i << "]" ;
#endif
	}
	else
	{
	    i++;
	}
#ifndef QT_NO_DEBUG
			debugg <<"next node = node[" <<  i << "]" ;
#endif
    }
    endS();
}

void ColinStruct::setScaleM(const double &s)
{
    if(this->mMax()*s>5000)
        return;
    scale_m=s;
    emit changedMScale(scale_m);
}

void ColinStruct::setScaleP(const double &s)
{
    if(this->pMax()*s>5000)
        return;
    scale_p=s;
    emit changedPScale(scale_p);
}

void ColinStruct::setScaleU(const double &s)
{
    if(this->uMax()*s>5000)
        return;
    scale_u=s;
    emit changedUScale(scale_u);
}

void ColinStruct::calculateShapes(const QList<int> &activebls, const QList<double> &multipliers) const
{
	ColinStruct *t = const_cast<ColinStruct*>(this);
    QMap<int, QLineF> line;
#ifdef SHAPES_VERBOSE
	qDebug() << "calculating load shapes in global koord system";
	qDebug() << "for bls" << activebls;
	qDebug() << "with multipliers" << multipliers;
#endif
	for(int i=0; i<load_n(); i++)
    {
		double fac;
		if(activebls.isEmpty())
			fac = 1.0;
		else if(!activebls.contains(load(i).set()))
			continue;
		else
			fac = multipliers.at(activebls.indexOf(load(i).set()));
#ifdef SHAPES_VERBOSE
		qDebug() << "calculating load " << i << " with multiplier " << fac;
#endif
		if(load(i).typ() == ColinLoad::decreasingLinearLoad    ||
		   load(i).typ() == ColinLoad::uniformlyDistibutedLoad ||
		   load(i).typ() == ColinLoad::increasingLinearLoad    )
		{
			if(!line.contains(load(i).at()))
			{
			line.insert(load(i).at(),
					(beam(load(i).at()).toQLineF()));
			}
			QPolygonF points(4);

			points[0] = line[load(i).at()].p1();
			points[3] = line[load(i).at()].p2();


			if(load(i).typ() == ColinLoad::uniformlyDistibutedLoad)
			{
			points[1] = points[0]-QPointF(load(i).Px(), load(i).Pz())*scaleP()*fac;
			points[2] = points[3]-QPointF(load(i).Px(), load(i).Pz())*scaleP()*fac;
			}
			else if(load(i).typ() == ColinLoad::increasingLinearLoad)
			{
			points[1]=points[0];
			points[2]= points[3]-QPointF(load(i).Px(), load(i).Pz())*scaleP()*fac;
			}
			else if(load(i).typ() == ColinLoad::decreasingLinearLoad)
			{
			points[1] = points[0]-QPointF(load(i).Px(), load(i).Pz())*scaleP()*fac;
			points[2] = points[3];
			}

			t->loads[i].setShape(points);


			line[load(i).at()].setP1(points[1]);
			line[load(i).at()].setP2(points[2]);
	}
		else if(load(i).typ() == ColinLoad::tempChange ||
				load(i).typ() == ColinLoad::tempDiffrence)
        {
            QRectF rect = QRectF(beam(load(i).at()).middleP()-QPointF(10.*70, 100.*70), QSizeF(20.*70, 100.*70));

            QPolygonF points(4);
            points[0] = rect.topLeft();
            points[1] = rect.topRight();
            points[2] = rect.bottomRight();
            points[3] = rect.bottomLeft();
        }
		else if(load(i).typ() == ColinLoad::nodeLoad)
        {
            QPolygonF points(2);
            points[0] = node(load(i).at()).toQPointF();
			points[1] = node(load(i).at()).toQPointF()-QPointF(load(i).Px(), load(i).Pz())*scaleP()*fac;
            t->loads[i].setShape(points);
        }
    }

}

void ColinStruct::mergeWith(ColinStruct *tw, QPointF dp)
{
    emit beginS(tr("paste"));
    QMap<int, int> n;
	QMap<int, int> blsConversation;
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
							ColinSupport bear;
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
                            setSupport(j, tw->node(i).bearing());
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
		ColinBeam b(tw->beam(i));
        b.setLeftNode(n.value(b.leftNodeI(), -1));
        b.setRightNode(n.value(b.rightNodeI(), -1));

        Q_ASSERT(tw->beam(i).leftNodeI() != -1);
        Q_ASSERT(tw->beam(i).rightNodeI() != -1);

		b.setStruct(this);
        addBeam(b);
    }
	for(int i=0; i<tw->bls_n(); i++)
	{
		if(bls_n())
		{
			for(int j=0; j<bls_n(); j++)
			{
				if(tw->bls(i).name() == bls(j).name())
				{
					blsConversation.insert(i, j);
					break;
				}

				if(j==bls_n()-1)
					blsConversation.insert(addBLS(tw->bls(i)), i);
			}
		}
		else
		{
			blsConversation.insert(addBLS(tw->bls(i)), i);
		}
	}
    for(int i=0; i<tw->load_n(); i++)
    {
		if(tw->load(i).typ() == ColinLoad::decreasingLinearLoad    ||
		   tw->load(i).typ() == ColinLoad::increasingLinearLoad    ||
		   tw->load(i).typ() == ColinLoad::uniformlyDistibutedLoad ||
		   tw->load(i).typ() == ColinLoad::tempChange              ||
		   tw->load(i).typ() == ColinLoad::tempDiffrence           ||
		   tw->load(i).typ() == ColinLoad::doubleLoadLeft          ||
		   tw->load(i).typ() == ColinLoad::doubleLoadRight         )
        {
			ColinLoad l = tw->load(i);
            l.setBeam(tw->load(i).at()+beamCount);
			if(l.set()!=-1)
				l.setSet(blsConversation[l.set()]);
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


void ColinStruct::copy(ColinStruct *tw)
{
	nodes.clear();
	beams.clear();
	loads.clear();
	basicloadsets.clear();
	combinedloadsets.clear();

	for(int i=0; i<tw->node_n(); i++)
	{
		nodes.append(tw->node(i));
		nodes[i].setStruct(this);
	}
	for(int i=0; i<tw->beam_n(); i++)
	{
		beams.append(tw->beam(i));
		beams[i].setStruct(this);
	}
	for(int i=0; i<tw->load_n(); i++)
	{
		loads.append(tw->load(i));
		loads[i].setStruct(this);
	}
	for(int i=0; i<tw->bls_n(); i++)
	{
		basicloadsets.append((tw->bls(i)));
		basicloadsets[i].setStruct(this);
	}
	for(int i=0; i<tw->cls_n(); i++)
	{
		combinedloadsets.append(tw->cls(i));
		combinedloadsets[i].setStruct(this);
	}
}
