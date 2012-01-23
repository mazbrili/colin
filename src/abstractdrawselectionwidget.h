#ifndef ABSTRACTDRAWSELECTIONWIDGET_H
#define ABSTRACTDRAWSELECTIONWIDGET_H

#include <QtGui/QWidget>
#include <QtCore/QPropertyAnimation>

class QPainter;

class abstractDrawSelectionWidget : public QWidget
{
    Q_OBJECT
	Q_PROPERTY(QSize fixedSize READ size WRITE setFixedSize)

public:
    explicit abstractDrawSelectionWidget(QWidget *parent = 0);

	void drawBackground(QPainter *p);
	void drawRect(QPainter *p, QRect rect, bool down, bool checked, bool mouseOver = false);
	inline int offset()const{return 4;}

	void leaveEvent(QEvent *);
	void enterEvent(QEvent *);
	void requestResize(QSize newSize);
	virtual void showEvent(QShowEvent *){requestResize(sizeHint());}
	virtual void hideEvent(QHideEvent *){setFixedSize(0, 0);}
signals:

public slots:

protected:
	bool mouseOver;
	QPropertyAnimation animation;
	QSize targetSize;


};

#endif // ABSTRACTDRAWSELECTIONWIDGET_H
