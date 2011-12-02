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
#include "printpreview.h"

class printoverlayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit printoverlayWidget(QWidget *parent = 0);
	~printoverlayWidget();

	void addSlider(QLabel *label, ColinBoolSlider *slider, QString labelName, QGridLayout *vlayout);
	void showEvent(QShowEvent *e);
	void paintEvent(QPaintEvent *e);
	void keyPressEvent(QKeyEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void resizeEvent(QResizeEvent *e);

private:

	void setFontTooltip();
	void setPrinterToolTip();
signals:

public slots:
	void fontDialog();
	void printDialog();
	void savePdfDialog();
private:
	QHBoxLayout *mainlayout;
	QVBoxLayout *vlayout;
	ColinPushButtonPart *save, *print;
	QRectF clipRect;

	QLabel *printerLabel,
		   *formatLabel,
		   *orientationLabel,
		   *morePrinterLabel,
		   *addBLSLabel,
		   *onePerCLSLabel,
		   *allCLSLabel,
		   *fourPerPageLabel,
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

	ColinPushButtonPart *morePrinter,
						*fontSetter;
	QComboBox *printerSelection,
			  *format;

	ColinBoolSlider *orientation,
					*addBLS,
					*onePerCLS,
					*allCLS,
					*forPerPage,
					*nodes_input,
					*beams_input,
					*loads_input,
					*nodes_res,
					*beam_fun,
					*beam_val;


	QFont printerFont;
	QPrinter *printer;


	printPreview *preview;


};


#endif // PRINTOVERLAYWIDGET_H
