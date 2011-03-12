#ifndef DLOADMENU_H
#define DLOADMENU_H

#include <QtGui/QLineEdit>
#include <QtGui/QButtonGroup>

#include "colinhmultibutton.h"
#include "colinmenu.h"

class dloadMenu : public ColinMenu
{
    Q_OBJECT
public:
    static dloadMenu &instance(){
        if(!instance_)
            instance_ = new dloadMenu();
        return *instance_;
    }

    void set(const int &i);
signals:

public slots:

private slots:
    void xChanged();
    void zChanged();
    void mChanged();
    void posChanged();
    void sideChanged(const int &i);
    void actionTriggered();
private:
    int cLoad;
    dloadMenu();
    static dloadMenu *instance_;
    QLineEdit *x,
              *z,
              *m,
              *pos;

    ColinPushButtonPart *left,
                        *right;


    ColinMenu *jointMenu;

    QButtonGroup *g;
};

#endif // DLOADMENU_H
