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

#include "viewportpresenter.h"
#include "viewportsettings.h"

#include <QtGui/QCleanlooksStyle>

viewPortPresenter::viewPortPresenter(cWidget *c, QWidget *parent) :
    QWidget(parent)
{
    central = c;

    setMouseTracking(true);

	rubber = new ColinRubberBand(QRubberBand::Rectangle, this);
	rubber->setGeometry(-1, -1, 0, 0);

    mouseRect = QRect(-1, -1, 0, 0);

    connect(c,                                  SIGNAL(viewChanged()),
            this,                               SLOT(repaint()));

    connect(&viewPortSettings::instance(),      SIGNAL(settingschanged()),
            this,                               SLOT(repaint()));
}


void viewPortPresenter::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    const int rowmax = central->rowmax();
    const int rw = width()/rowmax;
    const int wcount = central->viewCount();
    /*const int rh = (rowmax*(rowmax-1)>=(wcount))?
                   height()/(rowmax-1):
                   height()/rowmax;*/

    QColor cg1 = palette().color(QPalette::Light);
    QColor cg2 = palette().color(QPalette::Dark);
    QRadialGradient grad(rw/2, -rw/4, rw*5/4);
    grad.setColorAt(0, cg1);
    grad.setColorAt(1, cg2);

    p.setBrush(QBrush(grad));

    QRadialGradient grad2(grad);
    grad2.setColorAt(0, cg1.lighter(130));
    grad2.setColorAt(1, cg2.lighter(130));


    QStyleOptionMenuItem opt;
    opt.initFrom(this);
    opt.state = QStyle::State_Enabled   |
                QStyle::State_MouseOver |
                QStyle::State_Active    |
                QStyle::State_Selected;
    opt.rect = QRect(0, 0, rw-1, rw-1);

    QFont f = font();
    f.setPointSize(f.pointSize()*2);
    p.setFont(f);
    for(int i=0; i<wcount; )
    {
        for(int j=0; j<rowmax; j++)
        {
            if(p.transform().mapRect(QRect(7, 7, rw-14, rw-14)).intersects(mouseRect))
            {
                style()->drawControl(QStyle::CE_MenuItem, &opt, &p, this);
            }
            if(central->isVisible(i))
            {
                p.setBrush(QBrush(grad));
            }
            else
            {
                p.setBrush(QBrush(grad2));
            }

            p.drawRect(QRect(7, 7, rw-14, rw-14));
            p.save();
            int count(0);
            if(viewPortSettings::instance().elements(i).testFlag(Colin::u)) count++;
            if(viewPortSettings::instance().elements(i).testFlag(Colin::N)) count++;
            if(viewPortSettings::instance().elements(i).testFlag(Colin::Q)) count++;
            if(viewPortSettings::instance().elements(i).testFlag(Colin::M)) count++;
            if(count)
            {
                int symbolWidth = (rw-14-4)/count;
                if(viewPortSettings::instance().elements(i).testFlag(Colin::u))
                {
                    p.setPen(viewPortSettings::instance().color(Colin::C_Beam));
                    p.drawText(QRectF(7, rw/2-f.pointSize()-2, symbolWidth, f.pointSize()+4),
                               "u",
                               Qt::AlignHCenter | Qt::AlignVCenter);
                    p.translate(symbolWidth, 0);
                }
                if(viewPortSettings::instance().elements(i).testFlag(Colin::N))
                {
                    p.setPen(viewPortSettings::instance().color(Colin::C_Np));
                    p.drawText(QRectF(7, rw/2-f.pointSize()-2, symbolWidth, f.pointSize()+4),
                               "N",
                               Qt::AlignHCenter | Qt::AlignVCenter);
                    p.translate(symbolWidth, 0);
                }
                if(viewPortSettings::instance().elements(i).testFlag(Colin::Q))
                {
                    p.setPen(viewPortSettings::instance().color(Colin::C_Qp));
                    p.drawText(QRectF(7, rw/2-f.pointSize()-2, symbolWidth, f.pointSize()+4),
                               "Q",
                               Qt::AlignHCenter | Qt::AlignVCenter);
                    p.translate(symbolWidth, 0);
                }
                if(viewPortSettings::instance().elements(i).testFlag(Colin::M))
                {
                    p.setPen(viewPortSettings::instance().color(Colin::C_Mp));
                    p.drawText(QRectF(7, rw/2-f.pointSize()-2, symbolWidth, f.pointSize()+4),
                               "M",
                               Qt::AlignHCenter | Qt::AlignVCenter);
                    p.translate(symbolWidth, 0);
                }
            }
            p.restore();
            p.translate(rw, 0);
            i++;

            if(i>=wcount)
                break;
        }
        p.translate(-rw*(rowmax), rw);
    }
/*
    QLinearGradient gradm(mouseRect.topLeft(), mouseRect.bottomRight());
    gradm.setColorAt(0, QColor(255, 138, 0, 100));
    gradm.setColorAt(1, QColor(120, 52, 75, 100));
    p.setPen(QColor(255, 138, 0));
    p.setBrush(QBrush(gradm));
    p.resetTransform();
    p.drawRect(mouseRect);
*/
	if(rowmax*rowmax > wcount)
    {
        QIcon node(":/icons/node_128.png");
        node.paint(&p, QRect(width()-rw+7, height()-rw+7, rw-14, rw-14));
    }
}

void viewPortPresenter::mousePressEvent(QMouseEvent *e)
{
    mouseRect = QRect(e->pos().x()-1, e->pos().y()-1, 1, 1);
	rubber->show();
	rubber->move((e->pos()));
}

void viewPortPresenter::mouseMoveEvent(QMouseEvent *e)
{
    if(mouseRect.x()<0)
        return;

    mouseRect = QRect(mouseRect.x(), mouseRect.y(),
                      e->pos().x()-mouseRect.x(),
                      e->pos().y()-mouseRect.y());
	rubber->setGeometry((mouseRect.normalized().topLeft()).x(),
						(mouseRect.normalized().topLeft()).y(),
						mouseRect.normalized().width(),
						mouseRect.normalized().height());
	repaint();
}

void viewPortPresenter::mouseReleaseEvent(QMouseEvent *)
{
	rubber->hide();
    const int rowmax = central->rowmax();
    const int rw = width()/rowmax;
    const int wcount = central->viewCount();
    QTransform t;
    QByteArray vis(12, false);
    for(int i=0; i<wcount; )
    {
        for(int j=0; j<rowmax; j++)
        {
            if(t.mapRect(QRect(7, 7, rw-14, rw-14)).intersects(mouseRect))
            {
                vis[i]=true;
            }
            t.translate(rw, 0);
            i++;
            if(i>=wcount) break;
        }
        t.translate(-rw*(rowmax), rw);
    }
    central->setVisibleViews(vis);
    mouseRect = QRect(-1, -1, 0, 0);
    static_cast<QWidget*>(parent())->hide();
}


