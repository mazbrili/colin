#ifndef TEMPMENU_H
#define TEMPMENU_H

#include <QtGui/QLineEdit>
#include <QtGui/QButtonGroup>

#include "colinmenu.h"

#include "colinhmultibutton.h"


class tempMenu : public ColinMenu
{
    Q_OBJECT
public:
    static tempMenu &instance(){
        if(!instance_)
            instance_ = new tempMenu();
        return *instance_;
    }

    void set(const int &i);
signals:

public slots:

private slots:
    void formChanged(const int &i);
    void posChanged();
    void tempChanged();
    void actionTriggered();
private:
    int cTemp;
    tempMenu();
    static tempMenu *instance_;
    QLineEdit *temp,
              *pos;
    ColinPushButtonPart *h,
                        *v;
    QButtonGroup *g;
};

#endif // TEMPMENU_H
