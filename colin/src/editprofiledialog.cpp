#include "editprofiledialog.h"
#include "newprofiledialog.h"
#include "colinicons.h"
#include "wgv_lib.h"
#include "unitsettings.h"

#include <QtGui/QLabel>
#include <QtGui/QPushButton>


editProfileDialog::editProfileDialog(const int &i, QWidget *parent) :
    QDialog(parent)
{
    nr = i;
    QGridLayout *layout = new QGridLayout(this);
    QHBoxLayout *buttonbox = new QHBoxLayout();
    QPushButton *okButton = new QPushButton(colinIcons::instance().icon(Colin::Ok), tr("ok"), this);
    QPushButton *cancelButton = new QPushButton(colinIcons::instance().icon(Colin::Close), tr("return"), this);

    buttonbox->addStretch(500);
    buttonbox->addWidget(cancelButton);
    buttonbox->addWidget(okButton);

    QLabel *ALab = new QLabel("A", this);
    QLabel *ILab = new QLabel("I", this);
    QLabel *hLab = new QLabel("h", this);

    NLine = new QLineEdit(LIB.Profile(i).name(), this);
    NLine->setReadOnly(true);
    ALine = new QLineEdit(QString::number(LIB.Profile(i).A()*APREFIX), this);
    ILine = new QLineEdit(QString::number(LIB.Profile(i).I()*IPREFIX), this);
    hLine = new QLineEdit(QString::number(LIB.Profile(i).h()*HPREFIX), this);

    ALine->setAlignment(Qt::AlignRight);
    ILine->setAlignment(Qt::AlignRight);
    hLine->setAlignment(Qt::AlignRight);

    QLabel *Aeh = new QLabel(unitSettings::instance().Aeh(), this);
    QLabel *Ieh = new QLabel(unitSettings::instance().Ieh(), this);
    QLabel *heh = new QLabel(unitSettings::instance().heh(), this);

    layout->addWidget(NLine, 0, 0, 1, 3);

    layout->addWidget(ALab, 1, 0, 1, 1);
    layout->addWidget(ALine, 1, 1, 1, 1);
    layout->addWidget(Aeh, 1, 2, 1, 1);

    layout->addWidget(ILab, 2, 0, 1, 1);
    layout->addWidget(ILine, 2, 1, 1, 1);
    layout->addWidget(Ieh, 2, 2, 1, 1);

    layout->addWidget(hLab, 3, 0, 1, 1);
    layout->addWidget(hLine, 3, 1, 1, 1);
    layout->addWidget(heh, 3, 2, 1, 1);

    layout->addLayout(buttonbox, 4, 0, 1, 3);

    setWindowIcon(colinIcons::instance().icon(Colin::Profile));
    setAttribute(Qt::WA_QuitOnClose, true);

    connect(okButton,               SIGNAL(clicked()),
            this,                   SLOT(ok()));

    connect(cancelButton,           SIGNAL(clicked()),
            this,                   SLOT(cancel()));

}

void editProfileDialog::ok()
{
    double A, I, h;
    bool ok = true;
    A = ALine->text().toDouble(&ok)/APREFIX;
    if(!ok) return;
    I = ILine->text().toDouble(&ok)/IPREFIX;
    if(!ok) return;
    h = hLine->text().toDouble(&ok)/HPREFIX;
    if(!ok) return;

    wgv_profile p(A, I, h, NLine->text());
    LIB.editProfile(nr, p);
    close();
}

void editProfileDialog::cancel()
{
    close();
}
