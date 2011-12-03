#ifndef PRINTPREVIEW_H
#define PRINTPREVIEW_H

#include <QtGui/QScrollArea>
#include <QtGui/QLabel>
#include "structprinter.h"

class printPreview : public QScrollArea
{
    Q_OBJECT
public:
    explicit printPreview(QWidget *parent = 0);

	void mouseMoveEvent(QMouseEvent *e);
	void enterEvent(QEvent *e);
	void leaveEvent(QEvent *e);
signals:

public slots:
	void update(QPrinter *p, const painterContent &c);
	void updateHighlight(QPrinter *p, const painterContent &c);

private:
	QLabel *pages;
	QLabel *page;

	int previewheight;
	int mouseoverpage;

	QPrinter *printer;
	painterContent content;
};

#endif // PRINTPREVIEW_H
