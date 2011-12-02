#include "printoverlaywidget.h"

#include <QtGui/QPainter>
#include <QtGui/QKeyEvent>
#include <QtGui/QGraphicsBlurEffect>
#include <QtCore/QDebug>
#include <QtGui/QPrinter>
#include <QtGui/QPrintDialog>
#include <QtGui/QPrinterInfo>
#include <QtGui/QFontDialog>


printoverlayWidget::printoverlayWidget(QWidget *parent) :
    QWidget(parent)
{
	this->setAttribute(Qt::WA_TranslucentBackground, true);

	mainlayout = new QHBoxLayout(this);
	vlayout = new QVBoxLayout();

	mainlayout->addLayout(vlayout);
	vlayout->addSpacing(10);


	preview = new printPreview(this);

	mainlayout->addWidget(preview);

	save = new ColinPushButtonPart(tr("save as pdf"), this);
	print = new ColinPushButtonPart(tr("print"), this);

	save->setCutted(ColinPushButtonPart::Right);
	print->setCutted(ColinPushButtonPart::Left);



	ColinHMultiButton *hbutton = new ColinHMultiButton(this);

	hbutton->setFixedHeight(save->sizeHint().height());

	hbutton->addButton(save);
	hbutton->addButton(print);

	vlayout->addWidget(hbutton);



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
	vlayout->addLayout(sliderBox);

	sliderBox->addWidget(printing, sliderBox->rowCount(), 0, 1, 2);

	printerLabel = new QLabel(tr("printer"), this);
	printerSelection = new QComboBox(this);

	sliderBox->addWidget(printerLabel, sliderBox->rowCount(), 0, 1, 1);
	sliderBox->addWidget(printerSelection, sliderBox->rowCount()-1, 1, 1, 1);


	foreach(QPrinterInfo info, QPrinterInfo::availablePrinters())
	{
		printerSelection->addItem(info.printerName());
		if(info.isDefault())
			printerSelection->setCurrentIndex(printerSelection->count()-1);
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

	morePrinter = new ColinPushButtonPart(tr("printer"), this);
	fontSetter = new ColinPushButtonPart(tr("font"), this);

	hbutton = new ColinHMultiButton(this);
	hbutton->addButton(morePrinter);
	hbutton->addButton(fontSetter);
	hbutton->setFixedHeight(morePrinter->sizeHint().height());


	sliderBox->addWidget(morePrinterLabel, sliderBox->rowCount(), 0, 1, 1);
	sliderBox->addWidget(hbutton, sliderBox->rowCount()-1, 1, 1, 1, Qt::AlignRight);




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

	fourPerPageLabel = new QLabel(this);
	forPerPage = new ColinBoolSlider(this);
	addSlider(fourPerPageLabel, forPerPage, tr("4 pictures per page"), sliderBox);


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






	connect(fontSetter,				SIGNAL(clicked()),
			this,					SLOT(fontDialog()));

	connect(morePrinter,			SIGNAL(clicked()),
			this,					SLOT(printDialog()));



	/******************************************/

	QSettings settings("clazzes.org", "Colin", this);

	printerFont.fromString(settings.value("printing/font", this->font().toString()).toString());

	orientation->setChecked(settings.value("printing/orientation", false).toBool());
	addBLS->setChecked(settings.value("printing/bls", false).toBool());
	onePerCLS->setChecked(settings.value("printing/onePerCls", false).toBool());
	allCLS->setChecked(settings.value("printing/allCls", true).toBool());
	forPerPage->setChecked(settings.value("printing/fourPerPage", true).toBool());
	nodes_input->setChecked(settings.value("printing/nodesInput", false).toBool());
	beams_input->setChecked(settings.value("printing/beamsInput", false).toBool());
	loads_input->setChecked(settings.value("printing/loadsInput", false).toBool());
	nodes_res->setChecked(settings.value("printing/nodesRes", true).toBool());
	beam_fun->setChecked(settings.value("printing/beamsF", true).toBool());
	beam_val->setChecked(settings.value("printing/beamsV", false).toBool());


	printer = new QPrinter();


	setFontTooltip();

}

printoverlayWidget::~printoverlayWidget()
{
	QSettings settings("clazzes.org", "Colin", this);

	settings.setValue("printing/font", printerFont.toString());

	settings.setValue("printing/orientation", orientation->isChecked());
	settings.setValue("printing/bls", addBLS->isChecked());
	settings.setValue("printing/onePerCls", onePerCLS->isChecked());
	settings.setValue("printing/allCls", allCLS->isChecked());
	settings.setValue("printing/fourPerPage", forPerPage->isChecked());
	settings.setValue("printing/nodesInput", nodes_input->isChecked());
	settings.setValue("printing/beamsInput", beams_input->isChecked());
	settings.setValue("printing/loadsInput", loads_input->isChecked());
	settings.setValue("printing/nodesRes", nodes_res->isChecked());
	settings.setValue("printing/beamsF", beam_fun->isChecked());
	settings.setValue("printing/beamsV", beam_val->isChecked());

	delete printer;
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


void printoverlayWidget::fontDialog()
{
	QFontDialog dia(printerFont, this);
	dia.exec();
	printerFont = dia.selectedFont();


	setFontTooltip();
}

void printoverlayWidget::printDialog()
{
	QPrintDialog dia(printer, this);
	dia.exec();

	if(printer->orientation() == QPrinter::Landscape)
	{
		if(orientation->isChecked())
			orientation->setChecked(false);
	}
	else
	{
		if(!orientation->isChecked())
			orientation->setChecked(true);
	}
}

void printoverlayWidget::savePdfDialog()
{

}


void printoverlayWidget::setFontTooltip()
{
	fontSetter->setToolTip(printerFont.toString());
	fontSetter->setFont(printerFont);
}

void printoverlayWidget::setPrinterToolTip()
{

}
