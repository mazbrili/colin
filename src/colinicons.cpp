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

#include "colinicons.h"

#include <QtCore/QSettings>

colinIcons *colinIcons::instance_ = NULL;
QString colinIcons::icondir_ = QString();

void colinIcons::load()
{
    QSettings CSettings("clazzes.org", "Colin");

	dark = CSettings.value("icons/dark", true).toBool();

	node.addFile(icondir+"node.png");
    beam.addFile(icondir+"beam.png");
    bearingH.addFile(icondir+"bearingH.png");
    bearingV.addFile(icondir+"bearingV.png");
    bearingM.addFile(icondir+"bearingM.png");
    bearingHV.addFile(icondir+"bearingHV.png");
    bearingHM.addFile(icondir+"bearingHM.png");
    bearingVM.addFile(icondir+"bearingVM.png");
    bearingHVM.addFile(icondir+"bearingHVM.png");
    jointM.addFile(icondir+"joint.png");
    jointN.addFile(icondir+"jointN.png");
    jointQ.addFile(icondir+"jointQ.png");
    jointR.addFile(icondir+"joint_r.png");
    loadN.addFile(icondir+"load.png");
    moment.addFile(icondir+"moment.png");
    loadU.addFile(icondir+"ustload.png");
    loadD.addFile(icondir+"dstload.png");
    loadI.addFile(icondir+"istload.png");
    temp.addFile(icondir+"temp.png");
    tempDiff.addFile(icondir+"tempDiff.png");
    tempChange.addFile(icondir+"tempChange.png");
    doubleL.addFile(icondir+"doubleL.png");
    cross.addFile(icondir+"close.png");
    cross.addFile(icondir+"close_on.png", QSize(32,32), QIcon::Normal, QIcon::On);
    plus.addFile(icondir+"plus.png");
    plus.addFile(icondir+"plus_on.png", QSize(32, 32), QIcon::Normal, QIcon::On);
    gridI.addFile(icondir+"grid.png");
    catcher.addFile(icondir+"magnet.png");
    undo.addFile(icondir+"undo.png");
    redo.addFile(icondir+"redo.png");
    copy.addFile(icondir+"copy.png");
    paste.addFile(icondir+"paste.png");
    cut.addFile(icondir+"cut.png");
    select.addFile(icondir+"select.png");
    move.addFile(icondir+"move.png");
    calc.addFile(icondir+"calc.png");
    //calc_red.addFile(icondir+"calc_red.png");
    //calc_green.addFile(icondir+"calc_green.png");
    window.addFile(icondir+"window.png");
    windows.addFile(icondir+"windows.png");
    zoom.addFile(icondir+"zoom.png");
    zoomall.addFile(icondir+"zoom_all.png");
    zoomin.addFile(icondir+"zoom_in.png");
    zoomout.addFile(icondir+"zoom_out.png");
    zoomrect.addFile(icondir+"zoom_rect.png");
    zoomsel.addFile(icondir+"zoom_sel.png");
    open.addFile(icondir+"open.png");
    save.addFile(icondir+"save.png");
    settings.addFile(icondir+"settings.png");
    crossing.addFile(icondir+"crossing.png");
    middle.addFile(icondir+"middle.png");
    ortho_g.addFile(icondir+"ortho_g.png");
    ortho_l.addFile(icondir+"ortho_l.png");
    spring.addFile(icondir+"spring.png");
    mat.addFile(icondir+"mat.png");
//    mat.addFile(icondir+"mat_64.png", QSize(64, 65));
    prof.addFile(icondir+"prof.png");
//    prof.addFile(icondir+"prof_64.png", QSize(64, 65));
    ok.addFile(icondir+"ok.png");
    key_dark = QPixmap(icondir+"key_dark.png");
    key = QPixmap(icondir+"key_light.png");
	node_plus.addFile(icondir+"node_plus_on.png");
	beam_plus.addFile(icondir+"beam_plus_on.png");
	load_plus.addFile(icondir+"load_plus_on.png");
	dstload_plus.addFile(icondir+"dstload_plus_on.png");
	istload_plus.addFile(icondir+"istload_plus_on.png");
	ustload_plus.addFile(icondir+"ustload_plus_on.png");
	doubleload_plus.addFile(icondir+"doubleL_plus_on.png");
	moment_plus.addFile(icondir+"moment_plus_on.png");
	tempDiff_plus.addFile(icondir+"tempDiff_plus_on.png");
	tempChange_plus.addFile(icondir+"tempChange_plus_on.png");
	bls.addFile(icondir+"bls.png");
	cls.addFile(icondir+"cls.png");
	bls_plus.addFile(icondir+"bls_plus_on.png");
	cls_plus.addFile(icondir+"cls_plus_on.png");
	whatsthis.addFile(icondir+"whatsThis.png");
	leftBeam.addFile(icondir+"left.png");
	rightBeam.addFile(icondir+"right.png");

    loadToolTipIcons();
}

