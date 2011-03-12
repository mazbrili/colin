#ifndef LOADMENU_H
#define LOADMENU_H

#include <QtGui/QLineEdit>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>

#include "colinmenu.h"
#include "wgv_tw.h"
#include "filelist.h"
#include "colinicons.h"

#include "colinhmultibutton.h"


class loadMenu : public ColinMenu
{
    Q_OBJECT
public:
    static loadMenu &instance(){
        if(!instance_)
            instance_ = new loadMenu();
        return *instance_;
    }

    void set(const int &i);
signals:

public slots:

private slots:
    void xChanged();
    void zChanged();
    void posChanged();
    void formChanged(const int &i);
    void actionTriggered();
private:
    int cLoad;
    loadMenu();
    static loadMenu *instance_;
    QLineEdit *x,
              *z,
              *pos;
    ColinPushButtonPart *increasing,
                        *decreasing,
                        *uniformly;
    QButtonGroup *g;
};

#endif // LOADMENU_H
