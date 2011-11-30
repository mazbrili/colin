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

#include "structpainter.h"

#include "unitsettings.h"
#include "colinicons.h"

#ifndef QT_NO_DEBUG
#include <QtCore/QDebug>
#endif //QT_NO_DEBUG

const int loadBubblerad = 3;

const int roundness = 30;


const int arrowWidth = 6;

const int arrowLenght = 8;

const int momentRad = 50;


const int dh = 20;
const int alphaC = 100;



catcher::CatchCases structPainter::highlightMode = catcher::CatchedNothing;
int structPainter::highlight = -1;

structPainter::structPainter()
{
    v = &viewPortSettings::instance();

    ignoreSelection_=false;
    ignoreHotSpots_=false;
}



void structPainter::drawStruct(const ColinStruct &t, QPainter *painter, QTransform *trans, const Colin::Elements &toDraw_)
{
    p = painter;
    trm = trans;
    scaleM = t.scaleM();
    scaleP = t.scaleP();
    scaleU = t.scaleU();
    toDraw = toDraw_;

    drawResults = t.isCalculated();

	this->t = &t;



    p->setRenderHint(QPainter::Antialiasing, viewPortSettings::instance().antialiasing());



    //loads
    t.calculateShapes();
    if(toDraw.testFlag(Colin::nload))
    {
        for(int i=0; i<t.load_n(); i++)
        {
            if(t.load(i).typ() != ColinLoad::nodeLoad    &&
               t.load(i).typ() != ColinLoad::doubleLoadLeft &&
               t.load(i).typ() != ColinLoad::doubleLoadRight &&
               t.load(i).typ() != ColinLoad::moment      &&
               t.load(i).typ() != ColinLoad::tempChange  &&
               t.load(i).typ() != ColinLoad::tempDiffrence)
            {
                this->drawStLoad(t.load(i));
            }
            else if(t.load(i).typ() == ColinLoad::nodeLoad)
            {
                this->drawLoad(t.load(i), trm->map(t.node(t.load(i).at()).toQPointF()));
            }
            else if(t.load(i).typ() == ColinLoad::moment)
            {
                this->drawMoment(t.load(i), trm->map(t.node(t.load(i).at()).toQPointF()));
            }
            else if(t.load(i).typ() == ColinLoad::tempChange ||
                    t.load(i).typ() == ColinLoad::tempDiffrence)
            {
                this->drawTemp(t.load(i), trm->map(t.beam(t.load(i).at()).toQLineF()));
            }
	    else if(t.load(i).typ() == ColinLoad::doubleLoadLeft ||
		    t.load(i).typ() == ColinLoad::doubleLoadRight)
	    {
		this->drawDoubleLoad(t.load(i), trm->map(t.beam(t.load(i).at()).toQLineF()));
	    }

        }
    }


    //beams
    for(int i=0; i<t.beam_n(); i++)
	drawBeam(t.beam(i), i, t);

    //nodes
        for(int i=0; i<t.node_n(); i++)
            drawNode(t.node(i), i);



}


void structPainter::drawNode(const ColinNode &n, const int &i)
{
    QPointF po(n.x(), n.z());
    po = trm->map(po);
    if(n.hasbearing())
    {
        drawBearing(n.bearing(), po, drawResults && toDraw.testFlag(Colin::u));
    }
    if(drawResults && toDraw.testFlag(Colin::node) && !n.isSelected())
    {
	p->setPen(QPen(v->color(Colin::C_NodeRef), 4, Qt::SolidLine, Qt::RoundCap));
    }
    else
    {
        if(!n.isSelected() || ignoreSelection_)
	    p->setPen(QPen(v->color(Colin::C_Node), 4, Qt::SolidLine, Qt::RoundCap));
        else
	    p->setPen(QPen(v->color(Colin::C_Selection), 5, Qt::SolidLine, Qt::RoundCap));
    }
    p->drawPoint(po);
    p->drawText(QRect(po.x()-40, po.y()+5, 80, 80),
                QString("( %1 )").arg(i),
                Qt::AlignTop | Qt::AlignHCenter);


	QList<int> cls_list;
	if(t->cls_n()==0)
		cls_list << 0;
	if(drawResults && toDraw.testFlag(Colin::node))
	{
		foreach(int ls, cls_list)
		{
			{
			p->setPen(QPen(v->color(Colin::C_Node), 4, Qt::SolidLine, Qt::RoundCap));
				p->drawPoint(po+QPointF(n.u(ls), n.w(ls))*trm->m11()*scaleU);
				if(n.hasbearing())
				{
					drawBearing(n.bearing(), QPointF(po+QPointF(n.u(ls), n.w(ls))*trm->m11()*scaleU));
				}
			}
		}
	}

	if(((highlightMode.testFlag(catcher::CatchedNLine) ||
		 highlightMode.testFlag(catcher::CatchedQLine) ||
		 highlightMode.testFlag(catcher::CatchedMLine) ||
		 highlightMode.testFlag(catcher::CatchedULine) ) && highlight > -1 )||
		t->activeCLS_n() == 1 ||
		t->cls_n()==0)
	{
		int ls;
		if(t->cls_n()==0)
			ls = 0;
		else if(t->activeCLS_n() == 1)
			ls = t->activeCLS().values().at(0);
		else
			ls = highlight;
		if(drawResults && toDraw.testFlag(Colin::bearing) && n.hasbearing())
		{
			const int dP = 20;
			int dp;
			p->setPen(QPen(v->color(Colin::C_Reaction), 1, Qt::SolidLine, Qt::RoundCap));
			p->setBrush(v->color(Colin::C_Reaction));
			QString reactions;
			if(!qFuzzyIsNull(n.bearing().a_x(ls)))
			{
			reactions.append("Ax = "+QString::number(n.bearing().a_x(ls)*PPREFIX, 'f' , PRECISON)+unitSettings::instance().Peh()+ "\n");
				if(n.bearing().a_x(ls) > 0)
					dp = -dP;
				else
					dp = dP;
				p->drawLine(po+QPointF(dp, 0), po+QPointF(dp-n.bearing().a_x(ls)*scaleP*trm->m11(), 0));
				QPolygonF arrow(3);
				arrow[0]=po+QPointF(dp, 0);
				arrow[1]=po+QPointF(dp+(dp>0?1:-1)*arrowLenght, arrowWidth/2);
				arrow[2]=po+QPointF(dp+(dp>0?1:-1)*arrowLenght, -arrowWidth/2);
				p->drawPolygon(arrow);
			}
			if(!qFuzzyIsNull(n.bearing().a_z(ls)))
			{
			reactions.append("Az = "+QString::number(n.bearing().a_z(ls)*PPREFIX, 'f' , PRECISON)+unitSettings::instance().Peh()+ "\n");
				if(n.bearing().a_z(ls) > 0)
					dp = -dP;
				else
					dp = dP;
				p->drawLine(po+QPointF(0, dp), po+QPointF(0, dp-n.bearing().a_z(ls)*scaleP*trm->m11()));

				QPolygonF arrow(3);
				arrow[0]=po+QPointF(0, dp);
				arrow[1]=po+QPointF(arrowWidth/2, dp+(dp>0?1:-1)*arrowLenght);
				arrow[2]=po+QPointF(-arrowWidth/2, dp+(dp>0?1:-1)*arrowLenght);
				p->drawPolygon(arrow);
			}
			if(!qFuzzyIsNull(n.bearing().a_m(ls)))
			{
			reactions.append(QString("A%1 = ").arg(QChar(0x03C6))+QString::number(n.bearing().a_m(ls)*MPREFIX, 'f' , PRECISON)+unitSettings::instance().Meh()+ "\n");
				p->save();

				p->translate(po);
				const double angle = n.bearing().a_m(ls)*scaleM/2;
				p->drawArc(QRectF(-momentRad, -momentRad, 2*momentRad, 2*momentRad), -(90-angle*viewPortSettings::instance().MtoRad())*16, -(2*angle*viewPortSettings::instance().MtoRad())*16);
				p->rotate(90-angle*viewPortSettings::instance().MtoRad());

				QPointF points[3];
				points[0]=QPointF(momentRad+arrowWidth/2, 0);
				if(n.bearing().a_m(ls)>0)
					points[1]=QPointF(momentRad, -arrowLenght);
				else
					points[1]=QPointF(momentRad, +arrowLenght);
				points[2]=QPointF(momentRad-arrowWidth/2, 0);

				p->drawPolygon(points, 3);
				p->restore();
		}
		p->drawText(QRectF(po+QPointF(5, 10), QSize(500, 500)), reactions, Qt::AlignLeft | Qt::AlignTop);
		}
	}
}


