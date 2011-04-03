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

#ifndef MISCSETTINGSWIDGET_H
#define MISCSETTINGSWIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QComboBox>
#include <QtGui/QDoubleSpinBox>
#include "viewportsettings.h"
#include "grid.h"

class miscSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    miscSettingsWidget(QWidget *parent = 0);
private slots:
    void Echanged(const int &i);
    void Pchanged(const int &i);
    void Ichanged(const int &i);
    void setLang(const QString &lang);
    void ctrlWheelChanged(const int &i);
    void shiftWheelChanged(const int &i);
    void altWheelChanged(const int &i);
    void AAChanged(const int &i);
    void mRadChanged(const double &mR);
    void resultWChanged(const double &s);
    //void resChanged();
    void precisonChanged(const double &prec);
    void tooltipColor(const int &row);
private:
    QComboBox *EEhBox,
	      *IEhBox,
              *PEhBox;

    QComboBox *zBox[3];


    //QDoubleSpinBox *xRes,
    //               *yRes;
};

#endif // MISCSETTINGSWIDGET_H
