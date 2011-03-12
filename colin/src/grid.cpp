#include "grid.h"

grid* grid::instance_ = 0;


void grid::loadSettings()
{
    QSettings settings("clazzes.org", "Colin", this);
    maxabs = settings.value("grid/maxabs", 200).toInt();
    pFac = settings.value("grid/loadFactor", 4).toDouble();
    minabs = maxabs/10-1;

}

void grid::saveSettings()
{
    QSettings settings("clazzes.org", "Colin", this);
    settings.setValue("grid/maxabs", maxabs);
    settings.setValue("grid/loadFactor", pFac);
}

QPixmap grid::gridmap(double *zoomf)
{
    double z = *zoomf;
    forever
    {
        if(z>maxabs) z /= 10;
        else if(z<minabs) z *= 10;
        else break;
    }
    QPixmap pic(z, z);
    QPainter p(&pic);

//    p.setRenderHint(QPainter::Antialiasing, false);

    p.fillRect(pic.rect(), backgroundColor());

    p.setPen(QPen(QBrush(QColor(gridColor().red()/3+backgroundColor().red()*2/3,
                                gridColor().green()/3+backgroundColor().green()*2/3,
                                gridColor().blue()/3+backgroundColor().blue()*2/3)),
                                0.1));

    if(pic.size().width()<(maxabs+minabs)/2)
    {
        p.drawLine(pic.width()/2.0, 0, pic.width()/2.0, pic.height());
        p.drawLine(0, pic.height()/2.0, pic.width(), pic.height()/2.0);
    }
    else
    {
        for(int i = 1; i<5; i++)
        {
            p.drawLine(i*pic.width()/4.0, 0, i*pic.width()/4.0, pic.width());
            p.drawLine(0, i*pic.width()/4.0, pic.width(), i*pic.width()/4.0);
        }
    }
    p.setPen(QPen(QBrush(gridColor()),1));
    p.drawLine(0, 0, 0, pic.width());
    p.drawLine(0, 0, pic.width(), 0);

    *zoomf=(static_cast<double>(pic.width()))/z;

    return pic;
}



void grid::drawCordsys(QPainter *p)
{
    QPoint po[3];
    po[0]=QPoint(40, -3);
    po[1]=QPoint(40, 3);
    po[2]=QPoint(48, 0);
    p->setPen(Qt::green);
    p->setBrush(Qt::green);
    p->drawLine(-10, 0, 40, 0);
    p->drawPolygon(po, 3);


    po[0]=QPoint(-3, 40);
    po[1]=QPoint(3, 40);
    po[2]=QPoint(0, 48);
    p->setPen(Qt::red);
    p->setBrush(Qt::red);
    p->drawLine(0, -10, 0, 40);
    p->drawPolygon(po, 3);

    po[0]=QPoint(27, 0);
    po[1]=QPoint(33, 0);
    po[2]=QPoint(30, -8);

    QTransform t;
    t.rotate(-45);

    for(int i = 0; i<3; i++) po[i] = t.map(po[i]);
    p->setPen(Qt::blue);
    p->setBrush(Qt::blue);
    p->drawArc(-30, -30, 60, 60, -45*16, 90*16);
    p->drawPolygon(po, 3);

}


void grid::drawScale(QPainter *p, double scale)
{
    int unitPrefix=0;
    forever
    {
        if(scale>maxabs)
        {
            scale /= 10;
            unitPrefix--;
        }
        else if(scale<minabs)
        {
            scale *= 10;
            unitPrefix++;
        }
        else break;
    }
    p->setPen(QPen(Qt::black, 1));
    p->setRenderHint(QPainter::Antialiasing, false);
    p->setBrush(Qt::NoBrush);
    p->drawRect(10,10, scale, 10);
    for(int i=0; i<5; i++)
    {
        if(i%2 == 0)
            p->fillRect(QRect(10+scale/5.0*i, 10, scale/5.0+1, 10), Qt::black);
    }
    p->drawText(QRect(10, 20, 100, scale/5.0+1),
                QString("10 ^ %1 m").arg(unitPrefix));
}

bool grid::gridCatch(double *length, double z, const int &catchRange_)
{

    int maxabs_ = maxabs;
    int minabs_ = maxabs_/10-1;
    forever
    {
        if(z>maxabs_) z /= 10;
        else if(z<minabs_) z *= 10;
        else break;
    }
    double dl = mod(*length, z);
    if(abs(dl)<(catchRange_))
    {
        *length-=dl;
        return true;
    }
    else if(abs(dl+z)<(catchRange_))
    {
        *length-=(dl+z);
        return true;
    }
    return false;
}

bool grid::gridCatch(QPointF *point, const QTransform &t, const int &catchRange_)
{
    double z = t.m11();
    forever
    {
        if(z>maxabs) z /= 10;
        else if(z<minabs) z *= 10;
        else break;
    }

    double dx = mod(point->x()-t.dx(), z);
    double dy = mod(point->y()-t.dy(), z);

    //dx/dy is the difference between the input-point
    //and the next "fat" line in the grid in pixel!


    //compare the diffrence to the range in witch we chatch...

    if(abs(dx)<(catchRange_))
    {
        if(abs(dy)<(catchRange_))
        {
            //move the input point to the grid...
            point->rx()-=dx;
            point->ry()-=dy;
            return true;
        }
        else if(abs(dy+z)<(catchRange_))
        {
            point->rx()-=dx;
            point->ry()-=(dy+z);
            return true;
        }
        else if(abs(dy-z)<(catchRange_))
        {
            point->rx()-=dx;
            point->ry()-=(dy-z);
            return true;
        }
    }
    else if(abs(dx-z)<(catchRange_))
    {
        if(abs(dy)<(catchRange_))
        {
            point->rx()-=(dx-z);
            point->ry()-=dy;
            return true;
        }
        else if(abs(dy+z)<(catchRange_))
        {
            point->rx()-=(dx-z);
            point->ry()-=(dy+z);
            return true;
        }
        else if(abs(dy-z)<(catchRange_))
        {
            point->rx()-=(dx-z);
            point->ry()-=(dy-z);
            return true;
        }
    }
    else if(abs(dx+z)<(catchRange_))
    {
        if(abs(dy)<(catchRange_))
        {
            point->rx()-=(dx+z);
            point->ry()-=dy;
            return true;
        }
        else if(abs(dy+z)<(catchRange_))
        {
            point->rx()-=(dx+z);
            point->ry()-=(dy+z);
            return true;
        }
        else if(abs(dy-z)<(catchRange_))
        {
            point->rx()-=(dx+z);
            point->ry()-=(dy-z);
            return true;
        }
    }
    return false;
}

double grid::mod(double dend, double visor)
{
    int result = dend/visor;

    return (dend-result*visor);
}

void grid::setAbs(const int &i)
{
    maxabs = i;
}

const int &grid::mAbs() const
{
    return maxabs;
}

