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

#ifndef COLINBEAM_H
#define COLINBEAM_H

#include <cmath>
#include <QtCore/QRectF>
#include "colinlib.h"
#include "colinload.h"
#include "colinnode.h"
#include "polynom.h"

class ColinStruct;

class beam_result
{
public:
	beam_result(const polynom<2> &_N,
				const polynom<2> &_Q,
				const polynom<3> &_M,
				const polynom<3> &_u,
				const polynom<4> &_p,
				const polynom<5> &_w):
		N(_N), Q(_Q), M(_M), u(_u), p(_p), w(_w){}

	beam_result(){}
	polynom<2> N;
	polynom<2> Q;
	polynom<3> M;
	polynom<3> u;
	polynom<4> p;
	polynom<5> w;
};


class ColinBeam : public ColinElement
{

friend class wgv;
public:

	enum hingeForm{
		NoHinge =	0x0,

		Xleft =			0x1,
		Zleft =			0x2,
		Phileft =		0x4,

		fxleft =		0x8		|	Xleft,
		fzleft =		0x10	|	Zleft,
		fphileft =		0x20	|	Phileft,

		Xright =		0x40,
		Zright =		0x80,
		Phiright =		0x100,

		fxright =		0x200	|	Xright,
		fzright =		0x400	|	Zright,
		fphiright =		0x800	|	Phiright
	};

	Q_DECLARE_FLAGS(hingeForms, hingeForm);

	enum position{
		Nl = 0,
		Ql = 1,
		Ml = 2,
		Nr = 3,
		Qr = 4,
		Mr = 5};

	static inline hingeForms springfromPos(position p){
		switch (p){
		case Nl: return fxleft;
		case Ql: return fzleft;
		case Ml: return fphileft;
		case Nr: return fxright;
		case Qr: return fzright;
		case Mr: return fphiright;
		}
		return NoHinge;
	}

	static inline hingeForms hingefromPos(position p){
		switch (p){
		case Nl: return Xleft;
		case Ql: return Zleft;
		case Ml: return Phileft;
		case Nr: return Xright;
		case Qr: return Zright;
		case Mr: return Phiright;
		}
		return NoHinge;
	}

	ColinBeam(){par = 0; res=0; dirty = true;}
	ColinBeam(ColinStruct *parent);
	ColinBeam(ColinStruct *parent, int leftN, int rightN, int Mat, int qS);
	ColinBeam(ColinStruct *parent, int leftN, int rightN, QString Mat, QString qS);
	ColinBeam(const ColinBeam &other);
	ColinBeam &operator=(const ColinBeam &other);

	QTransform transform()const;
	inline QPointF middleP() const {return leftNode().toQPointF()/2+rightNode().toQPointF()/2;}
	const inline int &leftNodeI() const {return node_l;}
	const inline int &rightNodeI() const {return node_r;}
	const ColinNode &leftNode() const;
	const ColinNode &rightNode() const;

	QRectF boundingRect() const;
	QLineF toQLineF() const;

	const inline ColinMaterial &Mat() const {return LIB.mat(mt);}
	const inline ColinCrossSection &Profile() const {return LIB.Profile(qs);}
	const inline int &MatI() const {return mt;}
	const inline int &ProfileI() const {return qs;}

	const inline double &E() const {return LIB.mat(mt).E();}
	//const inline double &v() const {return LIB.mat(mt).v();}
	//const inline double &roh() const {return LIB.mat(mt).roh();}
	const inline double &aT() const {return LIB.mat(mt).alphaT();}
	const inline double &I() const {return LIB.Profile(qs).I();}
	const inline double &A() const {return LIB.Profile(qs).A();}
	//const inline double &Aq() const {return LIB.Profile(qs).Aq();}

	const double &angle() const;
	const double &l() const;

	inline bool joint(const position& pos) const{return (hinges&hingefromPos(pos))!=0;}
	inline bool joint(const int& pos) const{return joint(position(pos));}
	const inline double &spring(const position& pos) const {return spr[pos];}
	const inline double &spring(const int& pos) const {return spr[pos];}
	inline bool hasSpring(const position& pos) const {return (hinges&springfromPos(pos))==springfromPos(pos);}
	inline bool hasSpring(const int& pos) const {return hasSpring(position(pos));}
	void springs(double *double_array_6);

	double s(const int &i, const int& pos) const;
	double v(const int &i, const int& pos) const;

	bool wLinear() const;

	void setLeftNode(const int& ln) {node_l=ln; setDirty();}
	void setRightNode(const int& rn) {node_r=rn; setDirty();}
	void setQs(const int& nr) {qs=nr;}
	void setMat(const int& nr) {mt=nr;}

	inline bool hasJoints() const {return true;}

	void setJoints(hingeForms h);
	void setSprings(const double *array);
	void setJoint(const int& pos, const bool& thereIsAJoint);
	void setHasSpring(const int& pos, const bool& thereIsAJoint);
	void setSpring(const int& pos, const double& c_f);

	inline double N(const int &i, const double &x) const {return res[i].N(x);}
	inline double Q(const int &i, const double &x) const {return res[i].Q(x);}
	inline double M(const int &i, const double &x) const {return res[i].M(x);}
	inline double u(const int &i, const double &x) const {return res[i].u(x);}
	inline double phi(const int &i, const double &x) const {return res[i].p(x);}
	inline double w(const int &i, const double &x) const {return res[i].w(x);}

	inline double Nmax(const int &i, const int &j) const {return res[i].N.max(i);}
	inline double Qmax(const int &i, const int &j) const {return res[i].Q.max(i);}
	inline double Mmax(const int &i, const int &j) const {return res[i].M.max(i);}

	inline polynom<2> &N(const int &i){return res[i].N;}
	inline polynom<2> &Q(const int &i){return res[i].Q;}
	inline polynom<3> &M(const int &i){return res[i].M;}

	inline polynom<3> &u(const int &i){return res[i].u;}
	inline polynom<4> &phi(const int &i){return res[i].p;}
	inline polynom<5> &w(const int &i){return res[i].w;}

	const inline polynom<2> &Nconst(const int &i)const{return res[i].N;}
	const inline polynom<2> &Qconst(const int &i)const{return res[i].Q;}
	const inline polynom<3> &Mconst(const int &i)const{return res[i].M;}

	const inline polynom<3> &uconst(const int &i)const{return res[i].u;}
	const inline polynom<4> &phiconst(const int &i)const{return res[i].p;}
	const inline polynom<5> &wconst(const int &i)const{return res[i].w;}

	void setResults(beam_result *res){this->res=res;}
	beam_result *results(){return res;}


	double k11() const {return(E()*A()/l()); }
	double k22() const {return(12*E()*I()/(l()*l()*l())); }
	double k23() const {return(6*E()*I()/(l()*l())); }
	double k33() const {return(4*E()*I()/l()); }
	double k36() const {return(2*E()*I()/l()); }

	void inline setDirty() const {dirty = true;}

private:

	void calcLen() const;

	int node_l;
	int node_r;
	int mt;
	int qs;
	hingeForms hinges;
	double spr[6];

	mutable double len;
	mutable double alp;

	mutable bool dirty;

	beam_result *res;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ColinBeam::hingeForms);



#endif // COLINBEAM_H
