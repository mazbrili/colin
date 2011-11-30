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

#ifndef WGV_H
#define WGV_H

#include <QtCore/QThread>
#include "colinstruct.h"
#include "matrix.h"

class wgv : public QThread
{
    Q_OBJECT
public:
    explicit wgv(ColinStruct *parent);
    ~wgv();
    void run();

    ColinStruct *file() const {return tw;}

signals:
    void calcFinished();
public slots:
    void writeBack();
private:
    bool abort;
    const inline ColinNode &node(const int &i)const{return nodes[i];}
    const inline ColinBeam &beam(const int &i)const{return beams[i];}
    const inline ColinLoad &load(const int &i)const{return loads[i];}
    const inline ColinNode &leftN(const ColinBeam &b)const{return nodes[b.leftNodeI()];}
    const inline ColinNode &rightN(const ColinBeam &b)const{return nodes[b.rightNodeI()];}

    double d_alpha_l(const int &n) {
        if(node(beam(n).leftNodeI()).hasbearing())
            return beam(n).angle()-node(beam(n).leftNodeI()).angle();
        else
            return beam(n).angle();}

    double d_alpha_r(const int &n) {
        if(node(beam(n).rightNodeI()).hasbearing())
            return beam(n).angle()-node(beam(n).rightNodeI()).angle();
        else
            return beam(n).angle();}


    void u_formal();
    void make_k();

    void make_p();
    void make_s0();

    void writeResultsNodes();
    void writeResultsBeams();

	int f_n;			//degree of freedoms
	int a_n;			//locked degree of freedoms

	quadvector<double> *s0;
	quadvector<double> *p;
	quadvector<double> *u;
	quadvector<double> *ua;
	quadvector<double> *pa;

    quadvector<int> u_indexed;
    quadvector<int> k_index;

	quadmatrix<double> *K;
	submatrix<double> *Ka;

    ColinStruct *tw;
    ColinNode *nodes;
    ColinBeam *beams;
    ColinLoad *loads;
	ColinCLS *cls;
	ColinBLS *bls;
	polynom<1> **px;
	polynom<1> **pz;
	double **dT;
	double **dTz;

	polynom<1> *px_array;
	polynom<1> *pz_array;
	double *dT_array;
	double *dTz_array;

	double globalMMax;
	double globalPMax;
	double globalUMax;

	node_result *u_r;
	beam_result *b_r;
	bearing_result *a_r;

    int node_n;
    int beam_n;
    int load_n;
	int bls_n;
	int cls_n;

    bool success;
};

#endif // WGV_H
