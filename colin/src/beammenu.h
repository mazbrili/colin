#ifndef BEAMMENU_H
#define BEAMMENU_H

#include <QtGui/QLineEdit>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>

#include "colinmenu.h"
#include "wgv_tw.h"
#include "filelist.h"
#include "colinicons.h"

#include "colinhmultibutton.h"


class beamMenu : public ColinMenu
{
    Q_OBJECT
public:
    static beamMenu &instance(){
        if(!instance_)
            instance_ = new beamMenu();
        return *instance_;
    }

    void set(const int &i);
signals:

public slots:

private slots:
    void leftChanged();
    void rightChanged();
    void matChanged(const int &i);
    void profChanged(const int &i);
    void lJChanged();
    void rJChanged();
    void someJointChanged();
    void actionTriggered();
private:
    int cBeam;
    beamMenu();
    static beamMenu *instance_;
    QLineEdit *left,
              *right;
    ColinPushButtonPart *lJ,
                        *rJ;
    QMenu *jointM;
    QComboBox *mat,
              *prof;

    ColinMenu *jointMenu;

    QButtonGroup *g;
    QComboBox* springs[6];
};

#endif // BEAMMENU_H
