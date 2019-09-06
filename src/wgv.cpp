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
/***********************************************************************************************************
  Programming Is Like Sex: One mistake and you have to support it for the rest of your life. (Michael Sinz)
 ***********************************************************************************************************/


wgv::wgv(wgv_tw *parent) :
    QThread(parent)
{
    //copy all the suff

    //my english sucks :(

    tw = parent;

    nodes = new wgv_node[tw->node_n()];     //init arrays for objects
    beams = new wgv_beam[tw->beam_n()];
    loads = new wgv_load[tw->load_n()];

    node_n = tw->node_n();                  //count and store objects
    beam_n = tw->beam_n();
    load_n = tw->load_n();

    for(int i=0; i<tw->node_n(); i++)       //only required for bearings
        nodes[i] = tw->nodes[i];

    for(int i=0; i<tw->beam_n(); i++){
        beams[i] = tw->beam(i);
        beams[i].setParent(0);}             //for lenght and angle the beam requests x- and z-koordinates from his parent
                                            //operator=(...) stores them in the object so no more parent is required
    for(int i=0; i<tw->load_n(); i++)
        loads[i] = tw->load(i);
    //all coping done

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
	if(dT)
		delete[] dT;
	if(dTz)
		delete[] dTz;
	if(px)
		delete[] px;
	if(pz)
		delete[] pz;
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

    ua = quadvector<double>(a_n);       //for now...

    submatrix<double> Ka(K, 1, 0);





#ifndef QT_NO_DEBUG
    quadvector<double> pcopy = p;
    quadmatrix<double> Kcopy(K.fullSize());
    for(int i=0; i<K.fullSize(); i++)
    {
        for(int j=0; j<K.fullSize(); j++)
        {
            Kcopy[i][j]=K[i][j];
        }
    }
    Kcopy.cutAt(K.size());
#endif


    try
    {
        u = p/K;
    }
    catch(SingularMatrixException& e)
    {
        success = false;
        return;
    }
    pa = u*Ka;




#ifndef QT_NO_DEBUG
    QTextStream debugS(stdout);
    debugS << "u" << endl << u << endl;
    debugS << "a" << endl << pa << endl;
    debugS << "K" << endl << K << endl;
#endif



#ifndef QT_NO_DEBUG
    quadvector<double> pc = u;
    pc*=Kcopy;
    debugS << endl << "p;" << "pc;"<< "p-pc;"<< endl;

    for(int i = 0; i<pc.size(); i++)
    {
        debugS << pcopy[i] << ";" << pc[i] << ";" << pc[i]-pcopy[i] << endl;
    }
#endif


    writeResultsNodes();
    writeResultsBeams();


#ifndef QT_NO_DEBUG
    debugS << endl << QString("time elapsed: %1 ms").arg(t.elapsed()) << endl;
#endif //QT_NO_DEBUG

}