void colinIcons::loadToolTipIcons()
{
    if(!dark)
    {
        vp_node.addFile(icondir+"tooltip/vp_node.png");
        vp_beam.addFile(icondir+"tooltip/vp_beam.png");
        vp_jointN.addFile(icondir+"tooltip/vp_jointN.png");
        vp_jointM.addFile(icondir+"tooltip/vp_jointM.png");
        vp_jointQ.addFile(icondir+"tooltip/vp_jointQ.png");
        vp_copy.addFile(icondir+"tooltip/vp_copy.png");
        vp_crossing.addFile(icondir+"tooltip/vp_crossing.png");
        vp_cut.addFile(icondir+"tooltip/vp_cut.png");
        vp_dstload.addFile(icondir+"tooltip/vp_dstload.png");
        vp_istload.addFile(icondir+"tooltip/vp_istload.png");
        vp_ustload.addFile(icondir+"tooltip/vp_ustload.png");
        vp_nload.addFile(icondir+"tooltip/vp_load.png");
        vp_doubleload.addFile(icondir+"tooltip/vp_doublel.png");
        vp_moment.addFile(icondir+"tooltip/vp_moment.png");
        vp_grid.addFile(icondir+"tooltip/vp_grid.png");
        vp_magnet.addFile(icondir+"tooltip/vp_magnet.png");
        vp_middle.addFile(icondir+"tooltip/vp_middle.png");
        vp_ortho_g.addFile(icondir+"tooltip/vp_orthog.png");
        vp_ortho_l.addFile(icondir+"tooltip/vp_orthol.png");
		vp_paste.addFile(icondir+"tooltip/vp_paste.png");
		vp_temp.addFile(icondir+"tooltip/vp_temp.png");
		vp_cls.addFile(icondir+"tooltip/vp_cls.png");
        placeholder.addFile(icondir+"placeholder.png");
    }
    else
    {
        vp_node.addFile(icondir+"node.png");
        vp_beam.addFile(icondir+"beam.png");
        vp_jointN.addFile(icondir+"jointN.png");
        vp_jointM.addFile(icondir+"jointM.png");
        vp_jointQ.addFile(icondir+"jointQ.png");
        vp_copy.addFile(icondir+"copy.png");
        vp_crossing.addFile(icondir+"crossing.png");
        vp_cut.addFile(icondir+"cut.png");
        vp_dstload.addFile(icondir+"dstload.png");
        vp_istload.addFile(icondir+"istload.png");
        vp_ustload.addFile(icondir+"ustload.png");
        vp_nload.addFile(icondir+"load.png");
        vp_doubleload.addFile(icondir+"doublel.png");
        vp_moment.addFile(icondir+"moment.png");
        vp_grid.addFile(icondir+"grid.png");
        vp_magnet.addFile(icondir+"magnet.png");
        vp_middle.addFile(icondir+"middle.png");
        vp_ortho_g.addFile(icondir+"ortho_g.png");
        vp_ortho_l.addFile(icondir+"ortho_l.png");
        vp_paste.addFile(icondir+"paste.png");
        vp_temp.addFile(icondir+"temp.png");
		vp_cls.addFile(icondir+"cls.png");
		placeholder.addFile(icondir+"placeholder.png");
    }
}

void colinIcons::saveSettings()
{
    QSettings settings("clazzes.org", "Colin");
    settings.setValue("icons/dark", dark);
}

/*
const QIcon &colinIcons::icon(const Colin::otherAction &c, const int &best) const
{
    (void)(c);
    if(best==0)
        return calc_green;
    else if(best>0)
        return calc;
    else
        return calc_red;
}*/

