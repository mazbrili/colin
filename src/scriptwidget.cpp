#include "scriptwidget.h"

#include <QtGui/QVBoxLayout>

scriptWidget::scriptWidget(QWidget *parent) :
    QWidget(parent)
{
	QVBoxLayout *layout = new QVBoxLayout(this);

//	runButton = new ColinPushButtonPart(tr("run"), this);
//	openButton = new ColinPushButtonPart(tr("open"), this);
//	saveButton = new ColinPushButtonPart(tr("save"), this);
//	saveAsButton = new ColinPushButtonPart(tr("save as"), this);


//	openButton->setCutted(ColinPushButtonPart::Right);
//	saveButton->setCutted(ColinPushButtonPart::Right | ColinPushButtonPart::Left);
//	saveAsButton->setCutted(ColinPushButtonPart::Right | ColinPushButtonPart::Left);
//	runButton->setCutted(ColinPushButtonPart::Left);


//	buttonline =  new ColinHMultiButton(this);
//	buttonline->addButton(openButton);
//	buttonline->addButton(saveButton);
//	buttonline->addButton(saveAsButton);
//	buttonline->addButton(runButton);

//	layout->addWidget(buttonline);

	scriptView = new consoleWidget(this);

	layout->addWidget(scriptView);

}
