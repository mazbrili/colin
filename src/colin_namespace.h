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

#ifndef COLIN_NAMESPACE_H
#define COLIN_NAMESPACE_H

#include <QtGui/QIcon>
#include <QtCore/QObject>

namespace Colin
{
    enum Element{
        voidView =  0x0,
        u =         0x1,
        N =         0x2,
        Q =         0x4,
        M =         0x8,

        node =      0x10,
        beam =      0x20,
        bearing =   0x40,
        joint =     0x80,
        nload =     0x100,
        sload =     0x200,
        temp =      0x400,
        deform =    0x800,
        thI =       0x1000,          //mabye, in a few years,
        thIQ =      0x2000,          // colin will know more
        thII =      0x4000           //then one theory...
    };

    enum Color{
        C_Node =        0,
        C_Beam =        1,
        C_Bearing =     2,
        C_Load1 =       3,
        C_Load2 =       4,
        C_Reaction =    5,
        C_Background =  6,
        C_Selection =   7,
        C_NodeRef =     8,
        C_BeamRef =     9,
		C_BearingRef =  10,
		C_Grid =        11,
		C_Np =          12,
		C_Qp =          13,
		C_Mp =          14,
		C_Nm =          15,
		C_Qm =          16,
		C_Mm =          17
    };

    inline QString colorToString(Colin::Color c)
    {
        switch(c)
        {case Colin::C_Node:
            return QObject::tr("nodes");
         case Colin::C_Beam:
            return QObject::tr("beams");
         case Colin::C_Bearing:
            return QObject::tr("support");
         case Colin::C_Load1:
            return QObject::tr("loads");
         case Colin::C_Load2:
            return QObject::tr("loads hotspot");
         case Colin::C_Reaction:
            return QObject::tr("reactions");
        case Colin::C_Background:
           return QObject::tr("background");
         case Colin::C_Selection:
            return QObject::tr("selection");
         case Colin::C_NodeRef:
            return QObject::tr("nodes referececonfiguration");
         case Colin::C_BeamRef:
            return QObject::tr("beams referececonfiguration");
         case Colin::C_BearingRef:
	    return QObject::tr("support referececonfiguration");
         case Colin::C_Grid:
	    return QObject::tr("grid");
         case Colin::C_Np:
            return QObject::tr("normal force")+ " (+)";
         case Colin::C_Qp:
            return QObject::tr("shear force") + " (+)";
         case Colin::C_Mp:
            return QObject::tr("moment") + " (+)";
         case Colin::C_Nm:
            return QObject::tr("normal force") + " (-)";
         case Colin::C_Qm:
            return QObject::tr("shear force") + "  (-)";
         case Colin::C_Mm:
            return QObject::tr("moment") + "(-)";
        default:
            return "";
        }
    }

    Q_DECLARE_FLAGS (Elements, Element)

    enum Action{
        drawMove = 0x0,
        drawSelec = 0x1,

        drawNode = 0x2,

        drawBeam = 0x3,

        drawNLoad = 0x4,
        drawMoment = 0x5,
        drawULoad = 0x6,
        drawILoad = 0x7,
        drawDLoad = 0x8,
        drawTemp = 0x9,
		drawDoubleLoad = 0xA,


        drawBearing = 0x20,

        drawBearingH = 0x20+0x40,
        drawBearingV = 0x20+0x80,
        drawBearingM = 0x20+0x100,

        drawBearingHV = 0x20+0x40+0x80,
        drawBearingHM = 0x20+0x40+0x100,
        drawBearingVM = 0x20+0x80+0x100,

        drawBearingHVM = 0x20+0x40+0x80+0x100,


        drawJoint = 0x200,
        drawJointN = 0x200+0x400,
        drawJointQ = 0x200+0x800,
        drawJointM = 0x200+0x1000,


        drawZoom = 0x2000,
        drawZoomIn = 0x2000+0x4000,
        drawZoomOut = 0x2000+0x8000,
        drawZoomAll = 0x2000+0x10000,
        drawZoomRect = 0x2000+0x20000,

    };

    Q_DECLARE_FLAGS(Actions, Action)

    enum otherAction{
        startsAt        = 0x40000,
        Open            = 0x40011,
        Save            = 0x40012,
        Close           = 0x40013,
        Settings        = 0x40014,
        Grid            = 0x40015,
        Snap            = 0x40016,
        Undo            = 0x40017,
        Redo            = 0x40018,
        New             = 0x40019,
        Copy            = 0x4001A,
        Cut             = 0x4001B,
        Paste           = 0x4001C,
        Calculate       = 0x4001D,
        Window          = 0x4001E,
        Windows         = 0x4001F,
        Spring          = 0x40020,
        RightJoint      = 0x40021,
        Material        = 0x40022,
        Profile         = 0x40023,
        Delete          = 0x40024,
		Ok              = 0x40025,
		BLS				= 0x40026,
		CLS				= 0x40027,
		addNode			= 0x40028,
		addBeam			= 0x40029,
		addLoad			= 0x40030,
		addBLS			= 0x40032,
		addCLS			= 0x40033
    };

    enum clipboardAction{
        noRequest,
        copybasePointRequest,
        cutbasePointRequest,
        pasteRequest,
        pasteLoadRequestBeam,
        pasteLoadRequestNode
    };


};

Q_DECLARE_OPERATORS_FOR_FLAGS (Colin::Elements)
Q_DECLARE_OPERATORS_FOR_FLAGS (Colin::Actions)


namespace Colin{


    inline bool testForNLoad(const Actions &a){
        if(a == drawNLoad ||
           a == drawMoment)
            return true;
        else
            return false;
    };

    inline bool testForBLoad(const Actions &a){
        if(a == drawULoad ||
           a == drawDLoad ||
           a == drawILoad )
            return true;
        else
            return false;
    };

    inline bool testForLoad(const Actions &a){
        return(testForNLoad(a) || testForBLoad(a));
    };
};

#endif //COLIN_NAMESPACE_H
