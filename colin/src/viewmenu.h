#ifndef VIEWMENU_H
#define VIEWMENU_H

#include "colinmenu.h"
#include "scalewidget.h"
#include "filelist.h"

class viewMenu : public ColinMenu
{
    Q_OBJECT
public:
    explicit viewMenu(QWidget *parent = 0);
    viewMenu(const QString &title, QWidget *parent = 0);
    QAction *zoomAll,
            *zoomIn,
            *zoomOut;
    scaleWidget *uScale,
                *pScale,
                *mScale;

public slots:
    void setTw(wgv_tw *tw);
    void dumpTw(QObject *o);

    //scaleSlots connected with the struct
    void scaleU(const double &scale);
    void scaleP(const double &scale);
    void scaleM(const double &scale);

    //scaleSlots connected with the sliders
    void scaleU(const int &scale);
    void scaleP(const int &scale);
    void scaleM(const int &scale);

    //scaleUpDownSlots connected with the buttons
    void scaleDownU();
    void scaleUpU();
    void scaleDownP();
    void scaleUpP();
    void scaleDownM();
    void scaleUpM();

    //resets the Range for all sliders
    void setRanges();

    void autoScaleU();
    void autoScaleP();
    void autoScaleM();

private:
    void init();

    bool adjust;

    wgv_tw *t;



};

#endif // VIEWMENU_H
