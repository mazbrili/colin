#include "newmaterialdialog.h"
#include "colinicons.h"
#include "wgv_lib.h"
#include "unitsettings.h"

#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>

newMaterialDialog::newMaterialDialog(QWidget *parent) :
    QDialog(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    QHBoxLayout *buttonbox = new QHBoxLayout();
    QPushButton *okButton = new QPushButton(colinIcons::instance().icon(Colin::New), tr("ok"), this);
    QPushButton *cancelButton = new QPushButton(colinIcons::instance().icon(Colin::Close), tr("return"), this);

    buttonbox->addStretch(500);
    buttonbox->addWidget(cancelButton);
    buttonbox->addWidget(okButton);

    QLabel *ELab = new QLabel("E", this);
    QLabel *aLab = new QLabel(QChar(0x03B1), this);

    NLine = new QLineEdit(tr("name"), this);
    ELine = new QLineEdit(QString::number(2e+08*unitSettings::instance().Eprefix()), this);
    aLine = new QLineEdit(QString::number(1e-05), this);

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


void newMaterialDialog::ok()
{
    if(LIB.IndexFromMatByName(NLine->text())>-1)
        return;
    double E, a;
    bool ok = true;
    E = ELine->text().toDouble(&ok)/unitSettings::instance().Eprefix();
    if(!ok) return;
    a = aLine->text().toDouble(&ok);
    if(!ok) return;

    wgv_mat m(E, a, NLine->text());
    LIB.addMat(m);
    close();
}

void newMaterialDialog::cancel()
{
    close();
}
