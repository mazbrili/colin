#include "printpreview.h"


#include <QtGui/QMouseEvent>
#include <QtGui/QGraphicsDropShadowEffect>


const int pageSpacing = 5;
printPreview::printPreview(QWidget *parent) :
    QScrollArea(parent)
{

	mouseoverpage = -1;
	pages = new QLabel(this);
	page = new QLabel(this->parentWidget()->parentWidget()->parentWidget()->parentWidget()->parentWidget()); //uff
	page->setWindowFlags(Qt::FramelessWindowHint);
	this->setWidget(pages);
	pages->setMinimumWidth(50);
	pages->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	pages->setScaledContents(false);
	pages->setAttribute(Qt::WA_TransparentForMouseEvents);
	pages->setFocusPolicy(Qt::NoFocus);
	page->setAttribute(Qt::WA_TransparentForMouseEvents);
	page->setFocusPolicy(Qt::NoFocus);
	QGraphicsDropShadowEffect *eff = new QGraphicsDropShadowEffect(page);
	eff->setXOffset(5);
	eff->setYOffset(5);
	eff->setBlurRadius(20);
	page->setGraphicsEffect(eff);

	setMinimumWidth(60);
	page->hide();


	setMouseTracking(true);
	setFrameStyle(0);



}

void printPreview::mouseMoveEvent(QMouseEvent *e)
{
	QPoint po = pages->mapFromGlobal(QCursor::pos());
	mouseoverpage = po.y()/(previewheight+pageSpacing);
	updateHighlight(printer, content);
}

void printPreview::enterEvent(QEvent *e)
{

	QPoint po = pages->mapFromGlobal(QCursor::pos());
	mouseoverpage = po.y()/(previewheight+pageSpacing);
	page->show();
	updateHighlight(printer, content);
}

void printPreview::leaveEvent(QEvent *e)
{
	page->hide();
}

void printPreview::update(QPrinter *p, const painterContent &c)
{
	if(filelist::instance().currentFile()==0)
		return;
	structPrinter sP(filelist::instance().currentFile(), p, c);

	int pageCount = sP.requiredPages(p, c);

	int pagewidth = pages->width()-4;
	int pageheight;

	if(c.s.testFlag(painterContent::landscape))
		pageheight = pagewidth / M_SQRT2;
	else
		pageheight = pagewidth * M_SQRT2;
	QPixmap device(QSize(pages->width(), pageCount*pageheight+(pageCount-1)*pageSpacing+4));
	device.fill(QColor(palette().color(QPalette::Background)));
	QPainter painter(&device);
	painter.setBrush(Qt::white);
	for(int i=0; i<pageCount; i++)
	{
		QRect rect(QPoint(2, i*(pageheight+pageSpacing)+2), QSize(pages->width()-4, pageheight));
		painter.drawRect(rect);
		painter.drawText(rect, QString::number(i), Qt::AlignVCenter | Qt::AlignHCenter);
	}
	pages->setPixmap(device);
	pages->setFixedSize(device.size());
	previewheight = pageheight;

	this->printer = p;
	this->content = c;
}

void printPreview::updateHighlight(QPrinter *p, const painterContent &c)
{
	QSize s;
	QPoint ViewGeo=page->parentWidget()->mapFromGlobal(parentWidget()->parentWidget()->parentWidget()->mapToGlobal(parentWidget()->parentWidget()->parentWidget()->rect().topRight()));
	s.setHeight(parentWidget()->parentWidget()->parentWidget()->height());
	s.setWidth(s.height()*p->pageRect().width()/p->pageRect().height());
	QPixmap device(s);

	device.fill(Qt::white);


	structPrinter sP(filelist::instance().currentFile(),
					 p, c);


	sP.print(&device, mouseoverpage);

	page->setPixmap(device);
	page->setFixedSize(device.size());
	page->setGeometry(QRect(ViewGeo, device.size()));

}
