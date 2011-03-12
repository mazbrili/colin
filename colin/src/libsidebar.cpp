#include "libsidebar.h"
#include "colinicons.h"
#include "newprofiledialog.h"
#include "newmaterialdialog.h"
#include "editprofiledialog.h"
#include "editmaterialdialog.h"
#include "unitsettings.h"

libSidebar::libSidebar(QWidget *parent) :
    QSplitter(parent)
{
    Q_ASSERT(LIB.mats_n()>-1);
    Q_ASSERT(LIB.profiles_n()>-1);

    oldCurrentM = 0;
    oldCurrentP = 0;

    this->setOrientation(Qt::Vertical);

    QWidget *layoutHolder = new QWidget(this);
    layoutHolder->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    layout = new QGridLayout(layoutHolder);

#ifdef LIB_ICON
    proficon = new QLabel(this);
    proficon->setPixmap(colinIcons::instance().icon(Colin::Profile).pixmap(QSize(64, 64)));
    proficon->setFixedSize(64, 64);
#endif
    profname = new QLabel(LIB.Profile(0).name());
    QFont f = profname->font();
    f.setPixelSize(18);
    profname->setFont(f);
    profvals = new QLabel("A = "+QString::number(LIB.Profile(0).A()*APREFIX)+" "+ unitSettings::instance().Aeh() +
                          "\nI = "+QString::number(LIB.Profile(0).I()*IPREFIX)+" "+ unitSettings::instance().Ieh() +
                          "\nh = "+QString::number(LIB.Profile(0).h()*HPREFIX)+" "+ unitSettings::instance().heh());

    QVBoxLayout *profButtonBox = new QVBoxLayout();

    newP = new QPushButton(colinIcons::instance().icon(Colin::New), "", this);
    editP = new QPushButton(colinIcons::instance().icon(Colin::Settings), "", this);

    profButtonBox->addWidget(newP);
    profButtonBox->addWidget(editP);

#ifdef LIB_ICON
    maticon = new QLabel(this);
    maticon->setPixmap(colinIcons::instance().icon(Colin::Material).pixmap(QSize(64, 64)));
    maticon->setFixedSize(64, 64);
#endif
    matname = new QLabel(LIB.mat(0).name());
    matname->setFont(f);
    matvals = new QLabel("E = "+QString::number(LIB.mat(0).E()*EPREFIX) +" "+ unitSettings::instance().Eeh() +
                         "\n" +QChar(0x03B1)+" = "+QString::number(LIB.mat(0).alphaT())+" K");

    QVBoxLayout *matButtonBox = new QVBoxLayout();

    newM = new QPushButton(colinIcons::instance().icon(Colin::New), "", this);
    editM = new QPushButton(colinIcons::instance().icon(Colin::Settings), "", this);

    matButtonBox->addWidget(newM);
    matButtonBox->addWidget(editM);

#ifdef LIB_ICON
    layout->addWidget(proficon, 0, 0, 2, 1);
#endif
    layout->addWidget(profname, 0, 1, 1, 1);
    layout->addWidget(profvals, 1, 1, 1, 1);
    layout->addLayout(profButtonBox, 0, 2, 2, 1);


#ifdef LIB_ICON
    layout->addWidget(maticon, 0, 3, 2, 1);
#endif
    layout->addWidget(matname, 0, 4, 1, 1);
    layout->addWidget(matvals, 1, 4, 1, 1);
    layout->addLayout(matButtonBox, 0, 5, 2, 1);

#ifdef LIB_ICON
    layout->setColumnMinimumWidth(0, 64);
    layout->setColumnMinimumWidth(3, 64);
#endif

    profiles = new QTreeWidget(this);
    mats = new QTreeWidget(this);



    mats->setIconSize(QSize(16, 16));
    profiles->setIconSize(QSize(16, 16));
    for(int i=0; i< LIB.mats_n(); i++)
    {
        appendMat();
    }

    for(int i=0; i< LIB.profiles_n(); i++)
    {
        appendProfile();
    }

    mats->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    profiles->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    layout->setSpacing(2);
    layout->setContentsMargins(QMargins(2, 0, 0, 2));


    actualUnits();

    connect(&LIB,		        SIGNAL(matAdded()),
	    this,		        SLOT(appendMat()));

    connect(&LIB,		        SIGNAL(profileAdded()),
	    this,		        SLOT(appendProfile()));

    connect(&LIB,		        SIGNAL(matRemoved(int)),
	    this,		        SLOT(removeMat(int)));

    connect(&LIB,		        SIGNAL(profileRemoved(int)),
	    this,		        SLOT(removeProfile(int)));

    connect(&LIB,		        SIGNAL(matEdited(int)),
	    this,		        SLOT(setMat(int)));

    connect(&LIB,		        SIGNAL(profileEdited(int)),
	    this,		        SLOT(setProfile(int)));

    connect(&LIB,		        SIGNAL(currentPChanged(int)),
	    this,		        SLOT(setActiveP(int)));

    connect(&LIB,		        SIGNAL(currentMChanged(int)),
	    this,		        SLOT(setActiveM(int)));

    connect(newP,		        SIGNAL(clicked()),
	    this,		        SLOT(launchDialogNewP()));

    connect(newM,		        SIGNAL(clicked()),
	    this,		        SLOT(launchDialogNewM()));

    connect(editP,		        SIGNAL(clicked()),
	    this,		        SLOT(launchDialogEditP()));

    connect(editM,		        SIGNAL(clicked()),
	    this,		        SLOT(launchDialogEditM()));

    connect(mats,		        SIGNAL(clicked(QModelIndex)),
	    this,		        SLOT(activatedM(QModelIndex)));

    connect(profiles,		        SIGNAL(clicked(QModelIndex)),
	    this,		        SLOT(activatedP(QModelIndex)));

    connect(mats,		        SIGNAL(doubleClicked(QModelIndex)),
	    this,		        SLOT(launchDialogEditM(QModelIndex)));

    connect(profiles,		        SIGNAL(doubleClicked(QModelIndex)),
	    this,			SLOT(launchDialogEditP(QModelIndex)));

    connect(&unitSettings::instance(),  SIGNAL(settingschanged()),
	    this,			SLOT(actualUnits()));
}


