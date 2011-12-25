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

#ifndef COLINICONS_H
#define COLINICONS_H

#include <QtGui/QIcon>
#include <QtGui/QPainter>
#include <QtGui/QLinearGradient>
#include <QtGui/QKeySequence>
#include <QtCore/QStringList>

#include "colin_namespace.h"
#include "colinsupport.h"
#include "viewportsettings.h"
#include "catcher.h"
#include "colinload.h"

#define icondir colinIcons::icondir_

class colinIcons
{
public:
    static colinIcons& instance()
    {
        if(!instance_)
            instance_ = new colinIcons();
        return *instance_;
    }

    static QString icondir_;


    const QIcon &icon(const Colin::Actions &a, bool tooltip = false) const ;
    const QIcon &icon(const catcher::CatchCases &c, bool tooltip = false) const;
    const QIcon &icon(const Colin::otherAction &a, bool tooltip = false) const;
    const QIcon &icon(const ColinLoad::form &f, bool tooltip = false) const;
    const QIcon &icon(const ColinSupport::bearings &b) const;
	const QIcon &newIcon(const Colin::otherAction &a, int formhint);
	QIcon icon(const Colin::Color &c) const;
	QIcon icon(const QColor &c) const;
	QIcon smallIcon(const QColor &c) const;



    inline const QIcon &icon(const Colin::Action &a) const{return icon(Colin::Actions(a));}
    inline const QIcon &icon(const catcher::CatchCase &c) const{return icon(catcher::CatchCases(c));}

    QPixmap icon(QString keyS, bool dark = false) const;
    QPixmap icon(const QKeySequence &s, bool dark = false) const;

    static inline QSize iconSize() {return QSize(24, 24);}

    void setDark(bool dark_){dark = dark_; loadToolTipIcons();}
    const bool &darkIcons() {return dark;}
    void saveSettings();
private:
    colinIcons(){load();}

    QBrush lightBrush(QLinearGradient &grad) const;
    QBrush darkBrush(QLinearGradient &grad) const;
    void load();
    void loadToolTipIcons();
    static colinIcons *instance_;
    QIcon node,
          beam,
          bearingH,
          bearingV,
          bearingM,
          bearingHV,
          bearingHM,
          bearingVM,
		  bearingHVM,
          jointM,
          jointN,
          jointQ,
          jointR,
          loadN,
          moment,
          loadU,
          loadD,
          loadI,
          temp,
          tempDiff,
          tempChange,
          doubleL,
          cross,
          plus,
          gridI,
          catcher,
          undo,
          redo,
          copy,
          paste,
          cut,
          select,
          move,
          calc,
          //calc_red,
          //calc_green,
          window,
          windows,
          zoom,
          zoomall,
          zoomin,
          zoomout,
          zoomrect,
          zoomsel,
          open,
          save,
          settings,
          crossing,
          middle,
          ortho_g,
          ortho_l,
          spring,
          mat,
          prof,
          ok,
		  vp_node,
		  vp_beam,
          vp_jointN,
          vp_jointM,
          vp_jointQ,
		  vp_copy,
		  vp_crossing,
		  vp_cut,
		  vp_dstload,
		  vp_istload,
		  vp_ustload,
          vp_doubleload,
		  vp_nload,
		  vp_moment,
		  vp_grid,
		  vp_magnet,
		  vp_middle,
		  vp_ortho_g,
		  vp_ortho_l,
		  vp_paste,
		  vp_temp,
		  vp_cls,
		  node_plus,
		  beam_plus,
		  load_plus,
		  dstload_plus,
		  istload_plus,
		  ustload_plus,
		  doubleload_plus,
		  moment_plus,
		  tempDiff_plus,
		  tempChange_plus,
		  bls,
		  cls,
		  bls_plus,
		  cls_plus,
		  whatsthis,
		  leftBeam,
		  rightBeam;


    QPixmap key,
            key_dark;

    QIcon placeholder;

    bool dark;
};

#endif // COLINICONS_H
