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

#include "viewportsettings.h"
#include <QtGui/QApplication>
#include <QtGui/QPalette>

viewPortSettings *viewPortSettings::instance_ = NULL;

void viewPortSettings::loadSettings()
{
    QSettings settings("clazzes.org", "Colin", this);
    colors.clear();
    colors.append(settings.value("colors/node", QColor(255, 0, 0)).value<QColor>()); //0
    colors.append(settings.value("colors/beam", QColor(0, 0, 0)).value<QColor>()); //1
    colors.append(settings.value("colors/bearing", QColor(0, 0, 0)).value<QColor>()); //2
    colors.append(settings.value("colors/load1", QColor(0, 170, 255)).value<QColor>()); //3
    colors.append(settings.value("colors/load2", QColor(0, 255, 255)).value<QColor>()); //4
	colors.append(settings.value("colors/reaction", QColor(159, 0, 83)).value<QColor>()); //5
	colors.append(settings.value("colors/background", QColor(250, 250, 250)).value<QColor>()); //6
    colors.append(settings.value("colors/node_ref", QColor(255, 184, 61)).value<QColor>()); //8
    colors.append(settings.value("colors/beam_ref", QColor(136, 136, 136)).value<QColor>()); //9
    colors.append(settings.value("colors/bearing_ref", QColor(136, 136, 136)).value<QColor>()); //10
	colors.append(settings.value("colors/grid", QColor(240, 240, 240)).value<QColor>()); //13
    colors.append(settings.value("colors/N_p", QColor(0, 255, 0)).value<QColor>()); //16
    colors.append(settings.value("colors/Q_p", QColor(0, 85, 255)).value<QColor>()); //17
    colors.append(settings.value("colors/M_p", QColor(255, 0, 0)).value<QColor>()); //18
    colors.append(settings.value("colors/N_m", QColor(184, 255, 19)).value<QColor>()); //19
    colors.append(settings.value("colors/Q_m", QColor(170, 85, 255)).value<QColor>()); //20
    colors.append(settings.value("colors/M_m", QColor(255, 170, 0)).value<QColor>()); //21

    drawList.clear();
    drawList.append(static_cast<Colin::Element>(
                                    settings.value("elements/viewport1",
                                        static_cast<int>(
                                            Colin::u        |
                                            Colin::nload    |
                                            Colin::node     |
                                            Colin::beam     |
                                            Colin::bearing  |
                                            Colin::joint    |
                                            Colin::thI      |
                                            Colin::thIQ     |
                                            Colin::thII     ))
                                    .toInt()));

    drawList.append(static_cast<Colin::Element>(
                                    settings.value("elements/viewport2",
                                        static_cast<int>(
                                            Colin::N        |
					    Colin::beam     |
                                            Colin::thI      |
                                            Colin::thIQ     |
                                            Colin::thII     ))
                                    .toInt()));

    drawList.append(static_cast<Colin::Element>(
                                    settings.value("elements/viewport3",
                                        static_cast<int>(
                                            Colin::Q        |
					    Colin::beam     |
                                            Colin::thI      |
                                            Colin::thIQ     |
                                            Colin::thII     ))
                                    .toInt()));

    drawList.append(static_cast<Colin::Element>(
                                    settings.value("elements/viewport4",
                                        static_cast<int>(
                                            Colin::M        |
					    Colin::beam     |
                                            Colin::thI      |
                                            Colin::thIQ     |
                                            Colin::thII     ))
                                    .toInt()));
    //for(int i=5; i<13; i++)
    //    drawList.append(static_cast<Colin::Element>(
    //                                    settings.value(QString("elements/viewport%1").arg(i),
    //                                    0x0).toInt()));

	stdcolors << QColor("#000080")
			  << QColor("#0000CD")
			  << QColor("#20B2AA")
			  << QColor("#00BFFF")
			  << QColor("#00FA9A")
			  << QColor("#00FF00")
			  << QColor("#32CD32")
			  << QColor("#7CFC00")
			  << QColor("#ADFF2F")
			  << QColor("#FFFF00")
			  << QColor("#FFD700")
			  << QColor("#FF8C00")
			  << QColor("#FF7F50")
			  << QColor("#FF0000")
			  << QColor("#DC143C")
			  << QColor("#C71585")
			  << QColor("#9932CC");

	nextCol = stdcolors.constBegin();

    CtrlMod = static_cast<Colin::Elements>(settings.value("modifier/zoom_ctrl", static_cast<int>(Colin::N)).toInt());
    ShiftMod = static_cast<Colin::Elements>(settings.value("modifier/zoom_shift", static_cast<int>(Colin::u)).toInt());
    AltMod = static_cast<Colin::Elements>(settings.value("modifier/zoom_alt", static_cast<int>(Colin::M)).toInt());

    AAOn = (settings.value("drawing/anitAliasing", 1).toInt()==1);
    mRad = settings.value("drawing/mToRad", 40.).toDouble();
    rWS = settings.value("drawing/resultVisualWidgetScale", 100.).toDouble();
    exportS.setWidth(settings.value("drawing/exportWidth", 1400).toInt());
    exportS.setHeight(settings.value("drawing/exportHeight", 1000).toInt());

}

