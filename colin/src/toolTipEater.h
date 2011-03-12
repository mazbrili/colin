#ifndef TOOLTIPEATER_H
#define TOOLTIPEATER_H

#include <QtCore/QObject>
#include <QtCore/QEvent>

class toolTipEater : public QObject
{

    Q_OBJECT

protected:
    bool eventFilter(QObject *obj, QEvent *event){
        if(event->type() == QEvent::ToolTip)
            return true;
        else
            return QObject::eventFilter(obj, event);
    }
};

#endif // TOOLTIPEATER_H
