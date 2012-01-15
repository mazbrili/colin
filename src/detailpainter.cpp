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

#include "detailpainter.h"

#include <QtGui/QPainter>

#include "viewportsettings.h"
#include "filelist.h"


detailPainter::detailPainter()
{
}

double detailPainter::validAngle(double angle) const
{
	if(angle>2*M_PI)
	{
		angle-=2*M_PI;
	}
	else if(angle<0)
	{
		angle+=2*M_PI;
	}
	return angle;
}

void detailPainter::drawNode(QPainter *p, const ColinStruct &t, int object, const QList<int> &cls)
{
	QSize s = QSize(p->device()->width(), p->device()->height());
	p->setPen(viewPortSettings::instance().color(Colin::C_Beam));
	ColinStruct *tw = filelist::instance().currentFile();
	QMap<int, double>beams;
	QList<double>beamangles;
	for(int i=0; i<tw->beam_n(); i++)
	{
		if(tw->beam(i).leftNodeI() == object)
		{
			double angle = tw->beam(i).angle();
			beams.insert(i, validAngle(angle));
			beamangles.append(validAngle(angle));
		}
		else if(tw->beam(i).rightNodeI() == object)
		{
			double angle = tw->beam(i).angle()+M_PI;
			if(angle > 2*M_PI)
				angle-=2*M_PI;
			beams.insert(i, validAngle(angle));
			beamangles.append(validAngle(angle));
		}
	}
	qSort(beamangles);
	QList<double> clipangles;
	if(!beamangles.isEmpty())
	{
		clipangles.append((beamangles.last()+beamangles.first()-2*M_PI)/2);
		for(int i=1; i<beamangles.count(); i++)
		{
			clipangles.append((beamangles.at(i-1)+beamangles.at(i))/2);
		}

	}


	//drawing

	const int iRad = 8; //the "inner" radius of the node
	const double aRad = 10; //the "outer" radius of the node
	const double length = 15;
	const double pc = 2;
	const double radc = 8;
	const double d = 5;
	const double dys = d/2 + radc;
	const double dxs = sqrt(pow(aRad+radc, 2) - pow(dys, 2));
	const double sa = atan2(dys, dxs);
	const double NStart = 30;
	const double QStart = 40;
	const double MStart = 50;
	const double MExStart = 25;
	const double RxStart = 15;
	const double RzStart = 15;
	const double RpStart = 20;

	p->setRenderHint(QPainter::Antialiasing, true);
	p->translate(s.height()/2, s.height()/2);
	p->scale(s.height()/250., s.height()/250.);
	p->setPen(QPen(p->pen().color(), 0.5));

	QPainterPath beam;


	beam.moveTo(QPointF(dxs, d/2));
	beam.arcTo(QRectF(dxs-radc, dys-radc, 2*radc, 2*radc), 90, 90-sa*180/M_PI);
	beam.arcTo(QRectF(-aRad, -aRad, 2*aRad, 2*aRad), -sa*180/M_PI, -360+2*sa*180/M_PI);
	beam.arcTo(QRectF(dxs-radc, -dys+radc-2*radc, 2*radc, 2*radc), -180+sa*180/M_PI, 90-sa*180/M_PI);
	beam.lineTo(QPointF(aRad+length, -d/2));
	beam.cubicTo(QPointF(aRad+length+pc, -d/4), QPointF(aRad+length-pc, d/6),
				 QPointF(aRad+length, d/2));
	beam.closeSubpath();

	for(int i=0; i<clipangles.count(); i++)
	{
		if(clipangles.count()>1)
		{
			QPainterPath clipPath;
			clipPath.moveTo(0, 0);
			clipPath.lineTo(100*cos(clipangles.at(i)), 100*sin(clipangles.at(i)));
			clipPath.lineTo(100*cos(beamangles.at(i)), 100*sin((beamangles.at(i))));
			if(i==beamangles.count()-1)
				clipPath.lineTo(100*cos(clipangles.at(0)), 100*sin((clipangles.at(0))));
//              clipPath.arcTo(QRectF(-200, -200, 400, 400), clipangles.last()*180/M_PI, validAngle(clipangles.last()-clipangles.first()-2*M_PI)*180/M_PI);
			else
				clipPath.lineTo(100*cos(clipangles.at(i+1)), 100*sin((clipangles.at(i+1))));
//              clipPath.arcTo(QRectF(-200, -200, 400, 400), clipangles.at(i)*180/M_PI, validAngle(clipangles.at(i+1)-clipangles.at(i))*180/M_PI);
			clipPath.closeSubpath();
			p->setClipPath(clipPath);
		}
/*
		if(i==0)
			p->fillRect(QRect(-100, -100, 200, 200), QColor(255, 0, 0, 100));
		if(i==1)
			p->fillRect(QRect(-100, -100, 200, 200), QColor(0, 0, 255, 100));
		if(i==2)
			p->fillRect(QRect(-100, -100, 200, 200), QColor(0, 255, 0, 100));
*/

		QTransform t;
		t.rotate(beamangles.at(i)*180/M_PI);
		p->drawPath(t.map(beam));

/*      p->save();
		p->setPen(Qt::red);
		p->drawLine(0, 0, 100*cos(clipangles.at(i)), 100*sin(clipangles.at(i)));
		p->setPen(Qt::green);
		p->drawLine(0, 0, 100*cos(beamangles.at(i)), 100*sin(beamangles.at(i)));
		p->restore();
*/
	}

	p->setClipping(false);
	p->setBrush(p->pen().color());
	p->drawEllipse(QPoint(), iRad, iRad);


	if(!tw->isCalculated())
		return;

	QPolygonF arrow(3);
	double scale = viewPortSettings::instance().resultsWidgetScale();

	for(int i=0; i<beams.count(); i++)
	{
		int currentBeam = beams.keys().at(i);
		bool left = (tw->beam(currentBeam).leftNodeI() == object);



		p->save();
		p->rotate(beams.value(beams.keys().at(i))*180/M_PI);

	/*******
	*  N  *
	*******/

		double stress = tw->beam(currentBeam).N(cls.at(0), left? 0 : tw->beam(currentBeam).l());

		if(!qFuzzyIsNull(stress))
		{
			p->setPen(viewPortSettings::instance().color(Colin::C_Np));

			if(stress>0)
			{
				//p->setPen(viewPortSettings::instance().color(Colin::C_Np));
				arrow[0] = QPointF(NStart+stress*tw->scaleP()*scale+5, 0);
				arrow[1] = QPointF(NStart+stress*tw->scaleP()*scale, -3);
				arrow[2] = QPointF(NStart+stress*tw->scaleP()*scale, 3);
			}
			else
			{
				//p->setPen(viewPortSettings::instance().color(Colin::C_Nm));
				arrow[0] = QPointF(NStart-5, 0);
				arrow[1] = QPointF(NStart, -3);
				arrow[2] = QPointF(NStart, 3);
			}
			p->setBrush(p->pen().color());
			p->drawPolygon(arrow);
			p->drawLine(NStart, 0, NStart+fabs(stress*tw->scaleP()*scale), 0);
		}

	/*******
	*  Q  *
	*******/

		stress = tw->beam(currentBeam).Q(cls.at(0), left? 0 : tw->beam(currentBeam).l());

		if(!qFuzzyIsNull(stress))
		{
			arrow[0] = QPointF(QStart, stress*tw->scaleP()*scale/2+((stress>0)?5:-5));
			arrow[1] = QPointF(QStart-3, stress*tw->scaleP()*scale/2);
			arrow[2] = QPointF(QStart+3, stress*tw->scaleP()*scale/2);
			p->setPen(viewPortSettings::instance().color(Colin::C_Qp));
			//if(stress>0)
			//{
			//    p->setPen(viewPortSettings::instance().color(Colin::C_Qp));
			//}
			//else
			//{
			//    p->setPen(viewPortSettings::instance().color(Colin::C_Qm));
			//}
			p->setBrush(p->pen().color());
			p->drawPolygon(arrow);
			p->drawLine(QStart, fabs(stress*tw->scaleP()*scale)/2, QStart, -fabs(stress*tw->scaleP()*scale)/2);
		}


	/*******
	*  M  *
	*******/

		stress = tw->beam(currentBeam).M(cls.at(0), left? 0 : tw->beam(currentBeam).l())*(left?1:-1);

		if(!qFuzzyIsNull(stress))
		{
			arrow[0] = QPointF(MStart, (stress>0)?-5:5);
			arrow[1] = QPointF(MStart-3, 0);
			arrow[2] = QPointF(MStart+3, 0);
			p->setPen(viewPortSettings::instance().color(Colin::C_Mp));
			//if(stress>0)
			//{
			//    p->setPen(viewPortSettings::instance().color(Colin::C_Mp));
			//}
			//else
			//{
			//    p->setPen(viewPortSettings::instance().color(Colin::C_Mm));
			//}
			p->setBrush(p->pen().color());
			p->rotate(-stress*tw->scaleM()/2*viewPortSettings::instance().MtoRad());
			p->drawPolygon(arrow);
			p->drawArc(-MStart, -MStart, 2*MStart, 2*MStart, 0, -stress*tw->scaleM()*16*viewPortSettings::instance().MtoRad());
		}

		p->restore();
	}
	/*************
	*   loads   *
	*************/

	p->setPen(viewPortSettings::instance().color(Colin::C_Load1));
	p->setBrush(p->pen().color());

	for(int i=0; i<tw->load_n(); i++)
	{
		if(tw->load(i).at()!=object)
			continue;
		if(tw->load(i).typ() == ColinLoad::nodeLoad)
		{
			double nodeangle = atan2(tw->load(i).Pz(), tw->load(i).Px())*180./M_PI+180;
			double nodeabs = sqrt(pow(tw->load(i).Px(), 2)+pow(tw->load(i).Pz(), 2));
			p->rotate(nodeangle);


			arrow[0] = QPointF(RxStart-5, 0);
			arrow[1] = QPointF(RxStart, -3);
			arrow[2] = QPointF(RxStart, 3);

			p->setBrush(p->pen().color());
			p->drawPolygon(arrow);
			p->drawLine(RxStart, 0, RxStart+fabs(nodeabs*tw->scaleP()*scale), 0);

			p->rotate(-nodeangle);
		}
		else if(tw->load(i).typ() == ColinLoad::moment)
		{

			arrow[0] = QPointF(MExStart, (tw->load(i).M()>0)?-5:5);
			arrow[1] = QPointF(MExStart-3, 0);
			arrow[2] = QPointF(MExStart+3, 0);

			p->save();

			p->rotate(-90-tw->load(i).M()*tw->scaleM()/2*viewPortSettings::instance().MtoRad());
			p->drawPolygon(arrow);
			p->drawArc(-MExStart, -MExStart, 2*MExStart, 2*MExStart, 0, -tw->load(i).M()*tw->scaleM()*16*viewPortSettings::instance().MtoRad());

			p->restore();
		}
	}
	if(tw->node(object).hasbearing())
	{
		p->setPen(viewPortSettings::instance().color(Colin::C_Reaction));
		p->setBrush(p->pen().color());
		const ColinSupport &bear = tw->node(object).bearing();

	/*******
	* Ax  *
	*******/

		if(!qFuzzyIsNull(bear.a_x(cls.at(0))))
		{
			p->drawLine(-RxStart, 0, -RxStart-fabs(bear.a_x(cls.at(0))*scale*tw->scaleP()), 0);
			if(bear.a_x(cls.at(0))>0)
			{
				arrow[0] = QPointF(-RxStart+5, 0);
				arrow[1] = QPointF(-RxStart, -3);
				arrow[2] = QPointF(-RxStart, 3);
			}
			else
			{
				arrow[0] = QPointF(-RxStart+bear.a_x(cls.at(0))*scale*tw->scaleP()-5, 0);
				arrow[1] = QPointF(-RxStart+bear.a_x(cls.at(0))*scale*tw->scaleP(), -3);
				arrow[2] = QPointF(-RxStart+bear.a_x(cls.at(0))*scale*tw->scaleP(), 3);
			}
			p->drawPolygon(arrow);
		}


	/*******
	* Az  *
	*******/

		if(!qFuzzyIsNull(bear.a_z(cls.at(0))))
		{
			p->drawLine(0, RzStart, 0, RzStart+fabs(bear.a_z(cls.at(0))*scale*tw->scaleP()));
			if(bear.a_z(cls.at(0))<0)
			{
				arrow[0] = QPointF(0, RzStart-5);
				arrow[1] = QPointF(-3, RzStart);
				arrow[2] = QPointF(3, RzStart);
			}
			else
			{
				arrow[0] = QPointF(0, RzStart+bear.a_z(cls.at(0))*scale*tw->scaleP()+5);
				arrow[1] = QPointF(-3, RzStart+bear.a_z(cls.at(0))*scale*tw->scaleP());
				arrow[2] = QPointF(3, RzStart+bear.a_z(cls.at(0))*scale*tw->scaleP());
			}
			p->drawPolygon(arrow);
		}


	/*******
	* Ap  *
	*******/

		if(!qFuzzyIsNull(bear.a_m(cls.at(0))))
		{

			arrow[0] = QPointF(RpStart, (bear.a_m(cls.at(0))>0)?-5:5);
			arrow[1] = QPointF(RpStart-3, 0);
			arrow[2] = QPointF(RpStart+3, 0);

			p->save();

			p->rotate(90-bear.a_m(cls.at(0))*tw->scaleM()/2*viewPortSettings::instance().MtoRad());
			p->drawPolygon(arrow);
			p->drawArc(-RpStart, -RpStart, 2*RpStart, 2*RpStart, 0, -bear.a_m(cls.at(0))*tw->scaleM()*16*viewPortSettings::instance().MtoRad());

			p->restore();
		}
		p->resetTransform();
	}
}
void detailPainter::drawBeamExtern(QPainter *p, const ColinStruct &t, int i, const QList<int> &cls)
{

	double scale = viewPortSettings::instance().resultsWidgetScale();

	const ColinBeam &b = t.beam(i);

	double l = 120;
	p->translate(p->device()->height()/2, p->device()->height()/2);
	p->scale(p->device()->height()/200., p->device()->height()/200.);
	p->rotate(b.angle()*180./M_PI);
	p->setPen(QPen(p->pen().color(), 0.5));

	p->setRenderHint(QPainter::Antialiasing, true);
	p->drawLine(-l/2, 1.5, l/2, 1.5);
	p->drawLine(-l/2, -1.5, l/2, -1.5);


	QLineF loadLine;
	QTransform forLine;
	forLine.rotate(-b.angle()*180./M_PI);
	for(int j=0; j<(t.cls_n()?t.cls(cls.at(0)).count():1); j++)
	{
		double fac;
		if(t.cls_n())
			fac = t.cls(cls.at(0)).fac(j)*t.scaleP()*scale;
		else
			fac = t.scaleP()*scale;
		for(int l=0; l<t.load_n(); l++)
		{
			if((t.load(l).set() == (t.cls_n()?t.cls(cls.at(0)).bls(j):-1) && t.load(l).at() == i))
			{
				if(t.load(l).typ() == ColinLoad::uniformlyDistibutedLoad)
				{
					loadLine.setP1(loadLine.p1()+forLine.map(QPointF(-t.load(l).Px()*fac, -t.load(l).Pz()*fac)));
					loadLine.setP2(loadLine.p2()+forLine.map(QPointF(-t.load(l).Px()*fac, -t.load(l).Pz()*fac)));
				}
				else if(t.load(l).typ() == ColinLoad::increasingLinearLoad)
				{
					loadLine.setP2(loadLine.p2()+forLine.map(QPointF(-t.load(l).Px()*fac, -t.load(l).Pz()*fac)));
				}
				else if(t.load(l).typ() == ColinLoad::decreasingLinearLoad)
				{
					loadLine.setP1(loadLine.p1()+forLine.map(QPointF(-t.load(l).Px()*fac, -t.load(l).Pz()*fac)));
				}
				else
					continue;
				qDebug() << "adding load " << l << " * " << fac << " to line";
				qDebug() << "line = " << loadLine;
			}
		}
	}
	loadLine.setP1(loadLine.p1()+QPointF(-l/2, 0));
	loadLine.setP2(loadLine.p2()+QPointF(l/2, 0));

	QColor lcolor = viewPortSettings::instance().color(Colin::C_Load1);
	p->setPen(QPen(lcolor, p->pen().widthF()));
	lcolor.setAlpha(80);
	p->setBrush(lcolor);

	QPainterPath loads;


	loads.moveTo(-l/2, 0);

	loads.lineTo(loadLine.p1());

	loads.lineTo(loadLine.p2());
	loads.lineTo(l/2, 0);

	p->drawPath(loads);


	QPolygonF arrow(3);

	arrow[0] = QPointF(0, 0);
	arrow[1] = QPointF(-5, -10);
	arrow[2] = QPointF(5, -10);

	arrow = forLine.map(arrow);

	p->save();
	p->setBrush(lcolor);

	const int ac = 5;

	p->setClipPath(loads);
	p->setBrush(p->pen().color());

	for(int arr=0; arr<=ac; arr++)
	{
		QLineF sl(QPointF(-l/2+l*arr/ac, 0), loadLine.pointAt(double(arr)/double(ac)));
		p->drawLine(sl);
		QTransform forarrow;
		forarrow.translate(-l/2+l*arr/ac, 0);
		forarrow.rotate(-sl.angle()+b.angle()*180./M_PI+90);
		p->drawPolygon(forarrow.map(arrow));
	}

	p->restore();

	if(b.getStruct()->isCalculated())
	{
		p->save();
		p->translate(-l/2, 0);
		p->scale(-1, 1);
		drawLoads(p, b.N(cls.at(0), 0), -b.Q(cls.at(0), 0), b.M(cls.at(0), 0));
		p->restore();
		p->save();
		p->translate(l/2, 0);
		drawLoads(p, b.N(cls.at(0), b.l()), b.Q(cls.at(0), b.l()), b.M(cls.at(0), b.l()));
		p->restore();
	}
}