const QIcon &colinIcons::icon(const Colin::Actions &a, bool tooltip) const
{
    using namespace Colin;
    if(tooltip)
    {
	switch(a)
	{
	case drawNode:
	    return vp_node;
	case drawBeam:
	    return vp_beam;
	case drawDLoad:
	    return vp_dstload;
	case drawILoad:
	    return vp_istload;
	case drawULoad:
	    return vp_ustload;
	case drawMoment:
	    return vp_moment;
	case drawTemp:
		return vp_temp;
	case drawDoubleLoad:
		return vp_doubleload;
	case drawJointN:
		return vp_jointN;
	case drawJointQ:
		return vp_jointQ;
	case drawJointM:
		return vp_jointM;
	default:
	    break;
	}
    }
    switch(a)
    {
    case drawNode:
        return node;
    case drawBeam:
        return beam;
    case drawBearing:
        return bearingV;
    case drawBearingH:
        return bearingH;
    case drawBearingV:
        return bearingV;
    case drawBearingM:
        return bearingM;
    case drawBearingHV:
        return bearingHV;
    case drawBearingHM:
        return bearingHM;
    case drawBearingVM:
        return bearingVM;
    case drawBearingHVM:
        return bearingHVM;
    case drawJoint:
        return jointM;
    case drawJointN:
        return jointN;
    case drawJointQ:
        return jointQ;
    case drawJointM:
        return jointM;
    case drawNLoad:
        return loadN;
    case drawMoment:
        return moment;
    case drawULoad:
        return loadU;
    case drawDLoad:
        return loadD;
    case drawILoad:
        return loadI;
    case drawTemp:
        return temp;
    case drawDoubleLoad:
        return doubleL;
    case drawSelec:
        return select;
    case drawMove:
        return move;
    case drawZoom:
        return zoom;
    case drawZoomIn:
        return zoomin;
    case drawZoomOut:
        return zoomout;
    case drawZoomAll:
        return zoomall;
    case drawZoomRect:
        return zoomrect;
    default:
        return placeholder;
    }
}

const QIcon &colinIcons::icon(const Colin::otherAction &a, bool tooltip) const
{
    using namespace Colin;
    if(tooltip)
    {
        switch(a)
        {
        case Copy:
            return vp_copy;
        case Cut:
            return vp_cut;
        case Paste:
            return vp_paste;
        default:
            break;
        }
    }
    switch (a)
    {
    case Open:
        return open;
    case Save:
        return save;
    case Close:
        return cross;
    case Settings:
        return settings;
    case Grid:
        return gridI;
    case Snap:
        return catcher;
    case Undo:
        return undo;
    case Redo:
        return redo;
    case New:
        return plus;
    case Copy:
        return copy;
    case Cut:
        return cut;
    case Paste:
        return paste;
    case Calculate:
        return calc;
    case Window:
        return window;
    case Windows:
        return windows;
    case Spring:
        return spring;
    case RightJoint:
        return jointR;
    case Material:
        return mat;
    case Profile:
        return prof;
    case Ok:
        return ok;
	case addNode:
		return node_plus;
	case addBeam:
		return beam_plus;
	case addLoad:
		return load_plus;
	case addBLS:
		return bls_plus;
	case addCLS:
		return cls_plus;
	case BLS:
		return bls;
	case CLS:
		return cls;
	case what:
		return whatsthis;
	case leftSide:
		return leftBeam;
	case rightSide:
		return rightBeam;
    default:
        return placeholder;
    }
}


const QIcon &colinIcons::newIcon(const Colin::otherAction &a, int formhint)
{
	using namespace Colin;

	switch(a)
	{
	case addNode:
		return node_plus;
	case addBeam:
		return beam_plus;
	case addLoad:
		switch(formhint)
		{
		case ColinLoad::nodeLoad:
			return load_plus;
		case ColinLoad::uniformlyDistibutedLoad:
			return ustload_plus;
		case ColinLoad::increasingLinearLoad:
			return istload_plus;
		case ColinLoad::decreasingLinearLoad:
			return dstload_plus;
		case ColinLoad::moment:
			return moment_plus;
		case ColinLoad::doubleLoadLeft:
		case ColinLoad::doubleLoadRight:
			return doubleload_plus;
		case ColinLoad::tempChange:
			return tempChange_plus;
		case ColinLoad::tempDiffrence:
			return tempDiff_plus;
		default:
			return placeholder;
		}
	case addBLS:
		return bls_plus;
	case addCLS:
		return cls_plus;
	default:
		return placeholder;
	}
}

