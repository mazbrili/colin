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

#include "nwidget.h"
#include "colinicons.h"

nWidget::nWidget(QWidget *parent) :
    QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    QHBoxLayout *buttonbox = new QHBoxLayout();

    newB = new QPushButton(tr("new"), this);
    openB = new QPushButton(tr("open"), this);
    settingsB = new QPushButton(tr("settings"), this);
    libB = new QPushButton(tr("clear library"), this);


    //layout->setColumnMinimumWidth(0, 20);
    layout->addLayout(buttonbox, 0, 0, 1, 6, Qt::AlignTop);
    buttonbox->addWidget(newB);
    buttonbox->addWidget(openB);
    buttonbox->addWidget(settingsB);
    buttonbox->addWidget(libB);
    //layout->setColumnMinimumWidth(5, 20);
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 3);
    layout->setColumnStretch(2, 3);
    layout->setColumnStretch(3, 3);
    layout->setColumnStretch(4, 3);
    layout->setColumnStretch(5, 1);
    layout->setRowStretch(0, 1);
    layout->setRowStretch(1, 1);
    layout->setRowStretch(2, 3);
    layout->setRowStretch(3, 3);
    layout->setRowMinimumHeight(1, 20);



    newB->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    openB->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    settingsB->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    libB->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);



    renderer = new previewRenderer(QSize(196, 135), this);
    previewWidget::renderer = renderer;


    for(int i=0; i<8; i++)
    {
        previews[i] = new previewWidget(this);

        connect(&filelist::instance(),      SIGNAL(recUsedCanged()),
                previews[i],                SLOT(actualUrl()));

        connect(renderer,                   SIGNAL(finished(QString,QImage)),
                previews[i],                SLOT(giefPix(QString,QImage)));


	layout->addWidget(previews[i], 2+i/4, 1+i%4, 1, 1);
    }

    layout->setRowMinimumHeight(4, 140);

    colinicon = QPixmap(colinIcons::icondir_+"/tooltip/node_dark_64.png");
#ifdef CLAZZES_DOT_ORG_ICON
    clazzesicon = QPixmap(colinIcons::icondir_+"clazzes-logo-alpha_64x64.png");
#endif

}

void nWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(Qt::NoPen);

    //QRect rect;
    //rect.setTopLeft(newB->pos()-QPoint(10, 10));
    //rect.setBottomRight(libB->pos()+libB->rect().bottomRight()+QPoint(10, 10));

    //QLinearGradient grad(rect.topLeft(), rect.bottomLeft());
    //QColor gc = palette().color(QPalette::Mid);
    //grad.setColorAt(0, gc);
    //gc.setAlpha(50);
    //grad.setColorAt(1, gc);
    //p.setBrush(QBrush(grad));

    //p.drawRoundedRect(rect, 5, 5);

    p.drawPixmap(QRect(15, height()-15-64, 64, 64), colinicon);

    QRect br;
    QFont f(p.font());
    f.setPixelSize(40);
    p.setFont(f);
    QLinearGradient grad;
    grad.setStart(0, height()-15-128);
    grad.setFinalStop(0, height()-15);
    grad.setColorAt(0, QColor(30, 30, 30));
    grad.setColorAt(1, QColor(90, 90, 90));
    p.setPen(QPen(QBrush(grad), 2));

    p.drawText(QRect(15+64, height()-15-64,  width(), 64),
               Qt::AlignVCenter | Qt::AlignLeft,
               "[Colin]", &br);

    QPoint middle = br.topRight();

    const double polyCount = 36;
    const double innerRad = 23;
    const double outerRad = 26;
    QPolygon poly(polyCount);
    for(int i=0; i<polyCount; i++)
    {
        poly.setPoint(i, QPoint(cos(M_PI*2/polyCount*i)*((i%2==1)?innerRad:outerRad), sin(M_PI*2/polyCount*i)*((i%2==1)?innerRad:outerRad))+middle);
    }
    p.setPen(QPen(p.pen().brush(), 1));
    f.setPixelSize(23);
    p.setFont(f);
    p.setBrush(QColor(255, 200, 0));
    p.drawPolygon(poly);

    //p.save();
    //p.setPen(QPen(this->palette().color(QPalette::Window), 1));
    //p.setBrush(QColor(88, 139, 241));
    //p.drawPolygon(poly);
    //p.restore();
    //f.setPixelSize(23);
    //p.setFont(f);

    p.drawText(poly.boundingRect(), QString(QChar(0x03B2)), Qt::AlignHCenter | Qt::AlignVCenter);


    f.setPixelSize(40);
    p.setFont(f);

#ifdef CLAZZES_DOT_ORG_ICON
    p.drawPixmap(width()-15-64, height()-15-64, 64, 64, clazzesicon);
    p.drawText(QRect(width()-15-64-500-10, height()-15-64, 500, 64),
                     Qt::AlignVCenter | Qt::AlignRight,
                     "clazzes.org");
#endif



}
