#ifndef WGV_LIB_H
#define WGV_LIB_H

#include <QtCore/QCoreApplication>
#include <QtCore/QObject>
#include <QtCore/QSettings>



#include "wgv_mat.h"
#include "wgv_profile.h"

#define LIB wgv_lib::instance()

class wgv_lib : public QObject
{
    Q_OBJECT
public:
    static wgv_lib &instance(){
    if(!instance_)
        instance_ = new wgv_lib();
    return *instance_;}

    const inline wgv_mat &mat(const int &i) const {return mats.at(i);}
    const inline wgv_mat &mat(const QString &_name) const {return mat(IndexFromMatByName(_name));}
    int mats_n() const {return (mats.size());}
    int IndexFromMatByName(const QString &_name) const {
        for(int i=0; i<mats_n(); i++){
            if(mat(i).name()==_name) return i;}
                return -1;}
    void addProfile(const wgv_profile &n);
    void editProfile(const int &nr, const wgv_profile &n);
    void removeProfile(const int &nr);


    const inline wgv_profile &Profile(const int &i) const {return profiles.at(i);}
    const inline wgv_profile &Profile(const QString &_name) const {return Profile(IndexFromProfileByName(_name));}
    int profiles_n() const {return (profiles.size());}
    int IndexFromProfileByName(const QString &_name) const {
        for(int i=0; i<profiles_n(); i++){
             if(Profile(i).name()==_name) return i;}
            return -1;}
    void addMat(const wgv_mat &n);
    void editMat(const int &nr, const wgv_mat &n);
    void removeMat(const int &nr);

    void setCurrentMaterial(const int &i) {currentM = i; emit currentMChanged(i);}
    void setCurrentProfile(const int &i) {currentP = i; emit currentPChanged(i);}

    const inline int &currentMaterial() const {return currentM;}
    const inline int &currentProfile() const {return currentP;}

    void setUp();

signals:
    void matRemoved(const int &nr);
    void matEdited(const int &nr);
    void matAdded();

    void profileRemoved(const int &nr);
    void profileEdited(const int &nr);
    void profileAdded();

    void currentMChanged(const int &i);
    void currentPChanged(const int &i);

public slots:
    void dump();
private:

    wgv_lib();
    int currentM;
    int currentP;

    static wgv_lib *instance_;
    QList<wgv_mat> mats;
    QList<wgv_profile> profiles;
};

#endif // WGV_LIB_H
