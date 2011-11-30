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

#include "viewmenu.h"

const int absoluteMax = 0x40000000;

const int MaxPxl = 100;

 viewMenu:: viewMenu(QWidget *parent)
    : ColinMenu(parent)
{
    init();
}

 viewMenu:: viewMenu(const QString &title, QWidget *parent)
    : ColinMenu(title, parent)
{
    init();
}

void viewMenu::init()
{
    QAction *a;

    t = 0;
    addSeparator();
    a = addAction(QString("u [m%1m]").arg(QChar(0x2259)));
    uScale = new scaleWidget(this);
    uScale->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    addWidgetForAction(a, uScale);

    addSeparator();
    a = addAction(QString("P [N%1m]").arg(QChar(0x2259)));
    pScale = new scaleWidget(this);
    pScale->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    addWidgetForAction(a, pScale);

    addSeparator();
    a = addAction(QString("M [Nm%1m]").arg(QChar(0x2259)));
    mScale = new scaleWidget(this);
    mScale->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    addWidgetForAction(a, mScale);

    uScale->slider->setMinimum(0);
    pScale->slider->setMinimum(0);
    mScale->slider->setMinimum(0);
    uScale->slider->setMaximum(1000);
    pScale->slider->setMaximum(1000);
    mScale->slider->setMaximum(1000);

    uScale->slider->setTracking(false);
    pScale->slider->setTracking(false);
    mScale->slider->setTracking(false);

    connect(&filelist::instance(),                  SIGNAL(currentChanged(ColinStruct*)),
            this,                                   SLOT(setTw(ColinStruct*)));


    connect(uScale->slider,                         SIGNAL(sliderMoved(int)),
            this,                                   SLOT(scaleU(int)));

    connect(pScale->slider,                         SIGNAL(sliderMoved(int)),
            this,                                   SLOT(scaleP(int)));

    connect(mScale->slider,                         SIGNAL(sliderMoved(int)),
            this,                                   SLOT(scaleM(int)));

    connect(uScale->slider,                         SIGNAL(sliderReleased()),
            this,                                   SLOT(setRanges()));

    connect(pScale->slider,                         SIGNAL(sliderReleased()),
            this,                                   SLOT(setRanges()));

    connect(mScale->slider,                         SIGNAL(sliderReleased()),
            this,                                   SLOT(setRanges()));

    connect(uScale->down,                           SIGNAL(clicked()),
            this,                                   SLOT(scaleDownU()));

    connect(pScale->down,                           SIGNAL(clicked()),
            this,                                   SLOT(scaleDownP()));

    connect(mScale->down,                           SIGNAL(clicked()),
            this,                                   SLOT(scaleDownM()));

    connect(uScale->up,                             SIGNAL(clicked()),
            this,                                   SLOT(scaleUpU()));

    connect(pScale->up,                             SIGNAL(clicked()),
            this,                                   SLOT(scaleUpP()));

    connect(mScale->up,                             SIGNAL(clicked()),
            this,                                   SLOT(scaleUpM()));

    connect(uScale->restore,                        SIGNAL(clicked()),
            this,                                   SLOT(autoScaleU()));

    connect(pScale->restore,                        SIGNAL(clicked()),
            this,                                   SLOT(autoScaleP()));

    connect(mScale->restore,                        SIGNAL(clicked()),
            this,                                   SLOT(autoScaleM()));

    connect(this,                                   SIGNAL(aboutToShow()),
            this,                                   SLOT(setRanges()));


    adjust = true;
}

void viewMenu::scaleU(const double &scale)
{
    t->blockSignals(true);
    uScale->slider->setValue(scale);
    t->blockSignals(false);
}

void viewMenu::scaleP(const double &scale)
{
    t->blockSignals(true);
    pScale->slider->setValue(scale*1000);
    t->blockSignals(false);
}

void viewMenu::scaleM(const double &scale)
{
    t->blockSignals(true);
    mScale->slider->setValue(scale*1000);
    t->blockSignals(false);
}

void viewMenu::dumpTw(QObject *o)
{
    if(t == o)
    {
        t = 0;
    }
}

