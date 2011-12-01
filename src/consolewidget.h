#ifndef CONSOLEWIDGET_H
#define CONSOLEWIDGET_H

#include <QtGui/QTextEdit>
#include "scriptengine.h"

class consoleWidget : public QTextEdit
{
    Q_OBJECT
public:
	explicit consoleWidget(QWidget *parent = 0);
	~consoleWidget();


	void keyPressEvent(QKeyEvent *e);


signals:

public slots:
	void docChanged (int position, int charsRemoved, int charsAdded);

private:
	QStringList history;
	QString buffer;
	int posInHist;
	bool appendToBuffer;
	scriptEngine *engine;

};

#endif // CONSOLEWIDGET_H
