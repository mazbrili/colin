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

#include "tempmenu.h"

#include <QtGui/QApplication>
#include <QtGui/QClipboard>

#include "wgv_tw.h"
#include "filelist.h"
#include "colinicons.h"


tempMenu *tempMenu::instance_ = NULL;


tempMenu::tempMenu() :
    ColinMenu()
{
    QAction *act;
    act = addAction(colinIcons::instance().icon(Colin::drawNode), tr("temperature"));
    act->setDisabled(true);
    addSeparator();

    act = addAction(colinIcons::instance().icon(Colin::drawTemp), tr("temperature"));
    temp = new QLineEdit(this);
    temp->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    temp->setAlignment(Qt::AlignRight);
    addWidgetForAction(act, temp);

    act = addAction(colinIcons::instance().icon(Colin::drawBeam), tr("beam"));
    pos = new QLineEdit(this);
    pos->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    pos->setAlignment(Qt::AlignRight);
    addWidgetForAction(act, pos);
    addSeparator();

    act = addAction(tr("typ"));

    h = new ColinPushButtonPart(colinIcons::instance().icon(wgv_load::tempChange),"");
    v = new ColinPushButtonPart(colinIcons::instance().icon(wgv_load::tempDiffrence),"");

    g = new QButtonGroup(this);
    g->setExclusive(true);
    g->addButton(h, wgv_load::tempChange);
    g->addButton(v, wgv_load::tempDiffrence);


    ColinHMultiButton *multi = new ColinHMultiButton(this);
    h->setCheckable(true);
    v->setCheckable(true);

    multi->addButton(h);
    multi->addButton(v);
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




    connect(temp,               SIGNAL(editingFinished()),
            this,               SLOT(tempChanged()));

    connect(pos,                SIGNAL(editingFinished()),
            this,               SLOT(posChanged()));

    connect(g,                  SIGNAL(buttonClicked(int)),
            this,               SLOT(formChanged(int)));


}

void tempMenu::set(const int &i)
{
    cTemp = -1;
    actions()[0]->setText(QString(tr("temperature")+" #%1").arg(i));
    if(filelist::instance().currentFile()->load(i).typ() == wgv_load::tempChange)
    {
        h->setChecked(true);
        temp->setText(QString::number(filelist::instance().currentFile()->load(i).Px()));
    }
    else
    {
        v->setChecked(true);
        temp->setText(QString::number(filelist::instance().currentFile()->load(i).Pz()));
    }
    pos->setText(QString::number(filelist::instance().currentFile()->load(i).at()));
    cTemp = i;
}

void tempMenu::formChanged(const int &i)
{
    wgv_load l(filelist::instance().currentFile()->load(cTemp));
    l.setTyp(static_cast<wgv_load::form>(i));
    double p = l.Pz();
    l.setPz(l.Px());
    l.setPx(p);
    filelist::instance().currentFile()->editLoad(cTemp, l);
}

void tempMenu::posChanged()
{
    bool ok;
    int val;
    val = pos->text().toInt(&ok);
    if(ok && val > -1 && val< filelist::instance().currentFile()->beam_n())
        filelist::instance().currentFile()->setPos(cTemp, val);
    else
        pos->setText(QString::number(filelist::instance().currentFile()->load(cTemp).at()));
}

void tempMenu::tempChanged()
{
    bool ok;
    double val;
    val = temp->text().toInt(&ok);
    if(ok)
    {
        if(filelist::instance().currentFile()->load(cTemp).typ() == wgv_load::tempChange)
            filelist::instance().currentFile()->setPx(cTemp, val);
        else
            filelist::instance().currentFile()->setPz(cTemp, val);
    }
    else
    {
        if(filelist::instance().currentFile()->load(cTemp).typ() == wgv_load::tempChange)
            temp->setText(QString::number(filelist::instance().currentFile()->load(cTemp).Px()));
        else
            temp->setText(QString::number(filelist::instance().currentFile()->load(cTemp).Pz()));
    }
}


void tempMenu::actionTriggered()
{
    QAction *trigger = static_cast<QAction*>(sender());
    wgv_tw *tw = filelist::instance().currentFile();

    if(trigger->data().toInt() == Colin::Delete)
    {
        tw->removeLoad(cTemp);
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
        writer.writeLoad(*filelist::instance().currentFile(), cTemp);
        mimeData->setData("text/colinfile", data);
        clipboard->setMimeData(mimeData);
        if(trigger->data().toInt() == Colin::Cut)
            tw->removeLoad(cTemp);
        hide();
        return;
    }
}
