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

#include "nloadmenu.h"
#include "colinicons.h"

#include <QtGui/QApplication>
#include <QtGui/QClipboard>


nloadMenu *nloadMenu::instance_ = NULL;


nloadMenu::nloadMenu() :
    ColinMenu()
{
    QAction *act;
    act = addAction(colinIcons::instance().icon(Colin::drawNLoad), tr("nodal forces"));
    act->setDisabled(true);
    addSeparator();

    act = addAction("Px");
    x = new QLineEdit(this);
    x->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    addWidgetForAction(act,x);

    act = addAction("Pz");
    z = new QLineEdit(this);
    z->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    addWidgetForAction(act, z);

    addSeparator();

    act = addAction(tr("node"));
    pos = new QLineEdit(this);
    pos->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    addWidgetForAction(act, pos);

	addSeparator();

	act = addAction(tr("set"));
	lset = new QComboBox(this);
	lset->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	addWidgetForAction(act, lset);

    addSeparator();

    act = addAction(colinIcons::instance().icon(Colin::Copy), tr("copy"), this, SLOT(actionTriggered()));
    act->setData(QVariant(Colin::Copy));
    act = addAction(colinIcons::instance().icon(Colin::Cut), tr("cut"), this, SLOT(actionTriggered()));
    act->setData(QVariant(Colin::Cut));
    addSeparator();
    act = addAction(tr("delete"), this, SLOT(actionTriggered()));
    act->setData(QVariant(Colin::Delete));


    connect(x,              SIGNAL(editingFinished()),
            this,           SLOT(xChanged()));

    connect(z,              SIGNAL(editingFinished()),
            this,           SLOT(zChanged()));

    connect(pos,            SIGNAL(editingFinished()),
            this,           SLOT(posChanged()));

	connect(lset,			SIGNAL(currentIndexChanged(int)),
			this,			SLOT(setChanged(int)));

}

void nloadMenu::set(const int &i)
{
    Q_ASSERT(filelist::instance().currentFile()->load_n() >= i);

	ColinStruct *tw = filelist::instance().currentFile();

    cLoad = -1; //changes will not be send to currentFile...

    actions()[0]->setText(tr("load")+QString(" #%1").arg(i));

    const ColinLoad &l = filelist::instance().currentFile()->load(i);

    x->setText(QString::number(l.Px()));
    z->setText(QString::number(l.Pz()));
    pos->setText(QString::number(l.at()));

	lset->clear();

	lset->addItem(colinIcons::instance().icon(Colin::Close), tr("none"));

	for(int j=0; j<tw->bls_n(); j++)
	{
		lset->addItem(colinIcons::instance().icon(Colin::BLS), tw->bls(j).name());
	}
	lset->setCurrentIndex(l.set()+1);
	qDebug() << "set Setindex to "<< l.set()+1;

    cLoad = i;
}

void nloadMenu::xChanged()
{
    bool ok;
    double val;
    val = x->text().toDouble(&ok);
    if(ok)
        filelist::instance().currentFile()->setPx(cLoad, val);
    else
        x->setText(QString::number(filelist::instance().currentFile()->load(cLoad).Px()));
}

void nloadMenu::zChanged()
{
    bool ok;
    double val;
    val = z->text().toDouble(&ok);
    if(ok)
        filelist::instance().currentFile()->setPz(cLoad, val);
    else
        z->setText(QString::number(filelist::instance().currentFile()->load(cLoad).Pz()));
}

void nloadMenu::posChanged()
{
    bool ok;
    int val;
    ColinStruct *tw = filelist::instance().currentFile();
    val = pos->text().toInt(&ok);
    if(ok && val > -1 &&
       ((tw->load(cLoad).typ() == ColinLoad::moment ||tw->load(cLoad).typ() == ColinLoad::nodeLoad)?
        val < tw->node_n() :
        val < tw->beam_n()))
        filelist::instance().currentFile()->setPos(cLoad, val);
    else
        pos->setText(QString::number(filelist::instance().currentFile()->load(cLoad).at()));
}


void nloadMenu::setChanged(const int &i)
{
	if(cLoad<0)
		return;
	filelist::instance().currentFile()->setLoadSet(cLoad, i-1);
	qDebug() << "set Setindex to "<< i-1;

}
void nloadMenu::actionTriggered()
{
    QAction *trigger = static_cast<QAction*>(sender());
    ColinStruct *tw = filelist::instance().currentFile();

    if(trigger->data().toInt() == Colin::Delete)
    {
        tw->removeLoad(cLoad);
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
        writer.writeLoad(*filelist::instance().currentFile(), cLoad);
        mimeData->setData("text/colinfile", data);
        clipboard->setMimeData(mimeData);
        if(trigger->data().toInt() == Colin::Cut)
            tw->removeLoad(cLoad);
        hide();
        return;
    }
}
