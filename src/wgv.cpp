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

#include "wgv.h"

#include <QtCore/QTime>
#include <QtCore/QDebug>
/***********************************************************************************************************
  Programming Is Like Sex: One mistake and you have to support it for the rest of your life. (Michael Sinz)
 ***********************************************************************************************************/


wgv::wgv(ColinStruct *parent) :
    QThread(parent)
{
    //copy all the suff

    //my english sucks :(

    tw = parent;


    node_n = tw->node_n();                  //count and store objects
    beam_n = tw->beam_n();
	load_n = tw->load_n();
	bls_n = tw->bls_n();
	cls_n = tw->cls_n();

	if(cls_n==0){                          //at least one result, create cls if no one is given by the user!
		cls_n = 1;
		bls_n = 1;
	}


	nodes = new ColinNode[node_n];     //init arrays for objects
	beams = new ColinBeam[beam_n];
	loads = new ColinLoad[load_n];
	bls = new ColinBLS[bls_n];
	cls = new ColinCLS[cls_n];

	for(int i=0; i<tw->node_n(); i++)       //only required for bearings
        nodes[i] = tw->nodes[i];

	for(int i=0; i<tw->beam_n(); i++){
        beams[i] = tw->beam(i);
        beams[i].setStruct(0);}             //for lenght and angle the beam requests x- and z-koordinates from his parent
                                            //operator=(...) stores them in the object so no more parent is required
	for(int i=0; i<tw->load_n(); i++)
		loads[i] = tw->load(i);

	for(int i=0; i<tw->bls_n(); i++)
		bls[i] = tw->bls(i);

	for(int i=0; i<tw->cls_n(); i++)
		cls[i] = tw->cls(i);
    //all coping done

	if(tw->cls_n() == 0){
		cls[0].addBLS(0, 1);
		for(int i=0; i<load_n; i++)
			loads[i].setSet(0);
	}


    tw->calculationThread = this;

    connect(this, SIGNAL(finished()), this, SLOT(writeBack()));

    globalMMax = 0;
    globalPMax = 0;
    globalUMax = 0;

	dT = 0;
	dTz = 0;
	px = 0;
	pz = 0;


    success = true;



}

wgv::~wgv()
{

    delete[] nodes;         //clean up
    delete[] beams;
    delete[] loads;
	delete[] bls;
	delete[] cls;
	if(dT)
	{
		delete[] dT_array;
		delete[] dT;
	}
	if(dTz)
	{
		delete[] dTz_array;
		delete[] dTz;
	}
	if(px)
	{
		delete[] px_array;
		delete[] px;
	}
	if(pz)
	{
		delete[] pz_array;
		delete[] pz;
	}

	if(K)		delete[] K;
	if(u)		delete[] u;
	if(ua)		delete[] ua;
	if(p)		delete[] p;
	if(s0)		delete[] s0;
	if(pa)		delete[] pa;
	if(Ka)		delete[] Ka;

}

void wgv::run()
{



#ifndef QT_NO_DEBUG
    QTime t;
    t.start();
#endif //QT_NO_DEBUG

    u_formal();                   //create a formal u-vector
    make_k();
    make_s0();
    make_p();

	ua = new quadvector<double>[cls_n];
	for(int i=0; i<cls_n; i++)
	{
		ua[i]= quadvector<double>(a_n);       //for now...
	}

	Ka = new submatrix<double>[cls_n];
	for(int ls=0; ls<cls_n; ls++)
		Ka[ls] = submatrix<double>(K[ls], 1, 0);


	u = new quadvector<double>[cls_n];
	pa = new quadvector<double>[cls_n];

    try
	{
		for(int i=0; i<cls_n; i++)
			u[i]=p[i]/K[i];
    }
    catch(SingularMatrixException& e)
    {
        success = false;
        return;
    }



	for(int i=0; i<cls_n; i++)
		pa[i] = u[i]*Ka[i];

    writeResultsNodes();
    writeResultsBeams();


#ifndef QT_NO_DEBUG
	qDebug() << endl << QString("time elapsed: %1 ms").arg(t.elapsed()) << endl;
#endif //QT_NO_DEBUG

}


void wgv::writeBack()
{
#ifndef QT_NO_DEBUG
	qDebug() << "copy results to GUI thread";
#endif

    if(tw->calculationThread != this)
    {
        emit calcFinished();
        deleteLater();
        return;
    }
    if(!success)
    {
        emit calcFinished();
        deleteLater();
        return;
    }
    for(int i=0; i<node_n; i++)
    {
		tw->nodes[i].setU(nodes[i].results());
        if(nodes[i].hasbearing())
			tw->nodes[i].bearing_editable().setR(nodes[i].bearing_results());
	}
    for(int i=0; i<beam_n; i++)
	{
		tw->beams[i].setResults(beams[i].results());
    }
    tw->isCalc = true;
	tw->globalMMax = globalMMax;
	tw->globalPMax = globalPMax;
	tw->globalUMax = globalUMax;
	if(tw->n_res) delete[] tw->n_res;
	if(tw->a_res) delete[] tw->a_res;
	if(tw->b_res) delete[] tw->b_res;
	tw->n_res = u_r;
	tw->a_res = a_r;
	tw->b_res = b_r;
	tw->emitSelectionChanged();
    tw->emitCalculationFinished();
    emit calcFinished();
    deleteLater();
}

void wgv::writeResultsNodes()
{
    /***************
     *    nodes    *
     ***************/

	u_r = new node_result[cls_n*node_n];
	a_r = new bearing_result[cls_n*a_n];

	//double u_, w, phi;
	int i, j, m;

	for(i=0, j=0; i<node_n; i++){
		nodes[i].setU(u_r+cls_n*i);
		if(nodes[i].hasbearing())
			nodes[i].setR(a_r+cls_n*j++);
	}
	for(int ls=0; ls<cls_n; ls++)
	{
		qDebug() << "calculating node displacement for cls #" << ls;
		quadvector<double> &ua = this->ua[ls];
		quadvector<double> &u = this->u[ls];
		quadvector<double> &pa = this->pa[ls];
		quadvector<double> &p = this->p[ls];

		for(i=0,  j=0, m=0; i<node_n; i++)
		{
			qDebug() << "node #" << i;
			if(nodes[i].hasbearing())
			{
				double h1=((nodes[i].bearing().x()) ? ua[m++] : u[j++]);
				double h2=((nodes[i].bearing().z()) ? ua[m++] : u[j++]);

				nodes[i].results()[ls].u=h1*cos(nodes[i].angle())-h2*sin(nodes[i].angle());
				nodes[i].results()[ls].w=h1*sin(nodes[i].angle())+h2*cos(nodes[i].angle());

				nodes[i].results()[ls].phi=(nodes[i].bearing().phi() ? ua[m++] : u[j++]);

			}
			else
			{
				nodes[i].results()[ls].u = u[j++];
				nodes[i].results()[ls].w = u[j++];
				nodes[i].results()[ls].phi = u[j++];
			}

#ifndef QT_NO_DEBUG
			qDebug() << "u = " << nodes[i].u(ls) << ", w = " << nodes[i].w(ls) << ", phi = " << nodes[i].phi(ls);
#endif

		}
		// reaction forces:
		for(i=0,  m=0; i<node_n; i++)
		{
			if(nodes[i].hasbearing())
			{
				if(nodes[i].bearing().x())
				{
					nodes[i].bearing_results()[ls].x = pa[m]-p[f_n-a_n+m];
					m++;
				}
				else if(nodes[i].bearing().xf())
				{
					nodes[i].bearing_results()[ls].x = -(nodes[i].u(ls)*cos(nodes[i].angle())+nodes[i].w(ls)*sin(nodes[i].angle()))*nodes[i].bearing().c_x();
				}
				else
					nodes[i].bearing_results()[ls].x = 0;

				if(nodes[i].bearing().z())
				{
					nodes[i].bearing_results()[ls].z = pa[m]-p[f_n-a_n+m];
					m++;
				}
				else if(nodes[i].bearing().zf())
				{
					nodes[i].bearing_results()[ls].z = -(nodes[i].w(ls)*cos(nodes[i].angle())-nodes[i].u(ls)*sin(nodes[i].angle()))*nodes[i].bearing().c_z();
				}
				else
				{
					nodes[i].bearing_results()[ls].z = 0;
				}


				nodes[i].bearing_results()[ls].x =	nodes[i].bearing_results()[ls].x*cos(nodes[i].angle())-
													nodes[i].bearing_results()[ls].z*sin(nodes[i].angle());
				nodes[i].bearing_results()[ls].z=	nodes[i].bearing_results()[ls].x*sin(nodes[i].angle())+
													nodes[i].bearing_results()[ls].z*cos(nodes[i].angle());

				if(nodes[i].bearing().phi())
				{
					nodes[i].bearing_results()[ls].m = pa[m]-p[f_n-a_n+m];
					m++;
				}
				else if(nodes[i].bearing().phif())
				{
					nodes[i].bearing_results()[ls].m = -nodes[i].phi(ls)*nodes[i].bearing().c_phi();
				}
				else
				{
					nodes[i].bearing_results()[ls].m = 0;
				}


				globalPMax = qMax(globalUMax, nodes[i].bearing_results()[ls].x);
				globalPMax = qMax(globalUMax, nodes[i].bearing_results()[ls].z);
				globalMMax = qMax(globalMMax, nodes[i].bearing_results()[ls].m);

#ifndef QT_NO_DEBUG
			qDebug() << "Node has support";
			qDebug() << "Ax = " << nodes[i].Ax(ls) << ", Az = " << nodes[i].Az(ls) << ", M = " << nodes[i].Am(ls);
#endif
			}


		globalUMax = qMax(globalUMax, nodes[i].u(ls));
		globalUMax = qMax(globalUMax, nodes[i].w(ls));
		}
	}
}


