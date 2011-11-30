#include "javascriptwraper.h"

#include "colinnode.h"
#include "colinstruct.h"


QScriptValue NodetoScriptValue(QScriptEngine *engine, const ColinNode &n)
{
	qDebug() << "node -> script";
	qDebug() << "node->parent = " << n.getStruct();
	QScriptValue obj = engine->newObject();
	obj.setProperty("x", n.x());
	obj.setProperty("z", n.z());
	if(n.hasbearing())
		obj.setProperty("support", SupporttoScriptValue(engine, n.bearing()));
	else
		obj.setProperty("support", SupporttoScriptValue(engine, ColinSupport()));
	obj.setProperty("angle", n.angle());
	if(n.getStruct())
	{
		if(n.getStruct()->property("calculated").toBool())
		{
			QScriptValue u = engine->newArray(n.getStruct()->cls_n());
			QScriptValue w = engine->newArray(n.getStruct()->cls_n());
			QScriptValue phi = engine->newArray(n.getStruct()->cls_n());
			for(int i=0; i<n.getStruct()->cls_n(); i++){
				u.setProperty(i, n.u(i));
				w.setProperty(i, n.w(i));
				phi.setProperty(i, n.phi(i));
			}
			obj.setProperty("U", u);
			obj.setProperty("W", w);
			obj.setProperty("Phi", phi);

			obj.setProperty("u", n.u(0));
			obj.setProperty("w", n.w(0));
			obj.setProperty("phi", n.phi(0));

		}
	}

	return obj;
}

void NodefromScriptValue(const QScriptValue &obj, ColinNode &n)
{
	qDebug() << "script -> node";
	n.setX(obj.property("x").toNumber());
	n.setZ(obj.property("z").toNumber());
	if(obj.property("support").isValid())
	{
		ColinSupport s;
		SupportfromScriptValue(obj.property("support"), s);
		n.setBearing(s);
		n.bearing_editable().setAngle(obj.property("angle").toNumber());
	}
}

QScriptValue NodeCtor(QScriptContext *ctxt, QScriptEngine *eng)
{
	QScriptValue obj;
	if(ctxt->isCalledAsConstructor()){
		obj = ctxt->thisObject();
	}
	else {
		obj = eng->newObject();

		if(ctxt->argumentCount() == 2)
			obj = NodetoScriptValue(eng, ColinNode(ctxt->argument(0).toNumber(), ctxt->argument(1).toNumber()));
		else
			obj = NodetoScriptValue(eng, ColinNode());
		obj.setPrototype(ctxt->callee().property("prototype"));
	}
	return obj;
}


QScriptValue SupporttoScriptValue(QScriptEngine *engine, const ColinSupport &s)
{
	qDebug() << "support -> script";
	QScriptValue obj = engine->newObject();
	obj.setProperty("x", s.x());
	obj.setProperty("z", s.z());
	obj.setProperty("phi", s.phi());
	obj.setProperty("cx", s.c_x());
	obj.setProperty("cz", s.c_z());
	obj.setProperty("cphi", s.c_phi());
	return obj;
}

void SupportfromScriptValue(const QScriptValue &obj, ColinSupport &s)
{
	qDebug() << "script -> support";
	s.setX(obj.property("x").toBool());
	s.setZ(obj.property("z").toBool());
	s.setPhi(obj.property("phi").toBool());
	s.setCx(obj.property("cx").toNumber());
	s.setCz(obj.property("cz").toNumber());
	s.setCphi(obj.property("cphi").toNumber());
}


QScriptValue SupportCtor(QScriptContext *ctxt, QScriptEngine *eng)
{
	QScriptValue obj;
	if(ctxt->isCalledAsConstructor()){
		obj = ctxt->thisObject();
	}
	else {
		obj = eng->newObject();

		if(ctxt->argumentCount() == 3)
			obj = SupporttoScriptValue(eng, ColinSupport(ctxt->argument(0).toNumber(),
														 ctxt->argument(1).toNumber(),
														 ctxt->argument(2).toNumber()));
		else
			obj = SupporttoScriptValue(eng, ColinSupport());
		obj.setPrototype(ctxt->callee().property("prototype"));
	}
	return obj;
}

QScriptValue BeamtoScriptValue(QScriptEngine *engine, const ColinBeam &b)
{
	qDebug() << "beam -> script";
	QScriptValue obj = engine->newObject();
	obj.setProperty("leftNode", b.leftNodeI());
	obj.setProperty("rightNode", b.rightNodeI());
	obj.setProperty("material", b.MatI());
	obj.setProperty("crossSection", b.ProfileI());
	QScriptValue hing = engine->newArray(6);
	for(int i=0; i<6; i++)
		hing.setProperty(i, b.joint(i));
	obj.setProperty("hinges", hing);
	QScriptValue spri = engine->newArray(6);
	for(int i=0; i<6; i++)
		spri.setProperty(i, b.spring(i));
	obj.setProperty("springs", spri);
	if(b.getStruct())
	{
		if(b.getStruct()->isCalculated())
		{
		}
	}
	return obj;
}

void BeamfromScriptValue(const QScriptValue &obj, ColinBeam &b)
{
	qDebug() << "script -> beam";
	b.setLeftNode(obj.property("leftNode").toInteger());
	b.setRightNode(obj.property("rightNode").toInteger());
	b.setMat(obj.property("material").toInteger());
	b.setQs(obj.property("crossSection").toInteger());
}

QScriptValue BeamCtor(QScriptContext *ctxt, QScriptEngine *eng)
{
	QScriptValue obj;
	if(ctxt->isCalledAsConstructor()){
		obj = ctxt->thisObject();
	}
	else {
		obj = eng->newObject();

		if(ctxt->argumentCount() == 2)
		{
			ColinBeam beam;
			beam.setLeftNode(ctxt->argument(0).toInt32());
			beam.setRightNode(ctxt->argument(1).toInt32());
			obj = BeamtoScriptValue(eng, beam);
		}
		else
			obj = BeamtoScriptValue(eng, ColinBeam());
		obj.setPrototype(ctxt->callee().property("prototype"));
	}
	return obj;
}

QScriptValue LoadtoScriptValue(QScriptEngine *engine, const ColinLoad &l)
{
	qDebug() << "load -> script";
	QScriptValue obj = engine->newObject();
	obj.setProperty("x", l.Px());
	obj.setProperty("z", l.Pz());
	obj.setProperty("m", l.M());
	if(l.isOnBeam())
		obj.setProperty("beam", l.at());
	else
		obj.setProperty("node", l.at());
	obj.setProperty("type", l.typ());
	return obj;
}

void LoadfromScriptValue(const QScriptValue &obj, ColinLoad &l)
{

}

QScriptValue LoadCtor(QScriptContext *ctxt, QScriptEngine *eng)
{

}

QScriptValue BLStoScriptValue(QScriptEngine *engine, const ColinBLS &bls)
{

}

void BLSfromScriptValue(const QScriptValue &obj, ColinBLS &bls)
{

}

QScriptValue BLSCtor(QScriptContext *ctxt, QScriptEngine *eng)
{

}

QScriptValue CLStoScriptValue(QScriptEngine *engine, const ColinCLS &cls)
{

}

void CLSfromScriptValue(const QScriptValue &obj, ColinCLS &cls)
{

}

QScriptValue CLSCtor(QScriptContext *ctxt, QScriptEngine *eng)
{

}
