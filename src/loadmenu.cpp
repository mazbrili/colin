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

#include "loadmenu.h"
#include "unitsettings.h"


#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtCore/QDebug>


loadMenu *loadMenu::instance_ = NULL;


loadMenu::loadMenu() :
    ColinMenu()
{
    QAction *act;
    act = addAction(colinIcons::instance().icon(Colin::drawNLoad), tr("load"));
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

    act = addAction(tr("beam"));
    pos = new QLineEdit(this);
    pos->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    addWidgetForAction(act, pos);

	addSeparator();

	act = addAction(tr("set"));
	lset = new QComboBox(this);
	lset->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	addWidgetForAction(act, lset);

    addSeparator();

    act = addAction("p(x) = ");

    increasing = new ColinPushButtonPart(colinIcons::instance().icon(Colin::drawILoad),"");
    uniformly = new ColinPushButtonPart(colinIcons::instance().icon(Colin::drawULoad),"");
    decreasing = new ColinPushButtonPart(colinIcons::instance().icon(Colin::drawDLoad),"");

    g = new QButtonGroup(this);
    g->setExclusive(true);
    g->addButton(increasing, ColinLoad::increasingLinearLoad);
    g->addButton(uniformly, ColinLoad::uniformlyDistibutedLoad);
    g->addButton(decreasing, ColinLoad::decreasingLinearLoad);


    ColinHMultiButton *multi = new ColinHMultiButton(this);
    foreach(QAbstractButton *b, g->buttons())
    {
        b->setCheckable(true);
    }

    multi->addButton(increasing);
    multi->addButton(uniformly);
    multi->addButton(decreasing);
    multi->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    addWidgetForAction(act, multi);

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

    connect(g,              SIGNAL(buttonClicked(int)),
            this,           SLOT(formChanged(int)));

	connect(lset,			SIGNAL(currentIndexChanged(int)),
			this,			SLOT(setChanged(int)));


}

void loadMenu::set(const int &i)
{
    Q_ASSERT(filelist::instance().currentFile()->load_n() >= i);


	ColinStruct *tw = filelist::instance().currentFile();

    cLoad = -1; //changes will not be send to currentFile...

    actions()[0]->setText(QString(tr("load #%1")).arg(i));

	const ColinLoad &l = tw->load(i);

    x->setText(QString::number(l.Px()*PPREFIX));
    z->setText(QString::number(l.Pz()*PPREFIX));
    pos->setText(QString::number(l.at()));

	lset->clear();

	lset->addItem(colinIcons::instance().icon(Colin::Close), tr("none"));

	for(int j=0; j<tw->bls_n(); j++)
	{
		lset->addItem(colinIcons::instance().icon(Colin::BLS), tw->bls(j).name());
	}
	lset->setCurrentIndex(l.set()+1);
	qDebug() << "set Setindex to "<< l.set()+1;

    foreach(QAbstractButton *b, g->buttons())
    {
        if(g->id(b) == l.typ())
        {
            b->setChecked(true);
            break;
        }
    }

    cLoad = i;
}

void loadMenu::xChanged()
{
    bool ok;
    double val;
    val = x->text().toDouble(&ok);
    if(ok)
        filelist::instance().currentFile()->setPx(cLoad, val/PPREFIX);
    else
        x->setText(QString::number(filelist::instance().currentFile()->load(cLoad).Px()*PPREFIX));
}

void loadMenu::zChanged()
{
    bool ok;
    double val;
    val = z->text().toDouble(&ok);
    if(ok)
        filelist::instance().currentFile()->setPz(cLoad, val/PPREFIX);
    else
        z->setText(QString::number(filelist::instance().currentFile()->load(cLoad).Pz()*PPREFIX));
}

void loadMenu::posChanged()
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

void loadMenu::formChanged(const int &i)
{
    filelist::instance().currentFile()->setLoadTyp(cLoad, static_cast<ColinLoad::form>(i));
}

void loadMenu::setChanged(const int &i)
{
	if(cLoad<0)
		return;
	filelist::instance().currentFile()->setLoadSet(cLoad, i-1);
	qDebug() << "set Setindex to "<< i-1;

}

void loadMenu::actionTriggered()
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