void wgv::writeBack()
{
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
        tw->nodes[i].setU(nodes[i].u(), nodes[i].w(), nodes[i].phi());
        if(nodes[i].hasbearing())
            tw->nodes[i].bearing_editable().setR(nodes[i].Ax(), nodes[i].Az(), nodes[i].Am());
    }
    for(int i=0; i<beam_n; i++)
    {
        tw->beams[i].N() = beams[i].N();
        tw->beams[i].u() = beams[i].u();
        tw->beams[i].Q() = beams[i].Q();
        tw->beams[i].M() = beams[i].M();
        tw->beams[i].phi() = beams[i].phi();
        tw->beams[i].w() = beams[i].w();
    }
    tw->isCalc = true;
    tw->globalMMax = globalMMax;
    tw->globalPMax = globalPMax;
    tw->globalUMax = globalUMax;
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

    double u_, w, phi;
    int i, j, m;

    for(i=0,  j=0, m=0; i<node_n; i++)
    {
        if(nodes[i].hasbearing())
        {
            phi=((nodes[i].bearing().x()) ? ua[m++] : u[j++]);
            w=((nodes[i].bearing().z()) ? ua[m++] : u[j++]);

            u_=phi*cos(nodes[i].angle())-w*sin(nodes[i].angle());
            w=phi*sin(nodes[i].angle())+w*cos(nodes[i].angle());

            phi=(nodes[i].bearing().phi() ? ua[m++] : u[j++]);

        }
        else
        {
            u_ = u[j++];
            w = u[j++];
            phi = u[j++];
        }
#ifndef QT_NO_DEBUG

        QTextStream debugg(stdout);

        debugg << "u( "  << i  << " )"  << endl;
        debugg << u_ << ", "  << w << ", "  << phi  << endl << endl;

#endif

        nodes[i].setU(u_, w, phi);
    }
    for(i=0,  m=0; i<node_n; i++)
    {
        if(nodes[i].hasbearing())
        {
	    if(nodes[i].bearing().x())
            {
                phi = pa[m]-p[f_n-a_n+m];
                m++;
	    }
	    else if(nodes[i].bearing().xf())
	    {
		phi = -(nodes[i].u()*cos(nodes[i].angle())+nodes[i].w()*sin(nodes[i].angle()))*nodes[i].bearing().c_x();
	    }
            else
                phi = 0;

	    if(nodes[i].bearing().z())
            {
                w = pa[m]-p[f_n-a_n+m];
                m++;
	    }
	    else if(nodes[i].bearing().zf())
	    {
		w = -(nodes[i].w()*cos(nodes[i].angle())-nodes[i].u()*sin(nodes[i].angle()))*nodes[i].bearing().c_z();
	    }
            else
            {
                w = 0;
            }


	    u_=phi*cos(nodes[i].angle())-w*sin(nodes[i].angle());
            w=phi*sin(nodes[i].angle())+w*cos(nodes[i].angle());

	    if(nodes[i].bearing().phi())
            {
                phi = pa[m]-p[f_n-a_n+m];
                m++;
	    }
	    else if(nodes[i].bearing().phif())
	    {
		phi = -nodes[i].phi()*nodes[i].bearing().c_phi();
	    }
            else
            {
                phi = 0;
            }

            nodes[i].bearing_editable().setR(u_, w, phi);



	    globalPMax = qMax(globalUMax, nodes[i].bearing().a_x());
	    globalPMax = qMax(globalUMax, nodes[i].bearing().a_z());
	    globalMMax = qMax(globalMMax, nodes[i].bearing().a_m());
        }

	globalUMax = qMax(globalUMax, nodes[i].u());
	globalUMax = qMax(globalUMax, nodes[i].w());

#ifndef QT_NO_DEBUG


        QTextStream debugg(stdout);
        if(nodes[i].hasbearing())
        {
            debugg << "R( "  << i  << " )"  << endl;
            debugg << u_ << ", "  << w << ", "  << phi  << endl << endl;
        }
        else
        {
            debugg << "R( "  << i  << " )"  << endl;
            debugg << 0 << ", "  << 0 << ", "  << 0  << endl << endl;
        }
#endif
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
    for(int n=0; n<beam_n; n++)
    {
        if(!beam(n).joint(0))
            u0=leftN(beam(n)).u()*cos(beam(n).angle())+
                  leftN(beam(n)).w()*sin(beam(n).angle());
        else
            u0=u[u_indexed[6*n]-a_n];

        if(!beam(n).joint(1))
            w0=leftN(beam(n)).w()*cos(beam(n).angle())-
                  leftN(beam(n)).u()*sin(beam(n).angle());
        else
            w0=u[u_indexed[6*n+1]-a_n];

        if(!beam(n).joint(2))
            p0=leftN(beam(n)).phi();
        else
            p0=u[u_indexed[6*n+2]-a_n];


        if(!beam(n).joint(3))
            N1=rightN(beam(n)).u()*cos(beam(n).angle())+
                  rightN(beam(n)).w()*sin(beam(n).angle());
        else
            N1=u[u_indexed[6*n+3]-a_n];

        if(!beam(n).joint(4))
            Q1=rightN(beam(n)).w()*cos(beam(n).angle())-
                  rightN(beam(n)).u()*sin(beam(n).angle());
        else
            Q1=u[u_indexed[6*n+4]-a_n];

        if(!beam(n).joint(5))
            M1=rightN(beam(n)).phi();
        else
            M1=u[u_indexed[6*n+5]-a_n];




#ifndef QT_NO_DEBUG

        QTextStream debugg(stdout);
        debugg <<"vv( " << n << " ) " <<endl;
        debugg << u0 << ", "  << w0 << ", "  << p0 /* << ", "  << N1 << ", "  << Q1 << ", "  << M1 << ", "  */<< endl << endl;

#endif

        N0=-(beam(n).k11()*(u0-N1));
        Q0=-(beam(n).k22()*(w0-Q1)+
                beam(n).k23()*(-p0-M1));
        M0=-(beam(n).k33()*p0+
                beam(n).k36()*M1+
                beam(n).k23()*(Q1-w0));

        /*
        sv[3]= (N0);
        sv[4]= (Q0);
        sv[5]= (beam(n).k33()*M1+beam(n).k36()*p0+beam(n).k23()*(Q1-w0));
        */

#ifndef QT_NO_DEBUG
        debugg <<"sv( " << n << " ) -s0" <<endl;
        debugg << N0 << ", "  << Q0 << ", "  << M0 /*<< ", "  << sv[3] << ", "  << sv[4] << ", "  << sv[5] << endl */<< endl;
#endif


        //for(int j=0; j<3/*6*/; j++)
        //{
        //   /* if(j<3)*/ sv[j]-=s0[6*n+j];
        //   // else    sv[j]+=s0[6*n+j];
        //}


        N0-=s0[6*n];
        Q0-=s0[6*n+1];
        M0-=s0[6*n+2];

        beams[n].N() = px[n]*DiffOp::dx*(-1) + N0;

        beams[n].u() = (beams[n].N()*DiffOp::dx/(beams[n].E()*beams[n].A()) + u0) + polynom<3>(0, 0, dT[n]*beams[n].aT(), 0);


        beams[n].Q() = pz[n]*DiffOp::dx*(-1) + Q0;

        beams[n].M() = beams[n].Q()*DiffOp::dx + M0;

        beams[n].phi() = (beams[n].M()*DiffOp::dx*(-1)/(beams[n].E()*beams[n].I()) - p0) + polynom<4>(0, 0, 0, -dTz[n]*beams[n].aT(), 0);

        beams[n].w() = beams[n].phi()*DiffOp::dx + w0;

#ifndef QT_NO_DEBUG
        debugg << "px(x)[" << n << "] =" <<  px[n]<< endl;
        debugg << "N(x)[" << n << "] = " <<  beams[n]._N<< endl;
        debugg << "u(x)[" << n << "] = " <<  beams[n]._u<< endl<< endl;

        debugg << "pz(x)[" << n << "] =" <<  pz[n]<< endl;
        debugg << "Q(x)[" << n << "] = " <<  beams[n]._Q<< endl;
        debugg << "M(x)[" << n << "] = " <<  beams[n]._M<< endl;
        debugg << "p(x)[" << n << "] = " <<  beams[n]._p<< endl;
        debugg << "w(x)[" << n << "] = " <<  beams[n]._w<< endl;

        debugg << endl << endl;
#endif


        beams[n].N().calcMax();
        beams[n].Q().calcMax();
        beams[n].M().calcMax();

        globalMMax= qMax(globalMMax, fabs(beams[n].M(beams[n].M().max(0))));
        globalMMax= qMax(globalMMax, fabs(beams[n].M(beams[n].M().max(1))));
        globalMMax= qMax(globalMMax, fabs(beams[n].M(0)));
        globalMMax= qMax(globalMMax, fabs(beams[n].M(beams[n].l())));

        globalPMax= qMax(globalPMax, fabs(beams[n].Q(beams[n].Q().max(0))));
        globalPMax= qMax(globalPMax, fabs(beams[n].N(beams[n].N().max(0))));
        globalPMax= qMax(globalPMax, fabs(beams[n].N(0)));
        globalPMax= qMax(globalPMax, fabs(beams[n].N(beams[n].l())));
        globalPMax= qMax(globalPMax, fabs(beams[n].Q(0)));
        globalPMax= qMax(globalPMax, fabs(beams[n].Q(beams[n].l())));

        globalUMax=qMax(globalUMax, fabs(beams[n].w(beams[n].l()/4.)));
        globalUMax=qMax(globalUMax, fabs(beams[n].w(beams[n].l()/2.)));
        globalUMax=qMax(globalUMax, fabs(beams[n].w(beams[n].l()*3./4.)));
        globalUMax=qMax(globalUMax, fabs(beams[n].w(beams[n].l())));


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
    QTextStream debugS(stdout);
    debugS << endl << "building formal u-vektor" << endl;
    debugS << "f_n =" <<  f_n << endl;
    debugS << "a_n =" <<  a_n << endl;

    debugS << "k_index" << endl;

    for(int n=0; n<f_n; n++)
        debugS <<"DOF #" <<  n << " @ " << k_index[n] << endl;

    debugS << "u_indexed" << endl;
    for(int n=0; n<beam_n; n++)
    {
        debugS << "beam " <<  n << endl;
        for(int i=0; i<6; i++)
        {
            debugS << u_indexed[6*n+i] << endl;
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
    K.setSize(f_n); //init matrix. size calculated in u_()
                              //call only after u_()!

    double k11, k22, k23, k33, k36;             //factors from local k-matrix(6x6)
    double sin_l, cos_l, sin_r, cos_r;          //angles between the beams local kordsys
                                                //and the left/right nodes local kordsys
                                                //enough to descripe a rotation matrix

#ifndef QT_NO_DEBUG
    quadmatrix<double> kcopy(f_n);
#endif

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


#ifndef QT_NO_DEBUG
       for(int ii = 0; ii< f_n; ii++)
       {
           for(int jj = 0; jj<f_n; jj++)
           {
                kcopy[ii][jj] = K[ii][jj];
           }
       }
#endif



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


#ifndef QT_NO_DEBUG
       for(int ii = 0; ii< f_n; ii++)
       {
           for(int jj = 0; jj<f_n; jj++)
           {
                kcopy[ii][jj] = K[ii][jj]-kcopy[ii][jj];
           }
       }

       QTextStream debugS(stdout);
       debugS << endl << "global KMatrix [ " << n << "]" << endl << kcopy << endl;


#endif

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

    //make it symmetrical(spelling, lol!)
/*
    K.mirror();
*/
    /* old mirroring code
    for(int n=0; n<f_n; n++)
    {
        for(int m=0; m<n; m++)
        {
            K[n][m]=K[m][n];
        }
    }*/

#ifndef QT_NO_DEBUG
    QTextStream debugS(stdout);
    debugS << endl << "global KMatrix" << endl << K << endl;
#endif
    /* old print code

    printf("K\n");

    for(int n=0; n<f_n; n++)
    {
        for(int m=0; m<f_n; m++)
        {
            printf("%8.0lf   ", K[n][m]);
        }
        printf("\n");
    }
    printf("\n");

    */
}

void wgv::make_s0()
{
    s0 = quadvector<double>(beam_n*6);

    px = new polynom<1>[beam_n];
    pz = new polynom<1>[beam_n];
    dT = new double[beam_n];
    dTz = new double[beam_n];
    for(int i=0; i<beam_n; i++)
    {
        px[i]=polynom<1>(0, 0);
        pz[i]=polynom<1>(0, 0);
        dT[i]=0;
        dTz[i]=0;
    }
    //old allocation: s0_=new double[beam_n()*6];
    //for(int n=0; n<beam_n()*6; n++)
    //    s0_[n]=0;

    //all in the local koordsys of the beams....
    //mapping to global sys in p()
    int n;
    for(n=0; n<load_n; n++)
    {
        if(load(n).typ() == wgv_load::nodeLoad ||
           load(n).typ() == wgv_load::moment   )
            continue;

        //beams[load(n).at()].addtoqvonx(load(n));


        if(load(n).typ() == wgv_load::uniformlyDistibutedLoad)
        {
            px[load(n).at()] += polynom<1>(0,
                                              cos(beam(load(n).at()).angle())*load(n).Px()
                                              +sin(beam(load(n).at()).angle())*load(n).Pz());

            pz[load(n).at()] += polynom<1>(0,
                                               cos(beam(load(n).at()).angle())*load(n).Pz()+
                                               -sin(beam(load(n).at()).angle())*load(n).Px());

            s0[6*load(n).at()]+=   -cos(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()/2
                                    -sin(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()/2;
            s0[6*load(n).at()+1]+=  sin(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()/2
                                    -cos(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()/2;
            s0[6*load(n).at()+2]+= -sin(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()*beam(load(n).at()).l()/12
                                    +cos(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()*beam(load(n).at()).l()/12;


            s0[6*load(n).at()+3]+= -cos(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()/2
                                    -sin(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()/2;
            s0[6*load(n).at()+4]+=  sin(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()/2
                                    -cos(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()/2;
            s0[6*load(n).at()+5]+=  sin(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()*beam(load(n).at()).l()/12
                                    -cos(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()*beam(load(n).at()).l()/12;
        }
        else if(load(n).typ() == wgv_load::increasingLinearLoad)
        {

            px[load(n).at()] += polynom<1>( cos(beam(load(n).at()).angle())*load(n).Px()/beam(load(n).at()).l()
                                           +sin(beam(load(n).at()).angle())*load(n).Pz()/beam(load(n).at()).l(),
                                           0);

            pz[load(n).at()] += polynom<1>(cos(beam(load(n).at()).angle())*load(n).Pz()/beam(load(n).at()).l()+
                                           -sin(beam(load(n).at()).angle())*load(n).Px()/beam(load(n).at()).l(),
                                           0);

            s0[6*load(n).at()]+=-  cos(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()*3/20
                                   -sin(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()*3/20;
            s0[6*load(n).at()+1]+= sin(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()*3/20
                                   -cos(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()*3/20;
            s0[6*load(n).at()+2]+=-sin(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()*beam(load(n).at()).l()/30
                                   +cos(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()*beam(load(n).at()).l()/30;


            s0[6*load(n).at()+3]+=-cos(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()*7/20
                                   -sin(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()*7/20;
            s0[6*load(n).at()+4]+= sin(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()*7/20
                                   -cos(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()*7/20;
            s0[6*load(n).at()+5]+= sin(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()*beam(load(n).at()).l()/20
                                   -cos(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()*beam(load(n).at()).l()/20;
        }
        else if(load(n).typ() == wgv_load::decreasingLinearLoad)
        {
            px[load(n).at()] += polynom<1>(-cos(beam(load(n).at()).angle())*load(n).Px()/beam(load(n).at()).l()
                                           -sin(beam(load(n).at()).angle())*load(n).Pz()/beam(load(n).at()).l(),
                                            cos(beam(load(n).at()).angle())*load(n).Px()
                                           +sin(beam(load(n).at()).angle())*load(n).Pz());

            pz[load(n).at()] += polynom<1>(-cos(beam(load(n).at()).angle())*load(n).Pz()/beam(load(n).at()).l()
                                           +sin(beam(load(n).at()).angle())*load(n).Px()/beam(load(n).at()).l(),
                                            cos(beam(load(n).at()).angle())*load(n).Pz()
                                           -sin(beam(load(n).at()).angle())*load(n).Px());

            s0[6*load(n).at()]+=  -cos(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()*7/20
                                   -sin(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()*7/20;
            s0[6*load(n).at()+1]+= sin(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()*7/20
                                   -cos(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()*7/20;
            s0[6*load(n).at()+2]+=-sin(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()*beam(load(n).at()).l()/20
                                   +cos(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()*beam(load(n).at()).l()/20;


            s0[6*load(n).at()+3]+=-cos(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()*3/20
                                   -sin(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()*3/20;
            s0[6*load(n).at()+4]+= sin(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()*3/20
                                   -cos(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()*3/20;
            s0[6*load(n).at()+5]+= sin(beam(load(n).at()).angle())*load(n).Px()*beam(load(n).at()).l()*beam(load(n).at()).l()/30
                                   -cos(beam(load(n).at()).angle())*load(n).Pz()*beam(load(n).at()).l()*beam(load(n).at()).l()/30;

        }
        else if(load(n).typ() == wgv_load::tempChange)
        {
            s0[6*load(n).at()]+= +beam(load(n).at()).E()*beam(load(n).at()).A()*load(n).Px()*beam(load(n).at()).aT();
            //s0_[6*load(n).at()+1]+=0;
            //s0_[6*load(n).at()+2]+=0;

            s0[6*load(n).at()+3]+=-beam(load(n).at()).E()*beam(load(n).at()).A()*load(n).Px()*beam(load(n).at()).aT();
            //s0_[6*load(n).at()+4]+=0;
            //s0_[6*load(n).at()+5]+=0;

            dT[load(n).at()]+=load(n).Px();
        }
        else if(load(n).typ() == wgv_load::tempDiffrence)
        {
            //s0_[6*load(n).at()]+=0;
            //s0_[6*load(n).at()+1]+=0;
            s0[6*load(n).at()+2]+= beam(load(n).at()).E()*beam(load(n).at()).I()*load(n).Pz()*beam(load(n).at()).aT();

            //s0_[6*load(n).at()+3]+=0;
            //s0_[6*load(n).at()+4]+=0;
            s0[6*load(n).at()+5]+= -beam(load(n).at()).E()*beam(load(n).at()).I()*load(n).Pz()*beam(load(n).at()).aT();

            dTz[load(n).at()]+= load(n).Pz();
        }
        else if(load(n).typ() == wgv_load::doubleLoadLeft)
        {

        }
        else if(load(n).typ() == wgv_load::doubleLoadRight)
        {

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
    p = quadvector<double>(f_n);



    //p_ = new double[f_n];
    //for(n=0; n<f_n; n++)
    //    p_[n]=0;
    for(n=0; n<load_n; n++)
    {
        if(load(n).typ() == wgv_load::nodeLoad ||
           load(n).typ() == wgv_load::moment   )
        {
            if(node(load(n).at()).hasbearing())
            {
                p[k_index[3*load(n).at()]] += load(n).Px()*cos(node(load(n).at()).angle())+load(n).Pz()*sin(node(load(n).at()).angle());
                p[k_index[3*load(n).at()+1]] += -load(n).Px()*sin(node(load(n).at()).angle())+load(n).Pz()*cos(node(load(n).at()).angle());
                p[k_index[3*load(n).at()+2]] += load(n).M();
            }
            else
            {
                p[k_index[3*load(n).at()]] += load(n).Px();
                p[k_index[3*load(n).at()+1]] += load(n).Pz();
                p[k_index[3*load(n).at()+2]] += load(n).M();
            }
        }
	if(load(n).typ() == wgv_load::doubleLoadLeft)
	{
	    if(beam(load(n).at()).joint(0) && load(n).Px()!=0)
	    {
		p[k_index[3*beam(load(n).at()).leftNodeI()]] += cos(d_alpha_l(load(n).at()))*load(n).Px();
		p[k_index[3*beam(load(n).at()).leftNodeI()+1]] += sin(d_alpha_l(load(n).at()))*load(n).Px();
		p[k_index[u_indexed[6*load(n).at()]]] +=-load(n).Px();
	    }
	    if(beam(load(n).at()).joint(1) && load(n).Pz()!=0)
	    {
		p[k_index[3*beam(load(n).at()).leftNodeI()]] += -sin(d_alpha_l(load(n).at()))*load(n).Pz();
		p[k_index[3*beam(load(n).at()).leftNodeI()+1]] += cos(d_alpha_l(load(n).at()))*load(n).Pz();
		p[k_index[u_indexed[6*load(n).at()+1]]] +=-load(n).Pz();
	    }
	    if(beam(load(n).at()).joint(2) && load(n).M()!=0)
	    {
		p[k_index[3*beam(load(n).at()).leftNodeI()+2]] +=load(n).M();
		p[k_index[u_indexed[6*load(n).at()+2]]] +=-load(n).M();
	    }
	}
	if(load(n).typ() == wgv_load::doubleLoadRight)
	{
	    if(beam(load(n).at()).joint(3) && load(n).Px()!=0)
	    {
				p[k_index[3*beam(load(n).at()).rightNodeI()]] += -cos(d_alpha_r(load(n).at()))*load(n).Px();
				p[k_index[3*beam(load(n).at()).rightNodeI()+1]] += -sin(d_alpha_r(load(n).at()))*load(n).Px();
                p[k_index[u_indexed[6*load(n).at()+3]]] += load(n).Px();
	    }
	    if(beam(load(n).at()).joint(4) && load(n).Pz()!=0)
	    {
				p[k_index[3*beam(load(n).at()).rightNodeI()]] += sin(d_alpha_r(load(n).at()))*load(n).Pz();
				p[k_index[3*beam(load(n).at()).rightNodeI()+1]] += -cos(d_alpha_r(load(n).at()))*load(n).Pz();
                p[k_index[u_indexed[6*load(n).at()+4]]] += load(n).Pz();
	    }
	    if(beam(load(n).at()).joint(5) && load(n).M()!=0)
	    {
                p[k_index[3*beam(load(n).at()).rightNodeI()+2]] += load(n).M();
                p[k_index[u_indexed[6*load(n).at()+5]]] += -load(n).M();
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
#ifndef QT_NO_DEBUG
    QTextStream debugS(stdout);
    debugS << "s0 =" <<  s0 << endl;
    debugS << "p =" << p << endl;
    debugS << "p(x)" << endl;
    for(int i=0; i<beam_n; i++)
    {
        debugS << "px(x)[" << i << "] = " << px[i] << endl;
        debugS << "pz(x)[" << i << "] = " << pz[i] << endl;
    }
    debugS  << endl;
#endif
}
