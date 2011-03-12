#ifndef WGV_BEARING_H
#define WGV_BEARING_H

#include "colin_namespace.h"
#include "wgv_element.h"


class wgv_bearing : public wgv_element
{
public:
    enum bearing{
        NoBear =    0x0,
        X =         0x1,
        Z =         0x2,
        Phi =       0x4,

        XZ =        X | Z,
        XPhi =      X | Phi,
        ZPhi =      Z | Phi,
        XZPhi =     X | Z | Phi,

        fx =        0x8,
        fz =        0x10,
        fphi =      0x20,

        nX =        0xFFFFFFFF & ~X,
        nZ =        0xFFFFFFFF & ~Z,
        nPhi =      0xFFFFFFFF & ~Phi,
        nFX =       (0xFFFFFFFF & ~X) & ~fx,
        nFZ =       (0xFFFFFFFF & ~Z) & ~fz,
        nFPhi =     (0xFFFFFFFF & ~Phi) & ~fphi,
        nF =        ((0xFFFFFFFF & ~fx) & ~fz) & ~fphi
    };
    Q_DECLARE_FLAGS(bearings, bearing);

    static Colin::Actions enumToEnum(const wgv_bearing::bearings &b){
        Colin::Actions a;
        if((b&X)==X) a |= Colin::drawBearingH;
        if((b&Z)==Z) a |= Colin::drawBearingV;
        if((b&Phi)==Phi) a |= Colin::drawBearingM;
        return a;
    }


    static Colin::Actions formToDrawEnum(const wgv_bearing::bearings &b){
        Colin::Actions a = Colin::drawBearing;
        if(b.testFlag(wgv_bearing::X)) a |= Colin::drawBearingH;
        if(b.testFlag(wgv_bearing::Z)) a |= Colin::drawBearingV;
        if(b.testFlag(wgv_bearing::Phi)) a |= Colin::drawBearingM;
        return a;
    }


    wgv_bearing();
    wgv_bearing(Colin::Actions a);


    inline bool x() const {return (b&X)==X;}
    inline bool z() const {return (b&Z)==Z;}
    inline bool phi() const {return (b&Phi)==Phi;}
    inline bool xf() const {return (b&fx)==fx;}
    inline bool zf() const {return (b&fz)==fz;}
    inline bool phif() const {return (b&fphi)==fphi;}
    inline const wgv_bearing::bearings &form() const {return b;}
    inline wgv_bearing::bearings simpleform() const {return b & nF;}

    const inline double &c_x() const {return f_x;}
    const inline double &c_z() const {return f_z;}
    const inline double &c_phi() const {return f_phi;}

    const inline double &a_x() const {return ax;}
    const inline double &a_z() const {return az;}
    const inline double &a_m() const {return am;}

    const inline double &angle() const {return alp;}

    inline void setForm(const wgv_bearing::bearings b_){b=b_;}
    inline void setX(const bool& Xn) {(Xn)?b|=X:b&=nFX; setCx(0);}
    inline void setZ(const bool& Zn) {(Zn)?b|=Z:b&=nFZ; setCz(0);}
    inline void setPhi(const bool& Phin) {(Phin)?b|=Phi:b&=nFPhi; setCphi(0);}

    inline void setCx(const double& c) {f_x=c; if(c==0) b=b&~fx; else b=(b&nX)|fx;}
    inline void setCz(const double& c) {f_z=c; if(c==0) b=b&~fz; else b=(b&nZ)|fz;}
    inline void setCphi(const double& c) {f_phi=c; if(c==0) b=b&~fphi; else b=(b&nPhi)|fphi;}

    inline void setAngle(const double& alpha) {alp=alpha;}

    inline void setR(const double & Au, const double& Aw, const double &Aphi) {ax=Au; az=Aw; am=Aphi;}



    friend bool operator!=(wgv_bearing const& one, wgv_bearing const& sec)
    {
        if(one.form() != sec.form()) return true;
        if(one.c_x()!=sec.c_x()) return true;
        if(one.c_z()!=sec.c_z()) return true;
        if(one.c_phi()!=sec.c_phi()) return true;
        if(one.angle()!=sec.angle()) return true;
        return false;
    }


private:

    bearings b;

    double f_x;
    double f_z;
    double f_phi;

    double alp;


    double ax;
    double az;
    double am;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(wgv_bearing::bearings);

#endif // WGV_BEARING_H
