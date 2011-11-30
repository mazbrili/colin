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

#include "unitsettings.h"


#include <QtCore/QSettings>

unitSettings* unitSettings::instance_ = 0;

QString unitSettings::Eeh() const
{
    switch(EUnit)
    {
    case N_m2:
        return QString("N/m%1").arg(QChar(0x00B2));
    case kN_m2:
        return QString("kN/m%1").arg(QChar(0x00B2));
    case kN_cm2:
        return QString("kN/cm%1").arg(QChar(0x00B2));
    case N_cm2:
        return QString("N/cm%1").arg(QChar(0x00B2));
    default:
        return QString();
    }
}

QString unitSettings::Feh() const
{
    switch(EUnit)
    {
    case N_m2:
        return QString("N/m");
    case kN_m2:
        return QString("kN/m");
    case kN_cm2:
        return QString("kN/cm");
    case N_cm2:
        return QString("N/cm");
    default:
        return QString();
    }
}

QString unitSettings::FMeh() const
{
    switch(EUnit)
    {
    case N_m2:
        return QString("N/rad");
    case kN_m2:
        return QString("kN/rad");
    case kN_cm2:
        return QString("kN/rad");
    case N_cm2:
        return QString("N/rad");
    default:
        return QString();
    }
}

QString unitSettings::Peh() const
{
    switch(PMUnit)
    {
    case Nm:
    case Ncm:
        return QString("N");
    case kNm:
    case kNcm:
        return QString("kN");
    default:
        return QString("");
    }
}

QString unitSettings::Meh() const
{
    switch(PMUnit)
    {
    case Nm:
        return QString("Nm");
    case Ncm:
        return QString("Ncm");
    case kNm:
        return QString("kNm");
    case kNcm:
        return QString("kNcm");
    default:
        return QString("");
    }
}

QString unitSettings::Profeh() const
{
    switch(ProfUnit)
    {
    case m:
        return QString("m");
    case cm:
        return QString("cm");
    case mm:
        return QString("mm");
    default:
        return QString();
    }
}

double unitSettings::Eprefix() const
{
    switch(EUnit)
    {
    case N_m2:
        return 1;
    case kN_m2:
        return 0.001;
    case kN_cm2:
        return 10;
    case N_cm2:
        return 10000;
    default:
        return 1;
    }
}

double unitSettings::Fprefix() const
{
    switch(EUnit)
    {
    case N_m2:
        return 1;
    case kN_m2:
        return 0.001;
    case kN_cm2:
        return 0.1;
    case N_cm2:
        return 100;
    default:
        return 1;
    }
}

double unitSettings::FMprefix() const
{
    switch(EUnit)
    {
    case N_m2:
        return 1;
    case kN_m2:
        return 0.001;
    case kN_cm2:
        return 0.001;
    case N_cm2:
        return 1;
    default:
        return 1;
    }
}

double unitSettings::Pprefix() const
{
    switch(PMUnit)
    {
    case Nm:
    case Ncm:
        return 1;
    case kNm:
    case kNcm:
        return 0.001;
    default:
        return 1;
    }
}

double unitSettings::Mprefix() const
{
    switch(PMUnit)
    {
    case Nm:
        return 1;
    case Ncm:
        return 0.01;
    case kNm:
        return 0.001;
    case kNcm:
        return 0.00001;
    default:
        return 1;
    }
}

double unitSettings::Profprefix() const
{
    switch(ProfUnit)
    {
    case m:
        return 1;
    case cm:
        return 100;
    case mm:
        return 1000;
    default:
        return 1;
    }
}


void unitSettings::loadSettings()
{
    QSettings settings("clazzes.org", "Colin", this);
    EUnit = static_cast<EEH>(settings.value("EH/EUnit", 1).toInt());
    ProfUnit = static_cast<ProfEH>(settings.value("EH/ProfUnit", 1).toInt());
    PMUnit = static_cast<PMEH >(settings.value("EH/PMUnit", 1).toInt());
    prec = settings.value("EH/precison", 3).toInt();
}

void unitSettings::saveSettings()
{
    QSettings settings("clazzes.org", "Colin", this);
    settings.setValue("EH/EUnit", static_cast<int>(EUnit));
    settings.setValue("EH/ProfUnit", static_cast<int>(ProfUnit));
    settings.setValue("EH/PMUnit", static_cast<int>(PMUnit));
    settings.setValue("EH/precison", prec);
}

void unitSettings::setPEh(unitSettings::PMEH eh)
{
    PMUnit = eh;
    emit settingschanged();
}

void unitSettings::setEEh(unitSettings::EEH eh)
{
    EUnit = eh;
    emit settingschanged();
}

void unitSettings::setIEh(unitSettings::ProfEH eh)
{
    ProfUnit = eh;
    emit settingschanged();
}


void unitSettings::setPrecison(const int &p)
{
    prec = p;
    emit settingschanged();
}
