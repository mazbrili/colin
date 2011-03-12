#ifndef WGV_MAT_H
#define WGV_MAT_H

#include <QtCore/QString>
#include <QtGui/QIcon>

class wgv_mat
{
public:
    wgv_mat(){}
    wgv_mat(const double &_E, /*const double &_v, const double &_roh,*/ const double &_alphaT, const QString &_name)
    {e=_E; /*Dingsda=_v; r=_roh; */at=_alphaT; n=_name;}

    const double &E() const {return e;}
    //const double &v() const {return Dingsda;}
    //const double &roh() const {return r;}
    const double &alphaT() const {return at;}
    const QString &name() const {return n;}
    //const QString &iconname() const {return in;}
    //QIcon icon() const {return(QIcon(iconname()));}


    void setE(const double &_E){e=_E;}
    //void setV(const double &_v){Dingsda=_v;}
    //void setRoh(const double &_roh){r=_roh;}
    void setAlphaT(const double &_alphaT){at=_alphaT;}
    void setName(const QString &_name){n=_name;}
    //void setIconName(const QString &_name){in=_name;}


    friend bool operator!=(wgv_mat const& one, wgv_mat const& sec)
    {
        if(one.E() != sec.E()) return true;
        //if(one.v() != sec.v()) return true;
        //if(one.roh() != sec.roh()) return true;
        if(one.alphaT() != sec.alphaT()) return true;
        return false;
    }
    friend bool operator==(wgv_mat const& one, wgv_mat const& sec)
    {
        return !(one!=sec);
    }

private:
    double e;
//    double r;
    double at;
//    double Dingsda;
    QString n;
//    QString in;
};

#endif // WGV_MAT_H
