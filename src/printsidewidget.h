#ifndef PRINTOVERLAYWIDGET_H
#define PRINTOVERLAYWIDGET_H

#include <QtGui/QWidget>
#include <QtCore/QRectF>
#include <QtGui/QLabel>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QComboBox>
#include <QtGui/QButtonGroup>

#include "colinicons.h"
#include "colinhmultibutton.h"
#include "colinboolslider.h"
#include "printpreview.h"

class printsideWidget : public QWidget
{
    Q_OBJECT
public:
	explicit printsideWidget(QWidget *parent = 0);
	~printsideWidget();

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
	void setContent();
	void paintRequest(QPrinter *printer);
private:
	QHBoxLayout *mainlayout;
	QVBoxLayout *vlayout;
	ColinPushButtonPart *save, *print, *printDia;
	QRectF clipRect;

	QScrollArea *settingArea;
	QWidget *setting;

	QLabel *printerLabel,
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

	QComboBox *printerSelection;

	ColinBoolSlider *addBLS,
					*onePerCLS,
					*allCLS,
					*forPerPage,
					*nodes_input,
					*beams_input,
					*loads_input,
					*nodes_res,
					*beam_fun,
					*beam_val;

	QButtonGroup *buttonGroup;

	QPrinter *printer;
	painterContent pContent;


	printPreview *preview;


};


#endif // PRINTOVERLAYWIDGET_H
