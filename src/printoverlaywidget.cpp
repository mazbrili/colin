#include "printoverlaywidget.h"

#include <QtGui/QPainter>
#include <QtGui/QKeyEvent>
#include <QtGui/QGraphicsBlurEffect>
#include <QtCore/QDebug>
#include <QtGui/QPrinter>
#include <QtGui/QPrintDialog>
#include <QtGui/QPrinterInfo>


printoverlayWidget::printoverlayWidget(QWidget *parent) :
    QWidget(parent)
{
	this->setAttribute(Qt::WA_TranslucentBackground, true);

	layout = new QVBoxLayout(this);

	layout->addSpacing(10);


	save = new ColinPushButtonPart(tr("save as pdf"), this);
	print = new ColinPushButtonPart(tr("print"), this);

	save->setCutted(ColinPushButtonPart::Right);
	print->setCutted(ColinPushButtonPart::Left);



	ColinHMultiButton *hbutton = new ColinHMultiButton(this);

	hbutton->setFixedHeight(save->sizeHint().height());

	hbutton->addButton(save);
	hbutton->addButton(print);

	layout->addWidget(hbutton);



	QFont bigfont = font();
	bigfont.setBold(true);

	printing = new QLabel(tr("[print]"), this);
	printing->setFont(bigfont);

	plots = new QLabel(tr("[plots]"), this);
	plots->setFont(bigfont);

	input = new QLabel(tr("[input]"), this);
	input->setFont(bigfont);

	results = new QLabel(tr("[results]"), this);
	results->setFont(bigfont);



	QGridLayout *sliderBox = new QGridLayout();
	layout->addLayout(sliderBox);

	sliderBox->addWidget(printing, sliderBox->rowCount(), 0, 1, 2);

	printerLabel = new QLabel(tr("printer"), this);
	printer = new QComboBox(this);

	sliderBox->addWidget(printerLabel, sliderBox->rowCount(), 0, 1, 1);
	sliderBox->addWidget(printer, sliderBox->rowCount()-1, 1, 1, 1);


	foreach(QPrinterInfo info, QPrinterInfo::availablePrinters())
	{
		printer->addItem(info.printerName());
		if(info.isDefault())
			printer->setCurrentIndex(printer->count()-1);
	}

	formatLabel = new QLabel(tr("format"), this);

	format = new QComboBox(this);
	format->addItem("A4");
	format->addItem("A3");
	format->addItem("US-Letter");
	format->addItem("other");

	sliderBox->addWidget(formatLabel, sliderBox->rowCount(), 0, 1, 1);
	sliderBox->addWidget(format, sliderBox->rowCount()-1, 1, 1, 1);


	morePrinterLabel = new QLabel(tr("more"), this);

	morePrinter = new QPushButton(this);
	morePrinter->setIcon(colinIcons::instance().icon(Colin::Settings));

	sliderBox->addWidget(morePrinterLabel, sliderBox->rowCount(), 0, 1, 1);
	sliderBox->addWidget(morePrinter, sliderBox->rowCount()-1, 1, 1, 1, Qt::AlignRight);


	sliderBox->addWidget(plots, sliderBox->rowCount(), 0, 1, 2);

	addBLSLabel = new QLabel(this);
	addBLS = new ColinBoolSlider(this);
	addSlider(addBLSLabel, addBLS, tr("add plots of basic load sets"), sliderBox);

	allCLSLabel = new QLabel(this);
	allCLS = new ColinBoolSlider(this);
	addSlider(allCLSLabel, allCLS, tr("all combined load sets together"), sliderBox);

	onePerCLSLabel = new QLabel(this);
	onePerCLS = new ColinBoolSlider(this);
	addSlider(onePerCLSLabel, onePerCLS, tr("one plot per load set"), sliderBox);

	onePerFunctionLabel = new QLabel(this);
	onePerFunction = new ColinBoolSlider(this);
	addSlider(onePerFunctionLabel, onePerFunction, tr("4 pictures per page"), sliderBox);


	orientationLabel = new QLabel(tr("orientation"), this);

	orientation = new ColinBoolSlider(this);
	orientation->setFixedHeight(save->sizeHint().height());
	orientation->setText("Po", true);
	orientation->setText("LS", false);

	sliderBox->addWidget(orientationLabel, sliderBox->rowCount(), 0, 1, 1);
	sliderBox->addWidget(orientation, sliderBox->rowCount()-1, 1, 1, 1, Qt::AlignRight);

	sliderBox->addWidget(input, sliderBox->rowCount(), 0, 1, 2);


	nodes_inputLabel = new QLabel(this);
	nodes_input = new ColinBoolSlider(this);
	addSlider(nodes_inputLabel, nodes_input, tr("nodes"), sliderBox);

	beams_inputLabel = new QLabel(this);
	beams_input = new ColinBoolSlider(this);
	addSlider(beams_inputLabel, beams_input, tr("beams"), sliderBox);

	loads_inputLabel = new QLabel(this);
	loads_input = new ColinBoolSlider(this);
	addSlider(loads_inputLabel, loads_input, tr("loads"), sliderBox);


	sliderBox->addWidget(results, sliderBox->rowCount(), 0, 1, 2);

	nodes_resLabel = new QLabel(this);
	nodes_res = new ColinBoolSlider(this);
	addSlider(nodes_resLabel, nodes_res, tr("nodes"), sliderBox);

	beam_funLabel = new QLabel(this);
	beam_fun = new ColinBoolSlider(this);
	addSlider(beam_funLabel, beam_fun, tr("beams(functions)"), sliderBox);

	beam_valLabel = new QLabel(this);
	beam_val = new ColinBoolSlider(this);
	addSlider(beam_valLabel, beam_val, tr("beams(values)"), sliderBox);




}

