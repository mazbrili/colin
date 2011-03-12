#ifndef NODEMENU_H
#define NODEMENU_H

#include <QtGui/QLineEdit>
#include <QtGui/QButtonGroup>

#include "colinmenu.h"
#include "wgv_tw.h"
#include "filelist.h"
#include "colinicons.h"

#include "colinhmultibutton.h"


class nodeMenu : public ColinMenu
{
    Q_OBJECT
public:
    static nodeMenu &instance(){
        if(!instance_)
            instance_ = new nodeMenu();
        return *instance_;
    }

    void set(const int &i);
signals:

public slots:

private slots:
    void bearChanged();
    void xChanged();
    void zChanged();
    void actionTriggered();


private:
    int cNode;
    nodeMenu();
    static nodeMenu *instance_;
    QLineEdit *x,
              *z;
    ColinPushButtonPart *h,
                        *v,
                        *m;
    QButtonGroup *g;
};

#endif // NODEMENU_H
