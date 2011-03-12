#ifndef ABSTRACTCOLINTOOLTIP_H
#define ABSTRACTCOLINTOOLTIP_H

#include <QtGui/QLabel>
#include <QtGui/QStylePainter>
#include <QtGui/QStyleOptionFrame>
#include <QtGui/QToolTip>

class abstractColinToolTip : public QLabel
{
public:
    abstractColinToolTip(QWidget *parent = 0) :
	QLabel(parent)
    {
	
	setWindowFlags(Qt::ToolTip);
	/*
	setAttribute(Qt::WA_NoSystemBackground, true);
	setAttribute(Qt::WA_TranslucentBackground, true);
	setAttribute(Qt::WA_TransparentForMouseEvents, true);
	*/

	setForegroundRole(QPalette::ToolTipText);
	setBackgroundRole(QPalette::ToolTipBase);
	setPalette(QToolTip::palette());
	ensurePolished();
	setFrameStyle(QFrame::NoFrame);
	setWindowOpacity(style()->styleHint(QStyle::SH_ToolTipLabel_Opacity, 0, this)/255.);
    }

    void paintEvent(QPaintEvent *e)
    {
	QStylePainter p(this);
	QStyleOptionFrame opt;
	opt.init(this);
	p.drawPrimitive(QStyle::PE_PanelTipLabel, opt);
	p.end();

	QLabel::paintEvent(e);
	/*
	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing, true);



	QLinearGradient grad(0, 0, 0, height());
	grad.setColorAt(0, QColor(200, 200, 200, 100));
	grad.setColorAt(1, QColor(178, 178, 178, 100));
	p.setPen(QPen(QBrush(grad), 1));


	grad.setFinalStop(width(), height());
	grad.setColorAt(1, QColor(70, 70, 70, 200));
	grad.setColorAt(0, QColor(0, 0, 0, 200));
	p.setBrush(QBrush(grad));

	//    p.setBrush(QColor(0, 0, 0, 200));
	p.drawRoundedRect(QRect(3, 3, width()-6, height()-6), 5, 5);
	*/

    }

    QSize sizeHint() const
    {
	return QWidget::sizeHint();
    }
};

#endif // ABSTRACTCOLINTOOLTIP_H