void structPainter::drawBearing(const ColinSupport &b, const QPointF &po, bool refconf)
{
    if(refconf)
        p->setPen(QPen(v->color(Colin::C_BearingRef), 1));
    else if(b.isSelected() && !ignoreSelection_)
        p->setPen(QPen(v->color(Colin::C_Selection),1));
    else
        p->setPen(QPen(v->color(Colin::C_Bearing),1));

    p->save();
    p->translate(po);
    p->rotate(b.angle()*180/M_PI);


    switch(b.simpleform())
    {
    case ColinSupport::X:
        drawHBear(p, QPointF(0, 0));
        break;
    case ColinSupport::Z:
        drawVBear(p, QPointF(0, 0));
        break;
    case ColinSupport::Phi:
        drawMBear(p, QPointF(0, 0));
        break;
    case ColinSupport::XZ:
        drawHVBear(p, QPointF(0, 0));
        break;
    case ColinSupport::XPhi:
        drawHMBear(p, QPointF(0, 0));
        break;
    case ColinSupport::ZPhi:
        drawVMBear(p, QPointF(0, 0));
        break;
    case ColinSupport::XZPhi:
        drawHVMBear(p, QPointF(0, 0));
        break;
    case ColinSupport::NoBear:
        break;
    default:
        Q_ASSERT(true);
        break;
    }

    if(b.form().testFlag(ColinSupport::fx))
    {
        drawHSpring(p, QPointF(0, 0));
    }
    if(b.form().testFlag(ColinSupport::fz))
    {
        drawVSpring(p, QPointF(0, 0));
    }
    if(b.form().testFlag(ColinSupport::fphi))
    {
        drawMSpring(p, QPointF(0, 0));
    }

    p->restore();
}


