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

#ifndef WGV_TW_H
#define WGV_TW_H
#include "math.h"
#include <QtGui/QTransform>
#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QTextStream>
#ifndef QT_NO_DEBUG
#include <cstdio>
#endif

#include "wgv_abstract_tw.h"

#include "wgv_beam.h"
#include "wgv_load.h"

class wgv_tw : public wgv_abstract_tw
{
    friend class editNodeUndoCommand;
    friend class newNodeUndoCommand;
    friend class removeNodeUndoCommand;

    friend class editBeamUndoCommand;
    friend class newBeamUndoCommand;
    friend class removeBeamUndoCommand;

    friend class editLoadUndoCommand;
    friend class newLoadUndoCommand;
    friend class removeLoadUndoCommand;

    friend class wgv;

    friend class XmlReader;

    Q_OBJECT
public:
    wgv_tw();

    enum VarName{
        uneditable,
        ID_node,
        ID_beam,
        ID_load,
        ID_temp,
        ID_bearing,
        Bearing_h,  //node(i).bearing.x()
        Bearing_v,  //node(i).bearing.z()
        Bearing_m,  //node(i).bearing.phi()
        Joint, //beam(i).joint(j)
        x, //node(i).x()
        z, //node(i).z()
        Spring_h, //node(i).bearing().c_x()
        Spring_v, //node(i).bearing().c_z()
        Spring_m, //node(i).bearing().c_phi()
        alpha_k, //node(i).angle()
        E, //beam(i).E()
        I, //beam(i).I()
        A, //beam(i).A()
        Spring, //beam(i).spring(j)
        Px, //load(i).Px()
        Pz, //load(i).Pz()
        M, //load(i).M()
        LoadTyp, //load(i).typ()
        theLoadsBeam, //load(i).at()
        theTempsBeam, //temp(i).on()
        TempTyp, //temp(i).typ()
        Temp, //temp(i).T()
        alpha_T, //temp(i).alpT()
        leftNode, //beam(i).leftNodeI()
        rightNode, //beam(i).leftNodeI()
        Mat, //beam(i).mat()
        Profile //beam(i).qs
    };


    bool isCalculated() const;

    //Knoten

    //virtual const wgv_node &node(const int &i) const {return nodes.at(i);}
    //virtual int node_n() const {return(nodes.size());}


    int addNode(const wgv_node &n);
    void editNode(const int &k, const wgv_node &n);
    void setBearing(const int &k, const wgv_bearing &b);
    void removeNode(const int &o);
    void setX(const int &o, const double &xn);
    void setZ(const int &o, const double &zn);
    void setBx(const int &o, const bool &hn);
    void setBz(const int &o, const bool &vn);
    void setBm(const int &o, const bool &mn);
    void setCx(const int &o, const double &fn);
    void setCz(const int &o, const double &fn);
    void setCm(const int &o, const double &fn);
    void setAngle(const int &o, const double &alphan);
    void setXZ(const int &o, const double &xn, const double &zn);
    int knoten_von_auflager(const int &o) const;

    inline void moveNode(const int &o, const QPointF &p){setXZ(o, p.x(), p.y());}




    //Stbe


    const wgv_beam &beam(const int &i) const {return beams.at(i);}
    wgv_beam lastbeam() const {return beams.last();}

    int addBeam(const wgv_beam &n);
    void editBeam(const int &o, const wgv_beam &n);
    void removeBeam(const int &o);
    void setLeft(const int &o, const int &Ln);
    void setRight(const int &o, const int &Rn);
    void setJointsandSprings(const int &beamnr, const bool *joi_array_6, const double *spr_array_6);
    void setJoint(const int &beamnr, const int &pos, const bool &thereIsAJoint);
    void setSpring(const int &beamnr, const int &pos, const double &c_f);
    void setMat(const int &o, const int &matIndex);
    void setProfile(const int &o, const int &QsIndex);
    void setMat(const int &o, const QString &matName);
    void setProfile(const int &o, const QString &QsName);


    int f_n;

    //Lasten



    const wgv_load &load(const int &i) const {return loads.at(i);}

    int addLoad(const wgv_load &n);
    void editLoad(const int &o, const wgv_load &n);
    void removeLoad(const int &i);
    void setPx(const int &o, const double &Pxn);
    void setPz(const int &o, const double &Pzn);
    void setM(const int &o, const double &Mn);
    void setLoadTyp(const int &o, const wgv_load::form &Typn);
    void setPos(const int &o, const int &Pos);




    int addMat(const wgv_mat &n);
    int editMat(const int &nr, const wgv_mat &n);
    int removeMat(const int &nr, const wgv_mat &n);
    int editMatByName(const QString &name, const wgv_mat &n)
        {return editMat(LIB.IndexFromMatByName(name), n);}
    int removeQuerschnittByName(const QString &name, const wgv_mat &n)
        {return removeMat(LIB.IndexFromProfileByName(name), n);}




    int bestimmt();
    int bestimmt(int *aa, int *ss, int *kk, int *gg, int *ff);



    inline int beam_n() const {return(beams.size());}
    inline int load_n() const {return(loads.size());}




    void selectAll();
    void deselectAll();
    void invertSelection();
    void selectRect(const QRectF &r, bool keepSelec = false);

