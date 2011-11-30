#ifndef COLINSIDEBAR_H
#define COLINSIDEBAR_H

#include <QtGui/QButtonGroup>
#include <QtGui/QPainter>

#include "slidingstackedwidget.h"
#include "printoverlaywidget.h"
#include "treeview.h"
#include "libsidebar.h"
#include "colinhmultibutton.h"
#include "colinicons.h"
#include "consolewidget.h"

class ColinSidebarHeader : public QWidget
{
	Q_OBJECT
public:
	explicit ColinSidebarHeader(QWidget *parent = 0);

	void paintEvent(QPaintEvent *);
signals:

public slots:

public:
	QButtonGroup *group;
private:
	QPushButton *lib,
				*tree,
				*print,
				*console;
	//ColinHMultiButton *buttons;
	//ColinPushButtonPart *lib,
	//					*tree,
	//					*print;


};

class ColinSidebarContainer : public SlidingStackedWidget
{
	Q_OBJECT
public:
	explicit ColinSidebarContainer(QWidget *parent = 0);

signals:

public slots:

private:
	printoverlayWidget *print;
	treeView *tV;
	libSidebar *library;
	consoleWidget *console;

};


class ColinSidebar : public QWidget
{
	Q_OBJECT
public:
	explicit ColinSidebar(QWidget *parent = 0);

signals:

public slots:


public:
	ColinSidebarHeader *header;
	ColinSidebarContainer *container;
private:

};


#endif // COLINSIDEBAR_H
