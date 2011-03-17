#ifndef CATCHER_H
#define CATCHER_H

#include <QtCore/QSettings>
#include <cmath>

#include "wgv_tw.h"
#include "grid.h"
#include "colin_namespace.h"
#include "filelist.h"

#ifndef QT_NO_DEBUG
#include <QtCore/QTextStream>
#endif

class catcher
{
public:

    enum CatchCase{
        CatchedNothing          = 0x0,

        CatchedNode             = 0x2,          //prio 0

        CatchedGrid             = 0x4,          //prio 1

        CatchedCrossing         = 0x8,          //prio 2
        CatchedMiddle           = 0x10,         //prio 3
        CatchedBeam             = 0x20,         //prio 4
        CatchedBeamEnd          = 0x40,         //only for joints

        CatchedOrthoGlob       = 0x80,          //prio 5

        CatchedOrthoLokal      = 0x100,          //prio 6

        CatchedLoad		= 0x200,	//hovereffects

        CatchedLoadHotSpot      = 0x400,        //easy editing

        CatchedTemp             = 0x800,        //click to edit

        CatchStdMove		= CatchedNode +
                                  CatchedBeam +
                                  CatchedLoad +
                                  CatchedLoadHotSpot +
                                  CatchedTemp,

        CatchStdNode            = CatchedGrid +
                                  CatchedCrossing +
                                  CatchedMiddle +
                                  CatchedBeam,

        CatchStdBearing         = CatchedNode,

        CatchStdBeam           =  CatchedNode +
                                  CatchedGrid +
                                  CatchedCrossing +
                                  CatchedMiddle +
                                  CatchedBeam +
                                  CatchedOrthoGlob+
                                  CatchedOrthoLokal,

        CatchStdJoint           = CatchedBeamEnd+
                                  CatchedMiddle +
                                  CatchedBeam,

        CatchStdDLoad           = CatchedBeamEnd,

        CatchStdSLoad           = CatchedBeam +
                                  CatchedOrthoGlob +
                                  CatchedOrthoLokal+
                                  CatchedGrid,

        CatchStdSelect          = CatchedBeam +
                                  CatchedNode +
                                  CatchedLoad +
                                  CatchedTemp,

        CatchStdLoad            = CatchedNode +
                                  CatchedBeam +
                                  CatchedMiddle +
                                  CatchedOrthoGlob +
                                  CatchedOrthoLokal +
                                  CatchedGrid,

        CatchStdMoveLoad        = CatchedNode +
                                  CatchedBeam,

        CatchStdMoveLoadHotSpot = CatchedOrthoGlob +
                                  CatchedOrthoLokal +
                                  CatchedGrid,

        CatchStdMoveNode        = CatchedGrid,

        CatchPreSettings        = CatchedNode +         //node
                                  CatchedGrid +         //grid
                                  CatchedBeam +         //beam
                                  CatchedCrossing       //crossing
    };
    Q_DECLARE_FLAGS (CatchCases, CatchCase);

    static catcher &instance(){
        if(!instance_)
            instance_ = new catcher();
        return *instance_;
    }

    void setCatchOn(const catcher::CatchCases &c, const bool &on);
    void setCatchOn(const catcher::CatchCases &c){isOn = c;}
    void inline setCatchOn(const bool on){allOff = !on;}
    void setCatchRange(const int &r);

    const int &catchRange() const;
    bool isCatchOn(const catcher::CatchCases &c) const;
    const inline bool &isAllOf(){return allOff;}

    const inline catcher::CatchCases &whatsOn() const {return isOn;}

    catcher::CatchCases CasesForAction(const Colin::Actions &a)const;

    int doYourWork(QPointF *mouse, CatchCases *c,
                   const QTransform &t, int *adi = NULL, bool ignoreOff = false);

    int orthoCatch(const QTransform &t, const QPointF &oldPos, QPointF *newPos, CatchCases *c);

    void readSettings();
    void saveSettings();

private:
    Q_DISABLE_COPY(catcher)
    catcher();
    int catchRange_;
    CatchCases isOn;
    bool allOff;
    int lastCatch;
    CatchCases lastCase;
    static catcher *instance_;

};
Q_DECLARE_OPERATORS_FOR_FLAGS(catcher::CatchCases);

#endif // CATCHER_H