void structPainter::drawBeam(const ColinBeam &s, const int &i, const ColinStruct &tw)
{


//	qDebug() << "highlight N " << highlightMode.testFlag(catcher::CatchedNLine);
//	qDebug() << "highlight Q " << highlightMode.testFlag(catcher::CatchedQLine);
//	qDebug() << "highlight M " << highlightMode.testFlag(catcher::CatchedMLine);
//	qDebug() << "highlight U " << highlightMode.testFlag(catcher::CatchedULine);
//	qDebug() << "highlight # " << highlight;



    if(s.isSelected()&& !ignoreSelection_)
        p->setPen(QPen(v->color(Colin::C_Selection), 1.5, Qt::SolidLine, Qt::SquareCap));
    else if(drawResults && toDraw.testFlag(Colin::u))
        p->setPen(QPen(v->color(Colin::C_BeamRef), 1, Qt::SolidLine, Qt::SquareCap));
    else
        p->setPen(QPen(v->color(Colin::C_Beam), 1, Qt::SolidLine, Qt::SquareCap));

        const double len = s.l()*trm->m11();
    p->save();
    p->translate(s.leftNode().x()*trm->m11()+trm->dx(), s.leftNode().z()*trm->m11()+trm->dy());
    p->rotate(s.angle()*180.0/M_PI);

    double xl = 0,
           xr = 0;

    if(toDraw.testFlag(Colin::joint))
    {
        const double gelMSize = 8;
        const double gelQSize = 4;
        const double gelNSize = 7;
        const double gelQh = 5;
        const double gelNh = 3;
        const double springNSize = 12;
        const double springNh = 5;
        const double springQSize = 12;
        const double springQh = 7;
        p->setBrush(v->color(Colin::C_Background));
        if(s.joint(ColinBeam::Ml))
        {
            xl += gelMSize;
            p->drawEllipse(QPointF(gelMSize/2, 0), gelMSize/2, gelMSize/2);
        }

        if(s.spring(ColinBeam::Ml))
        {
            p->drawArc(0, -gelMSize, gelMSize*2, gelMSize*2, 0, 180*16);
        }

        if(s.spring(ColinBeam::Ql))
        {
            p->drawLine(xl, 0, xl, springQh);
            p->drawLine(xl, springQh, xl+springQSize/2, springQh);
            p->drawLine(xl+springQSize/2, springQh, xl+springQSize-2, springQh*3/4.);
            p->drawLine(xl+springQSize-2, springQh*3/4.,  xl+2, springQh/4);
            p->drawLine(xl+2, springQh/4, xl+springQSize-2, -springQh/4.);
            p->drawLine(xl+springQSize-2, -springQh/4., xl+2, -springQh*3/4);
            p->drawLine(xl+2, -springQh*3/4., xl+springQSize/2, -springQh);
            p->drawLine(xl+springQSize/2, -springQh, xl+springQSize, -springQh);
            p->drawLine(xl+springQSize, -springQh, xl+springQSize, 0);

            xl += springQSize;
        }
        else if(s.joint(ColinBeam::Ql))
        {
           p->drawLine(xl+1, -gelQh, xl+1, gelQh);
           p->drawLine(xl+gelQSize, -gelQh, xl+gelQSize, gelQh);
           xl += gelQSize;
        }

        if(s.spring(ColinBeam::Nl) )
        {
            p->drawLine(xl, 0, xl+springNSize/8., -springNh);
            p->drawLine(xl+springNSize/8, -springNh, xl+springNSize*3/8., springNh);
            p->drawLine(xl+springNSize*3./8, springNh, xl+springNSize*5./8, -springNh);
            p->drawLine(xl+springNSize*5./8, -springNh, xl+springNSize*7./8, springNh);
            p->drawLine(xl+springNSize*7./8, springNh, xl+springNSize, 0);
            xl+=springNSize;
        }
        else if(s.joint(ColinBeam::Nl))
        {
            p->drawLine(xl+1, -gelNh, xl, gelNh);
            p->drawLine(xl+1, -gelNh, xl+gelNSize, -gelNh);
            p->drawLine(xl+1, gelNh, xl+gelNSize, gelNh);
            p->drawLine(xl+3, 0, xl+gelNSize, 0);
            xl += gelNSize;
        }
        if(s.joint(ColinBeam::Mr))
        {
            xr += gelMSize;
            p->drawEllipse(QPointF(len-gelMSize/2, 0), gelMSize/2, gelMSize/2);
        }
        if(s.spring(ColinBeam::Mr) && len>gelMSize*4)
        {
            p->drawArc(len-gelMSize*2, -gelMSize, gelMSize*2, gelMSize*2, 0, 180*16);
        }

        if(s.spring(ColinBeam::Qr))
        {
            p->drawLine(len-(xr), 0, len-(xr), springQh);
            p->drawLine(len-(xr), springQh, len-(xr+springQSize/2), springQh);
            p->drawLine(len-(xr+springQSize/2), springQh, len-(xr+springQSize-2), springQh*3/4.);
            p->drawLine(len-(xr+springQSize-2), springQh*3/4.,  len-(xr+2), springQh/4);
            p->drawLine(len-(xr+2), springQh/4, len-(xr+springQSize-2), -springQh/4.);
            p->drawLine(len-(xr+springQSize-2), -springQh/4., len-(xr+2), -springQh*3/4);
            p->drawLine(len-(xr+2), -springQh*3/4., len-(xr+springQSize/2), -springQh);
            p->drawLine(len-(xr+springQSize/2), -springQh, len-(xr+springQSize), -springQh);
            p->drawLine(len-(xr+springQSize), -springQh, len-(xr+springQSize), 0);

            xr += springQSize;
        }
        else if(s.joint(ColinBeam::Qr))
        {
            p->drawLine(len-xr-1, -gelQh, len-xr, gelQh);
            p->drawLine(len-xr-gelQSize, -gelQh, len-xr-gelQSize, gelQh);
            xr += gelQSize;
        }

        if(s.spring(ColinBeam::Nr) )
        {
            p->drawLine(len-(xr), 0, len-(xr+springNSize/8), -springNh);
            p->drawLine(len-(xr+springNSize/8), -springNh, len-(xr+springNSize*3/8), springNh);
            p->drawLine(len-(xr+springNSize*3./8), springNh, len-(xr+springNSize*5./8), -springNh);
            p->drawLine(len-(xr+springNSize*5./8), -springNh, len-(xr+springNSize*7./8), springNh);
            p->drawLine(len-(xr+springNSize*7./8), springNh, len-(xr+springNSize), 0);
            xr+=springNSize;
        }
        else if(s.joint(ColinBeam::Nr))
        {
	    p->drawLine(len-xr-1, -gelNh, len-xr, gelNh);
            p->drawLine(len-xr-1, -gelNh, len-xr-gelNSize, -gelNh);
            p->drawLine(len-xr-1, gelNh, len-xr-gelNSize, gelNh);
            p->drawLine(len-xr-3, 0, len-xr-gelNSize, 0);
            xr += gelNSize;
        }
    }


	QPainterPath uline;
	QPolygonF forces;
    if(drawResults)
	{

		QList<int> activeCLS = tw.activeCLS().values();
		if(tw.cls_n()==0)
			activeCLS << 0;
        //u/w
		p->setBrush(Qt::NoBrush);
		p->setPen(v->color(Colin::C_Beam));
		if(toDraw.testFlag(Colin::u))
		{
			foreach(int ls, activeCLS){
				uline.moveTo(QPointF(s.u(ls, 0), s.w(ls, 0))*scaleU*trm->m11());
				double x=0;
				for(int i=0; i<roundness; i++)
				{
					x+=s.l()/roundness;
					uline.lineTo((QPointF(x, 0)+QPointF(s.u(ls, x), s.w(ls, x))*scaleU)*trm->m11());
				}
				p->save();
				if(highlightMode.testFlag(catcher::CatchedULine) && highlight == ls){
					QPen pen = p->pen();
					pen.setWidth(2);
					p->setPen(pen);
				}
				p->drawPath(uline);
				p->restore();
				uline = QPainterPath();
				ls++;
			}
		}
		//N
        if(toDraw.testFlag(Colin::N))
        {
			setFunctionGradient(p, v->color(Colin::C_Np), v->color(Colin::C_Nm));
			QList<const function*> funs;
			foreach(int i, activeCLS){
				funs.append(&s.Nconst(i));
			}
			drawFunction(p, funs, s.l(), scaleP, PPREFIX);
			if(highlightMode.testFlag(catcher::CatchedNLine) && highlight > -1){
				funs.clear();
				funs.append(&s.Nconst(highlight));
				p->setPen(Qt::black);
				drawFunction(p, funs, s.l(), scaleP, PPREFIX);
			}
		}
		//Q
        if(toDraw.testFlag(Colin::Q))
        {
			setFunctionGradient(p, v->color(Colin::C_Qp), v->color(Colin::C_Qm));
			QList<const function*> funs;
			foreach(int i, activeCLS){
				funs.append(&s.Qconst(i));
			}
			drawFunction(p, funs, s.l(), scaleP, PPREFIX);
			if(highlightMode.testFlag(catcher::CatchedQLine) && highlight > -1){
				funs.clear();
				funs.append(&s.Qconst(highlight));
				p->setPen(Qt::black);
				drawFunction(p, funs, s.l(), scaleP, PPREFIX);
			}
		}
		//M
		forces.clear();
        if(toDraw.testFlag(Colin::M))
        {
			setFunctionGradient(p, v->color(Colin::C_Mp), v->color(Colin::C_Mm));
			QList<const function*> funs;
			foreach(int i, activeCLS){
				funs.append(&s.Mconst(i));
			}
			drawFunction(p, funs, s.l(), scaleM, MPREFIX);
			if(highlightMode.testFlag(catcher::CatchedMLine) && highlight > -1){
				funs.clear();
				funs.append(&s.Mconst(highlight));
				p->setPen(Qt::black);
				drawFunction(p, funs, s.l(), scaleM, MPREFIX);
			}
		}
	}

    if(s.isSelected()&& !ignoreSelection_)
        p->setPen(QPen(v->color(Colin::C_Selection), 1.5, Qt::SolidLine, Qt::SquareCap));
    else if(drawResults && toDraw.testFlag(Colin::u))
        p->setPen(QPen(v->color(Colin::C_BeamRef), 1, Qt::SolidLine, Qt::SquareCap));
    else
        p->setPen(QPen(v->color(Colin::C_Beam), 1, Qt::SolidLine, Qt::SquareCap));


    p->drawLine(xl, 0, len-xr, 0);

    if(len>80)
    {
        QPen pen(p->pen());
		//pen.setStyle(Qt::DashLine);
        p->setPen(pen);
        p->drawLine(len*0.2, 3, len*0.8, 3);

        p->drawText(QRect(-100, 6, len+200, 100),
                    QString("[ %1 ]").arg(i),
                    Qt::AlignHCenter | Qt::AlignTop);
    }
	/*
    if(len>100 &&
       !toDraw.testFlag(Colin::M) &&
       !toDraw.testFlag(Colin::Q) &&
       !toDraw.testFlag(Colin::N) )
     {
         QImage matI(colinIcons::icondir_ +"mat.png");
         QImage profI(colinIcons::icondir_ +"prof.png");

         p->setPen(v->color(Colin::C_Beam));
         p->drawImage(QRect(6, 6, 16, 16), profI, QRect(0, 0, 32, 32));
         p->drawImage(QRect(6, 24, 16, 16), matI, QRect(0, 0, 32, 32));
         p->drawText(QRect(24, 6, len-24, 16),
                     s.Profile().name());
         p->drawText(QRect(24, 24, len-24, 16),
                     s.Mat().name());
     }
		*/

    p->restore();


}

