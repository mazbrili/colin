#include "colintooltip.h"

colinToolTip::colinToolTip(QWidget *parent) :
    abstractColinToolTip(parent)
{

    QGridLayout *layout = new QGridLayout(this);

    title = new QLabel("", this);
    icon = new QLabel("", this);
    description = new QLabel("", this);
    shortcut = new QLabel(tr("shortcut")+":", this);
    sequence = new QLabel("", this);
/*
    QLinearGradient grad(0, 0, 0, 64);
    grad.setColorAt(0, QColor(230, 230, 230));
    grad.setColorAt(1, QColor(178, 178, 178));

    QPalette pal = title->palette();
    pal.setBrush(QPalette::Foreground, QBrush(grad));
    title->setPalette(pal);
*/
    QFont f = title->font();
    f.setPixelSize(30);
    title->setFont(f);
    title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
/*
    description->setPalette(pal);
    shortcut->setPalette(pal);
*/

    description->setWordWrap(true);


    icon->setFixedSize(QSize(64, 64));
    title->setFixedHeight(64);
    sequence->setFixedHeight(32);




    layout->addWidget(icon, 0, 0, 1, 1);
    layout->addWidget(title, 0, 1, 1, 1);

    layout->setRowStretch(1, 20);

    layout->addWidget(description, 2, 0, 1, 2);

    layout->setRowStretch(3, 20);

    layout->addWidget(shortcut, 4, 0, 1, 2);
    layout->addWidget(sequence, 5, 0, 1, 2);


}


void colinToolTip::popup(const QString &title, const QString &text_, const QPixmap &icon,
           const QKeySequence &shortcut_, const QPoint &pos, bool r)
{

    move(pos);
    this->title->setText(title);
    this->description->setText(text_);
    this->icon->setPixmap(icon);
    if(shortcut_ != QKeySequence())
    {
        this->sequence->setPixmap(colinIcons::instance().icon(shortcut_, colinIcons::instance().darkIcons()));
        this->sequence->show();
        this->shortcut->show();
    }
    else
    {
        this->sequence->hide();
        this->shortcut->hide();
    }
    this->show();
    this->adjustSize();
    if(r)
        this->move(this->geometry().left()-this->width()+40, this->geometry().top());
}