void detailPainter::drawBeamIntern(QPainter *p, const ColinStruct &t, int i, const double &x, const QList<int> &cls)
{

}

void detailPainter::drawBeamFunctions(QPainter *p, const ColinStruct &t, int i, const QList<int> &cls)
{

}


void detailPainter::drawLoads(QPainter *p, double N, double Q, double M)
{

	const double NStart = 10;
	const double QStart = 20;
	const double MStart = 30;

	double scale = viewPortSettings::instance().resultsWidgetScale();
	ColinStruct *tw = filelist::instance().currentFile();
	QPolygonF arrow(3);

	if(!qFuzzyIsNull(N))
	{
		p->setPen(QPen(viewPortSettings::instance().color(Colin::C_Np), p->pen().widthF()));

		if(N>0)
		{
			arrow[0] = QPointF(NStart+N*tw->scaleP()*scale+5, 0);
			arrow[1] = QPointF(NStart+N*tw->scaleP()*scale, -3);
			arrow[2] = QPointF(NStart+N*tw->scaleP()*scale, 3);
		}
		else
		{
			arrow[0] = QPointF(NStart-5, 0);
			arrow[1] = QPointF(NStart, -3);
			arrow[2] = QPointF(NStart, 3);
		}
		p->setBrush(p->pen().color());
		p->drawPolygon(arrow);
		p->drawLine(NStart, 0, NStart+fabs(N*tw->scaleP()*scale), 0);
	}

	if(!qFuzzyIsNull(Q))
	{
		arrow[0] = QPointF(QStart, Q*tw->scaleP()*scale/2+((Q>0)?5:-5));
		arrow[1] = QPointF(QStart-3, Q*tw->scaleP()*scale/2);
		arrow[2] = QPointF(QStart+3, Q*tw->scaleP()*scale/2);
		p->setPen(QPen(viewPortSettings::instance().color(Colin::C_Qp), p->pen().widthF()));
		p->setBrush(p->pen().color());
		p->drawPolygon(arrow);
		p->drawLine(QStart, fabs(Q*tw->scaleP()*scale)/2, QStart, -fabs(Q*tw->scaleP()*scale)/2);
	}


	if(!qFuzzyIsNull(M))
	{
		arrow[0] = QPointF(MStart, (M>0)?-5:5);
		arrow[1] = QPointF(MStart-3, 0);
		arrow[2] = QPointF(MStart+3, 0);
		p->setPen(viewPortSettings::instance().color(Colin::C_Mp));
		p->setBrush(p->pen().color());
		p->rotate(-M*tw->scaleM()/2*viewPortSettings::instance().MtoRad());
		p->drawPolygon(arrow);
		p->drawArc(-MStart, -MStart, 2*MStart, 2*MStart, 0, -M*tw->scaleM()*16*viewPortSettings::instance().MtoRad());
	}

}

