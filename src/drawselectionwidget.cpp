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

#include "drawselectionwidget.h"


const double fontscale = 1.75;

drawSelectionWidget::drawSelectionWidget(int _id, QWidget *parent) :
	abstractDrawSelectionWidget(parent)
{
    id = _id;

    setMouseTracking(true);
    mouseOver = false;

	setWhatsThis(tr("<b>visible elements</b> <a href=\"view/visible\">open manual</a><br /> <br />")+
				 tr("Use this widget to specify the visible elements in the view by clicking the propriate symbol!<br />")+
				 tr("<b>u</b>: Show the displancment of the structure <br />")+
				 tr("<b>N</b>: Show normal forces in the structure <br />")+
				 tr("<b>Q</b>: Show shear forces in the structure <br />")+
				 tr("<b>M</b>: Show moments in the structure <br />")+
				 tr("<b>Node</b>: Show the displancment of loads <br />")+
				 tr("<b>Load</b>: Show loads <br />")+
				 tr("<b>Support</b>: Show Supports and reaction forces <br />")+
				 tr("<b>Hinge</b>: Show Hinges of the structure <br />"));
    
}

void drawSelectionWidget::mouseMoveEvent(QMouseEvent *)
{
    repaint();
}

void drawSelectionWidget::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
        mousePressed = true;
    repaint();
}

void drawSelectionWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if(mousePressed)
    {
        if(uR.contains(e->pos()))
        {
            Colin::Elements e = viewPortSettings::instance().elements(id);
            if(e.testFlag(Colin::u))
                e &= ~Colin::u;
            else
                e |= Colin::u;
            viewPortSettings::instance().setElements(id, e);
        }
        else if(nR.contains(e->pos()))
        {
            Colin::Elements e = viewPortSettings::instance().elements(id);
            if(e.testFlag(Colin::N))
                e &= ~Colin::N;
            else
                e |= Colin::N;
            viewPortSettings::instance().setElements(id, e);
        }
        else if(qR.contains(e->pos()))
        {
            Colin::Elements e = viewPortSettings::instance().elements(id);
            if(e.testFlag(Colin::Q))
                e &= ~Colin::Q;
            else
                e |= Colin::Q;
            viewPortSettings::instance().setElements(id, e);
        }
        else if(mR.contains(e->pos()))
        {
            Colin::Elements e = viewPortSettings::instance().elements(id);
            if(e.testFlag(Colin::M))
                e &= ~Colin::M;
            else
                e |= Colin::M;
            viewPortSettings::instance().setElements(id, e);
        }
        else if(bR.contains(e->pos()))
        {
            Colin::Elements e = viewPortSettings::instance().elements(id);
            if(e.testFlag(Colin::bearing))
                e &= ~Colin::bearing;
            else
                e |= Colin::bearing;
            viewPortSettings::instance().setElements(id, e);
        }
        else if(jR.contains(e->pos()))
        {
            Colin::Elements e = viewPortSettings::instance().elements(id);
            if(e.testFlag(Colin::joint))
                e &= ~Colin::joint;
            else
                e |= Colin::joint;
            viewPortSettings::instance().setElements(id, e);
        }
        else if(kR.contains(e->pos()))
        {
            Colin::Elements e = viewPortSettings::instance().elements(id);
            if(e.testFlag(Colin::node))
                e &= ~Colin::node;
            else
                e |= Colin::node;
            viewPortSettings::instance().setElements(id, e);
        }
        else if(lR.contains(e->pos()))
        {
            Colin::Elements e = viewPortSettings::instance().elements(id);
            if(e.testFlag(Colin::nload))
                e &= ~Colin::nload;
            else
                e |= Colin::nload;
            viewPortSettings::instance().setElements(id, e);
        }
    }
    mousePressed = false;
    repaint();
}

void drawSelectionWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    QFont f = font();
    f.setPointSize(f.pointSize()*fontscale);
    p.setFont(f);
	QPoint p0(offset(), offset());
    QSize boundingSize(p.font().pointSize()*1.5, p.font().pointSize()*2);


	this->drawBackground(&p);

	bool mouseover, checked;
    if(viewPortSettings::instance().elements(id).testFlag(Colin::u) || mouseOver)
    {
		checked = viewPortSettings::instance().elements(id).testFlag(Colin::u);

		uR = QRect(p0, boundingSize);

		mouseover = uR.contains(mapFromGlobal(QCursor::pos()));


		if(checked)
			p.setPen(viewPortSettings::instance().color(Colin::C_Beam));
		else
			p.setPen(palette().color(QPalette::Dark));

		drawRect(&p, uR, mousePressed && mouseover, checked, mouseover);
		p.drawText(uR, tr("u"), QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));

		p0+=QPoint(boundingSize.width()+offset(), 0);
	}
	if(viewPortSettings::instance().elements(id).testFlag(Colin::N) || mouseOver)
	{
		checked = viewPortSettings::instance().elements(id).testFlag(Colin::N);

		nR = QRect(p0, boundingSize);

		mouseover = nR.contains(mapFromGlobal(QCursor::pos()));


		if(checked)
			p.setPen(viewPortSettings::instance().color(Colin::C_Np));
		else
			p.setPen(palette().color(QPalette::Dark));

		drawRect(&p, nR, mousePressed && mouseover, checked, mouseover);
		p.drawText(nR, tr("N"), QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));

		p0+=QPoint(boundingSize.width()+offset(), 0);
	}
	if(viewPortSettings::instance().elements(id).testFlag(Colin::Q) || mouseOver)
	{
		checked = viewPortSettings::instance().elements(id).testFlag(Colin::Q);

		qR = QRect(p0, boundingSize);

		mouseover = qR.contains(mapFromGlobal(QCursor::pos()));


		if(checked)
			p.setPen(viewPortSettings::instance().color(Colin::C_Qp));
		else
			p.setPen(palette().color(QPalette::Dark));

		drawRect(&p, qR, mousePressed && mouseover, checked, mouseover);
		p.drawText(qR, tr("Q"), QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));

		p0+=QPoint(boundingSize.width()+offset(), 0);
	}
	if(viewPortSettings::instance().elements(id).testFlag(Colin::M) || mouseOver)
	{
		checked = viewPortSettings::instance().elements(id).testFlag(Colin::M);

		mR = QRect(p0, boundingSize);

		mouseover = mR.contains(mapFromGlobal(QCursor::pos()));


		if(checked)
			p.setPen(viewPortSettings::instance().color(Colin::C_Mp));
		else
			p.setPen(palette().color(QPalette::Dark));

		drawRect(&p, mR, mousePressed && mouseover, checked, mouseover);
		p.drawText(mR, tr("M"), QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));

		p0+=QPoint(boundingSize.width()+offset(), 0);
	}



    if(p0.x() < 5) // nothing selected ... draw something so it still can pop up to select
    {
		drawRect(&p, QRect(p0, boundingSize), false, true);
	}



    p0 = QPoint(4, 8 + boundingSize.height());


    if(!mouseOver)
        return;



	//node visible
	kR = QRect(p0, boundingSize);

	mouseover = kR.contains(mapFromGlobal(QCursor::pos()));
	checked = viewPortSettings::instance().elements(id).testFlag(Colin::node);
	if(checked)
		p.setPen(palette().color(QPalette::Highlight));
	else
		p.setPen(palette().color(QPalette::Dark));

	drawRect(&p, kR, mousePressed && mouseover, checked, mouseover);
	colinIcons::instance().icon(Colin::drawNode).paint(&p, kR, Qt::AlignCenter);


	p0+=QPoint(boundingSize.width()+offset(), 0);

	//loads visible
	lR = QRect(p0, boundingSize);

	mouseover = lR.contains(mapFromGlobal(QCursor::pos()));
	checked = viewPortSettings::instance().elements(id).testFlag(Colin::nload);
	if(checked)
		p.setPen(palette().color(QPalette::Highlight));
	else
		p.setPen(palette().color(QPalette::Dark));

	drawRect(&p, lR, mousePressed && mouseover, checked, mouseover);
	colinIcons::instance().icon(Colin::drawNLoad).paint(&p, lR, Qt::AlignCenter);
	p0+=QPoint(boundingSize.width()+offset(), 0);

	//bearings visible
	bR = QRect(p0, boundingSize);

	mouseover = bR.contains(mapFromGlobal(QCursor::pos()));
	checked = viewPortSettings::instance().elements(id).testFlag(Colin::bearing);
	if(checked)
		p.setPen(palette().color(QPalette::Highlight));
	else
		p.setPen(palette().color(QPalette::Dark));

	drawRect(&p, bR, mousePressed && mouseover, checked, mouseover);

     colinIcons::instance().icon(Colin::drawBearing).paint(&p, bR, Qt::AlignCenter);
	 p0+=QPoint(boundingSize.width()+offset(), 0);


	//joints visible
	jR = QRect(p0, boundingSize);

	mouseover = jR.contains(mapFromGlobal(QCursor::pos()));
	checked = viewPortSettings::instance().elements(id).testFlag(Colin::joint);
	if(checked)
		p.setPen(palette().color(QPalette::Highlight));
	else
		p.setPen(palette().color(QPalette::Dark));

	drawRect(&p, jR, mousePressed && mouseover, checked, mouseover);
	colinIcons::instance().icon(Colin::drawJoint).paint(&p, jR, Qt::AlignCenter);
	p0+=QPoint(boundingSize.width()+offset(), 0);



    /*
    u =         0x1,
    N =         0x2,
    Q =         0x4,
    M =         0x8,

    node =      0x10,
    beam =      0x20,
    bearing =   0x40,
    joint =     0x80,
    nload =     0x100,
    sload =     0x200,
    temp =      0x400,
    deform =    0x800,
    */
}



QSize drawSelectionWidget::sizeHint() const
{
    QSize boundingSize(this->font().pointSize()*1.5*fontscale, this->font().pointSize()*2*fontscale);

    if(mouseOver)
    {
	return QSize(boundingSize.width()*4+5*offset(),
			 boundingSize.height()*2+3*offset());
    }
    else
    {
	int rectcount = 0;
	if(viewPortSettings::instance().elements(id).testFlag(Colin::u))
	    rectcount++;
	if(viewPortSettings::instance().elements(id).testFlag(Colin::N))
	    rectcount++;
	if(viewPortSettings::instance().elements(id).testFlag(Colin::Q))
	    rectcount++;
	if(viewPortSettings::instance().elements(id).testFlag(Colin::M))
	    rectcount++;
	if(!rectcount)
	    rectcount++;
	return QSize(boundingSize.width()*rectcount+offset()*(rectcount+1),
			 boundingSize.height()+offset()*2);

    }
}
