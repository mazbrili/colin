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

#ifndef COLINLIB_H
#define COLINLIB_H

#include <QtCore/QCoreApplication>
#include <QtCore/QObject>
#include <QtCore/QSettings>



#include "colinmaterial.h"
#include "colincrosssection.h"

#define LIB ColinLibrary::instance()

class ColinLibrary : public QObject
{
    Q_OBJECT
public:
	static ColinLibrary &instance(){
    if(!instance_)
		instance_ = new ColinLibrary();
    return *instance_;}

    const inline ColinMaterial &mat(const int &i) const {return mats.at(i);}
    const inline ColinMaterial &mat(const QString &_name) const {return mat(IndexFromMatByName(_name));}
    int mats_n() const {return (mats.size());}
    int IndexFromMatByName(const QString &_name) const {
        for(int i=0; i<mats_n(); i++){
            if(mat(i).name()==_name) return i;}
                return -1;}
    void addProfile(const ColinCrossSection &n);
    void editProfile(const int &nr, const ColinCrossSection &n);
    void removeProfile(const int &nr);


    const inline ColinCrossSection &Profile(const int &i) const {return profiles.at(i);}
    const inline ColinCrossSection &Profile(const QString &_name) const {return Profile(IndexFromProfileByName(_name));}
    int profiles_n() const {return (profiles.size());}
    int IndexFromProfileByName(const QString &_name) const {
        for(int i=0; i<profiles_n(); i++){
             if(Profile(i).name()==_name) return i;}
            return -1;}
    void addMat(const ColinMaterial &n);
    void editMat(const int &nr, const ColinMaterial &n);
    void removeMat(const int &nr);

    void setCurrentMaterial(const int &i) {currentM = i; emit currentMChanged(i);}
    void setCurrentProfile(const int &i) {currentP = i; emit currentPChanged(i);}

    const inline int &currentMaterial() const {return currentM;}
    const inline int &currentProfile() const {return currentP;}

    void setUp();

signals:
    void matRemoved(const int &nr);
    void matEdited(const int &nr);
    void matAdded();

    void profileRemoved(const int &nr);
    void profileEdited(const int &nr);
    void profileAdded();

    void currentMChanged(const int &i);
    void currentPChanged(const int &i);

public slots:
    void dump();
private:

	ColinLibrary();
    int currentM;
    int currentP;

	static ColinLibrary *instance_;
    QList<ColinMaterial> mats;
    QList<ColinCrossSection> profiles;
};

#endif // COLINLIB_H
