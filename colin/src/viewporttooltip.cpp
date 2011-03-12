#include "viewporttooltip.h"

viewPortToolTip::viewPortToolTip(QWidget *parent):
	abstractColinToolTip(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    icon_ = new QLabel(this);
    text_ = new QLabel(this);
    extend_ = new QLabel(this);
    icon_->setFixedSize(32, 32);
    layout->addWidget(icon_, 0, 0, 1, 1);
    layout->addWidget(text_, 0, 1, 1, 1);
    layout->addWidget(extend_, 1, 0, 1, 2);
    layout->setMargin(2);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

void viewPortToolTip::set(const QIcon &i, const QString &s, const QString &more)
{
    if(i.isNull())
	icon_->hide();
    else
    {
	icon_->show();
	icon_->setPixmap(i.pixmap(QSize(32, 32), QIcon::Normal, QIcon::Off));
    }
    if(more.isEmpty())
	extend_->hide();
    else
    {
	extend_->show();
	extend_->setText(more);
    }
    text_->setText(s);
    setGeometry(QRect(pos(), sizeHint()));
}


bool viewPortToolTip::eventFilter(QObject *obj, QEvent *event)
{
    if(obj->parent() == parent())
    {
        if(event->type() == QEvent::Leave)
            show();
        else if(event->type() == QEvent::Enter)
            hide();
        return QObject::eventFilter(obj, event);
    }
    if(obj != parent())
        return QObject::eventFilter(obj, event);

    if(event->type() == QEvent::Leave)
    {
	this->hide();
    }

    if(event->type() == QEvent::Enter)
    {
	this->show();
    }
    if(event->type() == QEvent::MouseMove)
    {
        this->move(QCursor::pos() + QPoint(30, 30));
    }
    return QObject::eventFilter(obj, event);
}
