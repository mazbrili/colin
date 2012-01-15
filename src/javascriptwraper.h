#ifndef JAVASCRIPTWRAPER_H
#define JAVASCRIPTWRAPER_H

#include <QtCore/QObject>
#include <QtCore/QMetaType>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptContext>

#include "colinnode.h"
#include "colinbeam.h"
#include "colinsupport.h"
#include "colinbls.h"
#include "colincls.h"

Q_DECLARE_METATYPE(ColinNode)
Q_DECLARE_METATYPE(ColinBeam)
Q_DECLARE_METATYPE(ColinLoad)
Q_DECLARE_METATYPE(ColinSupport)
Q_DECLARE_METATYPE(ColinBLS)
Q_DECLARE_METATYPE(ColinCLS)


QScriptValue NodetoScriptValue(QScriptEngine *engine, const ColinNode &n);
void NodefromScriptValue(const QScriptValue &obj, ColinNode &n);
QScriptValue NodeCtor(QScriptContext *ctxt, QScriptEngine *eng);

QScriptValue SupporttoScriptValue(QScriptEngine *engine, const ColinSupport &s);
void SupportfromScriptValue(const QScriptValue &obj, ColinSupport &s);
QScriptValue SupportCtor(QScriptContext *ctxt, QScriptEngine *eng);

QScriptValue BeamtoScriptValue(QScriptEngine *engine, const ColinBeam &b);
void BeamfromScriptValue(const QScriptValue &obj, ColinBeam &b);
QScriptValue BeamCtor(QScriptContext *ctxt, QScriptEngine *eng);

QScriptValue LoadtoScriptValue(QScriptEngine *engine, const ColinLoad &l);
void LoadfromScriptValue(const QScriptValue &obj, ColinLoad &l);
QScriptValue LoadCtor(QScriptContext *ctxt, QScriptEngine *eng);

QScriptValue BLStoScriptValue(QScriptEngine *engine, const ColinBLS &bls);
void BLSfromScriptValue(const QScriptValue &obj, ColinBLS &bls);
QScriptValue BLSCtor(QScriptContext *ctxt, QScriptEngine *eng);

QScriptValue CLStoScriptValue(QScriptEngine *engine, const ColinCLS &cls);
void CLSfromScriptValue(const QScriptValue &obj, ColinCLS &cls);
QScriptValue CLSCtor(QScriptContext *ctxt, QScriptEngine *eng);

#endif // JAVASCRIPTWRAPER_H
