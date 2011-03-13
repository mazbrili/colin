#include "wgv_lib.h"

wgv_lib *wgv_lib::instance_ = NULL;

wgv_lib::wgv_lib()
{
    connect(qApp,           SIGNAL(aboutToQuit()),
            this,           SLOT(dump()));

    QSettings settings("clazzes.org", "Colin");

    bool ok;
    int matCount = settings.value("lib/matCount", QVariant(int(1))).toInt(&ok);
    if(!ok)
        matCount = 1;
    for(int i=0; i<matCount; i++)
    {
        QString name = settings.value("lib/mat"+QString::number(i)+"_name", QVariant(QString("IDontCare"))).toString();
        double E = settings.value("lib/mat"+QString::number(i)+"_E", QVariant(double(2e+11))).toDouble(&ok);
        if(!ok)
            break;
        double alphaT = settings.value("lib/mat"+QString::number(i)+"_alphaT", QVariant(double(1e-05))).toDouble(&ok);
        if(!ok)
            break;
        this->addMat(wgv_mat(E, alphaT, name));
    }

    int profileCount = settings.value("lib/profileCount", QVariant(int(1))).toInt(&ok);
    if(!ok)
        profileCount = 1;
    for(int i=0; i<matCount; i++)
    {
        QString name = settings.value("lib/profile"+QString::number(i)+"_name", QVariant(QString("IDontCare"))).toString();
        double A = settings.value("lib/profile"+QString::number(i)+"_A", QVariant(double(1e-02))).toDouble(&ok);
        if(!ok)
            break;
        double I = settings.value("lib/profile"+QString::number(i)+"_I", QVariant(double(1e-04))).toDouble(&ok);
        if(!ok)
            break;
        double h = settings.value("lib/profile"+QString::number(i)+"_h", QVariant(double(1e-01))).toDouble(&ok);
        if(!ok)
            break;
        this->addProfile(wgv_profile(A, I, h, name));

        currentM = 0;
        currentP = 0;
    }
}


void wgv_lib::addProfile(const wgv_profile &n)
{
    profiles.append(n);
    emit profileAdded();
}
void wgv_lib::editProfile(const int &nr, const wgv_profile &n)
{
     Q_ASSERT(!(nr<0 || nr>=profiles_n()));
     profiles[nr]=n;
     emit profileEdited(nr);
}
void wgv_lib::removeProfile(const int &nr)
{
    Q_ASSERT(!(nr<0 || nr>=profiles_n()));
    profiles.removeAt(nr);
    emit profileRemoved(nr);
}

void wgv_lib::addMat(const wgv_mat &n)
{
    mats.append(n);
    emit matAdded();
}
void wgv_lib::editMat(const int &nr, const wgv_mat &n)
{
    Q_ASSERT(!(nr<0 || nr>=profiles_n()));
    mats[nr]=n;
    emit matEdited(nr);
}
void wgv_lib::removeMat(const int &nr)
{
    Q_ASSERT(!(nr<0 || nr>=profiles_n()));
    mats.removeAt(nr);
    emit matRemoved(nr);
}

void wgv_lib::dump()
{

    QSettings settings("clazzes.org", "Colin");
    settings.remove("lib");
    settings.setValue("lib/matCount", mats_n());
    settings.setValue("lib/profileCount", profiles_n());
    for(int i=0; i<mats_n(); i++)
    {
        settings.setValue("lib/mat"+QString::number(i)+"_name", mat(i).name());
        settings.setValue("lib/mat"+QString::number(i)+"_E", mat(i).E());
        settings.setValue("lib/mat"+QString::number(i)+"_alphaT", mat(i).alphaT());
    }

    for(int i=0; i<profiles_n(); i++)
    {
        settings.setValue("lib/profile"+QString::number(i)+"_name", Profile(i).name());
        settings.setValue("lib/profile"+QString::number(i)+"_A", Profile(i).A());
        settings.setValue("lib/profile"+QString::number(i)+"_I", Profile(i).I());
        settings.setValue("lib/profile"+QString::number(i)+"_h", Profile(i).h());
    }
}

void wgv_lib::setUp()
{
    if(profiles_n()<1)
	addProfile(wgv_profile(1e-02, 1e-04, 1e-01, "IDontCare"));
    if(mats_n()<1)
	addMat(wgv_mat(2e+11, 1e-05, "IDontCare"));
}
