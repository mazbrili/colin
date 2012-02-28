#include "javascriptwraper.h"

#include "colinnode.h"
#include "colinstruct.h"

#include <QtGui/QApplication>
#include <QtGui/QWidget>


QScriptValue NodetoScriptValue(QScriptEngine *engine, const ColinNode &n)
{
#ifdef SCRIPTCONVERT_DEBUG
	qDebug() << "node -> script";
#endif
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
#ifdef SCRIPTCONVERT_DEBUG
	qDebug() << "script -> node";
#endif
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
#ifdef SCRIPTCONVERT_DEBUG
	qDebug() << "support -> script";
#endif
	QScriptValue obj = engine->newObject();
	obj.setProperty("x", s.x());
	obj.setProperty("z", s.z());
	obj.setProperty("phi", s.phi());
	obj.setProperty("cx", s.c_x());
	obj.setProperty("cz", s.c_z());
	obj.setProperty("cphi", s.c_phi());
	if(s.getStruct())
	{
		if(s.getStruct()->property("calculated").toBool())
		{
			QScriptValue x = engine->newArray(s.getStruct()->cls_n());
			QScriptValue z = engine->newArray(s.getStruct()->cls_n());
			QScriptValue phi = engine->newArray(s.getStruct()->cls_n());
			for(int i=0; i<s.getStruct()->cls_n(); i++){
				x.setProperty(i, s.a_x(i));
				z.setProperty(i, s.a_z(i));
				phi.setProperty(i, s.a_m(i));
			}
			obj.setProperty("X", x);
			obj.setProperty("Z", z);
			obj.setProperty("Phi", phi);

			obj.setProperty("u", s.a_x(0));
			obj.setProperty("w", s.a_z(0));
			obj.setProperty("phi", s.a_m(0));
		}
	}
	return obj;
}

void SupportfromScriptValue(const QScriptValue &obj, ColinSupport &s)
{
#ifdef SCRIPTCONVERT_DEBUG
	qDebug() << "script -> support";
#endif
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
#ifdef SCRIPTCONVERT_DEBUG
	qDebug() << "beam -> script";
#endif
	QScriptValue obj = engine->newObject();
	obj.setProperty("leftNode", b.leftNodeI());
	obj.setProperty("rightNode", b.rightNodeI());
	obj.setProperty("material", b.MatI());
	obj.setProperty("crossSection", b.ProfileI());
	obj.setProperty("l", b.l());
	obj.setProperty("angle", b.angle());
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
			QScriptValue args;
			//N
			args = engine->newArray(b.getStruct()->cls_n());
			for(int j=0; j<b.getStruct()->cls_n(); j++)
			{
				QScriptValue N = engine->newArray(3);
				for(int i=0; i<3; i++) N.setProperty(i, b.Nconst(j).a[i]);
				args.setProperty(j, N);
			}
			obj.setProperty("Narg", args);
			obj.setProperty("N", engine->globalObject().property("evalN"));

			//Q
			args = engine->newArray(b.getStruct()->cls_n());
			for(int j=0; j<b.getStruct()->cls_n(); j++)
			{
				QScriptValue Q = engine->newArray(3);
				for(int i=0; i<3; i++) Q.setProperty(i, b.Qconst(j).a[i]);
				args.setProperty(j, Q);
			}
			obj.setProperty("Qarg", args);
			obj.setProperty("Q", engine->globalObject().property("evalQ"));

			//M
			args = engine->newArray(b.getStruct()->cls_n());
			for(int j=0; j<b.getStruct()->cls_n(); j++)
			{
				QScriptValue M = engine->newArray(4);
				for(int i=0; i<4; i++) M.setProperty(i, b.Mconst(j).a[i]);
				args.setProperty(j, M);
			}
			obj.setProperty("Marg", args);
			obj.setProperty("M", engine->globalObject().property("evalM"));

			//u
			args = engine->newArray(b.getStruct()->cls_n());
			for(int j=0; j<b.getStruct()->cls_n(); j++)
			{
				QScriptValue u = engine->newArray(4);
				for(int i=0; i<4; i++) u.setProperty(i, b.uconst(j).a[i]);
				args.setProperty(j, u);
			}
			obj.setProperty("uarg", args);
			obj.setProperty("u", engine->globalObject().property("evalu"));

			//phi
			args = engine->newArray(b.getStruct()->cls_n());
			for(int j=0; j<b.getStruct()->cls_n(); j++)
			{
				QScriptValue phi = engine->newArray(4);
				for(int i=0; i<5; i++) phi.setProperty(i, b.phiconst(j).a[i]);
				args.setProperty(j, phi);
			}
			obj.setProperty("parg", args);
			obj.setProperty("phi", engine->globalObject().property("evalp"));

			//w
			args = engine->newArray(b.getStruct()->cls_n());
			for(int j=0; j<b.getStruct()->cls_n(); j++)
			{
				QScriptValue w = engine->newArray(4);
				for(int i=0; i<6; i++) w.setProperty(i, b.wconst(j).a[i]);
				args.setProperty(j, w);
			}
			obj.setProperty("warg", args);
			obj.setProperty("w", engine->globalObject().property("evalw"));

		}
	}
	return obj;
}

