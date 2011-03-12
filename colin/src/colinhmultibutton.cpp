#include "colinhmultibutton.h"


ColinHMultiButton::ColinHMultiButton(QWidget *parent)
    : QWidget(parent)
{

}


ColinHMultiButton::~ColinHMultiButton()
{

}



void ColinHMultiButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QStyleOptionButton option;
    option.initFrom(this);
    option.rect = QRect(0, 0, width(), height());
    painter.translate(0, height());
    painter.scale(1, -1);
    style()->drawControl(QStyle::CE_PushButton, &option, &painter, this);
}


void ColinHMultiButton::resizeEvent(QResizeEvent *event)
{
    adjustButtons(event->size() );
}


void ColinHMultiButton::adjustButtons()
{
    adjustButtons(size());
}

void ColinHMultiButton::adjustButtons(const QSize &size)
{
    int HMultiButtonth_ = (size.width()-buttonlist.size()+1)/(int)buttonlist.size();
    int rest = (size.width()-buttonlist.size()+1)%(int)buttonlist.size();

    for(int i = 0, xpos = 0; i<buttonlist.size(); i++)
    {
        buttonlist[i]->setGeometry(QRect(xpos, 0, HMultiButtonth_+((i<rest)? 1 : 0), height()));
        xpos+=HMultiButtonth_+((i<rest)? 2 : 1);
    }
}


void ColinHMultiButton::addButton(ColinPushButtonPart *but)
{
    if(!buttonlist.isEmpty())
    {
        buttonlist.last()->setCutted(ColinPushButtonPart::Right, true);
        but->setCutted(ColinPushButtonPart::Left, true);
    }


    buttonlist.append(but);
    but->setParent(this);
    adjustButtons();
}
