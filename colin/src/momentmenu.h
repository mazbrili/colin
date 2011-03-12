#ifndef MOMENTMENU_H
#define MOMENTMENU_H

#include <QtGui/QLineEdit>
#include "colinmenu.h"

class momentMenu : public ColinMenu
{
    Q_OBJECT
public:
    static momentMenu &instance(){
        if(!instance_)
            instance_ = new momentMenu();
        return *instance_;
    }

    void set(const int &i);
signals:

public slots:

private slots:
    void mChanged();
    void posChanged();
    void actionTriggered();
private:
    int cLoad;
    momentMenu();
    static momentMenu *instance_;
    QLineEdit *m,
              *pos;
};

#endif // MOMENTMENU_H
