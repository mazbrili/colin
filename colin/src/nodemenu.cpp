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

#include "nodemenu.h"


#include <QtGui/QApplication>
#include <QtGui/QClipboard>

nodeMenu *nodeMenu::instance_ = NULL;


nodeMenu::nodeMenu() :
    ColinMenu()
{
    QAction *act;
    act = addAction(colinIcons::instance().icon(Colin::drawNode), tr("node"));
    act->setDisabled(true);
    addSeparator();

    act = addAction("x");
    x = new QLineEdit(this);
    x->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    x->setAlignment(Qt::AlignRight);
    addWidgetForAction(act, x);

    act = addAction("z");
    z = new QLineEdit(this);
    z->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    z->setAlignment(Qt::AlignRight);
    addWidgetForAction(act, z);
    addSeparator();

    act = addAction(tr(""));
    addSeparator();

    h = new ColinPushButtonPart(colinIcons::instance().icon(Colin::drawBearingH),"");
    v = new ColinPushButtonPart(colinIcons::instance().icon(Colin::drawBearingV),"");
    m = new ColinPushButtonPart(colinIcons::instance().icon(Colin::drawBearingM),"");

    g = new QButtonGroup(this);
    g->setExclusive(false);
    g->addButton(h, wgv_bearing::X);
    g->addButton(v, wgv_bearing::Z);
    g->addButton(m, wgv_bearing::Phi);


    ColinHMultiButton *multi = new ColinHMultiButton(this);
    foreach(QAbstractButton *b, g->buttons())
    {
        b->setCheckable(true);
    }

    multi->addButton(h);
    multi->addButton(v);
    multi->addButton(m);
    multi->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    addWidgetForAction(act, multi);


    //addAction(colinIcons::instance().icon(Colin::drawNLoad), tr("add Load here"), this, SLOT(addLoad()));
    //addAction(colinIcons::instance().icon(Colin::drawMoment), tr("add Moment here"), this, SLOT(addMoment()));
    //addAction(colinIcons::instance().icon(Colin::drawBeam), tr("draw beam, starting here"), this, SLOT(addBeam()));
    //addSeparator();

    act = addAction(colinIcons::instance().icon(Colin::Copy), tr("copy"), this, SLOT(actionTriggered()));
    act->setData(QVariant(Colin::Copy));
    act = addAction(colinIcons::instance().icon(Colin::Cut), tr("cut"), this, SLOT(actionTriggered()));
    act->setData(QVariant(Colin::Cut));
    addSeparator();
    act = addAction(tr("delete"), this, SLOT(actionTriggered()));
    act->setData(QVariant(Colin::Delete));


    setTabOrder(x, z);
    setTabOrder(z, h);
    setTabOrder(h, v);
    setTabOrder(v, m);




    connect(x,                  SIGNAL(editingFinished()),
            this,               SLOT(xChanged()));

    connect(z,                  SIGNAL(editingFinished()),
            this,               SLOT(zChanged()));

    connect(g,                  SIGNAL(buttonClicked(int)),
            this,               SLOT(bearChanged()));


}

void nodeMenu::set(const int &i)
{
    cNode = -1;
    actions()[0]->setText(QString(tr("node #%1")).arg(i));
    x->setText(QString("%1").arg(filelist::instance().currentFile()->node(i).x()));
    z->setText(QString("%1").arg(filelist::instance().currentFile()->node(i).z()));
    if(filelist::instance().currentFile()->node(i).hasbearing())
    {
	h->setChecked((filelist::instance().currentFile()->node(i).bearing().x()));
	v->setChecked((filelist::instance().currentFile()->node(i).bearing().z()));
	m->setChecked((filelist::instance().currentFile()->node(i).bearing().phi()));
    }
    else
    {
        h->setChecked(false);
        v->setChecked(false);
        m->setChecked(false);
    }
    bearChanged();
    cNode = i;
}

void nodeMenu::bearChanged()
{
    wgv_bearing::bearings form = wgv_bearing::NoBear;
    foreach(QAbstractButton *b, g->buttons())
    {
        if(b->isChecked())
            form |= static_cast<wgv_bearing::bearing>(g->id(b));
    }


    if(form == wgv_bearing::NoBear)
        actions()[6]->setIcon(QIcon());
    else
        actions()[6]->setIcon(colinIcons::instance().icon(wgv_bearing::formToDrawEnum(form)));

    if (cNode == -1)
	return;
    wgv_bearing b;
    if(filelist::instance().currentFile()->node(cNode).hasbearing())
    {
        b = filelist::instance().currentFile()->node(cNode).bearing();
    }

    b.setX(form.testFlag(wgv_bearing::X));
    b.setZ(form.testFlag(wgv_bearing::Z));
    b.setPhi(form.testFlag(wgv_bearing::Phi));
    filelist::instance().currentFile()->setBearing(cNode, b);
}

void nodeMenu::xChanged()
{
    if (cNode == -1)
	return;
    bool ok;
    double newVal = x->text().toDouble(&ok);
    if(ok)
        filelist::instance().currentFile()->setX(cNode, newVal);
    x->setText(QString("%1").arg(filelist::instance().currentFile()->node(cNode).x()));
}

void nodeMenu::zChanged()
{
    if (cNode == -1)
	return;
    bool ok;
    double newVal = z->text().toDouble(&ok);
    if(ok)
        filelist::instance().currentFile()->setZ(cNode, newVal);
    z->setText(QString("%1").arg(filelist::instance().currentFile()->node(cNode).z()));
}

void nodeMenu::actionTriggered()
{
    if (cNode == -1)
	return;
    QAction *trigger = static_cast<QAction*>(sender());
    wgv_tw *tw = filelist::instance().currentFile();

    if(trigger->data().toInt() == Colin::Delete)
    {
        tw->removeNode(cNode);
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
        tw->selectNode(cNode, true);
        writer.writeSelection(*tw, tw->node(cNode).toQPointF());
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
