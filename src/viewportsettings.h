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

#ifndef VIEWPORTSETTINGS_H
#define VIEWPORTSETTINGS_H

#include <QtCore/QObject>
#include <QtGui/QColor>
#include <QtCore/QSettings>

#include "colin_namespace.h"

#define VIEWSETTINGS viewPortSettings::instance()

class viewPortSettings : public QObject
{
    Q_OBJECT
public:



    static viewPortSettings& instance()
    {
        if(!instance_)
            instance_ = new viewPortSettings();
        return *instance_;
    }

    inline const Colin::Actions &toDraw() const {return toDraw_;}
    inline const Colin::clipboardAction &toClip() const {return toClip_;}
    const QColor &color(Colin::Color e) const;
    const Colin::Elements &elements(const int &i)const{return drawList.at(i);}
    void setElements(const int &id_, const Colin::Elements &e){drawList[id_] = e; emit settingschanged();}
    void setColor(Colin::Color, const QColor &color);
    int bearingSize() const {return 32;}
    inline const int &widgetCount() const {return widgetC;}

    Colin::Elements wheelMod(const Qt::KeyboardModifiers &mod);

    const bool &antialiasing()const{return AAOn;}
    void setAntiAliasing(const bool &isOn){AAOn = isOn;}

    const double &MtoRad() const {return mRad;}
    void setMtoRad(const double &m) {mRad = m;}

    const QSize &exportSize() const {return exportS;}
    void setExportSize(const QSize &size){exportS = size;}

    const double &resultsWidgetScale() const {return rWS;}
    void setResultsWidgetScale(const double &s){rWS = s;}

	const QList<QColor> &standardColors()const{return stdcolors;}
	const QColor &firstStandardColor()const;
	const QColor &nextStandardColor(const QColor &c)const;

    void loadSettings();
    void saveSettings();


signals:
    void settingschanged();
    void countchanged(int i);
    void currentActionChanged(Colin::Actions a);
    void clipBoardActionChanged(Colin::clipboardAction a);


public slots:
    void setCurrentAction(Colin::Actions a);
    void setClipBoard(Colin::clipboardAction a);
    void setCtrlMod(const Colin::Elements &e);
    void setShiftMod(const Colin::Elements &e);
    void setAltMod(const Colin::Elements &e);
    inline void setWidgetCount(int i){widgetC = i; emit countchanged(i);}


private:

	Q_DISABLE_COPY(viewPortSettings)

    viewPortSettings():QObject(){
        toDraw_ = Colin::drawMove;
        toClip_ = Colin::noRequest;
        loadSettings();}


    static viewPortSettings* instance_;
    QList<Colin::Elements> drawList;
    QList<QColor> colors;
	QList<QColor> stdcolors;
	mutable QList<QColor>::const_iterator nextCol;
    Colin::Actions toDraw_;
    Colin::clipboardAction toClip_;
    Colin::Elements CtrlMod;
    Colin::Elements ShiftMod;
    Colin::Elements AltMod;
    int widgetC;
    bool AAOn;
    double mRad;
    double rWS;
    QSize exportS;
};




#endif // VIEWPORTSETTINGS_H