void structPainter::drawStLoad(const ColinLoad &l)
{

    p->setPen(Qt::black);

	setColor(p, l);



    QTransform loadT;
    loadT.rotate(atan2(l.Pz(), l.Px())*180.0/M_PI-90);

    QPolygonF points = trm->map(l.shape());

#ifndef QT_NO_DEBUG
	//qDebug() << "painting load. the points are:";
	//for(int i=0; i<points.size(); i++){
	//	qDebug() << QString("p%1(%2,%3)").arg(i).arg(points.at(i).x()).arg(points.at(i).y());
	//}
#endif

    p->drawPolygon(points);

    QPolygonF arrow(3);

    arrow[0] = QPointF(0, 0);
    arrow[1] = QPointF(-5, -10);
    arrow[2] = QPointF(5, -10);

    arrow = loadT.map(arrow);

    p->save();
    p->setBrush(p->pen().color());
    p->translate(points[0]);
    //p->setClipRegion(QRegion(p->transform().inverted().map(points).toPolygon()));

    const int ac = 5;
    QLineF line(points[0], points.last());
    for(int i=0; i<=ac; i++)
    {
        if(!(i==0 && l.typ() == ColinLoad::increasingLinearLoad )&&
           !(i==ac && l.typ() == ColinLoad::decreasingLinearLoad))
        p->drawPolygon(arrow);
	p->translate(line.dx()/ac, line.dy()/ac);
    }
    p->restore();
    for(int i=0; i<=ac; i++)
    {
        p->drawLine(points[3]*i/ac+points[0]*(ac-i)/ac,
                    points[2]*i/ac+points[1]*(ac-i)/ac);
    }

    p->setBrush(v->color(Colin::C_Load2));
    if(!ignoreHotSpots_)
    {
        if(l.typ()==ColinLoad::uniformlyDistibutedLoad)
            p->drawEllipse(QPointF(line.p1()/2+line.p2()/2)-QPointF(l.Px(), l.Pz())*trm->m11()*scaleP, loadBubblerad, loadBubblerad);
        else if(l.typ() == ColinLoad::increasingLinearLoad)
            p->drawEllipse(QPointF(line.p2())-QPointF(l.Px(), l.Pz())*trm->m11()*scaleP, loadBubblerad, loadBubblerad);
        else if(l.typ() == ColinLoad::decreasingLinearLoad)
            p->drawEllipse(QPointF(line.p1())-QPointF(l.Px(), l.Pz())*trm->m11()*scaleP, loadBubblerad, loadBubblerad);
    }

}

