#include "scalewidget.h"

scaleWidget::scaleWidget(QWidget *parent) :
    QWidget(parent)
{
    slider = new QSlider(Qt::Horizontal, this);

    down = new ColinPushButtonPart("-", this);
    down->setIcon(colinIcons::instance().icon(Colin::drawZoomOut));
    down->setCutted(ColinPushButtonPart::Right, true);

    up = new ColinPushButtonPart("+", this);
    up->setIcon(colinIcons::instance().icon(Colin::drawZoomIn));
    up->setCutted(ColinPushButtonPart::Left, true);

    restore = new QPushButton(colinIcons::instance().icon(Colin::drawZoomAll), "", this);


    setFixedWidth(180);

}



void scaleWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QStyleOptionButton option;
    option.initFrom(this);
    option.state = option.state & ~(QStyle::State_MouseOver);
    option.rect = QRect(0, 0, width()-height()-4, height());
    style()->drawControl(QStyle::CE_PushButton, &option, &painter, this);
    adjustButtons();
}


void scaleWidget::resizeEvent(QResizeEvent *event)
{
    adjustButtons(event->size());
}


void scaleWidget::adjustButtons()
{
    adjustButtons(size());
}

void scaleWidget::adjustButtons(const QSize &size)
{
    const int x = 20;

    down->setGeometry(          QRect(0, 0,
                                      x, size.height()));

    slider->setGeometry(        QRect(x, 0,
                                      size.width()-2*x-4-size.height(), size.height()));

    up->setGeometry(            QRect(size.width()-x-4-size.height(), 0,
                                      x, size.height()));

    restore->setGeometry(       QRect(size.width()-size.height(), 0,
                                      size.height(), size.height()));
}

