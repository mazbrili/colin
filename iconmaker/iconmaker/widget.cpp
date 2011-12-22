#include "widget.h"


#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QScrollArea>
#include <QtGui/QPushButton>
#include <QtGui/QApplication>
#include <QtCore/QDebug>
#include "icons.h"

class tWidget : public QWidget
{
public:
	tWidget(QWidget *parent = 0):QWidget(parent){}

	void paintEvent(QPaintEvent *){
		QPainter p(this);
		p.setRenderHint(QPainter::Antialiasing, true);
		iconMaker *iM = iconMaker::create(iconMaker::current);
		iM->paint(&p, iconMaker::light);
		delete iM;
	}
};

widget::widget(QWidget *parent) :
    QWidget(parent)
{
	this->setLayout(new QGridLayout());

	sizing = new QComboBox(this);
	sizing->addItem("32");
	sizing->addItem("64");
	sizing->addItem("128");
	sizing->addItem("512");
	sizing->setCurrentIndex(2);
	static_cast<QGridLayout*>(layout())->addWidget(sizing, 0, 0, 1, 1);

	lineWidth = new QComboBox(this);
	lineWidth->addItem("1");
	lineWidth->addItem("2");
	lineWidth->addItem("3");
	lineWidth->addItem("4");
	lineWidth->addItem("5");
	lineWidth->addItem("6");
	static_cast<QGridLayout*>(layout())->addWidget(lineWidth, 0, 1, 1, 1);

	coloring = new QComboBox(this);
	coloring->addItem("light");
	coloring->addItem("dark");
	static_cast<QGridLayout*>(layout())->addWidget(coloring, 0, 2, 1, 1);

	name = new QComboBox(this);
	QString n;
	int nr = 0;
	forever{
		n = iconMaker::iconName(static_cast<iconMaker::icon>(nr++));
		if(n.isEmpty())
			break;
		name->addItem(n);
	}
	name->setCurrentIndex(name->count()-1);
	iconMaker::current = static_cast<iconMaker::icon>(name->currentIndex());
	static_cast<QGridLayout*>(layout())->addWidget(name, 0, 3, 1, 1);

	save = new QPushButton("save", this);
	static_cast<QGridLayout*>(layout())->addWidget(save, 0, 4, 1, 1);


	QScrollArea *area = new QScrollArea(this);
	static_cast<QGridLayout*>(layout())->addWidget(area, 1, 0, 1, 5);
	area->setMinimumSize(320, 320);
	area->setAlignment(Qt::AlignCenter);


	paintTarget = new tWidget(area);
	paintTarget->setFixedSize(sizing->currentText().toInt(), sizing->currentText().toInt());
	area->setWidget(paintTarget);

	connect(sizing,				SIGNAL(activated(QString)),
			this,				SLOT(setTargetSize(QString)));

	connect(name,				SIGNAL(activated(QString)),
			this,				SLOT(setTargetName(QString)));

	connect(coloring,			SIGNAL(activated(QString)),
			this,				SLOT(setTargetColor(QString)));

	connect(lineWidth,			SIGNAL(activated(QString)),
			this,				SLOT(setTargetLineWidth(QString)));

	connect(save,				SIGNAL(clicked()),
			this,				SLOT(saveIcon()));
}


void widget::setTargetSize(QString s)
{
	paintTarget->setFixedSize(s.toInt(), s.toInt());
	paintTarget->update();
}

void widget::setTargetLineWidth(QString s)
{
	iconMaker::lineWidth = s.toInt();
	paintTarget->update();
}

void widget::setTargetColor(QString s)
{
	iconMaker::light == (s=="light");
	paintTarget->update();
}

void widget::setTargetName(QString s)
{
	iconMaker::current = iconMaker::fromString(s);
	paintTarget->update();
}

void widget::saveIcon()
{
	QPixmap pix(paintTarget->size());
	pix.fill(QColor(0, 0, 0, 0));
	QPainter p(&pix);
	p.setRenderHint(QPainter::Antialiasing, true);
	iconMaker *iM = iconMaker::create(iconMaker::current);
	iM->paint(&p, iconMaker::light);
	delete iM;

	pix.save(QApplication::applicationDirPath()+"/"+iconMaker::iconName(iconMaker::current)+".png");

	qDebug() << "icon saved as " << QApplication::applicationDirPath()+"/"+iconMaker::iconName(iconMaker::current)+".png";

}