void viewPortSettings::saveSettings()
{
    QSettings settings("clazzes.org", "Colin");
    int i=0;
    settings.setValue("colors/node", colors.at(i++));
    settings.setValue("colors/beam", colors.at(i++));
    settings.setValue("colors/bearing", colors.at(i++));
    settings.setValue("colors/load1", colors.at(i++));
    settings.setValue("colors/load2", colors.at(i++));
    settings.setValue("colors/reaction", colors.at(i++));
    settings.setValue("colors/background", colors.at(i++));
    settings.setValue("colors/node_ref", colors.at(i++));
    settings.setValue("colors/beam_ref", colors.at(i++));
    settings.setValue("colors/bearing_ref", colors.at(i++));
    settings.setValue("colors/grid", colors.at(i++));
    settings.setValue("colors/N_p", colors.at(i++));
    settings.setValue("colors/Q_p", colors.at(i++));
    settings.setValue("colors/M_p", colors.at(i++));
    settings.setValue("colors/N_m", colors.at(i++));
    settings.setValue("colors/Q_m", colors.at(i++));
    settings.setValue("colors/M_m", colors.at(i++));

    for(i=0; i<4; i++)
        settings.setValue(QString("elements/viewport%1").arg(i+1), static_cast<int>(drawList.at(i)));


    settings.setValue("modifier/zoom_ctrl", static_cast<int>(CtrlMod));
    settings.setValue("modifier/zoom_shift", static_cast<int>(ShiftMod));
    settings.setValue("modifier/zoom_alt", static_cast<int>(AltMod));

    settings.setValue("drawing/anitAliasing", (AAOn? 1 : 0));
    settings.setValue("drawing/mToRad", mRad);
    settings.setValue("drawing/resultVisualWidgetScale", rWS);

    settings.setValue("drawing/exportWidth", exportS.width());
    settings.setValue("drawing/exportHeight", exportS.height());


}

void viewPortSettings::setCurrentAction(Colin::Actions a)
{
    toDraw_=a;
    emit currentActionChanged(toDraw_);
}

void viewPortSettings::setClipBoard(Colin::clipboardAction a)
{
    toClip_ =a;
    emit clipBoardActionChanged(a);
}

const QColor &viewPortSettings::color(Colin::Color c) const
{
	if(c == Colin::C_Selection)
		return QApplication::palette().color(QPalette::Highlight);
    return colors[static_cast<int>(c)];
}

void viewPortSettings::setColor(Colin::Color c, const QColor &color)
{
    colors[static_cast<int>(c)] = color;
}

Colin::Elements viewPortSettings::wheelMod(const Qt::KeyboardModifiers &mod)
{
    if(mod.testFlag(Qt::ControlModifier))
	return CtrlMod;
    if(mod.testFlag(Qt::ShiftModifier))
	return ShiftMod;
    if(mod.testFlag(Qt::AltModifier))
	return AltMod;
    return Colin::voidView;
}


void viewPortSettings::setCtrlMod(const Colin::Elements &e)
{
    CtrlMod = e;
}

void viewPortSettings::setShiftMod(const Colin::Elements &e)
{
    ShiftMod = e;
}

void viewPortSettings::setAltMod(const Colin::Elements &e)
{
    AltMod = e;
}


const QColor &viewPortSettings::firstStandardColor() const
{
	nextCol=standardColors().constBegin();
	return *nextCol;
}

const QColor &viewPortSettings::nextStandardColor(const QColor &c)const
{
	while(*nextCol != c)
	{
		nextCol++;

		if(nextCol==standardColors().constEnd())
			nextCol = standardColors().constBegin();
	}
	nextCol++;
	if(nextCol==standardColors().constEnd())
		nextCol = standardColors().constBegin();
	return *nextCol;
}