void structPainter::drawStLoad(const ColinLoad &l, QLineF *line)
{



	setColor(p, l);



    QTransform loadT;
    loadT.rotate(atan2(l.Pz(), l.Px())*180.0/M_PI-90);

    QPolygonF points(4);

    points[0] = line->p1();
    points[3] = line->p2();


    if(l.typ() == ColinLoad::uniformlyDistibutedLoad)
    {
        points[1] = points[0]-QPointF(l.Px(), l.Pz())*trm->m11()*scaleP;
        points[2] = points[3]-QPointF(l.Px(), l.Pz())*trm->m11()*scaleP;
    }
    else if(l.typ() == ColinLoad::increasingLinearLoad)
    {
	points[1]=points[0];
        points[2]= points[3]-QPointF(l.Px(), l.Pz())*trm->m11()*scaleP;
    }
    else if(l.typ() == ColinLoad::decreasingLinearLoad)
    {
        points[1] = points[0]-QPointF(l.Px(), l.Pz())*trm->m11()*scaleP;
	points[2] = points[3];
    }

    p->drawPolygon(points);

    QPolygonF arrow(3);

    arrow[0] = QPointF(0, 0);
    arrow[1] = QPointF(-5, -10);
    arrow[2] = QPointF(5, -10);

    arrow = loadT.map(arrow);

    p->save();
    p->setBrush(p->pen().color());
    p->translate(points[0]);
    p->setClipRegion(QRegion(p->transform().inverted().map(points).toPolygon()));

    const int ac = 5;
    for(int i=0; i<=ac; i++)
    {
	p->drawPolygon(arrow);
	p->translate(line->dx()/ac, line->dy()/ac);
    }
    p->restore();
    p->save();
    p->setPen(QPen(p->pen().color(), 2));
    for(int i=0; i<=ac; i++)
    {
	p->drawLine(points[3]*i/ac+points[0]*(ac-i)/ac,
		    points[2]*i/ac+points[1]*(ac-i)/ac);
    }
    p->restore();

    line->setP1(points[1]);
    line->setP2(points[2]);
    //line->translate(-l.Px()*tr->m11()*scaleP, -l.Pz()*tr->m11()*scaleP);

}

void structPainter::drawLoad(const ColinLoad &l, const QPointF &po)
{

    p->save();

	setColor(p, l);

    p->drawLine(po, po-QPointF(l.Px(), l.Pz())*scaleP*trm->m11());

    p->setBrush(v->color(Colin::C_Load2));
    if(!ignoreHotSpots_)
        p->drawEllipse(po-QPointF(l.Px(), l.Pz())*scaleP*trm->m11(), loadBubblerad, loadBubblerad);
    double angle = atan2(l.Pz(), l.Px());

    QPointF points[3];
    points[0]=QPointF(0, 0);
    points[1]=QPointF(-8, 3);
    points[2]=QPointF(-8, -3);

    p->translate(po);
    p->rotate(angle*180/M_PI);
    p->setBrush(p->pen().color());
    p->drawPolygon(points, 3);
    p->restore();

}

void structPainter::drawMoment(const ColinLoad &l, const QPointF &po)
{

    p->save();

	setColor(p, l);



    p->translate(po);
    const double angle = l.M()*scaleM/2;
    p->drawArc(QRectF(-momentRad, -momentRad, 2*momentRad, 2*momentRad), (90-angle*viewPortSettings::instance().MtoRad())*16, (2*angle*viewPortSettings::instance().MtoRad())*16);
    p->rotate(-90-angle*viewPortSettings::instance().MtoRad());

    QPointF points[3];
    points[0]=QPointF(momentRad+3, 0);
    if(l.M()>0)
        points[1]=QPointF(momentRad, -8);
    else
        points[1]=QPointF(momentRad, 8);
    points[2]=QPointF(momentRad-3, 0);

    p->setBrush(p->pen().color());
    p->drawPolygon(points, 3);

    p->restore();
}


void structPainter::drawTemp(const ColinLoad &l, const QLineF &line)
{

    p->save();


    p->translate(line.p1());
    p->rotate(-line.angle());

    QLinearGradient grad(0, 20, 0, -20);

    if(l.typ() == ColinLoad::tempChange)
    {
        QColor color;
        if(l.Px() > 0)
        {
            color = QColor(255, 0, 0, 200);
        }
        else
        {
            color = QColor(0, 0, 255, 200);
        }
        grad.setColorAt(0, QColor(0, 0, 0, 0));
        grad.setColorAt(0.5, color);
        grad.setColorAt(1, QColor(0, 0, 0, 0));

    }
    else if(l.typ() == ColinLoad::tempDiffrence)
    {
        grad.setColorAt(0, QColor(0, 0, 0, 0));
        grad.setColorAt(0.50001, QColor(0, 0, 255, 200));
        grad.setColorAt(0.49999, QColor(255, 0, 0, 200));
        grad.setColorAt(1, QColor(0, 0, 0, 0));
    }
    p->setBrush(QBrush(grad));
    p->setPen(Qt::NoPen);
    p->drawRoundedRect(0, -20, line.length(), 40, 20, 20);

    p->restore();

    if(line.length() > 100 && p->transform().m11() >= 1.0) //not draw if the painter is scaled down
        drawThermometer(p, QRect((line.p1()/2+line.p2()/2).toPoint()-QPoint(20, 100),
                                 QSize(40, 100)),
                                  (l.typ() == ColinLoad::tempChange)?l.Px() : l.Pz(), l.isSelected());

}