void viewMenu::setTw(ColinStruct *tw)
{
    if(t)
    {
	t->disconnect(this,            SLOT(scaleU(double)));
	t->disconnect(this,            SLOT(scaleP(double)));
	t->disconnect(this,            SLOT(scaleM(double)));

	t->disconnect(this,            SLOT(autoScaleU()));
	t->disconnect(this,            SLOT(autoScaleP()));
	t->disconnect(this,            SLOT(autoScaleM()));
    }
    t=tw;
    if(tw == 0)
    {
        uScale->setDisabled(true);
        mScale->setDisabled(true);
        pScale->setDisabled(true);
        return;
    }
    uScale->setDisabled(false);
    mScale->setDisabled(false);
    pScale->setDisabled(false);


    connect(tw,                 SIGNAL(destroyed(QObject*)),
            this,               SLOT(dumpTw(QObject*)));

    connect(tw,                 SIGNAL(changedUScale(double)),
            this,               SLOT(scaleU(double)));

    connect(tw,                 SIGNAL(changedMScale(double)),
            this,               SLOT(scaleM(double)));

    connect(tw,                 SIGNAL(changedPScale(double)),
            this,               SLOT(scaleP(double)));

    connect(tw,                 SIGNAL(calculationFinished()),
            this,               SLOT(autoScaleM()));

    connect(tw,                 SIGNAL(calculationFinished()),
            this,               SLOT(autoScaleP()));

    connect(tw,                 SIGNAL(calculationFinished()),
            this,               SLOT(autoScaleU()));




    scaleU(tw->scaleU());
    scaleM(tw->scaleM());
    scaleP(tw->scaleP());
    setRanges();

}


void viewMenu::scaleU(const int &scale)
{
    if(t!=0)
    {
        blockSignals(true);
        t->setScaleU(scale);
        blockSignals(false);
    }
}

void viewMenu::scaleP(const int &scale)
{
    if(t!=0)
    {
        blockSignals(true);
        t->setScaleP(scale/1000.);
        blockSignals(false);
    }
}

void viewMenu::scaleM(const int &scale)
{
    if(t!=0)
    {
        blockSignals(true);
        t->setScaleM(scale/1000.);
        blockSignals(false);
    }
}

void viewMenu::scaleDownU()
{
    t->setScaleU(t->scaleU()/1.2);
}

void viewMenu::scaleUpU()
{
    t->setScaleU(t->scaleU()*1.2);
    if(t->scaleU()>uScale->slider->maximum()*4./5.)
        setRanges();
}

void viewMenu::scaleDownP()
{
    t->setScaleP(t->scaleP()/1.2);
}

void viewMenu::scaleUpP()
{
    t->setScaleP(t->scaleP()*1.2);
    if(t->scaleP()>pScale->slider->maximum()*4000./5.)
        setRanges();
}

void viewMenu::scaleDownM()
{
    t->setScaleM(t->scaleM()/1.2);
}

void viewMenu::scaleUpM()
{
    t->setScaleM(t->scaleM()*1.2);
    if(t->scaleM()>mScale->slider->maximum()*4000./5.)
        setRanges();
}

void viewMenu::setRanges()
{
    if(t)t->blockSignals(true);
    QSlider *send;
    for(int i=0; i<3; i++)
    {
        if(i==0) send = uScale->slider;
        else if(i==1) send = pScale->slider;
        else send = mScale->slider;
        send->blockSignals(true);
        send->setMaximum(qMax(send->value()*2., 50.));
        send->blockSignals(false);
    }
    if(t)t->blockSignals(false);
}


void viewMenu::autoScaleU()
{
    double uMax = t->uMax();
    double scaleMax = 0;
    for(int i=0; i<viewPortSettings::instance().widgetCount(); i++)
    {
        scaleMax = qMax(scaleMax, t->view(i).m11());
    }
    uScale->slider->blockSignals(true);
    t->setScaleU(MaxPxl/scaleMax/uMax);
    uScale->slider->setMaximum(t->scaleU()*2);
    uScale->slider->blockSignals(false);
}

void viewMenu::autoScaleP()
{
    double pMax = t->pMax();
    double scaleMax = 0;
    for(int i=0; i<viewPortSettings::instance().widgetCount(); i++)
    {
        scaleMax = qMax(scaleMax, t->view(i).m11());
    }
    pScale->slider->blockSignals(true);
    t->setScaleP(MaxPxl/scaleMax/pMax);
    pScale->slider->setMaximum(t->scaleP()*2000);
    pScale->slider->blockSignals(false);
}

void viewMenu::autoScaleM()
{
    double mMax = t->mMax();
    double scaleMax = 0;

    for(int i=0; i<viewPortSettings::instance().widgetCount(); i++)
    {
        scaleMax = qMax(scaleMax, t->view(i).m11());
    }
    mScale->slider->blockSignals(true);
    t->setScaleM(MaxPxl/scaleMax/mMax);
    mScale->slider->setMaximum(t->scaleM()*2000);
    mScale->slider->blockSignals(false);
}
