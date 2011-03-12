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

}

void nloadMenu::set(const int &i)
{
    Q_ASSERT(filelist::instance().currentFile()->load_n() >= i);

    cLoad = -1; //changes will not be send to currentFile...

    actions()[0]->setText(tr("load")+QString(" #%1").arg(i));

    const wgv_load &l = filelist::instance().currentFile()->load(i);

    x->setText(QString::number(l.Px()));
    z->setText(QString::number(l.Pz()));
    pos->setText(QString::number(l.at()));

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

void nloadMenu::actionTriggered()
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
