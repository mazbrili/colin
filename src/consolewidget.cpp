#include "consolewidget.h"
#include <QtGui/QKeyEvent>
#include <QtCore/QDebug>
#include <QtGui/QTextBlock>

#include <QtCore/QSettings>

#include "filelist.h"

consoleWidget::consoleWidget(QWidget *parent) :
    QTextEdit(parent)
{
	appendToBuffer=false;
	setUndoRedoEnabled(false);
	document()->setMaximumBlockCount(100);

	engine = new scriptEngine(this);

	connect(engine,							SIGNAL(out(QString)),
			this,							SLOT(append(QString)));

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

	posInHist = 0;

	QSettings settings("clazzes.org", "Colin");
	history = settings.value("console/history").toStringList();

	connect(this->document(),				SIGNAL(contentsChange(int,int,int)),
			this,							SLOT(docChanged(int,int,int)));


}

consoleWidget::~consoleWidget()
{
	while(history.size()>40)
		history.takeLast();
	QSettings settings("clazzes.org", "Colin");
	settings.setValue("console/history", history);

}

void consoleWidget::docChanged(int position, int charsRemoved, int charsAdded)
{
	if(!appendToBuffer)
		return;

	if(!charsAdded)
		return;
	QString text = document()->lastBlock().text();
	QString toAppend;
	toAppend.append(text.right(charsAdded));
	buffer.append(toAppend);
}

void consoleWidget::keyPressEvent(QKeyEvent *e)
{
	QTextCursor tc = textCursor();
	tc.movePosition(QTextCursor::End);
	setTextCursor(tc);

	if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
	{

		//if(engine->checkSyntax(buffer).state() == QScriptSyntaxCheckResult::Valid)
		//{
			engine->eval(buffer);
			history.push_front(buffer);
			buffer.clear();
			this->append(">");
		//	posInHist=-1;
			e->setAccepted(true);
		//}
		//else
		//{
		//	appendToBuffer=true;
		//	QTextEdit::keyPressEvent(e);
		//	appendToBuffer=false;
		//}
	}
	else if(e->key() == Qt::Key_Backspace)
	{
		if(!buffer.isEmpty()){
			buffer.chop(1);
			QTextEdit::keyPressEvent(e);
		}
		else
			e->setAccepted(true);
	}
	else if(e->key() == Qt::Key_Up)
	{
		if(!history.empty())
		{
			for(int i=0; i<buffer.size(); i++)
				textCursor().deletePreviousChar();
			buffer = history.at(posInHist);
			textCursor().insertText(buffer);
			posInHist++;
			if(posInHist>=history.size())
				posInHist=0;
			e->setAccepted(true);
		}
	}
	else if(e->key() == Qt::Key_Down)
	{
		if(!history.isEmpty())
		{
			for(int i=0; i<buffer.size(); i++)
				textCursor().deletePreviousChar();
			buffer = history.at(posInHist);
			textCursor().insertText(buffer);
			posInHist--;
			if(posInHist<0)
				posInHist=history.size()-1;
			e->setAccepted(true);
		}
	}
	else
	{
		appendToBuffer=true;
		QTextEdit::keyPressEvent(e);
		appendToBuffer=false;
	}
	qDebug() << "buffer = " << buffer;

}

