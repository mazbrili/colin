#include "scriptengine.h"

#include <QtCore/QDebug>
#include <QtGui/QMainWindow>
#include <QtGui/QApplication>

#include "javascriptwraper.h"
#include "filelist.h"


QScriptValue ScriptPrintFunction(QScriptContext *ctxt, QScriptEngine *engine)
{
	scriptEngine *scE = static_cast<scriptEngine*>(engine);
	scE->emitOut(ctxt->argument(0).toString());
	return QScriptValue(engine, 0);
}

scriptEngine::scriptEngine(QObject *parent) :
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


	QScriptValue file = newQObject(&filelist::instance());
	globalObject().setProperty("files", file);

	globalObject().setProperty("print", newFunction(ScriptPrintFunction));



	this->evaluate(QString("function evalN(n, x){if(typeof(x)=='undefined'){x=n; n=0;} return ")+
				   "x*x*this.Narg[n][2]+"+
				   "x*this.Narg[n][1]+"+
				   "this.Narg[n][0];}");
	this->evaluate(QString("function evalQ(n, x){if(typeof(x)=='undefined'){x=n; n=0;} return ")+
				   "x*x*this.Qarg[n][2]+"+
				   "x*this.Qarg[n][1]+"+
				   "this.Qarg[n][0];}");
	this->evaluate(QString("function evalM(n, x){if(typeof(x)=='undefined'){x=n; n=0;} return ")+
				   "x*x*x*this.Marg[n][3]+"+
				   "x*x*this.Marg[n][2]+"+
				   "x*this.Marg[n][1]+"+
				   "this.Marg[n][0];}");

	this->evaluate(QString("function evalu(n, x){if(typeof(x)=='undefined'){x=n; n=0;} return ")+
				   "x*x*x*this.uarg[n][3]+"+
				   "x*x*this.uarg[n][2]+"+
				   "x*this.uarg[n][1]+"+
				   "this.uarg[n][0];}");
	this->evaluate(QString("function evalp(n, x){if(typeof(x)=='undefined'){x=n; n=0;} return ")+
				   "x*x*x*x*this.parg[n][4]+"+
				   "x*x*x*this.parg[n][3]+"+
				   "x*x*this.parg[n][2]+"+
				   "x*this.parg[n][1]+"+
				   "this.parg[n][0];}");
	this->evaluate(QString("function evalw(n, x){if(typeof(x)=='undefined'){x=n; n=0;} return ")+
				   "x*x*x*x*this.warg[n][4]+"+
				   "x*x*x*this.warg[n][3]+"+
				   "x*x*this.warg[n][2]+"+
				   "x*this.warg[n][1]+"+
				   "this.warg[n][0];}");



	connect(&filelist::instance(),					SIGNAL(currentChanged(ColinStruct*)),
			this,									SLOT(setTw(ColinStruct*)));

	if(filelist::instance().currentFile())
		setTw(filelist::instance().currentFile());
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
