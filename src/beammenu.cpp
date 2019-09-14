/***********************************************************
 * $Id$
 *
 * Colin
 *
 * Copyright (C) 2011 Matthias Rauter (matthias.rauter@student.uibk.ac.at)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details."
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see
 *
 * http://www.gnu.org/licenses/.
 *
 * Author: Matthias Rauter
 *
 ***********************************************************/

#include "beammenu.h"
#include "unitsettings.h"


#include <QApplication>
#include <QClipboard>


beamMenu *beamMenu::instance_ = NULL;


beamMenu::beamMenu() :
    ColinMenu()
{
    QAction *act;
    act = addAction(colinIcons::instance().icon(Colin::drawBeam), tr("beam"));
    act->setDisabled(true);
    addSeparator();

    act = addAction(colinIcons::instance().icon(Colin::drawNode), tr("left node"));
    left = new QLineEdit(this);
    left->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    left->setAlignment(Qt::AlignRight);
    addWidgetForAction(act, left);

    act = addAction(colinIcons::instance().icon(Colin::drawNode), tr("right node"));
    right = new QLineEdit(this);
    right->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    right->setAlignment(Qt::AlignRight);
    addWidgetForAction(act, right);
    addSeparator();

    act = addAction(colinIcons::instance().icon(Colin::Profile), tr("cross section"));
    prof = new QComboBox(this);
    prof->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    for(int i=0; i<LIB.profiles_n(); i++)
    {
        prof->addItem(LIB.Profile(i).name());
    }
    addWidgetForAction(act, prof);

    act = addAction(colinIcons::instance().icon(Colin::Material), tr("material"));
    mat = new QComboBox(this);
    mat->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    for(int i=0; i<LIB.mats_n(); i++)
    {
        mat->addItem(LIB.mat(i).name());
    }
    addWidgetForAction(act, mat);

    addSeparator();

    act = addAction(colinIcons::instance().icon(Colin::drawJoint), tr("hinge"));

    addSeparator();

    lJ = new ColinPushButtonPart(colinIcons::instance().icon(Colin::drawJoint),"");
    lJ->setCheckable(true);
    rJ = new ColinPushButtonPart(colinIcons::instance().icon(Colin::RightJoint),"");
    rJ->setCheckable(true);

    ColinHMultiButton *multi = new ColinHMultiButton(this);

    multi->addButton(lJ);
    multi->addButton(rJ);

    multi->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    addWidgetForAction(act, multi);

    QAction *a[6];
    int c = 0;
    jointMenu = new ColinMenu(tr("more joints"));
    a[c++] = jointMenu->addAction(colinIcons::instance().icon(Colin::drawJointN), tr("u left")+"["+unitSettings::instance().Feh()+"]");
    a[c++] = jointMenu->addAction(colinIcons::instance().icon(Colin::drawJointQ), tr("w left")+"["+unitSettings::instance().Feh()+"]");
    a[c++] = jointMenu->addAction(colinIcons::instance().icon(Colin::drawJointM), QString("%1 ").arg(QChar(0x03C6))+tr("left")+"["+unitSettings::instance().FMeh()+"]");
    jointMenu->addSeparator();
    a[c++] = jointMenu->addAction(colinIcons::instance().icon(Colin::drawJointN), tr("u right")+"["+unitSettings::instance().Feh()+"]");
    a[c++] = jointMenu->addAction(colinIcons::instance().icon(Colin::drawJointQ), tr("w right")+"["+unitSettings::instance().Feh()+"]");
    a[c++] = jointMenu->addAction(colinIcons::instance().icon(Colin::drawJointM), QString("%1 ").arg(QChar(0x03C6))+tr("right")+"["+unitSettings::instance().FMeh()+"]");
    addMenu(jointMenu);
    addSeparator();

    /*
    act = addAction(colinIcons::instance().icon(Colin::drawDLoad), tr("add d. l. load here"), this, SLOT(actionTriggered()));
    act->setData(QVariant(Colin::drawDLoad));
    act = addAction(colinIcons::instance().icon(Colin::drawULoad), tr("add u. d. load here"), this, SLOT(actionTriggered()));
    act->setData(QVariant(Colin::drawULoad));
    act = addAction(colinIcons::instance().icon(Colin::drawILoad), tr("add i. d. load here"), this, SLOT(actionTriggered()));
    act->setData(QVariant(Colin::drawILoad));
    act = addAction(colinIcons::instance().icon(Colin::drawTemp), tr("add temperatur diffrence here"), this, SLOT(actionTriggered()));
    act->setData(QVariant(Colin::drawTemp));
    addSeparator();
    */
    act = addAction(colinIcons::instance().icon(Colin::Copy), tr("copy"), this, SLOT(actionTriggered()));
    act->setData(QVariant(Colin::Copy));
    act = addAction(colinIcons::instance().icon(Colin::Cut), tr("cut"), this, SLOT(actionTriggered()));
    act->setData(QVariant(Colin::Cut));
    addSeparator();
    act = addAction(tr("delete"), this, SLOT(actionTriggered()));
    act->setData(QVariant(Colin::Delete));

    for(int i=0; i<6; i++)
    {
        springs[i] = new QComboBox(this);
        springs[i]->setEditable(true);
        springs[i]->addItem(tr("free"));
        springs[i]->addItem(tr("locked"));
        springs[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        connect(springs[i],             SIGNAL(currentIndexChanged(int)),
                this,                   SLOT(someJointChanged()));
        jointMenu->addWidgetForAction(a[i], springs[i]);
    }



    connect(left,               SIGNAL(editingFinished()),
            this,               SLOT(leftChanged()));

    connect(right,              SIGNAL(editingFinished()),
            this,               SLOT(rightChanged()));

    connect(lJ,                 SIGNAL(clicked()),
            this,               SLOT(lJChanged()));

    connect(rJ,                 SIGNAL(clicked()),
            this,               SLOT(rJChanged()));

    connect(prof,               SIGNAL(currentIndexChanged(int)),
            this,               SLOT(profChanged(int)));

    connect(mat,                SIGNAL(currentIndexChanged(int)),
            this,               SLOT(matChanged(int)));


}

void beamMenu::set(const int &i)
{
    Q_ASSERT(filelist::instance().currentFile()->beam_n() >= i);

    cBeam = -1; //changes will not be send to currentFile...

    actions()[0]->setText(QString(tr("beam")+ "#%1").arg(i));
    const wgv_beam &b = filelist::instance().currentFile()->beam(i);
    left->setText(QString("%1").arg(b.leftNodeI()));
    right->setText(QString("%1").arg(b.rightNodeI()));
    lJ->setChecked(b.joint(wgv_beam::Ml));
    rJ->setChecked(b.joint(wgv_beam::Mr));
    prof->clear();
    mat->clear();
    for(int j=0; j<LIB.profiles_n(); j++)
    {
        prof->addItem(LIB.Profile(j).name());
    }
    for(int j=0; j<LIB.mats_n(); j++)
    {
        mat->addItem(LIB.mat(j).name());
    }
    mat->setCurrentIndex(b.MatI());
    prof->setCurrentIndex(b.ProfileI());
    for(int j=0; j<6; j++)
    {
        if(b.hasSpring(j))
        {
            if(j == 2 || j==5) //moment
                springs[j]->setEditText(QString::number(b.spring(j)*FMPREFIX));
            else
                springs[j]->setEditText(QString::number(b.spring(j)*FPREFIX));
        }
        else if(b.joint(j))
            springs[j]->setCurrentIndex(0);
        else
            springs[j]->setCurrentIndex(1);

    }
    cBeam = i;

}


void beamMenu::leftChanged()
{
    bool ok;
    int newVal = left->text().toInt(&ok);
    if(ok && newVal < filelist::instance().currentFile()->beam_n() && newVal>-1)
    {
        filelist::instance().currentFile()->setLeft(cBeam, newVal);
    }
    else
    {
        left->setText(QString("%1").arg(filelist::instance().currentFile()->beam(cBeam).leftNodeI()));
    }
}

void beamMenu::rightChanged()
{
    bool ok;
    int newVal = right->text().toInt(&ok);
    if(ok && newVal < filelist::instance().currentFile()->beam_n() && newVal>-1)
    {
        filelist::instance().currentFile()->setRight(cBeam, newVal);
    }
    else
    {
        right->setText(QString("%1").arg(filelist::instance().currentFile()->beam(cBeam).rightNodeI()));
    }
}

void beamMenu::matChanged(const int &i)
{
    if(cBeam < 0)
        return;
    filelist::instance().currentFile()->setMat(cBeam, i);
}

void beamMenu::profChanged(const int &i)
{
    if(cBeam < 0)
        return;
    filelist::instance().currentFile()->setProfile(cBeam, i);
}

void beamMenu::lJChanged()
{
    filelist::instance().currentFile()->setJoint(cBeam, 2, lJ->isChecked());
}

void beamMenu::rJChanged()
{
    filelist::instance().currentFile()->setJoint(cBeam, 5, rJ->isChecked());
}

void beamMenu::someJointChanged()
{
    if(cBeam<0)
        return;
    int position;
    bool ok;
    for(int i=0; i<6; i++)
    {
        if(sender() == springs[i])
        {
            position = i;
            break;
        }
    }
    double val = springs[position]->currentText().toDouble(&ok);
    if(springs[position]->currentText() == tr("free"))
    {
        filelist::instance().currentFile()->setJoint(cBeam, position, true);
    }
    else if(springs[position]->currentText() == tr("locked"))
    {
        filelist::instance().currentFile()->setJoint(cBeam, position, false);
    }
    else if(ok)
    {
        if(position == 2 || position == 5) //moment
            filelist::instance().currentFile()->setSpring(cBeam, position, val/FMPREFIX);
        else
            filelist::instance().currentFile()->setSpring(cBeam, position, val/FPREFIX);
    }
}



void beamMenu::actionTriggered()
{
    QAction *trigger = static_cast<QAction*>(sender());
    wgv_tw *tw = filelist::instance().currentFile();

    if(trigger->data().toInt() == Colin::Delete)
    {
        tw->removeBeam(cBeam);
        hide();
        return;
    }
    else if(trigger->data().toInt() == Colin::Copy ||
            trigger->data().toInt() == Colin::Cut  )
    {
        QClipboard *clipboard = QApplication::clipboard();
        QByteArray data;
        QMimeData *mimeData = new QMimeData();
        XmlWriter writer(&data);
        tw->deselectAll();
        tw->selectBeam(cBeam, true);
        writer.writeSelection(*tw, tw->beam(cBeam).leftNode().toQPointF());
        mimeData->setData("text/colinfile", data);
        clipboard->setMimeData(mimeData);
        if(trigger->data().toInt() == Colin::Cut)
            tw->deleteSelection();
        hide();
        return;
    }
    /*
    else if(trigger->data().toInt() == Colin::drawULoad ||
            trigger->data().toInt() == Colin::drawDLoad ||
            trigger->data().toInt() == Colin::drawILoad ||
            trigger->data().toInt() == Colin::drawTemp  )
    {
        tw->setlastObjectBeam(cBeam);
        tw->setlastObjectClick(tw->beam(cBeam).middleP());
        viewPortSettings::instance().setCurrentAction(static_cast<Colin::Actions>(trigger->data().toInt()));
    }
    */
}
