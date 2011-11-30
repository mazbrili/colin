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

#include "colinlib.h"

#include <QtCore/QDebug>

ColinLibrary *ColinLibrary::instance_ = NULL;

ColinLibrary::ColinLibrary()
{
	qDebug() << "creating library instance";

    connect(qApp,           SIGNAL(aboutToQuit()),
            this,           SLOT(dump()));

    QSettings settings("clazzes.org", "Colin");

    bool ok;
    int matCount = settings.value("lib/matCount", QVariant(int(1))).toInt(&ok);
	if(!ok || matCount == 0)
        matCount = 1;
    for(int i=0; i<matCount; i++)
    {
        QString name = settings.value("lib/mat"+QString::number(i)+"_name", QVariant(QString("IDontCare"))).toString();
        double E = settings.value("lib/mat"+QString::number(i)+"_E", QVariant(double(2e+11))).toDouble(&ok);
        if(!ok)
            break;
        double alphaT = settings.value("lib/mat"+QString::number(i)+"_alphaT", QVariant(double(1e-05))).toDouble(&ok);
        if(!ok)
            break;
        this->addMat(ColinMaterial(E, alphaT, name));
    }

    int profileCount = settings.value("lib/profileCount", QVariant(int(1))).toInt(&ok);
	if(!ok || profileCount == 0)
        profileCount = 1;
    for(int i=0; i<matCount; i++)
    {
        QString name = settings.value("lib/profile"+QString::number(i)+"_name", QVariant(QString("IDontCare"))).toString();
        double A = settings.value("lib/profile"+QString::number(i)+"_A", QVariant(double(1e-02))).toDouble(&ok);
        if(!ok)
            break;
        double I = settings.value("lib/profile"+QString::number(i)+"_I", QVariant(double(1e-04))).toDouble(&ok);
        if(!ok)
            break;
        double h = settings.value("lib/profile"+QString::number(i)+"_h", QVariant(double(1e-01))).toDouble(&ok);
        if(!ok)
            break;
        this->addProfile(ColinCrossSection(A, I, h, name));

        currentM = 0;
        currentP = 0;

    }

	qDebug() << profiles.count() << " cross sections";
	qDebug() << mats.count() << " materials";
}


void ColinLibrary::addProfile(const ColinCrossSection &n)
{
    profiles.append(n);
    emit profileAdded();
}
void ColinLibrary::editProfile(const int &nr, const ColinCrossSection &n)
{
     Q_ASSERT(!(nr<0 || nr>=profiles_n()));
     profiles[nr]=n;
     emit profileEdited(nr);
}
void ColinLibrary::removeProfile(const int &nr)
{
    Q_ASSERT(!(nr<0 || nr>=profiles_n()));
    profiles.removeAt(nr);
    emit profileRemoved(nr);
}

void ColinLibrary::addMat(const ColinMaterial &n)
{
    mats.append(n);
    emit matAdded();
}
void ColinLibrary::editMat(const int &nr, const ColinMaterial &n)
{
    Q_ASSERT(!(nr<0 || nr>=profiles_n()));
    mats[nr]=n;
    emit matEdited(nr);
}
void ColinLibrary::removeMat(const int &nr)
{
    Q_ASSERT(!(nr<0 || nr>=profiles_n()));
    mats.removeAt(nr);
    emit matRemoved(nr);
}

void ColinLibrary::dump()
{

    QSettings settings("clazzes.org", "Colin");
    settings.remove("lib");
    settings.setValue("lib/matCount", mats_n());
    settings.setValue("lib/profileCount", profiles_n());
    for(int i=0; i<mats_n(); i++)
    {
        settings.setValue("lib/mat"+QString::number(i)+"_name", mat(i).name());
        settings.setValue("lib/mat"+QString::number(i)+"_E", mat(i).E());
        settings.setValue("lib/mat"+QString::number(i)+"_alphaT", mat(i).alphaT());
    }

    for(int i=0; i<profiles_n(); i++)
    {
        settings.setValue("lib/profile"+QString::number(i)+"_name", Profile(i).name());
        settings.setValue("lib/profile"+QString::number(i)+"_A", Profile(i).A());
        settings.setValue("lib/profile"+QString::number(i)+"_I", Profile(i).I());
        settings.setValue("lib/profile"+QString::number(i)+"_h", Profile(i).h());
    }
}

void ColinLibrary::setUp()
{
    if(profiles_n()<1)
	addProfile(ColinCrossSection(1e-02, 1e-04, 1e-01, "IDontCare"));
    if(mats_n()<1)
	addMat(ColinMaterial(2e+11, 1e-05, "IDontCare"));
}