void structPainter::drawDoubleLoad(const ColinLoad &l, const QLineF &line)
{
    Q_ASSERT(l.typ()==ColinLoad::doubleLoadLeft|| l.typ()==ColinLoad::doubleLoadRight);
    p->save();


	setColor(p, l);



    if (l.typ()==ColinLoad::doubleLoadLeft)
    {
	p->translate(line.p1());
	p->rotate(-line.angle());
	p->translate(5, 0);
    }
    else /*if(l.typ()==wgv_load::doubleLoadRight)*/
    {
	p->translate(line.p2());
	p->rotate(-line.angle());
	p->translate(-5, 0);
    }
    QPointF points[3];
    if(l.M()!=0)
    {
        p->drawArc(-momentRad*2-2, -momentRad, momentRad*2, momentRad*2, -l.M()*16*viewPortSettings::instance().MtoRad()*scaleM/2, l.M()*viewPortSettings::instance().MtoRad()*scaleM*16);
        p->drawArc(2, -momentRad, momentRad*2, momentRad*2, 180*16-l.M()*16*viewPortSettings::instance().MtoRad()*scaleM/2, l.M()*viewPortSettings::instance().MtoRad()*scaleM*16);
	if(l.M()>0)
	    points[0]=QPointF(momentRad, -8);
	else
	    points[0]=QPointF(momentRad, 8);
	points[1]=QPointF(momentRad-3, 0);
	points[2]=QPointF(momentRad+3, 0);

	for(int i=0; i<2; i++)
	{
	    p->save();
	    if(i)p->scale(-1, 1);
	    p->translate(-momentRad-2, 0);
            p->rotate(-l.M()*viewPortSettings::instance().MtoRad()*scaleM/2);
	    p->setBrush(p->pen().color());
	    p->drawPolygon(points, 3);
	    p->restore();
	}
    }

    double q;
    for(int j=0; j<2; j++)
    {
	if(j==0)
	    q=l.Px()*scaleP*trm->m11();
	else
	{
	    q=l.Pz()*scaleP*trm->m11();
	    p->rotate(90);
	}

	if(q!=0)
	{
	    if(q>0)
		points[0] = QPointF(q/2+8, 3);
	    else
		points[0] = QPointF(q/2-8, 3);
	    points[1] = QPointF(q/2, 3);
	    points[2] = QPointF(q/2, 3+3);
	    for(int i=0; i<2; i++)
	    {
		p->save();
		if(i)p->rotate(180);
		p->drawLine(-q/2, 3, q/2, 3);
		p->drawPolygon(points, 3);
		p->restore();
	    }
	}
    }
    p->restore();
}

void structPainter::drawThermometer(QPainter *p, const QRect &boundingRect, const double &value, const bool &hasFocus, double *max, double *min)
{
    const double rad = 25;
    const double dr = 3;
    const double w = 24;
    const double h = 130;
    QPainterPath bubble;
    QPainterPath thermo;
    QPainterPath stingl;
    QPainterPath inside;

    double dMin;
    double dMax;

    if(max != 0)
        dMax = *max;
    else if(value>0)
        dMax = value*1.5;
    else
        dMax = -value/2;

    if(min != 0)
        dMin = *min;
    else if(value > 0)
        dMin = -value/2;
    else
        dMin = value*1.5;

    double nullH = -(dMin*(h-rad-w/2))/(dMax-dMin)+rad;
    double dValue = rad+(value-dMin)*(h-rad-w/2)/(dMax-dMin);

    thermo.addEllipse(-rad, -rad, 2*rad, 2*rad);
    bubble.addRect(-w/2., -h+w/2., w, h-w/2.);
    thermo.addEllipse(-w/2., -h, w, w);
    thermo += bubble;


    bubble = QPainterPath();
    inside.addEllipse(-rad+4, -rad+4, 2*rad-8, 2*rad-8);
    inside.addEllipse(-w/2+dr, -dValue-3, w-2*dr, 6);
    bubble.addRect(-w/2.+dr, -dValue, w-2*dr, dValue);
    inside += bubble;
    bubble = QPainterPath();


    bubble.moveTo(-(rad-dr), 0);
    bubble.arcTo(-(rad-dr), -(rad-dr), 2*(rad-dr), 2*(rad-dr), 180, -90-75);
    QPointF start(bubble.pointAtPercent(1)-QPointF(20, 20));
    bubble.cubicTo(start.x(), start.y(),
                   -(rad-dr)+20, 20,
                   -(rad-dr), 0);


    QLinearGradient bubbleLin(-rad, -rad, rad, 0);
    bubbleLin.setColorAt(0, QColor(255, 255, 255, 140));
    bubbleLin.setColorAt(1, QColor(201, 204, 218, 0));

    QRadialGradient bubbleRad(-rad*.4, -rad*.4, rad*.3);
    bubbleRad.setColorAt(0, QColor(255, 255, 255));
    bubbleRad.setColorAt(1, QColor(255, 255, 255, 0));

    QLinearGradient vertical(0, 0, 0, -h-w);
    vertical.setColorAt(0.5, QColor(255, 255, 255, 140));
    vertical.setColorAt(1, QColor(255, 255, 255, 0));

    QLinearGradient fill(0, -nullH+20, 0, -nullH-20);
    fill.setColorAt(0, QColor(0, 0, 255, 200));
    fill.setColorAt(1, QColor(255, 0, 0, 200));

    QTransform trans;
    trans.translate(0, -h+w/2);
    trans.scale(w/2/rad, w/2/rad);

    stingl.addRect(-w/2.+dr, -h+dr, w/2-2*dr, h-dr);
    stingl -= bubble;
    stingl -= trans.map(bubble);
    p->save();
    p->resetTransform();
    if(hasFocus)
        p->setPen(viewPortSettings::instance().color(Colin::C_Selection));
    else
        p->setPen(Qt::NoPen);
    p->setBrush(QColor(141, 194, 188, 200));
    double height = h+rad+4*dr;
    double width = 2*rad+4*dr;
    double scale = qMin(boundingRect.width()/width, boundingRect.height()/height);
    p->translate(boundingRect.x()+boundingRect.width()/2,
                 boundingRect.y());
    p->scale(scale, scale);
    p->translate(0, h+2*dr);
    p->drawPath(thermo);


    p->fillPath(inside, QBrush(fill));

    p->fillPath(bubble, QBrush(bubbleLin));
    p->fillPath(bubble, QBrush(bubbleRad));
    p->fillPath(stingl, QBrush(vertical));
    if(max != 0)
        *max = (p->transform().map(QPointF(0, -h))).y();
    if(min != 0)
        *min = (p->transform().map(QPointF(0, -rad))).y();
    p->save();
    p->setTransform(trans, true);
    p->fillPath(bubble, QBrush(bubbleLin));
    p->fillPath(bubble, QBrush(bubbleRad));
    p->restore();


    QFont bigger = p->font();
    bigger.setPointSizeF(bigger.pointSizeF()/scale);
    p->setFont(bigger);
    p->setPen(QColor(0, 0, 0));

    if(min!=0)
    {
        p->drawLine(-w/2+1, -rad, w/4, -rad);
        p->drawText(QRect(w/2, -rad-bigger.pointSizeF()/2-4, 50, bigger.pointSize()+8),
                    QString("%1").arg(dMin),
                    Qt::AlignTop |Qt::AlignLeft);
    }
    if(max!=0)
    {
        p->drawLine(-w/2+1, -h+w/2, w/4, -h+w/2);
        p->drawText(QRect(w/2, -h+w/2-bigger.pointSizeF()/2-4, 50, bigger.pointSize()+8),
                    QString("%1").arg(dMax),
                    Qt::AlignTop |Qt::AlignLeft);
    }

    if(max != 0 && min != 0 && dMin < 0 && dMax > 0)
    {
        p->drawLine(-w/2+1, -nullH, w/4, -nullH);
        p->drawText(QRect(w/2, -nullH-bigger.pointSizeF()/2-4, 50, bigger.pointSize()+8),
                    QString("0"),
                    Qt::AlignTop |Qt::AlignLeft);
    }

    p->drawLine(-w/2+1, -dValue, w/4, -dValue);
    p->drawText(QRect(w/2, -dValue-bigger.pointSizeF()/2-4, 50, bigger.pointSize()+8),
                QString("%1").arg(value),
                Qt::AlignTop | Qt::AlignLeft);

    p->restore();
}

