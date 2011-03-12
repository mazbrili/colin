#ifndef COLINICONS_H
#define COLINICONS_H

#include <QtGui/QIcon>
#include <QtGui/QPainter>
#include <QtGui/QLinearGradient>
#include <QtGui/QKeySequence>
#include <QtCore/QStringList>

#include "colin_namespace.h"
#include "wgv_bearing.h"
#include "viewportsettings.h"
#include "catcher.h"
#include "wgv_load.h"

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
    const QIcon &icon(const wgv_load::form &f, bool tooltip = false) const;
    const QIcon &icon(const wgv_bearing::bearings &b) const;
    //const QIcon &icon(const Colin::otherAction &c, const int &best) const;
    QIcon icon(const Colin::Color &c) const;



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
	  vp_temp;


    QPixmap key,
            key_dark;

    QIcon placeholder;

    bool dark;
};

#endif // COLINICONS_H
