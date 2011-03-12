#ifndef UNITSETTINGS_H
#define UNITSETTINGS_H

#include <QtCore/QObject>
#include <cmath>


#define EPREFIX unitSettings::instance().Eprefix()
#define FPREFIX unitSettings::instance().Fprefix()
#define APREFIX unitSettings::instance().Aprefix()
#define IPREFIX unitSettings::instance().Iprefix()
#define HPREFIX unitSettings::instance().hprefix()
#define PPREFIX unitSettings::instance().Pprefix()
#define MPREFIX unitSettings::instance().Mprefix()
#define ANGLEPREFIX (180/M_PI)
#define PRECISON unitSettings::instance().precison()


class unitSettings : public QObject
{
    Q_OBJECT
public:
    enum EEH
    {
        N_m2 = 1,
        kN_m2,
        kN_cm2,
        N_cm2
    };

    enum ProfEH
    {
        m = 1,
        cm,
        mm
    };

    enum PMEH
    {
        Nm = 1,
        Ncm,
        kNm,
        kNcm
    };

    static unitSettings& instance()
    {
        if(!instance_)
            instance_ = new unitSettings();
        return *instance_;
    }
    inline const EEH &EU(){return EUnit;}
    inline const ProfEH &IU(){return ProfUnit;}
    inline const PMEH &PU(){return PMUnit;}

    QString Eeh() const;
    QString Feh() const;
    QString Peh() const;
    QString Meh() const;
    inline QString Aeh() const {return Profeh()+QChar(0x00B2);}
    inline QString Ieh() const {return Profeh()+QChar(0x2074);}
    inline QString heh() const {return Profeh();}
    QString Profeh() const;

    double Eprefix() const;
    double Fprefix() const;
    double Pprefix() const;
    double Mprefix() const;
    inline double Aprefix() const {return pow(Profprefix(),2);}
    inline double Iprefix() const {return pow(Profprefix(),4);}
    inline double hprefix() const {return Profprefix();}
    double Profprefix() const;
    int precison() const {return prec;}
    void setPrecison(const int &p);
    void saveSettings();
    void loadSettings();
public slots:
    void setPEh(unitSettings::PMEH eh);
    void setEEh(unitSettings::EEH eh);
    void setIEh(unitSettings::ProfEH eh);

signals:
    void settingschanged();
private:
    Q_DISABLE_COPY(unitSettings);
    unitSettings():QObject(){loadSettings();}
    ~unitSettings(){saveSettings();}
    static unitSettings *instance_;
    int prec;
    EEH EUnit;
    ProfEH ProfUnit;
    PMEH PMUnit;

};

#endif // UNITSETTINGS_H