void wgv::writeResultsBeams()
{
    /**************
     *   beams    *
     **************/

    double N0, Q0, M0;
    double N1, Q1, M1;
    double u0, w0, p0;

	b_r = new beam_result[cls_n*beam_n];

	for(int n=0; n<beam_n; n++)
		beams[n].setResults(b_r+n*cls_n);


	for(int ls=0; ls<cls_n; ls++)
	{
		quadvector<double> &u = this->u[ls];
		quadvector<double> &s0 = this->s0[ls];
		polynom<1>* &px = this->px[ls];
		polynom<1>* &pz = this->pz[ls];
		double* &dT = this->dT[ls];
		double* &dTz = this->dTz[ls];

#ifndef QT_NO_DEBUG
		qDebug() << "creating beam functions for cls #" << ls;
#endif
		for(int n=0; n<beam_n; n++)
		{
#ifndef QT_NO_DEBUG
			qDebug() << "beam #" << n;
#endif
			if(!beam(n).joint(0))
				u0=leftN(beam(n)).u(ls)*cos(beam(n).angle())+
					  leftN(beam(n)).w(ls)*sin(beam(n).angle());
			else
				u0=u[u_indexed[6*n]-a_n];

			if(!beam(n).joint(1))
				w0=leftN(beam(n)).w(ls)*cos(beam(n).angle())-
					  leftN(beam(n)).u(ls)*sin(beam(n).angle());
			else
				w0=u[u_indexed[6*n+1]-a_n];

			if(!beam(n).joint(2))
				p0=leftN(beam(n)).phi(ls);
			else
				p0=u[u_indexed[6*n+2]-a_n];


			if(!beam(n).joint(3))
				N1=rightN(beam(n)).u(ls)*cos(beam(n).angle())+
					  rightN(beam(n)).w(ls)*sin(beam(n).angle());
			else
				N1=u[u_indexed[6*n+3]-a_n];

			if(!beam(n).joint(4))
				Q1=rightN(beam(n)).w(ls)*cos(beam(n).angle())-
					  rightN(beam(n)).u(ls)*sin(beam(n).angle());
			else
				Q1=u[u_indexed[6*n+4]-a_n];

			if(!beam(n).joint(5))
				M1=rightN(beam(n)).phi(ls);
			else
				M1=u[u_indexed[6*n+5]-a_n];




			N0=-(beam(n).k11()*(u0-N1));
			Q0=-(beam(n).k22()*(w0-Q1)+
					beam(n).k23()*(-p0-M1));
			M0=-(beam(n).k33()*p0+
					beam(n).k36()*M1+
					beam(n).k23()*(Q1-w0));


			N0-=s0[6*n];
			Q0-=s0[6*n+1];
			M0-=s0[6*n+2];

			beams[n].N(ls) = px[n]*DiffOp::dx*(-1) + N0;

			beams[n].u(ls) = (beams[n].N(ls)*DiffOp::dx/(beams[n].E()*beams[n].A()) + u0) + polynom<3>(0, 0, dT[n]*beams[n].aT(), 0);


			beams[n].Q(ls) = pz[n]*DiffOp::dx*(-1) + Q0;

			beams[n].M(ls) = beams[n].Q(ls)*DiffOp::dx + M0;

			beams[n].phi(ls) = (beams[n].M(ls)*DiffOp::dx*(-1)/(beams[n].E()*beams[n].I()) - p0) + polynom<4>(0, 0, 0, -dTz[n]*beams[n].aT(), 0);

			beams[n].w(ls) = beams[n].phi(ls)*DiffOp::dx + w0;

#ifndef QT_NO_DEBUG
			qDebug() << "N = " << beams[n].N(ls).toString(1, 'f', 3);
			qDebug() << "Q = " << beams[n].Q(ls).toString(1, 'f', 3);
			qDebug() << "M = " << beams[n].M(ls).toString(1, 'f', 3);
			qDebug() << "u = " << beams[n].u(ls).toString(1, 'e', 3);
			qDebug() << "w = " << beams[n].w(ls).toString(1, 'e', 3);
#endif



			beams[n].N(ls).calcMax();
			beams[n].Q(ls).calcMax();
			beams[n].M(ls).calcMax();

			globalMMax= qMax(globalMMax, fabs(beams[n].M(ls, beams[n].M(ls).max(0))));
			globalMMax= qMax(globalMMax, fabs(beams[n].M(ls, beams[n].M(ls).max(1))));
			globalMMax= qMax(globalMMax, fabs(beams[n].M(ls, 0)));
			globalMMax= qMax(globalMMax, fabs(beams[n].M(ls, beams[n].l())));

			globalPMax= qMax(globalPMax, fabs(beams[n].Q(ls, beams[n].Q(ls).max(0))));
			globalPMax= qMax(globalPMax, fabs(beams[n].N(ls, beams[n].N(ls).max(0))));
			globalPMax= qMax(globalPMax, fabs(beams[n].N(ls, 0)));
			globalPMax= qMax(globalPMax, fabs(beams[n].N(ls, beams[n].l())));
			globalPMax= qMax(globalPMax, fabs(beams[n].Q(ls, 0)));
			globalPMax= qMax(globalPMax, fabs(beams[n].Q(ls, beams[n].l())));

			globalUMax=qMax(globalUMax, fabs(beams[n].w(ls, beams[n].l()/4.)));
			globalUMax=qMax(globalUMax, fabs(beams[n].w(ls, beams[n].l()/2.)));
			globalUMax=qMax(globalUMax, fabs(beams[n].w(ls, beams[n].l()*3./4.)));
			globalUMax=qMax(globalUMax, fabs(beams[n].w(ls, beams[n].l())));

		}
	}
}

void wgv::u_formal()
{
    /*
    a_n         ... Number of bearings
    f_n         ... Full size of K-matrix
    k_index     ... formal u-vektor.
    u_indexed   ... position of aditional DOFs from joints
    */

    a_n=0;          //just init. count afterwards
    f_n=node_n*3;   //init to node count * DOF/node = 3 (2D)
                    //count joints afterwars


    for(int i=0; i<node_n; i++)     //count bearings
    {
        if(nodes[i].hasbearing())
        {
            if(nodes[i].bearing().x())
                a_n++;
            if(nodes[i].bearing().z())
                a_n++;
            if(nodes[i].bearing().phi())
                a_n++;
        }
    }

    u_indexed = quadvector<int>(beam_n*6);                  //position of joints in K-matrix

    for(int j=0; j<beam_n; j++)
    {
        for(int n=0; n<6; n++)
            u_indexed[6*j+n]=(beam(j).joint(n) ? f_n++ : 0);    //position of aditional DOFs from joints.
    }                                                           //0 if it doesn't exist

    k_index = quadvector<int>(f_n);                          //a formal u-vector.
                                                         //element i contains the position of the DOF #i in K-matrix


    int j=f_n-a_n;                                      //initializated(spelling?!) with number of DOFs. counts the locked DOFs
    int i;                                              //iterator
    int m=0;                                            //counts the non-locked DOFs
    for(i=0;  i<node_n; i++)
    {
        if(nodes[i].hasbearing())
        {
            k_index[3*i] = ((nodes[i].bearing().x()) ? j++ : m++);       //first free dofs(m = 0 .. f_n-a_n-1)
            k_index[3*i+1] = ((nodes[i].bearing().z()) ? j++ : m++);     //then locked dofs(j = f_n-a_n .. f_n)
            k_index[3*i+2] = ((nodes[i].bearing().phi()) ? j++ : m++);
        }
        else
        {
            k_index[3*i] = m++;
            k_index[3*i+1] = m++;
            k_index[3*i+2] = m++;
        }
    }
    for(i*=3; i<f_n; i++)                       //all joints between free DOFs from nodes and locked DOFs form nodes
    {                                           //for my future self:
        k_index[i]=m++;                         //for a fixed u_l, w_l, phi_l, u_r, w_r, phi_r begin here
    }



#ifndef QT_NO_DEBUG
	qDebug() << endl << "building formal u-vektor";
	qDebug() << "degree of freedoms =" <<  f_n;
	qDebug() << "locked degree of freedoms =" <<  a_n;

	qDebug() << "k_index";

    for(int n=0; n<f_n; n++)
		qDebug() <<"DOF #" <<  n << " @ " << k_index[n];

	qDebug() << "u_indexed";
    for(int n=0; n<beam_n; n++)
    {
		qDebug() << "beam #" <<  n;
        for(int i=0; i<6; i++)
        {
			qDebug() << u_indexed[6*n+i];
        }
    }
#endif


}

