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


#include <QApplication>
#include <QClipboard>
#include <QMimeData>


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

    act = addAction("p(x) = ");

    increasing = new ColinPushButtonPart(colinIcons::instance().icon(Colin::drawILoad),"");
    uniformly = new ColinPushButtonPart(colinIcons::instance().icon(Colin::drawULoad),"");
    decreasing = new ColinPushButtonPart(colinIcons::instance().icon(Colin::drawDLoad),"");

    g = new QButtonGroup(this);
    g->setExclusive(true);
    g->addButton(increasing, wgv_load::increasingLinearLoad);
    g->addButton(uniformly, wgv_load::uniformlyDistibutedLoad);
    g->addButton(decreasing, wgv_load::decreasingLinearLoad);


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


}

void loadMenu::set(const int &i)
{
    Q_ASSERT(filelist::instance().currentFile()->load_n() >= i);

    cLoad = -1; //changes will not be send to currentFile...

    actions()[0]->setText(QString(tr("load #%1")).arg(i));

    const wgv_load &l = filelist::instance().currentFile()->load(i);

    x->setText(QString::number(l.Px()*PPREFIX));
    z->setText(QString::number(l.Pz()*PPREFIX));
    pos->setText(QString::number(l.at()));

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
    wgv_tw *tw = filelist::instance().currentFile();
    val = pos->text().toInt(&ok);
    if(ok && val > -1 &&
       ((tw->load(cLoad).typ() == wgv_load::moment ||tw->load(cLoad).typ() == wgv_load::nodeLoad)?
        val < tw->node_n() :
        val < tw->beam_n()))
        filelist::instance().currentFile()->setPos(cLoad, val);
    else
        pos->setText(QString::number(filelist::instance().currentFile()->load(cLoad).at()));
}

void loadMenu::formChanged(const int &i)
{
    filelist::instance().currentFile()->setLoadTyp(cLoad, static_cast<wgv_load::form>(i));
}

void loadMenu::actionTriggered()
{
    QAction *trigger = static_cast<QAction*>(sender());
    wgv_tw *tw = filelist::instance().currentFile();

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

