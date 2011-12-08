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

#ifndef UNITSETTINGS_H
#define UNITSETTINGS_H

#include <QtCore/QObject>
#include <cmath>


#define EPREFIX unitSettings::instance().Eprefix()
#define FPREFIX unitSettings::instance().Fprefix()
#define FMPREFIX unitSettings::instance().FMprefix()
#define APREFIX unitSettings::instance().Aprefix()
#define IPREFIX unitSettings::instance().Iprefix()
#define HPREFIX unitSettings::instance().hprefix()
#define PPREFIX unitSettings::instance().Pprefix()
#define MPREFIX unitSettings::instance().Mprefix()
#define ANGLEPREFIX (180/M_PI)
#define PRECISON unitSettings::instance().precison()
#define UNIT unitSettings::instance()


class unitSettings : public QObject
{
    Q_OBJECT
public:
    enum EEH
    {
        N_m2 = 1,
        kN_m2,
        kN_cm2,
        N_cm2
    };

    enum ProfEH
    {
        m = 1,
        cm,
        mm
    };

    enum PMEH
    {
        Nm = 1,
        Ncm,
        kNm,
        kNcm
    };

    static unitSettings& instance()
    {
        if(!instance_)
            instance_ = new unitSettings();
        return *instance_;
    }
    inline const EEH &EU(){return EUnit;}
    inline const ProfEH &IU(){return ProfUnit;}
    inline const PMEH &PU(){return PMUnit;}

    QString Eeh() const;
    QString Feh() const;
    QString FMeh() const;
    QString Peh() const;
    QString Meh() const;
    inline QString Aeh() const {return Profeh()+QChar(0x00B2);}
    inline QString Ieh() const {return Profeh()+QChar(0x2074);}
    inline QString heh() const {return Profeh();}
    QString Profeh() const;

    double Eprefix() const;
    double Fprefix() const;
    double FMprefix() const;
    double Pprefix() const;
    double Mprefix() const;
    inline double Aprefix() const {return pow(Profprefix(),2);}
    inline double Iprefix() const {return pow(Profprefix(),4);}
    inline double hprefix() const {return Profprefix();}
    double Profprefix() const;
    int precison() const {return prec;}
    void setPrecison(const int &p);
    void saveSettings();
    void loadSettings();
public slots:
    void setPEh(unitSettings::PMEH eh);
    void setEEh(unitSettings::EEH eh);
    void setIEh(unitSettings::ProfEH eh);

signals:
    void settingschanged();
private:
    Q_DISABLE_COPY(unitSettings);
    unitSettings():QObject(){loadSettings();}
    ~unitSettings(){saveSettings();}
    static unitSettings *instance_;
    int prec;
    EEH EUnit;
    ProfEH ProfUnit;
    PMEH PMUnit;

};

#endif // UNITSETTINGS_H
