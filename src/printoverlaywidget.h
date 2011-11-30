#ifndef PRINTOVERLAYWIDGET_H
#define PRINTOVERLAYWIDGET_H

#include <QtGui/QWidget>
#include <QtCore/QRectF>
#include <QtGui/QLabel>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QComboBox>

#include "colinicons.h"
#include "colinhmultibutton.h"
#include "colinboolslider.h"

class printoverlayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit printoverlayWidget(QWidget *parent = 0);

	void addSlider(QLabel *label, ColinBoolSlider *slider, QString labelName, QGridLayout *layout);
	void showEvent(QShowEvent *e);
	void paintEvent(QPaintEvent *e);
	void keyPressEvent(QKeyEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void resizeEvent(QResizeEvent *e);
signals:

public slots:

private:
	QVBoxLayout *layout;
	ColinPushButtonPart *save, *print;
	QRectF clipRect;

	QLabel *printerLabel,
		   *formatLabel,
		   *orientationLabel,
		   *morePrinterLabel,
		   *addBLSLabel,
		   *onePerCLSLabel,
		   *allCLSLabel,
		   *onePerFunctionLabel,
		   *nodes_inputLabel,
		   *beams_inputLabel,
		   *loads_inputLabel,
		   *nodes_resLabel,
		   *beam_funLabel,
		   *beam_valLabel;

	QLabel *printing,
		   *input,
		   *results,
		   *plots;

	QPushButton *morePrinter;
	QComboBox *printer,
			  *format;

	ColinBoolSlider *orientation,
					*addBLS,
					*onePerCLS,
					*allCLS,
					*onePerFunction,
					*nodes_input,
					*beams_input,
					*loads_input,
					*nodes_res,
					*beam_fun,
					*beam_val;


};


#endif // PRINTOVERLAYWIDGET_H
