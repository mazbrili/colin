#include "scriptengine.h"

#include <QtCore/QDebug>

#include "javascriptwraper.h"


QScriptValue ScriptPrintFunction(QScriptContext *ctxt, QScriptEngine *engine)
{
	scriptEngine *scE = static_cast<scriptEngine*>(engine);
	scE->emitOut(ctxt->argument(0).toString());
	return QScriptValue(engine, 0);
}

scriptEngine::scriptEngine(ColinStruct *tw, QObject *parent) :
    QScriptEngine(parent)
{
	qRegisterMetaType<ColinLoad::form>("ColinLoad::form");

	qScriptRegisterMetaType<ColinNode>(this, &NodetoScriptValue, &NodefromScriptValue);
	qScriptRegisterMetaType<ColinSupport>(this, &SupporttoScriptValue, &SupportfromScriptValue);
	qScriptRegisterMetaType<ColinBeam>(this, &BeamtoScriptValue, &BeamfromScriptValue);
	qScriptRegisterMetaType<ColinLoad>(this, &LoadtoScriptValue, &LoadfromScriptValue);
	qScriptRegisterMetaType<ColinBLS>(this, &BLStoScriptValue, &BLSfromScriptValue);
	qScriptRegisterMetaType<ColinCLS>(this, &CLStoScriptValue, &CLSfromScriptValue);


	globalObject().setProperty("ColinNode", newFunction(NodeCtor));
	globalObject().setProperty("ColinBeam", newFunction(BeamCtor));
	globalObject().setProperty("ColinSupport", newFunction(SupportCtor));
	globalObject().setProperty("ColinLoad", newFunction(LoadCtor));
	globalObject().setProperty("ColinBLS", newFunction(BLSCtor));
	globalObject().setProperty("ColinCLS", newFunction(CLSCtor));

	globalObject().setProperty("print", newFunction(ScriptPrintFunction));

	setTw(tw);
}

void scriptEngine::eval(QString code)
{

	qDebug() << "evaluating " << code;

	QScriptValue output = QScriptEngine::evaluate(code);


	emit out(output.toString());

}

void scriptEngine::setTw(ColinStruct *tw)
{
	QScriptValue file = this->newQObject(tw);
	globalObject().setProperty("struct", file);
}
