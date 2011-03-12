#ifndef NLOADMENU_H
#define NLOADMENU_H

#include <QtGui/QLineEdit>

#include "colinmenu.h"

class nloadMenu : public ColinMenu
{
    Q_OBJECT
public:
    static nloadMenu &instance(){
        if(!instance_)
            instance_ = new nloadMenu();
        return *instance_;
    }

    void set(const int &i);
signals:

public slots:

private slots:
    void xChanged();
    void zChanged();
    void posChanged();
    void actionTriggered();
private:
    int cLoad;
    nloadMenu();
    static nloadMenu *instance_;
    QLineEdit *x,
              *z,
              *pos;
};

#endif // NLOADMENU_H
