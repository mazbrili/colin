#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include <QtScript/QScriptEngine>

#include "colinstruct.h"

class scriptEngine : public QScriptEngine
{
    Q_OBJECT
public:
	explicit scriptEngine(QObject *parent = 0);

	void eval(QString code);


	void emitOut(QString string){emit out(string);}
signals:
	void out(QString);
public slots:
	void setTw(ColinStruct *tw);

};

#endif // SCRIPTENGINE_H
