#include "colinsidebar.h"

#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QStylePainter>
#include <QtGui/QStyleOptionFrame>

ColinSidebarHeader::ColinSidebarHeader(QWidget *parent) :
	QWidget(parent)
{
	/*
	tree = new ColinPushButtonPart(tr("tree view"), this);
	lib = new ColinPushButtonPart(tr("libaray"), this);
	print = new ColinPushButtonPart(tr("print"), this);

	tree->setCutted(ColinPushButtonPart::Right);
	lib->setCutted(ColinPushButtonPart::Right);
	lib->setCutted(ColinPushButtonPart::Left);
	print->setCutted(ColinPushButtonPart::Left);

	tree->setCheckable(true);
	lib->setCheckable(true);
	print->setCheckable(true);

	buttons =  new ColinHMultiButton(this);

	buttons->addButton(tree);
	buttons->addButton(lib);
	buttons->addButton(print);

	buttons->setFixedHeight(tree->sizeHint().height());
	*/

	tree = new QPushButton(tr("tree"), this);
	lib = new QPushButton(tr("library"), this);
	print = new QPushButton(tr("print"), this);
	console = new QPushButton(tr("console"), this);

	tree->setFlat(true);
	lib->setFlat(true);
	print->setFlat(true);
	console->setFlat(true);

	QHBoxLayout *l = new QHBoxLayout(this);
	//l->addWidget(buttons);
	l->addWidget(tree);
	l->addWidget(lib);
	l->addWidget(print);
	l->addWidget(console);


	l->setMargin(0);


	group = new QButtonGroup(this);
	group->setExclusive(true);
	group->addButton(tree, 0);
	group->addButton(lib, 1);
	group->addButton(print, 2);
	group->addButton(console, 3);

	lib->setChecked(true);

}

void ColinSidebarHeader::paintEvent(QPaintEvent *)
{
	QPainter p(this);

	QLinearGradient grad(0, 0, 0, height());
	grad.setColorAt(0, palette().color(QPalette::Light));
	grad.setColorAt(1, palette().color(QPalette::Window));
	p.setPen(palette().color(QPalette::Dark));
	p.setBrush(grad);
	p.drawRect(rect().adjusted(0, 0, -1, -1));

}

ColinSidebarContainer::ColinSidebarContainer(QWidget *parent) :
	SlidingStackedWidget(parent)
{

	print = new printsideWidget(this);
	tV = new treeView(this);
	library = new libSidebar(this);
	console = new consoleWidget(this);
	this->addWidget(tV);
	this->addWidget(library);
	this->addWidget(print);
	this->addWidget(console);
}

ColinSidebar::ColinSidebar(QWidget *parent) :
	QWidget(parent)
{
	QVBoxLayout *l = new QVBoxLayout(this);

	header = new ColinSidebarHeader(this);
	container = new ColinSidebarContainer(this);

	l->addWidget(header);
	l->addWidget(container);

	l->setMargin(0);
	l->setSpacing(0);

	container->setCurrentIndex(header->group->checkedId());
	container->setAnimation(QEasingCurve::Linear);
	container->setSpeed(200);

	connect(header->group,			SIGNAL(buttonClicked(int)),
			container,				SLOT(slideInIdx(int)));

}
