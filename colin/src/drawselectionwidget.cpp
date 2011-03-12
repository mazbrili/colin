#include "drawselectionwidget.h"


const double fontscale = 1.75;

drawSelectionWidget::drawSelectionWidget(int _id, QWidget *parent) :
    QWidget(parent)
{
    id = _id;

    setMouseTracking(true);
    mouseOver = false;
    
}

void drawSelectionWidget::mouseMoveEvent(QMouseEvent *)
{
    repaint();
}

void drawSelectionWidget::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
        mousePressed = true;
    repaint();
}

void drawSelectionWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if(mousePressed)
    {
        if(uR.contains(e->pos()))
        {
            Colin::Elements e = viewPortSettings::instance().elements(id);
            if(e.testFlag(Colin::u))
                e &= ~Colin::u;
            else
                e |= Colin::u;
            viewPortSettings::instance().setElements(id, e);
        }
        else if(nR.contains(e->pos()))
        {
            Colin::Elements e = viewPortSettings::instance().elements(id);
            if(e.testFlag(Colin::N))
                e &= ~Colin::N;
            else
                e |= Colin::N;
            viewPortSettings::instance().setElements(id, e);
        }
        else if(qR.contains(e->pos()))
        {
            Colin::Elements e = viewPortSettings::instance().elements(id);
            if(e.testFlag(Colin::Q))
                e &= ~Colin::Q;
            else
                e |= Colin::Q;
            viewPortSettings::instance().setElements(id, e);
        }
        else if(mR.contains(e->pos()))
        {
            Colin::Elements e = viewPortSettings::instance().elements(id);
            if(e.testFlag(Colin::M))
                e &= ~Colin::M;
            else
                e |= Colin::M;
            viewPortSettings::instance().setElements(id, e);
        }
        else if(bR.contains(e->pos()))
        {
            Colin::Elements e = viewPortSettings::instance().elements(id);
            if(e.testFlag(Colin::bearing))
                e &= ~Colin::bearing;
            else
                e |= Colin::bearing;
            viewPortSettings::instance().setElements(id, e);
        }
        else if(jR.contains(e->pos()))
        {
            Colin::Elements e = viewPortSettings::instance().elements(id);
            if(e.testFlag(Colin::joint))
                e &= ~Colin::joint;
            else
                e |= Colin::joint;
            viewPortSettings::instance().setElements(id, e);
        }
        else if(kR.contains(e->pos()))
        {
            Colin::Elements e = viewPortSettings::instance().elements(id);
            if(e.testFlag(Colin::node))
                e &= ~Colin::node;
            else
                e |= Colin::node;
            viewPortSettings::instance().setElements(id, e);
        }
        else if(lR.contains(e->pos()))
        {
            Colin::Elements e = viewPortSettings::instance().elements(id);
            if(e.testFlag(Colin::nload))
                e &= ~Colin::nload;
            else
                e |= Colin::nload;
            viewPortSettings::instance().setElements(id, e);
        }
    }
    mousePressed = false;
    repaint();
}

void drawSelectionWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    QFont f = font();
    f.setPointSize(f.pointSize()*fontscale);
    p.setFont(f);
    QPoint p0(4, 4);
    QSize boundingSize(p.font().pointSize()*1.5, p.font().pointSize()*2);


    QLinearGradient backG(0, 0, 0, height());
    backG.setColorAt(0, viewPortSettings::instance().color(Colin::C_Background));
    backG.setColorAt(1, viewPortSettings::instance().color(Colin::C_Background).darker(115));

    QLinearGradient frontG(0, 4, 0, boundingSize.height());

    if(mouseOver)
    {
	p.setPen(viewPortSettings::instance().color(Colin::C_Grid).darker(140));
	p.setBrush(QBrush(backG));
        p.drawRect(rect().adjusted(0, 0, -1, -1));
	p.setBrush(Qt::NoBrush);
	backG.setColorAt(0, QColor(0, 0, 0, 0));
	backG.setColorAt(1, QColor(0, 0, 0, 0));
    }
    else
    {
	backG.setStart(0, 4);
	backG.setFinalStop(0, boundingSize.height()+4);
    }
    if(viewPortSettings::instance().elements(id).testFlag(Colin::u) || mouseOver)
    {
        if(viewPortSettings::instance().elements(id).testFlag(Colin::u))
        {
            frontG.setColorAt(0, viewPortSettings::instance().color(Colin::C_Beam).lighter(150));
            frontG.setColorAt(1, viewPortSettings::instance().color(Colin::C_Beam));
            //p.setPen(viewPortSettings::instance().color(Colin::C_Np));
            p.setPen(QPen(frontG, 1));
        }
        else
            p.setPen(viewPortSettings::instance().color(Colin::C_BeamRef));

        uR = QRect(p0, boundingSize);
	p.setBrush(QBrush(backG));
        p.drawRect(uR);
        if(uR.contains(mapFromGlobal(QCursor::pos())))
        {
            if(mousePressed)
            {
                frontG.setColorAt(0, viewPortSettings::instance().color(Colin::C_Background).darker(115));
                frontG.setColorAt(1, viewPortSettings::instance().color(Colin::C_Background));
                p.setBrush(frontG);
            }
            else
            {
                p.setBrush(Qt::NoBrush);
            }
            p.drawRect(uR.adjusted(-1, -1, 1, 1));
            p.setBrush(Qt::NoBrush);
        }
        p.drawText(uR, tr("u"), QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));

        p0+=QPoint(boundingSize.width()+4, 0);
    }
    if(viewPortSettings::instance().elements(id).testFlag(Colin::N) || mouseOver)
    {
        if(viewPortSettings::instance().elements(id).testFlag(Colin::N))
        {
            frontG.setColorAt(0, viewPortSettings::instance().color(Colin::C_Np));
            frontG.setColorAt(1, viewPortSettings::instance().color(Colin::C_Np).darker(115));
            //p.setPen(viewPortSettings::instance().color(Colin::C_Np));
            p.setPen(QPen(frontG, 1));
        }
        else
            p.setPen(viewPortSettings::instance().color(Colin::C_BeamRef));

	nR = QRect(p0, boundingSize);
	p.setBrush(QBrush(backG));
        p.drawRect(nR);
        if(nR.contains(mapFromGlobal(QCursor::pos())))
        {
            if(mousePressed)
            {
                frontG.setColorAt(0, viewPortSettings::instance().color(Colin::C_Background).darker(115));
                frontG.setColorAt(1, viewPortSettings::instance().color(Colin::C_Background));
                p.setBrush(frontG);
            }
            else
            {
                p.setBrush(Qt::NoBrush);
            }
            p.drawRect(nR.adjusted(-1, -1, 1, 1));
        }
        p.setBrush(Qt::NoBrush);
        p.drawText(nR, tr("N"), QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));
        p0+=QPoint(boundingSize.width()+4, 0);
    }
    if(viewPortSettings::instance().elements(id).testFlag(Colin::Q) || mouseOver)
    {
        if(viewPortSettings::instance().elements(id).testFlag(Colin::Q))
        {
            frontG.setColorAt(0, viewPortSettings::instance().color(Colin::C_Qp));
            frontG.setColorAt(1, viewPortSettings::instance().color(Colin::C_Qp).darker(115));
            //p.setPen(viewPortSettings::instance().color(Colin::C_Qp));
            p.setPen(QPen(frontG, 1));
        }
        else
            p.setPen(viewPortSettings::instance().color(Colin::C_BeamRef));

	qR = QRect(p0, boundingSize);
	p.setBrush(QBrush(backG));
        p.drawRect(qR);
        if(qR.contains(mapFromGlobal(QCursor::pos())))
        {
            if(mousePressed)
            {
                frontG.setColorAt(0, viewPortSettings::instance().color(Colin::C_Background).darker(115));
                frontG.setColorAt(1, viewPortSettings::instance().color(Colin::C_Background));
                p.setBrush(frontG);
            }
            else
            {
                p.setBrush(Qt::NoBrush);
            }
            p.drawRect(qR.adjusted(-1, -1, 1, 1));
        }
        p.setBrush(Qt::NoBrush);
        p.drawText(qR, tr("Q"), QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));
        p0+=QPoint(boundingSize.width()+4, 0);
    }
    if(viewPortSettings::instance().elements(id).testFlag(Colin::M) || mouseOver)
    {
        if(viewPortSettings::instance().elements(id).testFlag(Colin::M))
        {
            frontG.setColorAt(0, viewPortSettings::instance().color(Colin::C_Mp));
            frontG.setColorAt(1, viewPortSettings::instance().color(Colin::C_Mp).darker(115));
            //p.setPen(viewPortSettings::instance().color(Colin::C_Mp));
            p.setPen(QPen(frontG, 1));
        }
        else
            p.setPen(viewPortSettings::instance().color(Colin::C_BeamRef));

	mR = QRect(p0, boundingSize);
	p.setBrush(QBrush(backG));
        p.drawRect(mR);
        if(mR.contains(mapFromGlobal(QCursor::pos())))
        {
            if(mousePressed)
            {
                frontG.setColorAt(0, viewPortSettings::instance().color(Colin::C_Background).darker(115));
                frontG.setColorAt(1, viewPortSettings::instance().color(Colin::C_Background));
                p.setBrush(frontG);
            }
            else
            {
                p.setBrush(Qt::NoBrush);
            }
            p.drawRect(mR.adjusted(-1, -1, 1, 1));
        }
        p.setBrush(Qt::NoBrush);
        p.drawText(mR, tr("M"), QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));
        p0+=QPoint(boundingSize.width()+4, 0);
    }

    if(p0.x() < 5) // nothing selected ... draw something so it still can pop up to select
    {
	p.setPen(Qt::black);
	p.setBrush(QBrush(backG));
	p.drawRect(QRect(p0, boundingSize));
	p.setBrush(Qt::NoBrush);
	colinIcons::instance().icon(Colin::Close).paint(&p, mR, Qt::AlignCenter);
    }



    p0 = QPoint(4, 8 + boundingSize.height());


    if(!mouseOver)
        return;



    frontG.setStart(0, 8+boundingSize.height());
    frontG.setFinalStop(0, 8+2*boundingSize.height());


    frontG.setColorAt(0, viewPortSettings::instance().color(Colin::C_Background).darker(115));
    frontG.setColorAt(1, viewPortSettings::instance().color(Colin::C_Background));

    //node visible
     if(viewPortSettings::instance().elements(id).testFlag(Colin::node))
         p.setPen(viewPortSettings::instance().color(Colin::C_Selection));
     else
         p.setPen(viewPortSettings::instance().color(Colin::C_BeamRef));

     kR = QRect(p0, boundingSize);
     p.drawRect(kR);
     if(kR.contains(mapFromGlobal(QCursor::pos())))
     {
         if(mousePressed)
         {
             p.setBrush(frontG);
         }
         else
         {
             p.setBrush(Qt::NoBrush);
         }
         p.drawRect(kR.adjusted(-1, -1, 1, 1));
         p.setBrush(Qt::NoBrush);
     }
     colinIcons::instance().icon(Colin::drawNode).paint(&p, kR, Qt::AlignCenter);
     p0+=QPoint(boundingSize.width()+4, 0);

     //loads visible

     if(viewPortSettings::instance().elements(id).testFlag(Colin::nload))
         p.setPen(viewPortSettings::instance().color(Colin::C_Selection));
     else
         p.setPen(viewPortSettings::instance().color(Colin::C_BeamRef));

     lR = QRect(p0, boundingSize);
     p.drawRect(lR);
     if(lR.contains(mapFromGlobal(QCursor::pos())))
     {
         if(mousePressed)
         {
             p.setBrush(frontG);
         }
         else
         {
             p.setBrush(Qt::NoBrush);
         }
         p.drawRect(lR.adjusted(-1, -1, 1, 1));
         p.setBrush(Qt::NoBrush);
     }
     colinIcons::instance().icon(Colin::drawNLoad).paint(&p, lR, Qt::AlignCenter);
     p0+=QPoint(boundingSize.width()+4, 0);


     //bearings visible

     if(viewPortSettings::instance().elements(id).testFlag(Colin::bearing))
         p.setPen(viewPortSettings::instance().color(Colin::C_Selection));
     else
         p.setPen(viewPortSettings::instance().color(Colin::C_BeamRef));

     bR = QRect(p0, boundingSize);
     p.drawRect(bR);
     if(bR.contains(mapFromGlobal(QCursor::pos())))
     {
         if(mousePressed)
         {
             p.setBrush(frontG);
         }
         else
         {
             p.setBrush(Qt::NoBrush);
         }
         p.drawRect(bR.adjusted(-1, -1, 1, 1));
         p.setBrush(Qt::NoBrush);
     }
     colinIcons::instance().icon(Colin::drawBearing).paint(&p, bR, Qt::AlignCenter);
     p0+=QPoint(boundingSize.width()+4, 0);


     //joints visible

     if(viewPortSettings::instance().elements(id).testFlag(Colin::joint))
         p.setPen(viewPortSettings::instance().color(Colin::C_Selection));
     else
         p.setPen(viewPortSettings::instance().color(Colin::C_BeamRef));

     jR = QRect(p0, boundingSize);
     p.drawRect(jR);
     if(jR.contains(mapFromGlobal(QCursor::pos())))
     {
         if(mousePressed)
         {
             p.setBrush(frontG);
         }
         else
         {
             p.setBrush(Qt::NoBrush);
         }
         p.drawRect(jR.adjusted(-1, -1, 1, 1));
         p.setBrush(Qt::NoBrush);
     }
     colinIcons::instance().icon(Colin::drawJoint).paint(&p, jR, Qt::AlignCenter);
     p0+=QPoint(boundingSize.width()+4, 0);



    /*
    u =         0x1,
    N =         0x2,
    Q =         0x4,
    M =         0x8,

    node =      0x10,
    beam =      0x20,
    bearing =   0x40,
    joint =     0x80,
    nload =     0x100,
    sload =     0x200,
    temp =      0x400,
    deform =    0x800,
    */
}

void drawSelectionWidget::enterEvent(QEvent *)
{
    mouseOver = true;
    setFixedSize(sizeHint());
    repaint();
}

void drawSelectionWidget::leaveEvent(QEvent *)
{
    mouseOver = false;
    setFixedSize(sizeHint());
    repaint();
}


QSize drawSelectionWidget::sizeHint() const
{
    QSize boundingSize(this->font().pointSize()*1.5*fontscale, this->font().pointSize()*2*fontscale);

    if(mouseOver)
    {
	return QSize(boundingSize.width()*4+4*5+1,
		     boundingSize.height()*2+3*5+1);
    }
    else
    {
	int rectcount = 0;
	if(viewPortSettings::instance().elements(id).testFlag(Colin::u))
	    rectcount++;
	if(viewPortSettings::instance().elements(id).testFlag(Colin::N))
	    rectcount++;
	if(viewPortSettings::instance().elements(id).testFlag(Colin::Q))
	    rectcount++;
	if(viewPortSettings::instance().elements(id).testFlag(Colin::M))
	    rectcount++;
	if(!rectcount)
	    rectcount++;
	return QSize(boundingSize.width()*rectcount+4*(rectcount+1),
		     boundingSize.height()+8);

    }
}