void wgv::make_k()
{



  /*K  = new double*[f_n];
    for(int i = 0; i < f_n; i++)
    {
        K[i] = new double[f_n];
        for(int n=0; n<f_n; n++)
            K[i][n]=0;
    }*/

	K = new quadmatrix<double>[cls_n];

	for(int ls=0; ls< cls_n; ls++){
		quadmatrix<double> &K = this->K[ls];
		K.setSize(f_n); //init matrix. size calculated in u_()
								  //call only after u_()!

		double k11, k22, k23, k33, k36;             //factors from local k-matrix(6x6)
		double sin_l, cos_l, sin_r, cos_r;          //angles between the beams local kordsys
													//and the left/right nodes local kordsys
													//enough to descripe a rotation matrix



		for(int n=0; n<beam_n; n++) //foreach beam....
		{
			k11=beam(n).k11();
			k22=beam(n).k22();
			k23=beam(n).k23();
			k33=beam(n).k33();
			k36=beam(n).k36();
			sin_l=sin(d_alpha_l(n));
			cos_l=cos(d_alpha_l(n));
			sin_r=sin(d_alpha_r(n));
			cos_r=cos(d_alpha_r(n));



			//the following monster is the result from A*k*A^T (12x6)*(6x6)*(6x12)  = K(12x12) or so...
			//A is 12x6 to consider the joints (the left 6x6 part of A takes care of this)
			//a joint creates a new DOF in the local koord-sys of the beam.
			//so the right part of a contains a 1 instead of 0 in the diagonal foreach joint
			//(that column is then void in the left part of A)
			//the position of the entrys is described by k_index(considers bearings) and u_index(considers joints)
			//only the upper-right side of the matrix is required.
			//the rest will be mirrowed afterwards

			//just copy and pasted and searched and replaced :)

			K[k_index[beam(n).leftNodeI()*3]][k_index[beam(n).leftNodeI()*3]] += cos_l * cos_l * (beam(n).joint(0) ? 0 : 1) * (beam(n).joint(0) ? 0 : 1) * k11 + sin_l * sin_l * (beam(n).joint(1) ? 0 : 1) * (beam(n).joint(1) ? 0 : 1) * k22;
			K[k_index[beam(n).leftNodeI()*3]][k_index[beam(n).leftNodeI()*3+1]] += cos_l * (beam(n).joint(0) ? 0 : 1) * (beam(n).joint(0) ? 0 : 1) * k11 * sin_l - sin_l * (beam(n).joint(1) ? 0 : 1) * (beam(n).joint(1) ? 0 : 1) * k22 * cos_l;
			K[k_index[beam(n).leftNodeI()*3]][k_index[beam(n).leftNodeI()*3+2]] += sin_l * (beam(n).joint(1) ? 0 : 1) * k23 * (beam(n).joint(2) ? 0 : 1);
			K[k_index[beam(n).leftNodeI()*3]][k_index[beam(n).rightNodeI()*3]] += -cos_l * (beam(n).joint(0) ? 0 : 1) * k11 * cos_r * (beam(n).joint(3) ? 0 : 1) - sin_l * (beam(n).joint(1) ? 0 : 1) * k22 * sin_r * (beam(n).joint(4) ? 0 : 1);
			K[k_index[beam(n).leftNodeI()*3]][k_index[beam(n).rightNodeI()*3+1]] += -cos_l * (beam(n).joint(0) ? 0 : 1) * k11 * sin_r * (beam(n).joint(3) ? 0 : 1) + sin_l * (beam(n).joint(1) ? 0 : 1) * k22 * cos_r * (beam(n).joint(4) ? 0 : 1);
			K[k_index[beam(n).leftNodeI()*3]][k_index[beam(n).rightNodeI()*3+2]] += sin_l * (beam(n).joint(1) ? 0 : 1) * k23 * (beam(n).joint(5) ? 0 : 1);
			K[k_index[beam(n).leftNodeI()*3]][k_index[u_indexed[6*n]]] += cos_l * (beam(n).joint(0) ? 0 : 1) * k11 * (beam(n).joint(0) ? 1 : 0);
			K[k_index[beam(n).leftNodeI()*3]][k_index[u_indexed[6*n+1]]] += -sin_l * (beam(n).joint(1) ? 0 : 1) * k22 * (beam(n).joint(1) ? 1 : 0);
			K[k_index[beam(n).leftNodeI()*3]][k_index[u_indexed[6*n+2]]] += sin_l * (beam(n).joint(1) ? 0 : 1) * k23 * (beam(n).joint(2) ? 1 : 0);
			K[k_index[beam(n).leftNodeI()*3]][k_index[u_indexed[6*n+3]]] += -cos_l * (beam(n).joint(0) ? 0 : 1) * k11 * (beam(n).joint(3) ? 1 : 0);
			K[k_index[beam(n).leftNodeI()*3]][k_index[u_indexed[6*n+4]]] += sin_l * (beam(n).joint(1) ? 0 : 1) * k22 * (beam(n).joint(4) ? 1 : 0);
			K[k_index[beam(n).leftNodeI()*3]][k_index[u_indexed[6*n+5]]] += sin_l * (beam(n).joint(1) ? 0 : 1) * k23 * (beam(n).joint(5) ? 1 : 0);

			K[k_index[beam(n).leftNodeI()*3+1]][k_index[beam(n).leftNodeI()*3]] += cos_l * (beam(n).joint(0) ? 0 : 1) * (beam(n).joint(0) ? 0 : 1) * k11 * sin_l - sin_l * (beam(n).joint(1) ? 0 : 1) * (beam(n).joint(1) ? 0 : 1) * k22 * cos_l;
			K[k_index[beam(n).leftNodeI()*3+1]][k_index[beam(n).leftNodeI()*3+1]] += sin_l * sin_l * (beam(n).joint(0) ? 0 : 1) * (beam(n).joint(0) ? 0 : 1) * k11 + cos_l * cos_l * (beam(n).joint(1) ? 0 : 1) * (beam(n).joint(1) ? 0 : 1) * k22;
			K[k_index[beam(n).leftNodeI()*3+1]][k_index[beam(n).leftNodeI()*3+2]] += -cos_l * (beam(n).joint(1) ? 0 : 1) * k23 * (beam(n).joint(2) ? 0 : 1);
			K[k_index[beam(n).leftNodeI()*3+1]][k_index[beam(n).rightNodeI()*3]] += -sin_l * (beam(n).joint(0) ? 0 : 1) * k11 * cos_r * (beam(n).joint(3) ? 0 : 1) + cos_l * (beam(n).joint(1) ? 0 : 1) * k22 * sin_r * (beam(n).joint(4) ? 0 : 1);
			K[k_index[beam(n).leftNodeI()*3+1]][k_index[beam(n).rightNodeI()*3+1]] += -sin_l * (beam(n).joint(0) ? 0 : 1) * k11 * sin_r * (beam(n).joint(3) ? 0 : 1) - cos_l * (beam(n).joint(1) ? 0 : 1) * k22 * cos_r * (beam(n).joint(4) ? 0 : 1);
			K[k_index[beam(n).leftNodeI()*3+1]][k_index[beam(n).rightNodeI()*3+2]] += -cos_l * (beam(n).joint(1) ? 0 : 1) * k23 * (beam(n).joint(5) ? 0 : 1);
			K[k_index[beam(n).leftNodeI()*3+1]][k_index[u_indexed[6*n]]] += sin_l * (beam(n).joint(0) ? 0 : 1) * k11 * (beam(n).joint(0) ? 1 : 0);
			K[k_index[beam(n).leftNodeI()*3+1]][k_index[u_indexed[6*n+1]]] += cos_l * (beam(n).joint(1) ? 0 : 1) * k22 * (beam(n).joint(1) ? 1 : 0);
			K[k_index[beam(n).leftNodeI()*3+1]][k_index[u_indexed[6*n+2]]] += -cos_l * (beam(n).joint(1) ? 0 : 1) * k23 * (beam(n).joint(2) ? 1 : 0);
			K[k_index[beam(n).leftNodeI()*3+1]][k_index[u_indexed[6*n+3]]] += -sin_l * (beam(n).joint(0) ? 0 : 1) * k11 * (beam(n).joint(3) ? 1 : 0);
			K[k_index[beam(n).leftNodeI()*3+1]][k_index[u_indexed[6*n+4]]] += -cos_l * (beam(n).joint(1) ? 0 : 1) * k22 * (beam(n).joint(4) ? 1 : 0);
			K[k_index[beam(n).leftNodeI()*3+1]][k_index[u_indexed[6*n+5]]] += -cos_l * (beam(n).joint(1) ? 0 : 1) * k23 * (beam(n).joint(5) ? 1 : 0);

			K[k_index[beam(n).leftNodeI()*3+2]][k_index[beam(n).leftNodeI()*3]] += sin_l * (beam(n).joint(1) ? 0 : 1) * k23 * (beam(n).joint(2) ? 0 : 1);
			K[k_index[beam(n).leftNodeI()*3+2]][k_index[beam(n).leftNodeI()*3+1]] += -cos_l * (beam(n).joint(1) ? 0 : 1) * k23 * (beam(n).joint(2) ? 0 : 1);
			K[k_index[beam(n).leftNodeI()*3+2]][k_index[beam(n).leftNodeI()*3+2]] += (beam(n).joint(2) ? 0 : 1) * (beam(n).joint(2) ? 0 : 1) * k33;
			K[k_index[beam(n).leftNodeI()*3+2]][k_index[beam(n).rightNodeI()*3]] += -(beam(n).joint(2) ? 0 : 1) * k23 * sin_r * (beam(n).joint(4) ? 0 : 1);
			K[k_index[beam(n).leftNodeI()*3+2]][k_index[beam(n).rightNodeI()*3+1]] += (beam(n).joint(2) ? 0 : 1) * k23 * cos_r * (beam(n).joint(4) ? 0 : 1);
			K[k_index[beam(n).leftNodeI()*3+2]][k_index[beam(n).rightNodeI()*3+2]] += (beam(n).joint(2) ? 0 : 1) * k36 * (beam(n).joint(5) ? 0 : 1);
			K[k_index[beam(n).leftNodeI()*3+2]][k_index[u_indexed[6*n]]] += 0;
			K[k_index[beam(n).leftNodeI()*3+2]][k_index[u_indexed[6*n+1]]] += -(beam(n).joint(2) ? 0 : 1) * k23 * (beam(n).joint(1) ? 1 : 0);
			K[k_index[beam(n).leftNodeI()*3+2]][k_index[u_indexed[6*n+2]]] += (beam(n).joint(2) ? 0 : 1) * k33 * (beam(n).joint(2) ? 1 : 0);
			K[k_index[beam(n).leftNodeI()*3+2]][k_index[u_indexed[6*n+3]]] += 0;
			K[k_index[beam(n).leftNodeI()*3+2]][k_index[u_indexed[6*n+4]]] += (beam(n).joint(2) ? 0 : 1) * k23 * (beam(n).joint(4) ? 1 : 0);
			K[k_index[beam(n).leftNodeI()*3+2]][k_index[u_indexed[6*n+5]]] += (beam(n).joint(2) ? 0 : 1) * k36 * (beam(n).joint(5) ? 1 : 0);

			K[k_index[beam(n).rightNodeI()*3]][k_index[beam(n).leftNodeI()*3]] += -cos_l * (beam(n).joint(0) ? 0 : 1) * k11 * cos_r * (beam(n).joint(3) ? 0 : 1) - sin_l * (beam(n).joint(1) ? 0 : 1) * k22 * sin_r * (beam(n).joint(4) ? 0 : 1);
			K[k_index[beam(n).rightNodeI()*3]][k_index[beam(n).leftNodeI()*3+1]] += -sin_l * (beam(n).joint(0) ? 0 : 1) * k11 * cos_r * (beam(n).joint(3) ? 0 : 1) + cos_l * (beam(n).joint(1) ? 0 : 1) * k22 * sin_r * (beam(n).joint(4) ? 0 : 1);
			K[k_index[beam(n).rightNodeI()*3]][k_index[beam(n).leftNodeI()*3+2]] += -(beam(n).joint(2) ? 0 : 1) * k23 * sin_r * (beam(n).joint(4) ? 0 : 1);
			K[k_index[beam(n).rightNodeI()*3]][k_index[beam(n).rightNodeI()*3]] += cos_r * cos_r * (beam(n).joint(3) ? 0 : 1) * (beam(n).joint(3) ? 0 : 1) * k11 + sin_r * sin_r * (beam(n).joint(4) ? 0 : 1) * (beam(n).joint(4) ? 0 : 1) * k22;
			K[k_index[beam(n).rightNodeI()*3]][k_index[beam(n).rightNodeI()*3+1]] += cos_r * (beam(n).joint(3) ? 0 : 1) * (beam(n).joint(3) ? 0 : 1) * k11 * sin_r - sin_r * (beam(n).joint(4) ? 0 : 1) * (beam(n).joint(4) ? 0 : 1) * k22 * cos_r;
			K[k_index[beam(n).rightNodeI()*3]][k_index[beam(n).rightNodeI()*3+2]] += -sin_r * (beam(n).joint(4) ? 0 : 1) * k23 * (beam(n).joint(5) ? 0 : 1);
			K[k_index[beam(n).rightNodeI()*3]][k_index[u_indexed[6*n]]] += -cos_r * (beam(n).joint(3) ? 0 : 1) * k11 * (beam(n).joint(0) ? 1 : 0);
			K[k_index[beam(n).rightNodeI()*3]][k_index[u_indexed[6*n+1]]] += sin_r * (beam(n).joint(4) ? 0 : 1) * k22 * (beam(n).joint(1) ? 1 : 0);
			K[k_index[beam(n).rightNodeI()*3]][k_index[u_indexed[6*n+2]]] += -sin_r * (beam(n).joint(4) ? 0 : 1) * k23 * (beam(n).joint(2) ? 1 : 0);
			K[k_index[beam(n).rightNodeI()*3]][k_index[u_indexed[6*n+3]]] += cos_r * (beam(n).joint(3) ? 0 : 1) * k11 * (beam(n).joint(3) ? 1 : 0);
			K[k_index[beam(n).rightNodeI()*3]][k_index[u_indexed[6*n+4]]] += -sin_r * (beam(n).joint(4) ? 0 : 1) * k22 * (beam(n).joint(4) ? 1 : 0);
			K[k_index[beam(n).rightNodeI()*3]][k_index[u_indexed[6*n+5]]] += -sin_r * (beam(n).joint(4) ? 0 : 1) * k23 * (beam(n).joint(5) ? 1 : 0);

			K[k_index[beam(n).rightNodeI()*3+1]][k_index[beam(n).leftNodeI()*3]] += -cos_l * (beam(n).joint(0) ? 0 : 1) * k11 * sin_r * (beam(n).joint(3) ? 0 : 1) + sin_l * (beam(n).joint(1) ? 0 : 1) * k22 * cos_r * (beam(n).joint(4) ? 0 : 1);
			K[k_index[beam(n).rightNodeI()*3+1]][k_index[beam(n).leftNodeI()*3+1]] += -sin_l * (beam(n).joint(0) ? 0 : 1) * k11 * sin_r * (beam(n).joint(3) ? 0 : 1) - cos_l * (beam(n).joint(1) ? 0 : 1) * k22 * cos_r * (beam(n).joint(4) ? 0 : 1);
			K[k_index[beam(n).rightNodeI()*3+1]][k_index[beam(n).leftNodeI()*3+2]] += (beam(n).joint(2) ? 0 : 1) * k23 * cos_r * (beam(n).joint(4) ? 0 : 1);
			K[k_index[beam(n).rightNodeI()*3+1]][k_index[beam(n).rightNodeI()*3]] += cos_r * (beam(n).joint(3) ? 0 : 1) * (beam(n).joint(3) ? 0 : 1) * k11 * sin_r - sin_r * (beam(n).joint(4) ? 0 : 1) * (beam(n).joint(4) ? 0 : 1) * k22 * cos_r;
			K[k_index[beam(n).rightNodeI()*3+1]][k_index[beam(n).rightNodeI()*3+1]] += sin_r * sin_r * (beam(n).joint(3) ? 0 : 1) * (beam(n).joint(3) ? 0 : 1) * k11 + cos_r * cos_r * (beam(n).joint(4) ? 0 : 1) * (beam(n).joint(4) ? 0 : 1) * k22;
			K[k_index[beam(n).rightNodeI()*3+1]][k_index[beam(n).rightNodeI()*3+2]] += cos_r * (beam(n).joint(4) ? 0 : 1) * k23 * (beam(n).joint(5) ? 0 : 1);
			K[k_index[beam(n).rightNodeI()*3+1]][k_index[u_indexed[6*n]]] += -sin_r * (beam(n).joint(3) ? 0 : 1) * k11 * (beam(n).joint(0) ? 1 : 0);
			K[k_index[beam(n).rightNodeI()*3+1]][k_index[u_indexed[6*n+1]]] += -cos_r * (beam(n).joint(4) ? 0 : 1) * k22 * (beam(n).joint(1) ? 1 : 0);
			K[k_index[beam(n).rightNodeI()*3+1]][k_index[u_indexed[6*n+2]]] += cos_r * (beam(n).joint(4) ? 0 : 1) * k23 * (beam(n).joint(2) ? 1 : 0);
			K[k_index[beam(n).rightNodeI()*3+1]][k_index[u_indexed[6*n+3]]] += sin_r * (beam(n).joint(3) ? 0 : 1) * k11 * (beam(n).joint(3) ? 1 : 0);
			K[k_index[beam(n).rightNodeI()*3+1]][k_index[u_indexed[6*n+4]]] += cos_r * (beam(n).joint(4) ? 0 : 1) * k22 * (beam(n).joint(4) ? 1 : 0);
			K[k_index[beam(n).rightNodeI()*3+1]][k_index[u_indexed[6*n+5]]] += cos_r * (beam(n).joint(4) ? 0 : 1) * k23 * (beam(n).joint(5) ? 1 : 0);

			K[k_index[beam(n).rightNodeI()*3+2]][k_index[beam(n).leftNodeI()*3]] += sin_l * (beam(n).joint(1) ? 0 : 1) * k23 * (beam(n).joint(5) ? 0 : 1);
			K[k_index[beam(n).rightNodeI()*3+2]][k_index[beam(n).leftNodeI()*3+1]] += -cos_l * (beam(n).joint(1) ? 0 : 1) * k23 * (beam(n).joint(5) ? 0 : 1);
			K[k_index[beam(n).rightNodeI()*3+2]][k_index[beam(n).leftNodeI()*3+2]] += (beam(n).joint(2) ? 0 : 1) * k36 * (beam(n).joint(5) ? 0 : 1);
			K[k_index[beam(n).rightNodeI()*3+2]][k_index[beam(n).rightNodeI()*3]] += -sin_r * (beam(n).joint(4) ? 0 : 1) * k23 * (beam(n).joint(5) ? 0 : 1);
			K[k_index[beam(n).rightNodeI()*3+2]][k_index[beam(n).rightNodeI()*3+1]] += cos_r * (beam(n).joint(4) ? 0 : 1) * k23 * (beam(n).joint(5) ? 0 : 1);
			K[k_index[beam(n).rightNodeI()*3+2]][k_index[beam(n).rightNodeI()*3+2]] += (beam(n).joint(5) ? 0 : 1) * (beam(n).joint(5) ? 0 : 1) * k33;
			K[k_index[beam(n).rightNodeI()*3+2]][k_index[u_indexed[6*n]]] += 0;
			K[k_index[beam(n).rightNodeI()*3+2]][k_index[u_indexed[6*n+1]]] += -(beam(n).joint(5) ? 0 : 1) * k23 * (beam(n).joint(1) ? 1 : 0);
			K[k_index[beam(n).rightNodeI()*3+2]][k_index[u_indexed[6*n+2]]] += (beam(n).joint(5) ? 0 : 1) * k36 * (beam(n).joint(2) ? 1 : 0);
			K[k_index[beam(n).rightNodeI()*3+2]][k_index[u_indexed[6*n+3]]] += 0;
			K[k_index[beam(n).rightNodeI()*3+2]][k_index[u_indexed[6*n+4]]] += (beam(n).joint(5) ? 0 : 1) * k23 * (beam(n).joint(4) ? 1 : 0);
			K[k_index[beam(n).rightNodeI()*3+2]][k_index[u_indexed[6*n+5]]] += (beam(n).joint(5) ? 0 : 1) * k33 * (beam(n).joint(5) ? 1 : 0);

			K[k_index[u_indexed[6*n]]][k_index[beam(n).leftNodeI()*3]] += cos_l * (beam(n).joint(0) ? 0 : 1) * k11 * (beam(n).joint(0) ? 1 : 0);
			K[k_index[u_indexed[6*n]]][k_index[beam(n).leftNodeI()*3+1]] += sin_l * (beam(n).joint(0) ? 0 : 1) * k11 * (beam(n).joint(0) ? 1 : 0);
			K[k_index[u_indexed[6*n]]][k_index[beam(n).leftNodeI()*3+2]] += 0;
			K[k_index[u_indexed[6*n]]][k_index[beam(n).rightNodeI()*3]] += -cos_r * (beam(n).joint(3) ? 0 : 1) * k11 * (beam(n).joint(0) ? 1 : 0);
			K[k_index[u_indexed[6*n]]][k_index[beam(n).rightNodeI()*3+1]] += -sin_r * (beam(n).joint(3) ? 0 : 1) * k11 * (beam(n).joint(0) ? 1 : 0);
			K[k_index[u_indexed[6*n]]][k_index[beam(n).rightNodeI()*3+2]] += 0;
			K[k_index[u_indexed[6*n]]][k_index[u_indexed[6*n]]] += (beam(n).joint(0) ? 1 : 0) * (beam(n).joint(0) ? 1 : 0) * k11;
			K[k_index[u_indexed[6*n]]][k_index[u_indexed[6*n+1]]] += 0;
			K[k_index[u_indexed[6*n]]][k_index[u_indexed[6*n+2]]] += 0;
			K[k_index[u_indexed[6*n]]][k_index[u_indexed[6*n+3]]] += -(beam(n).joint(0) ? 1 : 0) * k11 * (beam(n).joint(3) ? 1 : 0);
			K[k_index[u_indexed[6*n]]][k_index[u_indexed[6*n+4]]] += 0;
			K[k_index[u_indexed[6*n]]][k_index[u_indexed[6*n+5]]] += 0;

			K[k_index[u_indexed[6*n+1]]][k_index[beam(n).leftNodeI()*3]] += -sin_l * (beam(n).joint(1) ? 0 : 1) * k22 * (beam(n).joint(1) ? 1 : 0);
			K[k_index[u_indexed[6*n+1]]][k_index[beam(n).leftNodeI()*3+1]] += cos_l * (beam(n).joint(1) ? 0 : 1) * k22 * (beam(n).joint(1) ? 1 : 0);
			K[k_index[u_indexed[6*n+1]]][k_index[beam(n).leftNodeI()*3+2]] += -(beam(n).joint(2) ? 0 : 1) * k23 * (beam(n).joint(1) ? 1 : 0);
			K[k_index[u_indexed[6*n+1]]][k_index[beam(n).rightNodeI()*3]] += sin_r * (beam(n).joint(4) ? 0 : 1) * k22 * (beam(n).joint(1) ? 1 : 0);
			K[k_index[u_indexed[6*n+1]]][k_index[beam(n).rightNodeI()*3+1]] += -cos_r * (beam(n).joint(4) ? 0 : 1) * k22 * (beam(n).joint(1) ? 1 : 0);
			K[k_index[u_indexed[6*n+1]]][k_index[beam(n).rightNodeI()*3+2]] += -(beam(n).joint(5) ? 0 : 1) * k23 * (beam(n).joint(1) ? 1 : 0);
			K[k_index[u_indexed[6*n+1]]][k_index[u_indexed[6*n]]] += 0;
			K[k_index[u_indexed[6*n+1]]][k_index[u_indexed[6*n+1]]] += (beam(n).joint(1) ? 1 : 0) * (beam(n).joint(1) ? 1 : 0) * k22;
			K[k_index[u_indexed[6*n+1]]][k_index[u_indexed[6*n+2]]] += -(beam(n).joint(1) ? 1 : 0) * k23 * (beam(n).joint(2) ? 1 : 0);
			K[k_index[u_indexed[6*n+1]]][k_index[u_indexed[6*n+3]]] += 0;
			K[k_index[u_indexed[6*n+1]]][k_index[u_indexed[6*n+4]]] += -(beam(n).joint(1) ? 1 : 0) * k22 * (beam(n).joint(4) ? 1 : 0);
			K[k_index[u_indexed[6*n+1]]][k_index[u_indexed[6*n+5]]] += -(beam(n).joint(1) ? 1 : 0) * k23 * (beam(n).joint(5) ? 1 : 0);

			K[k_index[u_indexed[6*n+2]]][k_index[beam(n).leftNodeI()*3]] += sin_l * (beam(n).joint(1) ? 0 : 1) * k23 * (beam(n).joint(2) ? 1 : 0);
			K[k_index[u_indexed[6*n+2]]][k_index[beam(n).leftNodeI()*3+1]] += -cos_l * (beam(n).joint(1) ? 0 : 1) * k23 * (beam(n).joint(2) ? 1 : 0);
			K[k_index[u_indexed[6*n+2]]][k_index[beam(n).leftNodeI()*3+2]] += (beam(n).joint(2) ? 0 : 1) * k33 * (beam(n).joint(2) ? 1 : 0);
			K[k_index[u_indexed[6*n+2]]][k_index[beam(n).rightNodeI()*3]] += -sin_r * (beam(n).joint(4) ? 0 : 1) * k23 * (beam(n).joint(2) ? 1 : 0);
			K[k_index[u_indexed[6*n+2]]][k_index[beam(n).rightNodeI()*3+1]] += cos_r * (beam(n).joint(4) ? 0 : 1) * k23 * (beam(n).joint(2) ? 1 : 0);
			K[k_index[u_indexed[6*n+2]]][k_index[beam(n).rightNodeI()*3+2]] += (beam(n).joint(5) ? 0 : 1) * k36 * (beam(n).joint(2) ? 1 : 0);
			K[k_index[u_indexed[6*n+2]]][k_index[u_indexed[6*n]]] += 0;
			K[k_index[u_indexed[6*n+2]]][k_index[u_indexed[6*n+1]]] += -(beam(n).joint(1) ? 1 : 0) * k23 * (beam(n).joint(2) ? 1 : 0);
			K[k_index[u_indexed[6*n+2]]][k_index[u_indexed[6*n+2]]] += (beam(n).joint(2) ? 1 : 0) * (beam(n).joint(2) ? 1 : 0) * k33;
			K[k_index[u_indexed[6*n+2]]][k_index[u_indexed[6*n+3]]] += 0;
			K[k_index[u_indexed[6*n+2]]][k_index[u_indexed[6*n+4]]] += (beam(n).joint(2) ? 1 : 0) * k23 * (beam(n).joint(4) ? 1 : 0);
			K[k_index[u_indexed[6*n+2]]][k_index[u_indexed[6*n+5]]] += (beam(n).joint(2) ? 1 : 0) * k36 * (beam(n).joint(5) ? 1 : 0);

			K[k_index[u_indexed[6*n+3]]][k_index[beam(n).leftNodeI()*3]] += -cos_l * (beam(n).joint(0) ? 0 : 1) * k11 * (beam(n).joint(3) ? 1 : 0);
			K[k_index[u_indexed[6*n+3]]][k_index[beam(n).leftNodeI()*3+1]] += -sin_l * (beam(n).joint(0) ? 0 : 1) * k11 * (beam(n).joint(3) ? 1 : 0);
			K[k_index[u_indexed[6*n+3]]][k_index[beam(n).leftNodeI()*3+2]] += 0;
			K[k_index[u_indexed[6*n+3]]][k_index[beam(n).rightNodeI()*3]] += cos_r * (beam(n).joint(3) ? 0 : 1) * k11 * (beam(n).joint(3) ? 1 : 0);
			K[k_index[u_indexed[6*n+3]]][k_index[beam(n).rightNodeI()*3+1]] += sin_r * (beam(n).joint(3) ? 0 : 1) * k11 * (beam(n).joint(3) ? 1 : 0);
			K[k_index[u_indexed[6*n+3]]][k_index[beam(n).rightNodeI()*3+2]] += 0;
			K[k_index[u_indexed[6*n+3]]][k_index[u_indexed[6*n]]] += -(beam(n).joint(0) ? 1 : 0) * k11 * (beam(n).joint(3) ? 1 : 0);
			K[k_index[u_indexed[6*n+3]]][k_index[u_indexed[6*n+1]]] += 0;
			K[k_index[u_indexed[6*n+3]]][k_index[u_indexed[6*n+2]]] += 0;
			K[k_index[u_indexed[6*n+3]]][k_index[u_indexed[6*n+3]]] += (beam(n).joint(3) ? 1 : 0) * (beam(n).joint(3) ? 1 : 0) * k11;
			K[k_index[u_indexed[6*n+3]]][k_index[u_indexed[6*n+4]]] += 0;
			K[k_index[u_indexed[6*n+3]]][k_index[u_indexed[6*n+5]]] += 0;

			K[k_index[u_indexed[6*n+4]]][k_index[beam(n).leftNodeI()*3]] += sin_l * (beam(n).joint(1) ? 0 : 1) * k22 * (beam(n).joint(4) ? 1 : 0);
			K[k_index[u_indexed[6*n+4]]][k_index[beam(n).leftNodeI()*3+1]] += -cos_l * (beam(n).joint(1) ? 0 : 1) * k22 * (beam(n).joint(4) ? 1 : 0);
			K[k_index[u_indexed[6*n+4]]][k_index[beam(n).leftNodeI()*3+2]] += (beam(n).joint(2) ? 0 : 1) * k23 * (beam(n).joint(4) ? 1 : 0);
			K[k_index[u_indexed[6*n+4]]][k_index[beam(n).rightNodeI()*3]] += -sin_r * (beam(n).joint(4) ? 0 : 1) * k22 * (beam(n).joint(4) ? 1 : 0);
			K[k_index[u_indexed[6*n+4]]][k_index[beam(n).rightNodeI()*3+1]] += cos_r * (beam(n).joint(4) ? 0 : 1) * k22 * (beam(n).joint(4) ? 1 : 0);
			K[k_index[u_indexed[6*n+4]]][k_index[beam(n).rightNodeI()*3+2]] += (beam(n).joint(5) ? 0 : 1) * k23 * (beam(n).joint(4) ? 1 : 0);
			K[k_index[u_indexed[6*n+4]]][k_index[u_indexed[6*n]]] += 0;
			K[k_index[u_indexed[6*n+4]]][k_index[u_indexed[6*n+1]]] += -(beam(n).joint(1) ? 1 : 0) * k22 * (beam(n).joint(4) ? 1 : 0);
			K[k_index[u_indexed[6*n+4]]][k_index[u_indexed[6*n+2]]] += (beam(n).joint(2) ? 1 : 0) * k23 * (beam(n).joint(4) ? 1 : 0);
			K[k_index[u_indexed[6*n+4]]][k_index[u_indexed[6*n+3]]] += 0;
			K[k_index[u_indexed[6*n+4]]][k_index[u_indexed[6*n+4]]] += (beam(n).joint(4) ? 1 : 0) * (beam(n).joint(4) ? 1 : 0) * k22;
			K[k_index[u_indexed[6*n+4]]][k_index[u_indexed[6*n+5]]] += (beam(n).joint(4) ? 1 : 0) * k23 * (beam(n).joint(5) ? 1 : 0);

			K[k_index[u_indexed[6*n+5]]][k_index[beam(n).leftNodeI()*3]] += sin_l * (beam(n).joint(1) ? 0 : 1) * k23 * (beam(n).joint(5) ? 1 : 0);
			K[k_index[u_indexed[6*n+5]]][k_index[beam(n).leftNodeI()*3+1]] += -cos_l * (beam(n).joint(1) ? 0 : 1) * k23 * (beam(n).joint(5) ? 1 : 0);
			K[k_index[u_indexed[6*n+5]]][k_index[beam(n).leftNodeI()*3+2]] += (beam(n).joint(2) ? 0 : 1) * k36 * (beam(n).joint(5) ? 1 : 0);
			K[k_index[u_indexed[6*n+5]]][k_index[beam(n).rightNodeI()*3]] += -sin_r * (beam(n).joint(4) ? 0 : 1) * k23 * (beam(n).joint(5) ? 1 : 0);
			K[k_index[u_indexed[6*n+5]]][k_index[beam(n).rightNodeI()*3+1]] += cos_r * (beam(n).joint(4) ? 0 : 1) * k23 * (beam(n).joint(5) ? 1 : 0);
			K[k_index[u_indexed[6*n+5]]][k_index[beam(n).rightNodeI()*3+2]] += (beam(n).joint(5) ? 0 : 1) * k33 * (beam(n).joint(5) ? 1 : 0);
			K[k_index[u_indexed[6*n+5]]][k_index[u_indexed[6*n]]] += 0;
			K[k_index[u_indexed[6*n+5]]][k_index[u_indexed[6*n+1]]] += -(beam(n).joint(1) ? 1 : 0) * k23 * (beam(n).joint(5) ? 1 : 0);
			K[k_index[u_indexed[6*n+5]]][k_index[u_indexed[6*n+2]]] += (beam(n).joint(2) ? 1 : 0) * k36 * (beam(n).joint(5) ? 1 : 0);
			K[k_index[u_indexed[6*n+5]]][k_index[u_indexed[6*n+3]]] += 0;
			K[k_index[u_indexed[6*n+5]]][k_index[u_indexed[6*n+4]]] += (beam(n).joint(4) ? 1 : 0) * k23 * (beam(n).joint(5) ? 1 : 0);
			K[k_index[u_indexed[6*n+5]]][k_index[u_indexed[6*n+5]]] += (beam(n).joint(5) ? 1 : 0) * (beam(n).joint(5) ? 1 : 0) * k33;


			//adding the springs to the k-matrix



			if(beam(n).joint(0) && beam(n).spring(0)!=0)
			{
				K[k_index[beam(n).leftNodeI()*3]][k_index[beam(n).leftNodeI()*3]] += cos_l *cos_l * beam(n).spring(0);
				K[k_index[beam(n).leftNodeI()*3]][k_index[beam(n).leftNodeI()*3+1]] += cos_l * beam(n).spring(0) * sin_l;
				K[k_index[beam(n).leftNodeI()*3]][k_index[u_indexed[6*n]]] += -cos_l * beam(n).spring(0);

				K[k_index[beam(n).leftNodeI()*3+1]][k_index[beam(n).leftNodeI()*3]] += cos_l * beam(n).spring(0) * sin_l;
				K[k_index[beam(n).leftNodeI()*3+1]][k_index[beam(n).leftNodeI()*3+1]] += sin_l * sin_l * beam(n).spring(0);
				K[k_index[beam(n).leftNodeI()*3+1]][k_index[u_indexed[6*n]]] += -sin_l * beam(n).spring(0);

				K[k_index[u_indexed[6*n]]][k_index[beam(n).leftNodeI()*3]] += -cos_l * beam(n).spring(0);
				K[k_index[u_indexed[6*n]]][k_index[beam(n).leftNodeI()*3+1]] += -sin_l * beam(n).spring(0);
				K[k_index[u_indexed[6*n]]][k_index[u_indexed[6*n]]] += beam(n).spring(0);
			}

			if(beam(n).joint(1) && beam(n).spring(1)!=0)
			{
				K[k_index[beam(n).leftNodeI()*3]][k_index[beam(n).leftNodeI()*3]] += sin_l *sin_l * beam(n).spring(1);
				K[k_index[beam(n).leftNodeI()*3]][k_index[beam(n).leftNodeI()*3+1]] += -cos_l * beam(n).spring(1) * sin_l;
				K[k_index[beam(n).leftNodeI()*3]][k_index[u_indexed[6*n+1]]] += sin_l * beam(n).spring(1);

				K[k_index[beam(n).leftNodeI()*3+1]][k_index[beam(n).leftNodeI()*3]] += -cos_l * beam(n).spring(1) * sin_l;
				K[k_index[beam(n).leftNodeI()*3+1]][k_index[beam(n).leftNodeI()*3+1]] += cos_l * cos_l * beam(n).spring(1);
				K[k_index[beam(n).leftNodeI()*3+1]][k_index[u_indexed[6*n+1]]] += -cos_l * beam(n).spring(1);

				K[k_index[u_indexed[6*n+1]]][k_index[beam(n).leftNodeI()*3]] += sin_l * beam(n).spring(1);
				K[k_index[u_indexed[6*n+1]]][k_index[beam(n).leftNodeI()*3+1]] += -cos_l * beam(n).spring(1);
				K[k_index[u_indexed[6*n+1]]][k_index[u_indexed[6*n+1]]] += beam(n).spring(1);

			}

			if(beam(n).joint(2) && beam(n).spring(2)!=0)
			{
				K[k_index[beam(n).leftNodeI()*3+2]][k_index[beam(n).leftNodeI()*3+2]] +=beam(n).spring(2);
				K[k_index[beam(n).leftNodeI()*3+2]][k_index[u_indexed[6*n+2]]] +=-beam(n).spring(2);

				K[k_index[u_indexed[6*n+2]]][k_index[beam(n).leftNodeI()*3+2]] +=-beam(n).spring(2);
				K[k_index[u_indexed[6*n+2]]][k_index[u_indexed[6*n+2]]] +=beam(n).spring(2);
			}

			if(beam(n).joint(3) && beam(n).spring(3)!=0)
			{
				K[k_index[beam(n).rightNodeI()*3]][k_index[beam(n).rightNodeI()*3]] += cos_r *cos_r * beam(n).spring(3);
				K[k_index[beam(n).rightNodeI()*3]][k_index[beam(n).rightNodeI()*3+1]] += cos_r * beam(n).spring(3) * sin_r;
				K[k_index[beam(n).rightNodeI()*3]][k_index[u_indexed[6*n+3]]] += -cos_r * beam(n).spring(3);

				K[k_index[beam(n).rightNodeI()*3+1]][k_index[beam(n).rightNodeI()*3]] += cos_r * beam(n).spring(3) * sin_r;
				K[k_index[beam(n).rightNodeI()*3+1]][k_index[beam(n).rightNodeI()*3+1]] += sin_r * sin_r * beam(n).spring(3);
				K[k_index[beam(n).rightNodeI()*3+1]][k_index[u_indexed[6*n+3]]] += -sin_r * beam(n).spring(3);

				K[k_index[u_indexed[6*n+3]]][k_index[beam(n).rightNodeI()*3]] += -cos_r * beam(n).spring(3);
				K[k_index[u_indexed[6*n+3]]][k_index[beam(n).rightNodeI()*3+1]] += -sin_r * beam(n).spring(3);
				K[k_index[u_indexed[6*n+3]]][k_index[u_indexed[6*n+3]]] += beam(n).spring(3);
			}

			if(beam(n).joint(4) && beam(n).spring(4)!=0)
			{
				K[k_index[beam(n).rightNodeI()*3]][k_index[beam(n).rightNodeI()*3]] += sin_r *sin_r * beam(n).spring(4);
				K[k_index[beam(n).rightNodeI()*3]][k_index[beam(n).rightNodeI()*3+1]] += -cos_r * beam(n).spring(4) * sin_r;
				K[k_index[beam(n).rightNodeI()*3]][k_index[u_indexed[6*n+4]]] += sin_r * beam(n).spring(4);

				K[k_index[beam(n).rightNodeI()*3+1]][k_index[beam(n).rightNodeI()*3]] += -cos_r * beam(n).spring(4) * sin_r;
				K[k_index[beam(n).rightNodeI()*3+1]][k_index[beam(n).rightNodeI()*3+1]] += cos_r * cos_r * beam(n).spring(4);
				K[k_index[beam(n).rightNodeI()*3+1]][k_index[u_indexed[6*n+4]]] += -cos_r * beam(n).spring(4);

				K[k_index[u_indexed[6*n+4]]][k_index[beam(n).rightNodeI()*3]] += sin_r * beam(n).spring(4);
				K[k_index[u_indexed[6*n+4]]][k_index[beam(n).rightNodeI()*3+1]] += -cos_r * beam(n).spring(4);
				K[k_index[u_indexed[6*n+4]]][k_index[u_indexed[6*n+4]]] += beam(n).spring(4);
			}

			if(beam(n).joint(5) && beam(n).spring(5)!=0)
			{
				K[k_index[beam(n).rightNodeI()*3+2]][k_index[beam(n).rightNodeI()*3+2]] +=beam(n).spring(5);
				K[k_index[beam(n).rightNodeI()*3+2]][k_index[u_indexed[6*n+5]]] +=-beam(n).spring(5);

				K[k_index[u_indexed[6*n+5]]][k_index[beam(n).rightNodeI()*3+2]] +=-beam(n).spring(5);
				K[k_index[u_indexed[6*n+5]]][k_index[u_indexed[6*n+5]]] +=beam(n).spring(5);
			}
		}

		//springs from bearings:
		for(int n=0; n<node_n; n++)
		{
			if(node(n).hasbearing())
			{
				if(node(n).bearing().c_x()!=0)
					K[k_index[3*n]][k_index[3*n]] +=node(n).bearing().c_x();
				if(node(n).bearing().c_z()!=0)
					K[k_index[3*n+1]][k_index[3*n+1]] +=node(n).bearing().c_z();
				if(node(n).bearing().c_phi()!=0)
					K[k_index[3*n+2]][k_index[3*n+2]] +=node(n).bearing().c_phi();
			}
		}

#ifndef QT_NO_DEBUG
		QTextStream debugS(stderr);
		debugS << endl << "global KMatrix #" << ls << endl << K << endl;
#endif
	}
}