void printoverlayWidget::addSlider(QLabel *label, ColinBoolSlider *slider, QString labelName, QGridLayout *layout)
{
	label->setParent(this);
	slider->setParent(this);
	label->setText(labelName);
	slider->setFixedHeight(save->sizeHint().height());
	slider->setText(tr("yes"), true);
	slider->setText(tr("no"), false);

	int rows = layout->rowCount();
	layout->addWidget(label, rows, 0, 1, 1);
	layout->addWidget(slider, rows, 1, 1, 1, Qt::AlignRight);

}

void printoverlayWidget::showEvent(QShowEvent *e)
{
	//layout->setGeometry(QRect(width()-400, 0, 400, height()));
	//clipRect = QRect(parentWidget()->rect().adjusted(20, 20, -200, -20));
	setFocus(Qt::PopupFocusReason);
	QWidget::showEvent(e);
}

void printoverlayWidget::paintEvent(QPaintEvent *e)
{
	//QPainter p(this);
	//p.setRenderHint(QPainter::Antialiasing, true);
	//QPainterPath clipping;
	//clipping.addRect(rect());
	//clipping.addRect(clipRect);
	//p.setClipPath(clipping);
	//p.fillRect(rect(), QColor(50, 50, 50, 200));

}

void printoverlayWidget::keyPressEvent(QKeyEvent *e)
{
	if(e->key() == Qt::Key_Escape)
		this->hide();
	QWidget::keyPressEvent(e);
}

void printoverlayWidget::mouseMoveEvent(QMouseEvent *e)
{
	QWidget::mouseMoveEvent(e);
}

void printoverlayWidget::mousePressEvent(QMouseEvent *e)
{
	this->setFocus();
	QWidget::mousePressEvent(e);
}

void printoverlayWidget::mouseReleaseEvent(QMouseEvent *e)
{
	QWidget::mouseReleaseEvent(e);
}

void printoverlayWidget::resizeEvent(QResizeEvent *e)
{
	//clipRect.adjust(0, 0, width()-e->oldSize().width(), height()-e->oldSize().height());

	//layout->setGeometry(QRect(width()-400, 0, 400, height()));
	//qDebug() << layout->geometry();
}
