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

#ifndef WGV_BEAM_H
#define WGV_BEAM_H

#include <cmath>
#include <QtCore/QRectF>
#include "wgv_abstract_tw.h"
#include "wgv_load.h"
#include "polynom.h"


class wgv_beam : public wgv_element
{

friend class wgv;
public:

    enum position{
        Nl = 0,
        Ql = 1,
        Ml = 2,
        Nr = 3,
        Qr = 4,
        Mr = 5};


    wgv_beam(){dirty = true;}
    wgv_beam(wgv_abstract_tw *parent);
    wgv_beam(wgv_abstract_tw *parent, int leftN, int rightN, int Mat, int qS);
    wgv_beam(wgv_abstract_tw *parent, int leftN, int rightN, QString Mat, QString qS);
    wgv_beam(const wgv_beam &other);
    wgv_beam &operator=(const wgv_beam &other);

    QTransform transform()const;
    inline QPointF middleP() const {return leftNode().toQPointF()/2+rightNode().toQPointF()/2;}
    const inline int &leftNodeI() const {return node_l;}
    const inline int &rightNodeI() const {return node_r;}
    const inline wgv_node &leftNode() const {return (par->node(node_l));}
    const inline wgv_node &rightNode() const {return (par->node(node_r));}

    const inline QRectF boundingRect() const {return QRectF(leftNode().toQPointF(),
                                                            rightNode().toQPointF());}
    const inline QLineF toQLineF() const {return QLineF(leftNode().toQPointF(),
                                                        rightNode().toQPointF());}

    const inline wgv_mat &Mat() const {return LIB.mat(mt);}
    const inline wgv_profile &Profile() const {return LIB.Profile(qs);}
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

    const inline bool &joint(const position& pos) const{return gel[pos];}
    const inline bool &joint(const int& pos) const{return gel[pos];}
    void joints(bool *bool_array_6);
    const inline double &spring(const position& pos) const {return spr[pos];}
    const inline double &spring(const int& pos) const {return spr[pos];}
    inline bool hasSpring(const int &pos) const {return spr[pos]!=0?true:false;}
    void springs(double *double_array_6);

    double s(const int& pos) const;
    double v(const int& pos) const;

    bool wLinear() const;

    void setLeftNode(const int& ln) {node_l=ln; setDirty();}
    void setRightNode(const int& rn) {node_r=rn; setDirty();}
    void setQs(const int& nr) {qs=nr;}
    void setMat(const int& nr) {mt=nr;}

    inline bool hasJoints() const {return true;}

    void setJoints(const bool *array);
    void setSprings(const double *array);
    void setJointsandSprings(const bool *barray, const double *darray);
    void setJoint(const int& pos, const bool& thereIsAJoint);
    void setSpring(const int& pos, const double& c_f);

    inline void setN(const polynom<2> &fun){_N = fun;}
    inline void setQ(const polynom<2> &fun){_Q = fun;}
    inline void setM(const polynom<3> &fun){_M = fun;}
    inline void setU(const polynom<3> &fun){_u = fun;}
    inline void setPhi(const polynom<4> &fun){_p = fun;}
    inline void setW(const polynom<5> &fun){_w = fun;}

    inline double N(const double &x) const {return _N(x);}
    inline double Q(const double &x) const {return _Q(x);}
    inline double M(const double &x) const {return _M(x);}
    inline double u(const double &x) const {return _u(x);}
    inline double phi(const double &x) const {return _p(x);}
    inline double w(const double &x) const {return _w(x);}


    inline double Nmax(const int &i) const {return _N.max(i);}
    inline double Qmax(const int &i) const {return _Q.max(i);}
    inline double Mmax(const int &i) const {return _M.max(i);}

    inline polynom<2> &N(){return _N;}
    inline polynom<2> &Q(){return _Q;}
    inline polynom<3> &M(){return _M;}

    inline polynom<3> &u(){return _u;}
    inline polynom<4> &phi(){return _p;}
    inline polynom<5> &w(){return _w;}

    const inline polynom<2> &Nconst()const{return _N;}
    const inline polynom<2> &Qconst()const{return _Q;}
    const inline polynom<3> &Mconst()const{return _M;}

    const inline polynom<3> &uconst()const{return _u;}
    const inline polynom<4> &phiconst()const{return _p;}
    const inline polynom<5> &wconst()const{return _w;}


    double k11() const {return(E()*A()/l()); }
    double k22() const {return(12*E()*I()/(l()*l()*l())); }
    double k23() const {return(6*E()*I()/(l()*l())); }
    double k33() const {return(4*E()*I()/l()); }
    double k36() const {return(2*E()*I()/l()); }

    void inline setDirty() const {dirty = true;}
    void inline setParent(wgv_abstract_tw *tw){par = tw;}

private:

    void calcLen() const;

    int node_l;
    int node_r;
    int mt;
    int qs;
    bool gel[6];
    double spr[6];

    mutable double len;
    mutable double alp;

    mutable bool dirty;


    polynom<2> _N;
    polynom<2> _Q;
    polynom<3> _M;

    polynom<3> _u;
    polynom<4> _p;
    polynom<5> _w;


    wgv_abstract_tw *par;
};



#endif // WGV_BEAM_H
