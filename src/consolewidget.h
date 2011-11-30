#ifndef CONSOLEWIDGET_H
#define CONSOLEWIDGET_H

#include <QtGui/QTextEdit>
#include "scriptengine.h"

class consoleWidget : public QTextEdit
{
    Q_OBJECT
public:
	explicit consoleWidget(QWidget *parent = 0);

	void keyPressEvent(QKeyEvent *e);


signals:

public slots:
	void setTw(ColinStruct *tw);
	void removeTw(QObject *tw);
private:
	scriptEngine *engine;
	QMap<ColinStruct*, QTextDocument*> logs;
	ColinStruct *tw;

};

#endif // CONSOLEWIDGET_H
