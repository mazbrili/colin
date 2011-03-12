#include "editmaterialdialog.h"
#include "colinicons.h"
#include "wgv_lib.h"
#include "unitsettings.h"

#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>

editMaterialDialog::editMaterialDialog(const int &i, QWidget *parent) :
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

    QLabel *ELab = new QLabel("E", this);
    QLabel *aLab = new QLabel("alpha", this);

    NLine = new QLineEdit(LIB.mat(i).name(), this);
    NLine->setReadOnly(true);
    ELine = new QLineEdit(QString::number(LIB.mat(i).E()), this);
    aLine = new QLineEdit(QString::number(LIB.mat(i).alphaT()), this);

    ELine->setAlignment(Qt::AlignRight);
    aLine->setAlignment(Qt::AlignRight);

    QLabel *Eeh = new QLabel(unitSettings::instance().Eeh(), this);
    QLabel *aeh = new QLabel("1/K", this);

    layout->addWidget(NLine, 0, 0, 1, 3);

    layout->addWidget(ELab, 1, 0, 1, 1);
    layout->addWidget(ELine, 1, 1, 1, 1);
    layout->addWidget(Eeh, 1, 2, 1, 1);

    layout->addWidget(aLab, 2, 0, 1, 1);
    layout->addWidget(aLine, 2, 1, 1, 1);
    layout->addWidget(aeh, 2, 2, 1, 1);

    layout->addLayout(buttonbox, 3, 0, 1, 3);

    connect(okButton,               SIGNAL(clicked()),
            this,                   SLOT(ok()));

    connect(cancelButton,           SIGNAL(clicked()),
            this,                   SLOT(cancel()));

    setWindowIcon(colinIcons::instance().icon(Colin::Material));
    setAttribute(Qt::WA_QuitOnClose, true);
}


void editMaterialDialog::ok()
{
    double E, a;
    bool ok = true;
    E = ELine->text().toDouble(&ok)/unitSettings::instance().Eprefix();
    if(!ok) return;
    a = aLine->text().toDouble(&ok);
    if(!ok) return;

    wgv_mat m(E, a, NLine->text());
    LIB.editMat(nr, m);
    close();
}

void editMaterialDialog::cancel()
{
    close();
}