void libSidebar::appendMat()
{
    int i = mats->topLevelItemCount();
    QTreeWidgetItem *item = new QTreeWidgetItem(mats);
    item->setIcon(0, colinIcons::instance().icon(Colin::Material));
    setMat(i);

}

void libSidebar::appendProfile()
{
    int i = profiles->topLevelItemCount();
    QTreeWidgetItem *item = new QTreeWidgetItem(profiles);
    item->setIcon(0, colinIcons::instance().icon(Colin::Profile));
    setProfile(i);
}

void libSidebar::setActiveP(const int &i)
{
    profiles->topLevelItem(oldCurrentP)->setIcon(0, colinIcons::instance().icon(Colin::Profile));
    profiles->topLevelItem(i)->setIcon(0, colinIcons::instance().icon(Colin::Ok));
    profname->setText(LIB.Profile(i).name());
    profvals->setText("A = "+QString::number(LIB.Profile(i).A()*APREFIX)+" "+ unitSettings::instance().Aeh() +
                      "\nI = "+QString::number(LIB.Profile(i).I()*IPREFIX)+" "+ unitSettings::instance().Ieh() +
                      "\nh = "+QString::number(LIB.Profile(i).h()*HPREFIX)+" "+ unitSettings::instance().heh());
    oldCurrentP = i;

}

void libSidebar::setActiveM(const int &i)
{
    mats->topLevelItem(oldCurrentM)->setIcon(0, colinIcons::instance().icon(Colin::Material));
    mats->topLevelItem(i)->setIcon(0, colinIcons::instance().icon(Colin::Ok));
    matname->setText(LIB.mat(i).name());
    matvals->setText("E = "+QString::number(LIB.mat(i).E()*EPREFIX) +" "+ unitSettings::instance().Eeh() +
                     "\n"+ QChar(0x03B1)+" = "+QString::number(LIB.mat(i).alphaT())+" K");
    oldCurrentM = i;

}

void libSidebar::removeMat(const int &o)
{
    delete mats->topLevelItem(o);
}

void libSidebar::removeProfile(const int &o)
{
    delete profiles->topLevelItem(o);
}

void libSidebar::launchDialogNewP()
{
    newProfileDialog *dia = new newProfileDialog(this);
    dia->exec();
}

void libSidebar::launchDialogNewM()
{
    newMaterialDialog *dia = new newMaterialDialog(this);
    dia->exec();
}

void libSidebar::launchDialogEditP()
{
    if(profiles->selectedItems().count() < 1)
        return;
    editProfileDialog *dia = new editProfileDialog(LIB.IndexFromProfileByName(profiles->selectedItems().at(0)->text(0)), this);
    dia->exec();
}

void libSidebar::launchDialogEditM()
{
    if(mats->selectedItems().count() < 1)
        return;
    editMaterialDialog *dia = new editMaterialDialog(LIB.IndexFromMatByName(mats->selectedItems().at(0)->text(0)), this);
    dia->exec();
}

void libSidebar::launchDialogEditM(const QModelIndex &mI)
{
    editMaterialDialog *dia = new editMaterialDialog(mI.row(), this);
    dia->exec();
}

void libSidebar::launchDialogEditP(const QModelIndex &mI)
{
    editProfileDialog *dia = new editProfileDialog(mI.row(), this);
    dia->exec();
}

void libSidebar::activatedP(const QModelIndex &mI)
{
    LIB.setCurrentProfile(mI.row());
}

void libSidebar::activatedM(const QModelIndex &mI)
{
    LIB.setCurrentMaterial(mI.row());
}

void libSidebar::setProfile(const int &o)
{
    QTreeWidgetItem *item = profiles->topLevelItem(o);
    item->setText(0, LIB.Profile(o).name());
    item->setText(1, QString::number(LIB.Profile(o).A()*APREFIX));
    item->setText(2, QString::number(LIB.Profile(o).I()*IPREFIX));
    item->setText(3, QString::number(LIB.Profile(o).h()*HPREFIX));
}

void libSidebar::setMat(const int &o)
{
    QTreeWidgetItem *item = mats->topLevelItem(o);
    item->setText(0, LIB.mat(o).name());
    item->setText(1, QString::number(LIB.mat(o).E()*EPREFIX));
    item->setText(2, QString::number(LIB.mat(o).alphaT()));
}

void libSidebar::actualUnits()
{
    for(int i=0; i<profiles->topLevelItemCount(); i++)
    {
        setProfile(i);
    }
    for(int i=0; i<mats->topLevelItemCount(); i++)
    {
        setMat(i);
    }
    setActiveP(LIB.currentProfile());
    setActiveM(LIB.currentMaterial());
    QStringList header;
    header.append(tr("material"));
    header.append(QString("E[")+unitSettings::instance().Eeh()+"]");
    header.append(QString("%1%2").arg(QChar(0x03B1)).arg("[1/K]"));
    mats->setColumnCount(3);
    mats->setHeaderLabels(header);

    header.clear();

    header.append(tr("cross section"));
    header.append(QString("A[")+unitSettings::instance().Aeh()+"]");
    header.append(QString("I[")+unitSettings::instance().Ieh()+"]");
    header.append(QString("h[")+unitSettings::instance().heh()+"]");
    profiles->setColumnCount(4);
    profiles->setHeaderLabels(header);
}
