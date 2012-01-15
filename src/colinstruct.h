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
#include <QtCore/QSet>
#include <QtCore/QObject>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>
#ifndef QT_NO_DEBUG
#include <cstdio>
#endif


#include "colinbeam.h"
#include "colinload.h"
#include "colinbls.h"
#include "colincls.h"



class ColinStruct : public QObject
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

	friend class editBLSUndoCommand;
	friend class newBLSUndoCommand;
	friend class removeBLSUndoCommand;

	friend class editCLSUndoCommand;
	friend class newCLSUndoCommand;
	friend class removeCLSUndoCommand;

	friend class insertBLStoCLSUndoCommand;
	friend class removeBLSfromCLSUndoCommand;

    friend class wgv;

    friend class XmlReader;

    Q_OBJECT

public:
	Q_PROPERTY(int nodeCount READ node_n)
	Q_PROPERTY(int beamCount READ beam_n)
	Q_PROPERTY(int loadCount READ load_n)
	Q_PROPERTY(int clsCount READ cls_n)
	Q_PROPERTY(int blsCount READ bls_n)
	Q_PROPERTY(bool calculated READ isCalculated)



	ColinStruct();


	Q_INVOKABLE ColinNode Node(const int &i);
	Q_INVOKABLE void setNode(const int &k, const ColinNode &n);
	Q_INVOKABLE int addNode(double x, double y);
	Q_INVOKABLE int addNode(const ColinNode &n);
	Q_INVOKABLE void removeNode(const int &o);
	Q_INVOKABLE void setBearing(const int &k, const ColinSupport &b);

	Q_INVOKABLE ColinBeam Beam(const int &i);
	Q_INVOKABLE void setBeam(const int &k, const ColinBeam &b);
	Q_INVOKABLE int addBeam(const ColinBeam &n);
	Q_INVOKABLE int addBeam(int left, int right);
	Q_INVOKABLE void removeBeam(const int &o);

	Q_INVOKABLE ColinLoad Load(const int &i);
	Q_INVOKABLE int addLoad(const ColinLoad &n);
	Q_INVOKABLE void removeLoad(const int &i);

	Q_INVOKABLE ColinBLS BLS(const int &i);
	Q_INVOKABLE void setBLS(const int &i, const ColinBLS &n);
	Q_INVOKABLE int addBLS(const ColinBLS &n);
	Q_INVOKABLE void removeBLS(const int &i);

	Q_INVOKABLE ColinCLS CLS(const int &i);
	Q_INVOKABLE void setCLS(const int &i, const ColinCLS &n);
	Q_INVOKABLE int addCLS(const ColinCLS &n);
	Q_INVOKABLE void removeCLS(const int &i);


    enum VarName{
		uneditable = 0x0,
		creation = 0x1,
		ID_node = 0x2,
		ID_beam = 0x4,
		ID_load = 0x6,
		ID_temp = 0x8,
		ID_bearing = 0x10,
		ID_bls = 0x12,
		ID_cls = 0x14,
		ID_clsbls = 0x15,
		Bearing_h = 0x102,  //node(i).bearing.x()
		Bearing_v = 0x104,  //node(i).bearing.z()
		Bearing_m = 0x106,  //node(i).bearing.phi()
		Joint = 0x108, //beam(i).joint(j)
		x = 0x110, //node(i).x()
		z = 0x112, //node(i).z()
		Spring_h = 0x114, //node(i).bearing().c_x()
		Spring_v = 0x116, //node(i).bearing().c_z()
		Spring_m = 0x118, //node(i).bearing().c_phi()
		alpha_k = 0x120, //node(i).angle()
		E = 0x122, //beam(i).E()
		I = 0x124, //beam(i).I()
		A = 0x126, //beam(i).A()
		Spring = 0x128, //beam(i).spring(j)
		Px = 0x130, //load(i).Px()
		Pz = 0x132, //load(i).Pz()
		M = 0x134, //load(i).M()
		LoadTyp = 0x136, //load(i).typ()
		NodalLoadTyp = 0x138, //load(i).typ()
		BeamLoadTyp = 0x140, //temp(i).typ()
		TempLoadTyp = 0x142, //temp(i).T()
		DoubleLoadTyp = 0x144, //load(i).set()
		LoadSet = 0x146, //load(i).at()
		Pos = 0x148, //load(i).at()
		alpha_T = 0x150, //temp(i).alpT()
		leftNode = 0x152, //beam(i).leftNodeI()
		rightNode = 0x154, //beam(i).leftNodeI()
		Mat = 0x156, //beam(i).mat()
		CrossSection = 0x158, //beam(i).qs
		BLSname = 0x160, //bls(i).name
		BLScolor = 0x162, //bls(i).color
		CLSname = 0x164, //cls(i).name
		CLSBLS = 0x166, //cls(i).bls(j)
		CLSBLSfac = 0x168 //cls(i).fac(j)
    };
	Q_DECLARE_FLAGS(VarNames, VarName);





	bool isCalculated() const;

    //Knoten

	const ColinNode &node(const int &i) const {return nodes.at(i);}


	void editNode(const int &k, const ColinNode &n);
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


	const ColinBeam &beam(const int &i) const {return beams.at(i);}
	ColinBeam lastbeam() const {return beams.last();}

	void editBeam(const int &o, const ColinBeam &n);
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



	const ColinLoad &load(const int &i) const {return loads.at(i);}

	void editLoad(const int &o, const ColinLoad &n);
    void setPx(const int &o, const double &Pxn);
    void setPz(const int &o, const double &Pzn);
    void setM(const int &o, const double &Mn);
	void setLoadTyp(const int &o, const ColinLoad::form &Typn);
    void setPos(const int &o, const int &Pos);
	void setLoadSet(const int &o, const int &ls);


	//BLS

	const ColinBLS &bls(const int &i) const {return basicloadsets.at(i);}

	void editBLS(const int &o, const ColinBLS &n);
	void setBLSName(const int &i, const QString &name);
	void setBLSColor(const int &, const QColor &color);
	int getBLSIDbyName(const QString &name) const;


	//CLS

	const ColinCLS &cls(const int &i) const {return combinedloadsets.at(i);}

	void editCLS(const int &o, const ColinCLS &n);
	void setCLSName(const int &i, const QString &name);
	void addBLStoCLS(const int &clsi, const int &blsi, const double &fac);
	void removeBLSbyIndex(const int &clsi, const int &i);
	void setFacbyIndex(const int &clsi, const int &i, const double &fac);
	int getCLSIDbyName(const QString &name) const;



	void setActiveCLS(int clsI, bool active=true);
	bool isActiveCLS(int clsI)const;
	const QSet<int> &activeCLS()const;
	int activeCLS_n() const;



    int bestimmt();
    int bestimmt(int *aa, int *ss, int *kk, int *gg, int *ff);



	inline int node_n() const {return(nodes.size());}
	inline int beam_n() const {return(beams.size());}
	inline int load_n() const {return(loads.size());}
	inline int bls_n() const {return(basicloadsets.size());}
	inline int cls_n() const {return(combinedloadsets.size());}




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

	void calculateShapes(const QList<int> &activebls = QList<int>(), const QList<double> &multipliers = QList<double>()) const;

    //catch
    int nearestNode(const QPointF &p, const double &maxabs) const;
    int nearestBeam(const QPointF &p, const double &maxabs, QPointF *n = 0) const;
	double nearestPointOnBeam(const QPointF &p, const int &beam) const;
    void crossingBeams(const QPointF &p, const double &maxabs, int *first, int *second, QPointF *n = 0) const;
    int nearestLoad(const QPointF &p, const double &maxabs) const;

    //makro
    void beginScript(const QString &s) {emit beginS(s);}
    void endScript() {emit endS();}

	void mergeWith(ColinStruct *tw, QPointF dp = QPointF());

	void copy(ColinStruct *tw);

