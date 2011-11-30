#include "consolewidget.h"
#include <QtGui/QKeyEvent>
#include <QtCore/QDebug>
#include <QtGui/QTextBlock>

#include "filelist.h"

consoleWidget::consoleWidget(QWidget *parent) :
    QTextEdit(parent)
{
	setUndoRedoEnabled(false);

	engine = 0;
	tw = 0;


	connect(&filelist::instance(),				SIGNAL(currentChanged(ColinStruct*)),
			this,								SLOT(setTw(ColinStruct*)));

}


void consoleWidget::keyPressEvent(QKeyEvent *e)
{
	if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
	{

		QTextBlock lastBlock = document()->lastBlock();
		QString code = lastBlock.text();
		code.remove(0, 1);
		engine->eval(code);

		this->append(">");
		e->setAccepted(true);
	}
//	else if(e->key() == Qt::Key_Backspace)
//	{
//		e->setAccepted(true);
//	}
	else if(e->key() == Qt::Key_Up)
	{
		e->setAccepted(true);
	}
	else if(e->key() == Qt::Key_Down)
	{
		e->setAccepted(true);
	}
	else
	{
		QTextEdit::keyPressEvent(e);
	}
}


void consoleWidget::setTw(ColinStruct *tw)
{
	if(this->tw)
		logs[this->tw]=this->document();

	if(engine)
		engine->disconnect(this,		SLOT(append(QString)));


	this->tw = tw;

	if(logs.contains(tw))
		this->setDocument(logs[tw]);


	engine = filelist::instance().Engine();

	connect(engine,				SIGNAL(out(QString)),
			this,				SLOT(append(QString)));

	if(!logs.contains(tw))
	{
		this->setDocument(new QTextDocument(this));

		connect(tw,					SIGNAL(destroyed(QObject*)),
				this,				SLOT(removeTw(QObject*)));

		setAlignment(Qt::AlignCenter);
		append( QString("\n")+
				QString("This is the JavaScript Console of Colin.\n")+
				QString("For an overview visit:\n")+
				QString("http://en.wikipedia.org/wiki/JavaScript\n")+
				QString("Video lectures are availible here:\n")+
				QString("http://yuiblog.com/crockford/\n\n"));
		setAlignment(Qt::AlignLeft);
		append(">1+1");
		append("2");
		append(">");
	}
}

void consoleWidget::removeTw(QObject *tw)
{
	logs.remove(static_cast<ColinStruct*>(tw));
}
