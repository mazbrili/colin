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

#ifndef LIBSIDEBAR_H
#define LIBSIDEBAR_H

#include <QtGui/QWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QLabel>
#include <QtGui/QGridLayout>
#include <QtGui/QSplitter>
#include <QtGui/QListWidgetItem>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

#include "filelist.h"

class libSidebar : public QSplitter
{
    Q_OBJECT
public:
    explicit libSidebar(QWidget *parent = 0);

signals:

public slots:
    void appendMat();
    void appendProfile();
    void removeMat(const int &o);
    void removeProfile(const int &o);
    void actualUnits();
    void setProfile(const int &o);
    void setMat(const int &o);

private slots:
    void launchDialogNewP();
    void launchDialogNewM();
    void launchDialogEditP();
    void launchDialogEditM();
    void launchDialogEditP(const QModelIndex &mI);
    void launchDialogEditM(const QModelIndex &mI);
    void activatedP(const QModelIndex &mI);
    void activatedM(const QModelIndex &mI);
    void setActiveP(const int &i);
    void setActiveM(const int &i);

private:
    QGridLayout *layout;
#ifdef LIB_ICON
    QLabel *proficon,
           *maticon;
#endif
    QLabel *profvals,
           *profname,
           *matname,
           *matvals;

    QPushButton *newM,
                *newP,
                *editM,
                *editP;

    QTreeWidget *mats,
                *profiles;
    int oldCurrentM,
        oldCurrentP;
};

#endif // LIBSIDEBAR_H