void BeamfromScriptValue(const QScriptValue &obj, ColinBeam &b)
{
#ifdef SCRIPTCONVERT_DEBUG
	qDebug() << "script -> beam";
#endif
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
#ifdef SCRIPTCONVERT_DEBUG
	qDebug() << "load -> script";
#endif
	QScriptValue obj = engine->newObject();
	obj.setProperty("x", l.Px());
	obj.setProperty("z", l.Pz());
	obj.setProperty("m", l.M());
	if(l.isOnBeam())
		obj.setProperty("beam", l.at());
	else
		obj.setProperty("node", l.at());
	obj.setProperty("type", l.typ());
	obj.setProperty("set", l.set());
	return obj;
}

void LoadfromScriptValue(const QScriptValue &obj, ColinLoad &l)
{
#ifdef SCRIPTCONVERT_DEBUG
	qDebug() << "script -> load";
#endif
	l.setPx(obj.property("x").toNumber());
	l.setPz(obj.property("z").toNumber());
	l.setM(obj.property("m").toNumber());
	l.setTyp(static_cast<ColinLoad::form>(obj.property("type").toInt32()));
	l.setSet(obj.property("set").toInt32());
	if(l.isOnBeam())
		l.setBeam(obj.property("beam").toInt32());
	else
		l.setBeam(obj.property("node").toInt32());
}

QScriptValue LoadCtor(QScriptContext *ctxt, QScriptEngine *eng)
{
	QScriptValue obj;
	if(ctxt->isCalledAsConstructor()){
		obj = ctxt->thisObject();
	}
	else {
		obj = eng->newObject();

		if(ctxt->argumentCount() ==5)
			obj = LoadtoScriptValue(eng, ColinLoad(static_cast<ColinLoad::form>(ctxt->argument(0).toInt32()),
												   ctxt->argument(1).toInt32(),
												   ctxt->argument(2).toNumber(),
												   ctxt->argument(3).toNumber(),
												   ctxt->argument(4).toNumber(),
												   -1));
		else if(ctxt->argumentCount() == 6)
			obj = LoadtoScriptValue(eng, ColinLoad(static_cast<ColinLoad::form>(ctxt->argument(0).toInt32()),
												   ctxt->argument(1).toInt32(),
												   ctxt->argument(2).toNumber(),
												   ctxt->argument(3).toNumber(),
												   ctxt->argument(4).toNumber(),
												   ctxt->argument(5).toInt32()));
		else
			obj = LoadtoScriptValue(eng, ColinLoad());
		obj.setPrototype(ctxt->callee().property("prototype"));
	}
	return obj;}

QScriptValue BLStoScriptValue(QScriptEngine *engine, const ColinBLS &bls)
{
#ifdef SCRIPTCONVERT_DEBUG
	qDebug() << "bls -> script";
#endif
	QScriptValue obj = engine->newObject();
	obj.setProperty("name", bls.name());
	return obj;
}

void BLSfromScriptValue(const QScriptValue &obj, ColinBLS &bls)
{
#ifdef SCRIPTCONVERT_DEBUG
	qDebug() << "script -> bls";
#endif
	bls.setName(obj.property("name").toString());
}

QScriptValue BLSCtor(QScriptContext *ctxt, QScriptEngine *eng)
{
	QScriptValue obj;
	if(ctxt->isCalledAsConstructor()){
		obj = ctxt->thisObject();
	}
	else {
		obj = eng->newObject();

		if(ctxt->argumentCount() == 1)
			obj = BLStoScriptValue(eng, ColinBLS(ctxt->argument(0).toString()));
		else
			obj = BLStoScriptValue(eng, ColinBLS());
		obj.setPrototype(ctxt->callee().property("prototype"));
	}
	return obj;
}

QScriptValue CLStoScriptValue(QScriptEngine *engine, const ColinCLS &cls)
{
#ifdef SCRIPTCONVERT_DEBUG
	qDebug() << "cls -> script";
#endif
	QScriptValue obj = engine->newObject();
	obj.setProperty("name", cls.name());
	obj.setProperty("active", cls.isSelected());
	QScriptValue bls = engine->newArray(cls.count());
	for(int i=0; i<cls.count(); i++)
	{
		QScriptValue item = engine->newObject();
		item.setProperty("bls", cls.bls(i));
		item.setProperty("factor", cls.fac(i));
		bls.setProperty(i, item);
	}
	obj.setProperty("children", bls);
	return obj;
}

void CLSfromScriptValue(const QScriptValue &obj, ColinCLS &cls)
{
#ifdef SCRIPTCONVERT_DEBUG
	qDebug() << "script -> cls";
#endif
	cls.setName(obj.property("name").toString());
	cls.select(obj.property("active").toBool());
	QScriptValue array = obj.property("children");
	if(!array.isArray())
		return;
	int length = array.property("length").toInt32();

	for(int i = 0; i<length; i++){
		cls.addBLS(array.property(i).property("bls").toInt32(),
				   array.property(i).property("factor").toNumber());
	}
}

QScriptValue CLSCtor(QScriptContext *ctxt, QScriptEngine *eng)
{
	QScriptValue obj;
	if(ctxt->isCalledAsConstructor()){
		obj = ctxt->thisObject();
	}
	else {
		obj = eng->newObject();

		if(ctxt->argumentCount() == 1)
			obj = CLStoScriptValue(eng, ColinCLS(ctxt->argument(0).toString()));
		else
			obj = CLStoScriptValue(eng, ColinCLS());
		obj.setPrototype(ctxt->callee().property("prototype"));
	}
	return obj;
}