    QRectF boundingNodeRect() const {return boundingRect(false);}
    QRectF boundingRect(const bool &alsoLoads = true) const;
    double mMax() const;
    double pMax() const;
    double uMax() const;

    void deleteSelection();
    bool isSelectionEmpty();

    void selectNode(const int &i, const bool &sel){nodes[i].select(sel); emit selectionChanged();}
    void selectBeam(const int &i, const bool &sel){beams[i].select(sel); emit selectionChanged();}
    void selectLoad(const int &i, const bool &sel){loads[i].select(sel); emit selectionChanged();}
    void selectBearing(const int &i, const bool &sel){nodes[i].bearing_editable().select(sel);}
    void emitSelectionChanged() {emit selectionChanged();}


    //void calculate();
    //void erase();


    //things for the gui
    QTransform &view(int i);
    void setView(int i, const QTransform &t);

    const double &scaleM()const {return scale_m;}
    void setScaleM(const double &s);

    const double &scaleP()const {return scale_p;}
    void setScaleP(const double &s);

    const double &scaleU()const {return scale_u;}
    void setScaleU(const double &s);

    const int &lastObjectBeam() const { return lOBeam;}
    void setlastObjectBeam(const int &i) {lOBeam=i;}

    const int &lastObjectNode() const { return lONode;}
    void setlastObjectNode(const int &i) {lONode=i;}

    const int &lastObjectLoad() const { return lOLoad;}
    void setlastObjectLoad(const int &i) {lOLoad=i;}

    const QPointF &lastObjectClick() const { return lOClick;}
    void setlastObjectClick(const QPointF &p) { lOClick = p;}

    void calculateShapes() const;

    //catch
    int nearestNode(const QPointF &p, const double &maxabs) const;
    int nearestBeam(const QPointF &p, const double &maxabs, QPointF *n = 0) const;
    void crossingBeams(const QPointF &p, const double &maxabs, int *first, int *second, QPointF *n = 0) const;
    int nearestLoad(const QPointF &p, const double &maxabs) const;

    //makro
    void beginScript(const QString &s) {emit beginS(s);}
    void endScript() {emit endS();}

    void mergeWith(wgv_tw *tw, QPointF dp = QPointF());

public slots:
    //void reset();
    void removeViewPort(int i);
    void resetCals(){isCalc = false; calculationThread = 0;}

private:




    void CommandEditNode(const int &k, const wgv_node &n);
    void CommandRemoveNode(const int &k);
    void CommandInsertNode(const int &o, const wgv_node &n);
    void CommandEditBeam(const int &o, const wgv_beam &n);
    void CommandRemoveBeam(const int &o);
    void CommandInsertBeam(const int &o,const wgv_beam &n);
    void CommandEditLoad(const int &o, const wgv_load &n);
    void CommandRemoveLoad(const int &o);
    void CommandInsertLoad(const int &o, const wgv_load &n);


    void emitCalculationFinished(){emit calculationFinished();}
    //QList<wgv_node> nodes;
    //QList<wgv_mat> mats;
    //QList<wgv_querschnitt> querschnitte;
    QList<wgv_beam> beams;
    QList<wgv_load> loads;

    QList<QTransform> views;
    double scale_p;
    double scale_m;
    double scale_u;
    int lOBeam;
    int lONode;
    int lOLoad;
    QPointF lOClick;


    QObject *calculationThread;


    double globalMMax;
    double globalPMax;
    double globalUMax;


    //int a_n;
    bool isCalc;

    //int steps_done;

    //double d_alpha_l(const int &n) {return beam(n).angle()-beam(n).leftNode().angle();}
    //double d_alpha_r(const int &n) {return beam(n).angle()-beam(n).rightNode().angle();}

    //void p();
    //int u();
    //void k();
    //void s0();
    //void v();
    //void s();
    //void reaktionen();
    //bool solve( double **a, double *b, double *x, int size);


    /****************************/
    /*MATRIZEN, VEKTOREN UND CO.*/
    /****************************/

    //double **kglobmatrix;
    //double *pvektor;
    //double *s0vektor;
    //double *uvektor;

    //int *u_indexed;
    //int *k_index;







signals:
    void selectionChanged();
    void calculationFinished();


    //signals for undocommander
    void edited(const int&, const wgv_beam&);
    void appended(const wgv_beam&);
    void removed(const int&, const wgv_beam&);

    void edited(const int&, wgv_node);
    void appended(const wgv_node&);
    void removed(const int&, const wgv_node&);

    void edited(const int&, const wgv_load&);
    void appended(const wgv_load&);
    void removed(const int&, const wgv_load&);

    void beginS(const QString &s);
    void endS();

    //signals for the list

    void changedNode(const int &i);
    void changedBeam(const int &i);
    void changedLoad(const int &i);

    void erasedNode(const int &i);
    void erasedBeam(const int &i);
    void erasedLoad(const int &i);

    void addedNode(const int &i);
    void addedBeam(const int &i);
    void addedLoad(const int &i);

    //signals for the interface (viewPort & viewMenu)
    void changedUScale(const double &s);
    void changedPScale(const double &s);
    void changedMScale(const double &s);


    //last started Thread



};


#endif // WGV_TW_H
