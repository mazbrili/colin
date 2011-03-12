#include "momentmenu.h"

#include <QtGui/QApplication>
#include <QtGui/QClipboard>

#include "colinicons.h"
#include "filelist.h"

momentMenu *momentMenu::instance_ = NULL;


momentMenu::momentMenu() :
    ColinMenu()
{
    QAction *act;
    act = addAction(colinIcons::instance().icon(Colin::drawNLoad), tr("moment"));
    act->setDisabled(true);
    addSeparator();

    act = addAction("M");
    m = new QLineEdit(this);
    m->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    addWidgetForAction(act,m);

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


    connect(m,              SIGNAL(editingFinished()),
            this,           SLOT(mChanged()));

    connect(pos,            SIGNAL(editingFinished()),
            this,           SLOT(posChanged()));

}

void momentMenu::set(const int &i)
{
    m->setText(QString::number(filelist::instance().currentFile()->load(i).M()));
    pos->setText(QString::number(filelist::instance().currentFile()->load(i).at()));
    cLoad = i;
}

void momentMenu::mChanged()
{
    bool ok;
    double val;
    val = m->text().toDouble(&ok);
    if(ok)
        filelist::instance().currentFile()->setM(cLoad, val);
    else
        m->setText(QString::number(filelist::instance().currentFile()->load(cLoad).M()));
}

void momentMenu::posChanged()
{
    bool ok;
    int val;
    wgv_tw *tw = filelist::instance().currentFile();
    val = pos->text().toInt(&ok);
    if(ok && val > -1 && val < tw->node_n())
        filelist::instance().currentFile()->setPos(cLoad, val);
    else
        pos->setText(QString::number(filelist::instance().currentFile()->load(cLoad).at()));
}


void momentMenu::actionTriggered()
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