const double showValueThres = 1e-3;

void structPainter::drawFunction(QPainter *p, const QList<const function*> &f, const double &l, const double &scale, const double &prefix)
{
	QPainterPath forces;
	forces.setFillRule(Qt::WindingFill);
	QTransform tran(trm->m11(),	0,					0,
					0,			trm->m11()*scale,	0,
					0,			0,					1);
	foreach(const function *fun, f){
		QPainterPath add;
		add.moveTo(0, 0);
		double x = 0;
		for(int i=0; i<=roundness; i++)
		{
			add.lineTo(QPointF(x, (*fun)(x)));
			x+=l/roundness;
		}
		add.lineTo(l, 0);
		p->save();
		p->setBrush(Qt::NoBrush);
		p->drawPath(tran.map(add));
		p->restore();
		forces.addPath(add);
	}
	forces = tran.map(forces);
	p->fillPath(forces, p->brush());

	double max0=0, maxl=0, min0=0, minl=0;
	foreach(const function *fun, f){
		max0 = qMax((*fun)(0), max0);
		maxl = qMax((*fun)(l), maxl);
		min0 = qMin((*fun)(0), min0);
		minl = qMin((*fun)(l), minl);
		for(int i=0; i<fun->maxCount(); i++){
			bool draw=true;
			foreach(const function *other, f){
				if((*fun)(fun->max(i))>0){
					if((*fun)(fun->max(i))<(*other)(fun->max(i))){
						draw=false;
						break;
					}
				}
				else{
					if((*fun)(fun->max(i))>(*other)(fun->max(i))){
						draw=false;
						break;
					}
				}
			}
			if(!draw)
				continue;
			double max = fun->max(i);
			if(max>=0 && max<=l)
			{
				p->save();
				if((*fun)(max) <= -showValueThres)
				{
					p->translate(max*trm->m11(), (*fun)(max)*scale*trm->m11()-6);
					p->drawLine(0, 0, 0, 6);
					p->rotate(-45);
					p->drawLine(0, 0, 20, 0);
					p->drawText(QPointF(5, -2), QString::number((*fun)(max)*prefix, 'f' , PRECISON));
				}
				else if((*fun)(max) >= showValueThres)
				{
					p->translate(max*trm->m11(), (*fun)(max)*scale*trm->m11()+6);
					p->drawLine(0, 0, 0, -6);
					p->rotate(45);
					p->drawLine(0, 0, 20, 0);
					p->drawText(QPointF(5, -2), QString::number((*fun)(max)*prefix, 'f' , PRECISON));
				}
				p->restore();
			}
		}
	}
	//randpunkte
	if(max0 >= showValueThres)
	{
		p->save();
		p->translate(0, max0*scale*trm->m11()+6);
		p->drawLine(0, 0, 0, -6);
		p->rotate(45);

		p->drawLine(0, 0, 20, 0);
		p->drawText(QPointF(5, -2), QString::number(max0*prefix, 'f' , PRECISON));
		p->restore();
	}
	if(maxl >= showValueThres)
	{
		p->save();
		p->translate(l*trm->m11(), maxl*scale*trm->m11()+6);
		p->drawLine(0, 0, 0, -6);
		p->rotate(45);

		p->drawLine(0, 0, 20, 0);
		p->drawText(QPointF(5, -2), QString::number(maxl*prefix, 'f' , PRECISON));
		p->restore();
	}
	if(min0 <= -showValueThres)
	{
		p->save();
		p->translate(0, min0*scale*trm->m11()-6);
		p->drawLine(0, 0, 0, 6);
		p->rotate(-45);
		p->drawLine(0, 0, 20, 0);
		p->drawText(QPointF(5, -2), QString::number(min0*prefix, 'f' , PRECISON));
		p->restore();
	}
	if(minl <= -showValueThres)
	{
		p->save();
		p->translate(l*trm->m11(), minl*scale*trm->m11()-6);
		p->drawLine(0, 0, 0, 6);
		p->rotate(-45);
		p->drawLine(0, 0, 20, 0);
		p->drawText(QPointF(5, -2), QString::number(minl*prefix, 'f' , PRECISON));
		p->restore();
	}
}