const QIcon &colinIcons::icon(const catcher::CatchCases &c, bool tooltip) const
{
    if(tooltip)
    {
		switch (c)
		{
		case catcher::CatchedNode:
			return vp_node;
		case catcher::CatchedBeam:
			return vp_beam;
		case catcher::CatchedGrid:
			return vp_grid;
		case catcher::CatchedCrossing:
			return vp_crossing;
		case catcher::CatchedMiddle:
			return vp_middle;
		case catcher::CatchedOrthoGlob:
			return vp_ortho_g;
		case catcher::CatchedOrthoLokal:
			return vp_ortho_l;
		case catcher::CatchedTemp:
			return vp_temp;
		case catcher::CatchedNLine:
			return vp_cls;
		case catcher::CatchedQLine:
			return vp_cls;
		case catcher::CatchedMLine:
			return vp_cls;
		case catcher::CatchedULine:
			return vp_cls;
		default:
			break;
		}
    }
    switch(c)
    {
    case catcher::CatchedNode:
        return node;
    case catcher::CatchedBeam:
        return beam;
    case catcher::CatchedGrid:
        return gridI;
    case catcher::CatchedCrossing:
        return crossing;
    case catcher::CatchedMiddle:
        return middle;
    case catcher::CatchedOrthoGlob:
        return ortho_g;
    case catcher::CatchedOrthoLokal:
        return ortho_l;
	case catcher::CatchedNLine:
		return cls;
	case catcher::CatchedQLine:
		return cls;
	case catcher::CatchedMLine:
		return cls;
    default:
        return placeholder;
    }
}


QPixmap colinIcons::icon(QString keyS, bool dark) const
{
    QPixmap keypix;

    const QPixmap &Key=(dark?key_dark:key);

    if(keyS == QObject::tr("Ctrl", "shortcuts | Control-Key")  ||
       keyS == QObject::tr("Tab", "shortcuts | Tab-Key")   ||
       keyS == QObject::tr("Backtab", "shortcuts | Backtab(Shift-Key+Tab-Key)"))
    {
        keypix = QPixmap(32+16, 32);
        keypix.fill(QColor(0, 0, 0, 0));
        QPainter p(&keypix);

        p.setClipRect(0, 0, 16, 32);
        p.drawPixmap(0, 0, 32, 32, Key);

        p.setClipRect(32+16-16, 0, 16, 32);
        p.drawPixmap(32+16-32, 0, 32, 32, Key);

        p.setClipRect(16, 0, 16, 32);
        p.drawPixmap(8, 0, 32, 32, Key);

    }
    else if(keyS == QObject::tr("Shift", "shortcuts | Shift-Key") ||
            keyS == QObject::tr("Space", "shortcuts | Space-Key") )
    {
        keypix = QPixmap(32+32, 32);
        keypix.fill(QColor(0, 0, 0, 0));
        QPainter p(&keypix);

        p.setClipRect(0, 0, 16, 32);
        p.drawPixmap(0, 0, 32, 32, Key);

        p.setClipRect(32+32-16, 0, 16, 32);
        p.drawPixmap(32+32-32, 0, 32, 32, Key);

        p.setClipRect(16, 0, 16, 32);
        p.drawPixmap(8, 0, 32, 32, Key);

        p.setClipRect(32, 0, 16, 32);
        p.drawPixmap(8+16, 0, 32, 32, Key);
    }
    else
    {
        keypix = Key;
    }

    QPainter p(&keypix);

    QLinearGradient grad(0, 0, 0, keypix.height());
    if(dark)
        p.setPen(QPen(darkBrush(grad), 1));
    else
        p.setPen(QPen(lightBrush(grad), 1));
    QFont f = p.font();
    f.setPixelSize(9);
    p.setFont(f);
    p.drawText(QRect(8, 4, keypix.width()-8, keypix.height()-8),
               keyS,
               Qt::AlignLeft | Qt::AlignTop);
    p.end();

    return keypix;
}

const QIcon &colinIcons::icon(const ColinLoad::form &f, bool tooltip) const
{
    if(tooltip)
    {
        switch(f)
        {
        case ColinLoad::decreasingLinearLoad:
            return vp_dstload;
        case ColinLoad::increasingLinearLoad:
            return vp_istload;
        case ColinLoad::tempChange:
            return vp_temp;
        case ColinLoad::tempDiffrence:
            return vp_temp;
        case ColinLoad::uniformlyDistibutedLoad:
            return vp_ustload;
        case ColinLoad::nodeLoad:
            return vp_nload;
        case ColinLoad::moment:
            return vp_moment;
	case ColinLoad::doubleLoadLeft:
	case ColinLoad::doubleLoadRight:
	    return vp_doubleload;
        default:
            break;
        }
    }
    switch(f)
    {
    case ColinLoad::decreasingLinearLoad:
        return loadD;
    case ColinLoad::increasingLinearLoad:
        return loadI;
    case ColinLoad::tempChange:
        return tempChange;
    case ColinLoad::tempDiffrence:
        return tempDiff;
    case ColinLoad::uniformlyDistibutedLoad:
        return loadU;
    case ColinLoad::nodeLoad:
        return loadN;
    case ColinLoad::moment:
        return moment;
    case ColinLoad::doubleLoadLeft:
    case ColinLoad::doubleLoadRight:
	return doubleL;
    default:
        return placeholder;
    }
}

