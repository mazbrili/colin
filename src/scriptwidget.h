#ifndef SCRIPTWIDGET_H
#define SCRIPTWIDGET_H

#include "consolewidget.h"

class scriptWidget : public QWidget
{
    Q_OBJECT
public:
    explicit scriptWidget(QWidget *parent = 0);

signals:

public slots:

private:
	consoleWidget *scriptView;
//	ColinHMultiButton *buttonline;
//	ColinPushButtonPart *runButton,
//						*openButton,
//						*saveButton,
//						*saveAsButton;
};

#endif // SCRIPTWIDGET_H