void structPainter::setHighlightedObject(catcher::CatchCases c, int object)
{
	highlightMode=c;
	highlight = object;
}

void structPainter::setHighlightedObject(catcher::CatchCases c)
{
	highlightMode=c;
}

const double springL = 40;
const double springDL= 5;
const double springW = 8;
const double springC = 4;

void structPainter::drawHSpring(QPainter *p, const QPointF &po, const double &d)
{
    double l = springL-d-2*springDL;
    p->drawLine(po, po-QPointF(springDL, 0));
    p->drawLine(po-QPointF(springL, 0), po-QPointF(springL-springDL, 0));

    double dl = l/springC;
    for(int i=0; i<springC; i++)
    {
        p->drawLine(po-QPointF(springDL+dl*i, 0), po-QPointF(springDL+dl*(i+0.25), -springW));
        p->drawLine(po-QPointF(springDL+dl*(i+0.25), -springW), po-QPointF(springDL+dl*(i+0.75), springW));
        p->drawLine(po-QPointF(springDL+dl*(i+0.75), springW), po-QPointF(springDL+dl*(i+1), 0));
    }
    drawHBear(p, po-QPointF(springL, 0));
}

void structPainter::drawMSpring(QPainter *p, const QPointF &po)
{
    p->drawArc(QRectF(po.x()-springL, po.y()-springL/2, springL, springL), 0, 135*16);
    p->save();
    p->translate(po-QPointF(springL/2, 0));
    p->rotate(22.5);
    p->drawArc(QRectF(-springL/4., po.y()-springL/2, springL/2, springL/2), 0, 360*16);
    p->drawArc(QRectF(-springL/4.+1, po.y()-springL/2+1, springL/2, springL/2), 0, 360*16);
    p->drawArc(QRectF(-springL/4.-1, po.y()-springL/2-1, springL/2, springL/2), 0, 360*16);
    p->rotate(45-22.5);
    drawHVMBear(p, -QPointF(springL/2, 0));
    p->restore();
}


void structPainter::drawMBear(QPainter *p, const QPointF &po)
{
    p->drawLine(po+QPointF(-10, 0), po+QPointF(10, 0));
    p->drawLine(po+QPointF(-10, 0), po+QPointF(-10, 20));
    double ds = 3;
    p->save();
    p->translate(po);
    p->rotate(90);
    drawGround(p, QLineF(QPointF(30, 10+ds), QPointF(-10, 10+ds)));
    p->restore();
}

void structPainter::drawHVMBear(QPainter *p, const QPointF &po, bool space)
{
    double ds = 0;
    if(space)
    {
        p->drawLine(po+QPointF(-10, 0), po+QPointF(10, 0));
        ds = 3;
    }
    drawGround(p, QLineF(po+QPointF(-20, ds), po+QPointF(20, ds)));
}

void structPainter::drawHVBear(QPainter *p, const QPointF &po, bool space)
{
    p->drawLine(po, po+QPointF(-10, 12));
    p->drawLine(po, po+QPointF(10, 12));
    double ds = 0;
    if(space)
    {
        p->drawLine(po+QPointF(-10, 12), po+QPointF(10, 12));
        ds = 3;
    }
    drawGround(p, QLineF(po+QPointF(-20, 12+ds), po+QPointF(20, 12+ds)));

}

void structPainter::drawGround(QPainter *p, const QLineF &l)
{
    QPainterPath ground;
    ground.moveTo(l.p1());
    ground.lineTo(l.p2());
    ground.cubicTo(l.p2()+QPointF(-5, 6),
                   l.p1()/2+l.p2()/2+QPointF(5, 8),
                   l.p1()/2+l.p2()/2+QPointF(0, 10));
    ground.cubicTo(l.p1()/2+l.p2()/2+QPointF(-5, 12),
                   l.p1()+QPointF(5, 4),
                   l.p1());
    p->setClipPath(ground);
    const int linecount = 9;
    for(int i=0; i<linecount+1; i++)
    {
        p->drawLine(l.p1()/linecount*(linecount-i)+l.p2()/linecount*i,
                    l.p1()/linecount*(linecount-i)+l.p2()/linecount*i+QPointF(-30, 30));
    }
    p->setClipping(false);
    p->drawLine(l);
}

void structPainter::ignoreHotSpots(const bool &ignore)
{
    ignoreHotSpots_ = ignore;
}

void structPainter::ignoreSelection(const bool &ignore)
{
    ignoreSelection_ = ignore;
}

void structPainter::setFunctionGradient(QPainter *p, QColor c1, QColor c2)
{
	QLinearGradient grad(0, dh, 0, -dh);
	grad.setColorAt(0, c1);
	grad.setColorAt(1, c2);
	p->setPen(QPen(QBrush(grad), 1));
	c1.setAlpha(alphaC);
	c2.setAlpha(alphaC);
	grad.setColorAt(0, c1);
	grad.setColorAt(1, c2);
	p->setBrush(QBrush(grad));
}

void structPainter::setColor(QPainter *p, const ColinLoad &l, bool hotspot)
{
	QColor lc;
	if(l.isSelected() && !ignoreSelection_)
		lc = QColor(v->color(Colin::C_Selection));
	else{
		if(l.set()==-1)
			lc = QColor(v->color(Colin::C_Load1));
		else
			lc = t->bls(l.set()).color();
	}
	p->setPen(QPen(lc, 1));

	lc.setAlpha(alphaC);

	p->setBrush(QBrush(lc));
}
