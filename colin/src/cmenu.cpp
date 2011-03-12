#include "cmenu.h"

cMenu::cMenu(QAction *a, QWidget *parent) :
    QMenu(parent)
{
    const int bs = colinIcons::iconSize().width()+18;  //buttonsize
    const int as = 3;                          //space between 2 buttons
    const int is = colinIcons::iconSize().width()+8;   //iconsize

    this->setAttribute(Qt::WA_TranslucentBackground, true);

    range = new QSlider(Qt::Vertical, this);
    range->setMinimum(10);
    range->setMaximum(50);
    range->setValue(catcher::instance().catchRange());

    group = new QButtonGroup(this);

    group->addButton(new QPushButton(colinIcons::instance().icon(catcher::CatchedGrid)
                                     , "", this),
                     catcher::CatchedGrid);
    group->addButton(new QPushButton(colinIcons::instance().icon(catcher::CatchedNode)
                                     , "", this),
                     catcher::CatchedNode);
    group->addButton(new QPushButton(colinIcons::instance().icon(catcher::CatchedCrossing)
                                     , "", this),
                     catcher::CatchedCrossing);
    group->addButton(new QPushButton(colinIcons::instance().icon(catcher::CatchedMiddle)
                                     , "", this),
                     catcher::CatchedMiddle);
    group->addButton(new QPushButton(colinIcons::instance().icon(catcher::CatchedBeam)
                                     , "", this),
                     catcher::CatchedBeam);;
    group->addButton(new QPushButton(colinIcons::instance().icon(catcher::CatchedOrthoGlob)
                                     , "", this),
                     catcher::CatchedOrthoGlob);
    group->addButton(new QPushButton(colinIcons::instance().icon(catcher::CatchedOrthoLokal)
                                     , "", this),
                     catcher::CatchedOrthoLokal);


    group->setExclusive(false);

    int i = 0;
    catcher::CatchCases isOn = catcher::instance().whatsOn();
    foreach(QAbstractButton *p, group->buttons())
    {
        p->setCheckable(true);
        p->setIconSize(QSize(is, is));
        p->setGeometry(0, (bs+as)*i++, bs, bs);
        if((static_cast<catcher::CatchCases>(group->id(p)) & isOn) ==
           static_cast<catcher::CatchCases>(group->id(p)))
            p->setChecked(true);
    }


    range->setGeometry(bs+as, 0, 15, bs*7+as*6);

    this->setFixedSize(bs+2*as+15, bs*7+as*8);

    theAction = a;
    a->setMenu(this);
    a->setData(QVariant(catcher::instance().whatsOn()));
    a->setCheckable(true);
    a->setChecked(true);



    connect(group,                      SIGNAL(buttonClicked(int)),
            this,                       SLOT(click()));

    connect(range,                      SIGNAL(valueChanged(int)),
            this,                       SLOT(setRange(int)));

    connect(a,                          SIGNAL(triggered(bool)),
            this,                       SLOT(allOn(bool)));
}

void cMenu::click()
{
    int nr = 0;
    foreach(QAbstractButton *p, group->buttons())
    {
        if(p->isChecked())
            nr |= group->id(p);
    }
    catcher::instance().setCatchOn(static_cast<catcher::CatchCases>(nr));
}

void cMenu::setRange(int i)
{
    catcher::instance().setCatchRange(i);
}

void cMenu::allOn(bool on)
{
    catcher::instance().setCatchOn(on);
}

void cMenu::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.translate(0, height());
    p.rotate(-90);
    QRect rangeRect = p.transform().inverted().mapRect(range->geometry());
    QRect textRect;
    p.setPen(palette().color(QPalette::Text));
    QFont f = p.font();
    f.setPixelSize(rangeRect.height()-3);
    p.setFont(f);
    p.drawText(QRect(10, rangeRect.y(), rangeRect.width(), rangeRect.height()),
               Qt::AlignVCenter | Qt::AlignLeft,
               QString("["+tr("range")+"]"),
               &textRect);

    rangeRect = range->geometry();
    rangeRect.setHeight(height()-textRect.width()-10);
    range->setGeometry(rangeRect);
}