const QIcon &colinIcons::icon(const ColinSupport::bearings &b) const
{
    Colin::Actions a = Colin::drawBearing;
    if((b&ColinSupport::X)==ColinSupport::X)      a |= Colin::drawBearingH;
    if((b&ColinSupport::Z)==ColinSupport::Z)      a |= Colin::drawBearingV;
    if((b&ColinSupport::Phi)==ColinSupport::Phi)  a |= Colin::drawBearingM;
    return icon(a);
}

QPixmap colinIcons::icon(const QKeySequence &s, bool dark) const
{
    const int dx = 10;

    //get the string
    QString sec = s.toString(QKeySequence::NativeText);

    //get the first sequence only
    QStringList keylist = sec.split(",");

    //check for "++" like in "ctrl++" (zoom in)
    bool plus = false;
    if(keylist[0].contains("++"))
        plus = true;

    //split to single keys
    keylist = keylist[0].split("+");

    if(plus)
        keylist.append("+");

    keylist.removeAll("");

    //calculate splace
    int pixwidth = keylist.size()*32+(keylist.size()-1)*dx;

    //shift and Ctrl are bigger than normal keys...
    if(keylist.contains(QObject::tr("Shift", "shortcuts | Shift-Key")))
       pixwidth+=32;
    if(keylist.contains(QObject::tr("Space", "shortcuts | Space-Key")))
       pixwidth+=32;
    if(keylist.contains(QObject::tr("Ctrl", "shortcuts | Control-Key")))
        pixwidth+=16;
    if(keylist.contains(QObject::tr("Tab", "shortcuts | Tab-Key")))
        pixwidth+=16;
    if(keylist.contains(QObject::tr("Backtab", "shortcuts | Backtab(Shift-Key+Tab-Key)")))
        pixwidth+=16;

    //reserve space
    QPixmap sequence(pixwidth, 32);

    //keysequence is void
    if(keylist.isEmpty())
        return sequence;

    //initialise
    sequence.fill(QColor(0, 0, 0, 0));

    //initialse painter
    QPainter p(&sequence);

    //nice brush
    QLinearGradient grad(0, 0, 0, 32);
    if(dark)
        p.setPen(QPen(darkBrush(grad), 1));
    else
        p.setPen(QPen(lightBrush(grad), 1));

    //nice fontsize
    QFont f = p.font();
    f.setPixelSize(dx*2);
    p.setFont(f);

    //counter for keyposition
    int wc = 0;

    //draw the single keys
    for(int i=0; i<keylist.size(); i++)
    {
        //pixmap out of key...
        QPixmap px = icon(keylist[i], dark);
        //..and draw it
        p.drawPixmap(wc, 0, px.width(), px.height(), px);

        //draw "+"
        if(i!=0)
        {
            p.drawText(QRect(wc-dx, 0, dx, 32),
                       Qt::AlignCenter,
                       "+");
        }

        wc += px.width()+dx;


    }

    return sequence;
}

QIcon colinIcons::icon(const Colin::Color &c) const
{
    QPixmap pix(32, 32);
    pix.fill(viewPortSettings::instance().color(c));
    return QIcon(pix);
}

QIcon colinIcons::icon(const QColor &c) const
{
	QPixmap pix(32, 32);
	pix.fill(c);
	return QIcon(pix);
}

QIcon colinIcons::smallIcon(const QColor &c) const
{
	QPixmap pix(32, 32);
	pix.fill(QColor(0,0,0,0));
	QPainter p(&pix);
	p.fillRect(QRect(8, 8, 24, 24), c);
	return QIcon(pix);
}

QBrush colinIcons::lightBrush(QLinearGradient &grad) const
{
    grad.setColorAt(0, QColor(230, 230, 230));
    grad.setColorAt(1, QColor(178, 178, 178));
    return QBrush(grad);
}

QBrush colinIcons::darkBrush(QLinearGradient &grad) const
{
    grad.setColorAt(0, QColor(30, 30, 30));
    grad.setColorAt(1, QColor(90, 90, 90));
    return QBrush(grad);
}