public slots:
    //void reset();
    void removeViewPort(int i);
    void resetCals(){isCalc = false; calculationThread = 0;}

private:




	void CommandEditNode(const int &k, const ColinNode &n);
    void CommandRemoveNode(const int &k);
	void CommandInsertNode(const int &o, const ColinNode &n);
	void CommandEditBeam(const int &o, const ColinBeam &n);
    void CommandRemoveBeam(const int &o);
	void CommandInsertBeam(const int &o,const ColinBeam &n);
	void CommandEditLoad(const int &o, const ColinLoad &n);
	void CommandRemoveLoad(const int &o);
	void CommandInsertLoad(const int &o, const ColinLoad &n);
	void CommandEditBLS(const int &o, const ColinBLS &n);
	void CommandRemoveBLS(const int &o);
	void CommandInsertBLS(const int &o, const ColinBLS &n);
	void CommandEditCLS(const int &o, const ColinCLS &n);
	void CommandRemoveCLS(const int &o);
	void CommandInsertCLS(const int &o, const ColinCLS &n);

	void resetResults();

    void emitCalculationFinished(){emit calculationFinished();}


	QList<ColinNode> nodes;
	QList<ColinBeam> beams;
	QList<ColinLoad> loads;
	QList<ColinBLS> basicloadsets;
	QList<ColinCLS> combinedloadsets;

    QList<QTransform> views;
	QSet<int> activeLS;
    double scale_p;
    double scale_m;
    double scale_u;
    int lOBeam;
    int lONode;
    int lOLoad;
    QPointF lOClick;


    QObject *calculationThread;


	node_result *n_res;
	bearing_result *a_res;
	beam_result *b_res;

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
	void edited(const int&, const ColinBeam&);
	void edited(const int&, const ColinNode&);
	void edited(const int&, const ColinLoad&);
	void edited(const int&, const ColinBLS&);
	void edited(const int&, const ColinCLS&);

	void appended(const ColinBeam&);
	void appended(const ColinNode&);
	void appended(const ColinLoad&);
	void appended(const ColinBLS&);
	void appended(const ColinCLS&);

	void removed(const int&, const ColinBeam&);
	void removed(const int&, const ColinNode&);
	void removed(const int&, const ColinLoad&);
	void removed(const int&, const ColinBLS&);
	void removed(const int&, const ColinCLS&);

	void insertedBLStoCLS(const int&, const int&, const double&);


    void beginS(const QString &s);
    void endS();

    //signals for the list

    void changedNode(const int &i);
	void changedBeam(const int &i);
	void changedLoad(const int &i);
	void changedBLS(const int &i);
	void changedCLS(const int &i);

    void erasedNode(const int &i);
	void erasedBeam(const int &i);
	void erasedLoad(const int &i);
	void erasedBLS(const int &i);
	void erasedCLS(const int &i);

    void addedNode(const int &i);
	void addedBeam(const int &i);
	void addedLoad(const int &i);
	void addedBLS(const int &i);
	void addedCLS(const int &i);


    //signals for the interface (viewPort & viewMenu)
    void changedUScale(const double &s);
    void changedPScale(const double &s);
    void changedMScale(const double &s);

	void changedActiveCLS();


    //last started Thread



};

Q_DECLARE_OPERATORS_FOR_FLAGS(ColinStruct::VarNames);


#endif // WGV_TW_H