void wgv::make_s0()
{
	dT = new double*[cls_n];
	dTz = new double*[cls_n];
	px = new polynom<1>*[cls_n];
	pz = new polynom<1>*[cls_n];

	dT_array = new double[cls_n*beam_n];
	dTz_array = new double[cls_n*beam_n];
	px_array = new polynom<1>[cls_n*beam_n];
	pz_array = new polynom<1>[cls_n*beam_n];

	s0 = new quadvector<double>[cls_n];

	for(int ls=0; ls<cls_n; ls++)
	{
		dT[ls] = &dT_array[ls*beam_n];
		dTz[ls] = &dTz_array[ls*beam_n];
		px[ls] = &px_array[ls*beam_n];
		pz[ls] = &pz_array[ls*beam_n];

		s0[ls] = quadvector<double>(beam_n*6);

		for(int i=0; i<beam_n; i++)
		{
			px[ls][i]=polynom<1>(0, 0);
			pz[ls][i]=polynom<1>(0, 0);
			dT[ls][i]=0;
			dTz[ls][i]=0;
		}
		//old allocation: s0_=new double[beam_n()*6];
		//for(int n=0; n<beam_n()*6; n++)
		//    s0_[n]=0;

		//all in the local koordsys of the beams....
		//mapping to global sys in p()
		int n;
		for(n=0; n<load_n; n++)
		{
			if(!cls[ls].contains(load(n).set()))
				continue;
			if(load(n).typ() == ColinLoad::nodeLoad ||
			   load(n).typ() == ColinLoad::moment   )
				continue;

			double clsFak = cls[ls].facOfBLS(load(n).set());

			//beams[load(n).at()].addtoqvonx(load(n));


			if(load(n).typ() == ColinLoad::uniformlyDistibutedLoad)
			{
				px[ls][load(n).at()] += polynom<1>(0,
												  clsFak*	(cos(beam(load(n).at()).angle())*load(n).Px()
															 +sin(beam(load(n).at()).angle())*load(n).Pz()));

				pz[ls][load(n).at()] += polynom<1>(0,
												   clsFak*	(cos(beam(load(n).at()).angle())*load(n).Pz()+
															 -sin(beam(load(n).at()).angle())*load(n).Px()));

				s0[ls][6*load(n).at()]+=	clsFak*	(-cos(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()/2
													 -sin(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()/2);
				s0[ls][6*load(n).at()+1]+=	clsFak*	(sin(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()/2
													 -cos(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()/2);
				s0[ls][6*load(n).at()+2]+=	clsFak*	(-sin(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()*beam(load(n).at()).l()/12
													 +cos(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()*beam(load(n).at()).l()/12);


				s0[ls][6*load(n).at()+3]+=	clsFak*	(-cos(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()/2
													-sin(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()/2);
				s0[ls][6*load(n).at()+4]+=	clsFak*	(sin(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()/2
													-cos(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()/2);
				s0[ls][6*load(n).at()+5]+=	clsFak*	(sin(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()*beam(load(n).at()).l()/12
													-cos(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()*beam(load(n).at()).l()/12);
			}
			else if(load(n).typ() == ColinLoad::increasingLinearLoad)
			{

				px[ls][load(n).at()] += polynom<1>(	clsFak*	(cos(beam(load(n).at()).angle())*load(n).Px()/beam(load(n).at()).l()
															+sin(beam(load(n).at()).angle())*load(n).Pz()/beam(load(n).at()).l()),
													0);

				pz[ls][load(n).at()] += polynom<1>(	clsFak*	(cos(beam(load(n).at()).angle())*load(n).Pz()/beam(load(n).at()).l()+
															-sin(beam(load(n).at()).angle())*load(n).Px()/beam(load(n).at()).l()),
													0);

				s0[ls][6*load(n).at()]+=	clsFak*	(-cos(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()*3/20
													-sin(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()*3/20);
				s0[ls][6*load(n).at()+1]+=	clsFak*	(sin(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()*3/20
													-cos(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()*3/20);
				s0[ls][6*load(n).at()+2]+=	clsFak*	(-sin(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()*beam(load(n).at()).l()/30
													+cos(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()*beam(load(n).at()).l()/30);


				s0[ls][6*load(n).at()+3]+=	clsFak*	(-cos(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()*7/20
													-sin(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()*7/20);
				s0[ls][6*load(n).at()+4]+=	clsFak*	(sin(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()*7/20
													-cos(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()*7/20);
				s0[ls][6*load(n).at()+5]+=	clsFak*	(sin(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()*beam(load(n).at()).l()/20
													-cos(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()*beam(load(n).at()).l()/20);
			}
			else if(load(n).typ() == ColinLoad::decreasingLinearLoad)
			{
				px[ls][load(n).at()] += polynom<1>(	clsFak*	(-cos(beam(load(n).at()).angle())*load(n).Px()/beam(load(n).at()).l()
															-sin(beam(load(n).at()).angle())*load(n).Pz()/beam(load(n).at()).l()),
													clsFak*	(cos(beam(load(n).at()).angle())*load(n).Px()
															+sin(beam(load(n).at()).angle())*load(n).Pz()));

				pz[ls][load(n).at()] += polynom<1>(	clsFak*	(-cos(beam(load(n).at()).angle())*load(n).Pz()/beam(load(n).at()).l()
															+sin(beam(load(n).at()).angle())*load(n).Px()/beam(load(n).at()).l()),
													clsFak*	(cos(beam(load(n).at()).angle())*load(n).Pz()
															-sin(beam(load(n).at()).angle())*load(n).Px()));

				s0[ls][6*load(n).at()]+=	clsFak*	(-cos(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()*7/20
													-sin(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()*7/20);
				s0[ls][6*load(n).at()+1]+=	clsFak*	(sin(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()*7/20
													-cos(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()*7/20);
				s0[ls][6*load(n).at()+2]+=	clsFak*	(-sin(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()*beam(load(n).at()).l()/20
													+cos(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()*beam(load(n).at()).l()/20);


				s0[ls][6*load(n).at()+3]+=	clsFak*	(-cos(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()*3/20
													-sin(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()*3/20);
				s0[ls][6*load(n).at()+4]+=	clsFak*	(sin(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()*3/20
													-cos(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()*3/20);
				s0[ls][6*load(n).at()+5]+=	clsFak*	(sin(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()*beam(load(n).at()).l()/30
													-cos(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()*beam(load(n).at()).l()/30);

			}
			else if(load(n).typ() == ColinLoad::tempChange)
			{
				s0[ls][6*load(n).at()]+=	clsFak*	(+beam(load(n).at()).E()*beam(load(n).at()).A()*load(n).Px()*beam(load(n).at()).aT());

				s0[ls][6*load(n).at()+3]+=	clsFak*	(-beam(load(n).at()).E()*beam(load(n).at()).A()*load(n).Px()*beam(load(n).at()).aT());

				dT[ls][load(n).at()]+=	clsFak*	(load(n).Px());
			}
			else if(load(n).typ() == ColinLoad::tempDiffrence)
			{
				s0[ls][6*load(n).at()+2]+=	clsFak*	(beam(load(n).at()).E()*beam(load(n).at()).I()*load(n).Pz()*beam(load(n).at()).aT());

				s0[ls][6*load(n).at()+5]+=	clsFak*	(-beam(load(n).at()).E()*beam(load(n).at()).I()*load(n).Pz()*beam(load(n).at()).aT());

				dTz[ls][load(n).at()]+=	clsFak*	(load(n).Pz());
			}
			else if(load(n).typ() == ColinLoad::doubleLoadLeft)
			{

			}
			else if(load(n).typ() == ColinLoad::doubleLoadRight)
			{

			}

		}
}

/*
#ifndef QT_NO_DEBUG
    QTextStream debugS(stdout);
    debugS << endl << "s0" << endl << s0_ << endl;
#endif
*/
}

void wgv::make_p()
{
    int n;
	p = new quadvector<double>[cls_n];

	for(int ls=0; ls<cls_n; ls++)
		p[ls] = quadvector<double>(f_n);

	for(int ls=0; ls<cls_n; ls++)
	{
		quadvector<double> &p = this->p[ls];
		quadvector<double> &s0 = this->s0[ls];
		quadmatrix<double> &K = this->K[ls];
		for(n=0; n<load_n; n++)
		{
			if(!cls[ls].contains(load(n).set()))
				continue;
			double clsFak = cls[ls].facOfBLS(load(n).set());
			if(load(n).typ() == ColinLoad::nodeLoad ||
			   load(n).typ() == ColinLoad::moment   )
			{
				if(node(load(n).at()).hasbearing())
				{
					p[k_index[3*load(n).at()]] +=	clsFak*	(load(n).Px()*cos(node(load(n).at()).angle())+load(n).Pz()*sin(node(load(n).at()).angle()));
					p[k_index[3*load(n).at()+1]] += clsFak*	(-load(n).Px()*sin(node(load(n).at()).angle())+load(n).Pz()*cos(node(load(n).at()).angle()));
					p[k_index[3*load(n).at()+2]] += clsFak*	(load(n).M());
				}
				else
				{
					p[k_index[3*load(n).at()]] +=	clsFak*	(load(n).Px());
					p[k_index[3*load(n).at()+1]] +=	clsFak*	(load(n).Pz());
					p[k_index[3*load(n).at()+2]] +=	clsFak*	(load(n).M());
				}
			}
			if(load(n).typ() == ColinLoad::doubleLoadLeft)
			{
				if(beam(load(n).at()).joint(0) && load(n).Px()!=0)
				{
				p[k_index[3*beam(load(n).at()).leftNodeI()]] +=		clsFak*	(cos(d_alpha_l(load(n).at()))*load(n).Px());
				p[k_index[3*beam(load(n).at()).leftNodeI()+1]] +=	clsFak*	(sin(d_alpha_l(load(n).at()))*load(n).Px());
				p[k_index[u_indexed[6*load(n).at()]]] +=			clsFak*	(-load(n).Px());
				}
				if(beam(load(n).at()).joint(1) && load(n).Pz()!=0)
				{
				p[k_index[3*beam(load(n).at()).leftNodeI()]] +=		clsFak*	(-sin(d_alpha_l(load(n).at()))*load(n).Pz());
				p[k_index[3*beam(load(n).at()).leftNodeI()+1]] +=	clsFak*	(cos(d_alpha_l(load(n).at()))*load(n).Pz());
				p[k_index[u_indexed[6*load(n).at()+1]]] +=			clsFak*	(-load(n).Pz());
				}
				if(beam(load(n).at()).joint(2) && load(n).M()!=0)
				{
				p[k_index[3*beam(load(n).at()).leftNodeI()+2]] +=	clsFak*	(load(n).M());
				p[k_index[u_indexed[6*load(n).at()+2]]] +=			clsFak*	(-load(n).M());
				}
			}
			if(load(n).typ() == ColinLoad::doubleLoadRight)
			{
				if(beam(load(n).at()).joint(3) && load(n).Px()!=0)
				{
						p[k_index[3*beam(load(n).at()).rightNodeI()]] +=	clsFak*	(-cos(d_alpha_r(load(n).at()))*load(n).Px());
						p[k_index[3*beam(load(n).at()).rightNodeI()+1]] +=	clsFak*	(-sin(d_alpha_r(load(n).at()))*load(n).Px());
						p[k_index[u_indexed[6*load(n).at()+3]]] +=			clsFak*	(load(n).Px());
				}
				if(beam(load(n).at()).joint(4) && load(n).Pz()!=0)
				{
						p[k_index[3*beam(load(n).at()).rightNodeI()]] +=	clsFak*	(sin(d_alpha_r(load(n).at()))*load(n).Pz());
						p[k_index[3*beam(load(n).at()).rightNodeI()+1]] +=	clsFak*	(-cos(d_alpha_r(load(n).at()))*load(n).Pz());
						p[k_index[u_indexed[6*load(n).at()+4]]] +=			clsFak*	(load(n).Pz());
				}
				if(beam(load(n).at()).joint(5) && load(n).M()!=0)
				{
						p[k_index[3*beam(load(n).at()).rightNodeI()+2]] +=	clsFak*	(load(n).M());
						p[k_index[u_indexed[6*load(n).at()+5]]] +=			clsFak*	(-load(n).M());
				}
			}
		}

		for(n=0;n<beam_n; n++)
		{
			if(!beam(n).joint(0))
			{
				p[k_index[beam(n).leftNodeI()*3]] += -cos(d_alpha_l(n))*s0[n*6];
				p[k_index[beam(n).leftNodeI()*3+1]] += -sin(d_alpha_l(n))*s0[n*6];
			}
			else
			{
				p[k_index[u_indexed[6*n]]] += -s0[n*6];
				//p_[u_indexed[6*n+1)]] += -s0_[n*6);
			}
			if(!beam(n).joint(1))
			{
				p[k_index[beam(n).leftNodeI()*3]] += sin(d_alpha_l(n))*s0[n*6+1];
			p[k_index[beam(n).leftNodeI()*3+1]] += -cos(d_alpha_l(n))*s0[n*6+1];
			}
			else
			{
				//p_[u_indexed[6*n)]] += -s0_[n*6+1);
				p[k_index[u_indexed[6*n+1]]] -= s0[n*6+1];
			}

			if(!beam(n).joint(2))
				p[k_index[beam(n).leftNodeI()*3+2]] -= s0[n*6+2];
			else
				p[k_index[u_indexed[6*n+2]]] -= s0[n*6+2];


			//----------------------------------------------------------


			if(!beam(n).joint(3))
			{
				p[k_index[beam(n).rightNodeI()*3]] -= cos(d_alpha_r(n))*s0[n*6+3];
				p[k_index[beam(n).rightNodeI()*3+1]] -= sin(d_alpha_r(n))*s0[n*6+3];
			}
			else
			{
				p[k_index[u_indexed[6*n+3]]] -= s0[n*6+3];
				//p_[u_indexed[6*n+4)]] -= s0_[n*6+3);
			}
			if(!beam(n).joint(4))
			{
				p[k_index[beam(n).rightNodeI()*3]] += sin(d_alpha_r(n))*s0[n*6+4];
				p[k_index[beam(n).rightNodeI()*3+1]] -= cos(d_alpha_r(n))*s0[n*6+4];
			}
			else
			{
				//p_[u_indexed[6*n+3)]] += -s0_[n*6+4);
				p[k_index[u_indexed[6*n+4]]] -= s0[n*6+4];
			}

			if(!beam(n).joint(5))
				p[k_index[beam(n).rightNodeI()*3+2]] -= s0[n*6+5];
			else
				p[k_index[u_indexed[6*n+5]]] -= s0[n*6+5];

		}

		K.cutAt(f_n-a_n);
	}





}
