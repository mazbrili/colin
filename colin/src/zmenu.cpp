#include "zmenu.h"

zMenu::zMenu(QAction *a, QWidget *parent) :
    QMenu(parent)
{
    const int bs = colinIcons::iconSize().width()+18;  //buttonsize
    const int as = 3;                          //space between 2 buttons
    const int is = colinIcons::iconSize().width()+8;   //iconsize

    this->setAttribute(Qt::WA_TranslucentBackground, true);

    QPushButton *in = new QPushButton(colinIcons::instance().icon(Colin::drawZoomIn)
                                      , "", this);
    QPushButton *out = new QPushButton(colinIcons::instance().icon(Colin::drawZoomOut)
                                       , "", this);
    QPushButton *rect = new QPushButton(colinIcons::instance().icon(Colin::drawZoomRect)
                                        , "", this);


    in->setGeometry(0, 0, bs, bs);
    out->setGeometry(0, bs+as, bs, bs);
    rect->setGeometry(0, (bs+as)*2, bs, bs);

    group = new QButtonGroup(this);
    group->addButton(in, Colin::drawZoomIn);
    group->addButton(out, Colin::drawZoomOut);
    group->addButton(rect, Colin::drawZoomRect);

    foreach(QAbstractButton *p, group->buttons())
    {
        p->setCheckable(true);
        p->setIconSize(QSize(is, is));
    }

    group->setExclusive(true);

    rect->setChecked(true);

    this->setFixedSize(bs, bs*3+as*2);

    theAction = a;
    a->setMenu(this);
    a->setIcon(group->buttons().at(2)->icon());
    a->setData(QVariant(Colin::drawZoomRect));

    connect(group,              SIGNAL(buttonClicked(int)),
            this,               SLOT(hide()));

    connect(group,              SIGNAL(buttonClicked(int)),
            this,               SLOT(click(int)));

}

void zMenu::click(int nr)
{
    theAction->setIcon(group->button(nr)->icon());
    theAction->setData(QVariant(nr));
    theAction->trigger();
}
