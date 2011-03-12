#include "colinpushbuttonpart.h"

void ColinPushButtonPart::paintEvent(QPaintEvent *)
{
    QStyleOptionButton option;
    option.initFrom(this);



    if(isDefault())
        option.features |= QStyleOptionButton::DefaultButton;

    if(isDown())
        option.state = QStyle::State_Sunken;
    else
        option.state = QStyle::State_Raised;
    
    if(hasFocus())
        option.state |= QStyle::State_HasFocus;

    if(isEnabled())
        option.state |= QStyle::State_Enabled;

    if(option.rect.contains(mapFromGlobal(cursor().pos())))
        option.state |= QStyle::State_MouseOver;

    if(isChecked())
        option.state |= QStyle::State_On;


    if((pos & ColinPushButtonPart::Left) == ColinPushButtonPart::Left)
    {
        option.rect.moveLeft(-4);
        option.rect.setWidth(option.rect.width()+4);
    }

    if((pos & ColinPushButtonPart::Right) == ColinPushButtonPart::Right)
    {
        option.rect.setWidth(option.rect.width()+4);
    }

    if((pos & ColinPushButtonPart::Up) == ColinPushButtonPart::Up)
    {
        option.rect.moveTop(-4);
        option.rect.setHeight(option.rect.width()+4);
    }

    if((pos & ColinPushButtonPart::Bottom) == ColinPushButtonPart::Bottom)
    {
        option.rect.setHeight(option.rect.width()+4);
    }

    QPainter painter(this);
    style()->drawControl(QStyle::CE_PushButton, &option, &painter, this);
/*
    style()->drawItemText(&painter, QRect(2, 2, width()-4, height()-4),
                          Qt::AlignCenter, palette(), isEnabled(), text(), QPalette::Text);
*/
    QRect r = rect();

    r.setLeft(r.x()+2);
    r.setRight(r.right()-2);
    r.setTop(r.y()+2);
    r.setBottom(r.bottom()-2);
    icon().paint(&painter, r, Qt::AlignCenter,
                 isEnabled() ? QIcon::Normal
                             : QIcon::Disabled,
                 isChecked() || isDown() ? QIcon::On
                                         : QIcon::Off);

}

void ColinPushButtonPart::setCutted(ColinPushButtonPart::Position pos_, bool cuted)
{
    if (cuted)
        pos = static_cast<ColinPushButtonPart::Position>(pos | pos_);
    else
        pos = static_cast<ColinPushButtonPart::Position>(pos & ~pos_);
}
