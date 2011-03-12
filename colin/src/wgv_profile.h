#ifndef WGV_PROFILE_H
#define WGV_PROFILE_H

#include <QtCore/QString>
#include <QtGui/QPolygon>
#include <QtGui/QPixmap>
#include <QtGui/QIcon>
#include <QtCore/QPointF>
#include <QtGui/QPainterPath>
#include <QtCore/QList>



class wgv_profile
{
public:
    wgv_profile(){}
    wgv_profile(const double &_A, const double &_I, /*const double &_Aq,*/ const double &_h, const QString &_name)
    {a=_A; i=_I; /*aq=_Aq;*/  height=_h; n=_name;}
    const inline double &A() const {return a;}
    const inline double &I() const {return i;}
    //const inline double &Aq() const {return aq;}
    const inline double &h() const {return height;}
    const inline QString &name() const {return n;}
    //const inline QString &iconname() const {return in;}
    //inline QIcon icon() const {return(QIcon(iconname()));}

    inline void setName(const QString &name_){n = name_;}
    inline void setA(const double &A_){a=A_;}
    inline void setI(const double &I_){i=I_;}
    //inline void setAq(const double &A_){aq=A_;}
    inline void setH(const double &h_){height=h_;}
    //inline void setIconName(const QString &_name){in=_name;}


    friend bool operator!=(wgv_profile const& one, wgv_profile const& sec)
    {
        if(one.A() != sec.A()) return true;
        if(one.I() != sec.I()) return true;
        if(one.h() != sec.h()) return true;
        return false;
    }
    friend bool operator==(wgv_profile const& one, wgv_profile const& sec)
    {
        return !(one!=sec);
    }

private:
    double a;
    double i;
    //double aq;
    double height;
    QString n;
    //QString in;

};

#endif // WGV_PROFILE_H
