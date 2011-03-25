#include "resultvisualwidget.h"
#include <QtCore/QTimer>
#include <QtGui/QMoveEvent>
#include <QtGui/QResizeEvent>
#include <QtGui/QPainter>
#include <QtGui/QGraphicsBlurEffect>
#include <QtGui/QStylePainter>
#include <QtGui/QStyleOptionFrame>
#include <QtGui/QToolTip>

#include "colinicons.h"
#include "shortcutsettings.h"
#include "viewportsettings.h"
#include "structpainter.h"
#include "unitsettings.h"

resultVisualWidget *resultVisualWidget::instance_ = NULL;

int blurRadius = 15;


class blurOpa : public QGraphicsBlurEffect
{
public:
    blurOpa(QObject *parent = 0):QGraphicsBlurEffect(parent){}
protected:
    void draw(QPainter *p)
    {
        p->setOpacity(0.5);
        QGraphicsBlurEffect::draw(p);
    }
};

double validAngle(double angle)
{
    if(angle>2*M_PI)
    {
        angle-=2*M_PI;
    }
    else if(angle<0)
    {
        angle+=2*M_PI;
    }
    return angle;
}

resultVisualWidget::resultVisualWidget(QWidget *parent)
    : QLabel(parent)
{
    if(instance_)
        delete instance_;
    instance_ = this;
    objectType = catcher::CatchedNothing;
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setForegroundRole(QPalette::ToolTipText);
    setBackgroundRole(QPalette::ToolTipBase);
    setPalette(QToolTip::palette());
    ensurePolished();
    setFrameStyle(QFrame::NoFrame);
    setWindowOpacity(style()->styleHint(QStyle::SH_ToolTipLabel_Opacity, 0, this)/255.);
    setFixedSize(bSize());


    shortcutSettings &sCs = shortcutSettings::instance();
    mode = new QPushButton(colinIcons::instance().icon(Colin::Settings), "", parent);
    mode->setFixedSize(30, 20);
    QMenu *menu = new QMenu(mode);
    QAction *action;
    aGroup = new QActionGroup(this);
    action = menu->addAction(tr("show details"));
    action->setData(Details);
    aGroup->addAction(action);
    action = menu->addAction(tr("show results"));
    action->setData(Results);
    aGroup->addAction(action);
    action = menu->addAction(tr("show minimap"));
    action->setData(Minimap);
    aGroup->addAction(action);
    action = menu->addAction(tr("hide"));
    action->setData(Hide);
    aGroup->addAction(action);

    foreach(action, aGroup->actions())
    {
        sCs.addAction(action, true);
    }
    sH = Results;
    mode->setMenu(menu);




    QGraphicsBlurEffect *effect = new blurOpa(this);
    effect->setBlurRadius(blurRadius/4);
    setGraphicsEffect(effect);



    QTimer *timer = new QTimer(this);
    timer->setInterval(100);
    timer->start();

    mode->hide();
    hide();


    connect(timer,                  SIGNAL(timeout()),
            this,                   SLOT(checkForMouseOver()));

    connect(aGroup,                 SIGNAL(triggered(QAction*)),
            this,                   SLOT(setMode(QAction*)));

    connect(&filelist::instance(),  SIGNAL(currentChanged(wgv_tw*)),
            this,                   SLOT(fileChanged(wgv_tw*)));


}

void resultVisualWidget::checkForMouseOver()
{
    bool actualMouseIsOver;
    if(rect().contains(mapFromGlobal(QCursor::pos())))
        actualMouseIsOver = true;
    else
        actualMouseIsOver = false;
    if(actualMouseIsOver != mouseIsOver)
    {
        mouseIsOver = actualMouseIsOver;
        if(mouseIsOver)
        {
            graphicsEffect()->setEnabled(true);
        }
        else
        {
            graphicsEffect()->setEnabled(false);
        }
        repaint();
    }
}

void resultVisualWidget::paintEvent(QPaintEvent *e)
{
    //QPainter p(this);
    //p.setRenderHint(QPainter::Antialiasing, true);
    //p.setPen(Qt::NoPen);
    //p.setBrush(QBrush(QPixmap("/home/matti/Pictures/03.jpg")));
    //p.drawRoundedRect(blurRadius, blurRadius, width()-2*blurRadius, height()-2*blurRadius, 2, 2);
    QStylePainter p(this);
    if(!mouseIsOver)
    {
        QPainterPath clip;
        clip.addRoundedRect(rect(), 2, 2);
        p.setClipPath(clip);
    }
    QStyleOptionFrame opt;
    opt.init(this);
    p.drawPrimitive(QStyle::PE_PanelTipLabel, opt);
    p.end();

    QLabel::paintEvent(e);
    QPainter painter(this);

    switch(sH)
    {
    case Results:
        paintResults(&painter);
        return;
    case Details:
        paintDetails(&painter);
        return;
    case Minimap:
        if(filelist::instance().currentFile())
            paintMinimap(&painter);
        return;
    default:
        return;
    }


}

bool resultVisualWidget::eventFilter(QObject *obj, QEvent *event){

    if(obj == parent())
    {
        switch(event->type())
        {
        case QEvent::Move:
        case QEvent::Resize:
            actualGeometry();
            break;
        default:
            break;
        }
    }
    return QObject::eventFilter(obj, event);
}

void resultVisualWidget::actualGeometry()
{
    QWidget *mW = static_cast<QWidget*>(parent());
    move(QPoint(mW->width(), mW->height())-QPoint(bSize().width()+2, bSize().height()+32)/*+QPoint(blurRadius, blurRadius)*/);
    setFixedSize(bSize());
    if(sH == Hide)
        mode->setGeometry(QRect(mapToParent(QPoint(bSize().width()-mode->size().width()/*-blurRadius*/, bSize().height()/*-blurRadius*/-mode->height())), mode->size()));
    else
        mode->setGeometry(QRect(mapToParent(QPoint(bSize().width()-mode->size().width()/*-blurRadius*/, 0/*-blurRadius*/)), mode->size()));

}

void resultVisualWidget::setMode(QAction *a)
{
    setMode(static_cast<showHint>(a->data().toInt()));
}

void resultVisualWidget::setMode(const showHint &sH)
{
    this->sH = sH;
    if(sH == Hide)
        hide();
    else
        show();
    actualGeometry();
}

void resultVisualWidget::paintDetails(QPainter *p)
{
    const QRect iconRect(10, 10, 64, 64);
    const QRect titleRect(10+64+10, 10, width()-10-10-64-10, 64);
    const QRect textRect(10, 10+64+10, width()-10-10, height()-10-64-10-10);
    QFont titleFont = p->font();
    QFont textFont = p->font();
    titleFont.setPointSize(qMax(22., p->font().pointSize()*1.5));
    if(objectType == catcher::CatchedNothing)
        return;
    if(objectType == catcher::CatchedNode)
    {
        const wgv_node &n = filelist::instance().currentFile()->node(object);

        QString text = QString("x = %1 m\n").arg(n.x())+
                       QString("z = %1 m\n").arg(n.z());
        if(n.hasbearing())
        {
            text.append(QChar(0x03B1));
            text.append(QString(" = %1\n").arg(n.angle()*ANGLEPREFIX));
            text.append(QString("H = "));
            if(n.bearing().form().testFlag(wgv_bearing::fx))
                text.append(QString(tr("spring")+", c = %1 ").arg(n.bearing().c_x()) + FPREFIX + "\n");
            else if(n.bearing().form().testFlag(wgv_bearing::X))
                text.append(tr("locked\n"));
            else
                text.append(tr("free"));
            text.append(QString("V = "));
            if(n.bearing().form().testFlag(wgv_bearing::fz))
                text.append(QString(tr("spring")+", c = %1 ").arg(n.bearing().c_z()) + FPREFIX + "\n");
            else if(n.bearing().form().testFlag(wgv_bearing::Z))
                text.append(tr("locked\n"));
            else
                text.append(tr("free"));
            text.append(QString("Phi = "));
            if(n.bearing().form().testFlag(wgv_bearing::fphi))
                text.append(QString(tr("spring")+", c = %1 ").arg(n.bearing().c_phi()) + FPREFIX + "\n");
            else if(n.bearing().form().testFlag(wgv_bearing::Phi))
                text.append(tr("locked\n"));
            else
                text.append(tr("free"));
        }

        p->drawPixmap(iconRect, QPixmap(colinIcons::icondir_ +"tooltip/node_64.png"));
        p->setFont(titleFont);
        p->drawText(titleRect, tr("node")+ " #" + QString::number(object), Qt::AlignVCenter | Qt::AlignLeft);
        p->setFont(textFont);
        p->drawText(textRect, text);
    }
    if(objectType == catcher::CatchedBeam)
    {
        const wgv_beam &b = filelist::instance().currentFile()->beam(object);
        QString text;

        text.append(QString(tr("left Node")+": #%1 (%2 m, %3 m)\n").arg(b.leftNodeI()).arg(b.leftNode().x()).arg(b.leftNode().z()));
        text.append(QString(tr("right Node")+": #%1 (%2 m, %3 m)\n").arg(b.rightNodeI()).arg(b.rightNode().x()).arg(b.rightNode().z()));
        text.append(QString(tr("lenght")+" = %1 m\n").arg(b.l()));
        text.append(QString("%1 = %2\n").arg(QChar(0x03B1)).arg(b.angle()*ANGLEPREFIX));
        text.append(QString(tr("material")+" = %1\n").arg(b.Mat().name()));
        text.append(QString(tr("cross section")+" = %1\n").arg(b.Profile().name()));
        p->drawPixmap(iconRect, QPixmap(colinIcons::icondir_ +"tooltip/beam_64.png"));
        p->setFont(titleFont);
        p->drawText(titleRect, tr("beam")+ " #" + QString::number(object), Qt::AlignVCenter | Qt::AlignLeft);
        p->setFont(textFont);
        p->drawText(textRect, text);

    }
    if(objectType == catcher::CatchedLoad || objectType == catcher::CatchedTemp)
    {
        const wgv_load &l = filelist::instance().currentFile()->load(object);
        QString text;

        switch(l.typ())
        {
        case wgv_load::uniformlyDistibutedLoad:
            text.append(tr("uniform load")+"\n");
            text.append(tr("on beam")+" #");
            break;
        case wgv_load::increasingLinearLoad:
            text.append(tr("increasing, linear load")+"\n");
            text.append(tr("on beam")+" #");
            break;
        case wgv_load::decreasingLinearLoad:
            text.append(tr("decreasing, linear load")+"\n");
            text.append(tr("on beam")+" #");
            break;
        case wgv_load::doubleLoadLeft:
        case wgv_load::doubleLoadRight:
            text.append(tr("double load")+"\n");
            text.append(tr("on beam")+" #");
            break;
        case wgv_load::moment:
            text.append(tr("moment")+"\n");
            text.append(tr("on node")+" #");
            break;
        case wgv_load::nodeLoad:
            text.append(tr("load on node")+"\n");
            text.append(tr("on node")+" #");
            break;
        case wgv_load::tempChange:
            text.append(tr("temperature change")+"\n");
            text.append(tr("on beam")+" #");
            break;
        case wgv_load::tempDiffrence:
            text.append(tr("temperature diffrence")+"\n");
            text.append(tr("on beam")+" #");
            break;
        default:
            break;
        }
        text.append(QString::number(l.at())+"\n");
        if(l.typ()==wgv_load::moment)
        {
            text.append(QString("M = %1").arg(l.M()*MPREFIX, 0, 'f', PRECISON)+unitSettings::instance().Meh()+"\n");
        }
        else if(l.typ() == wgv_load::tempChange)
        {
            text.append(QString("T = %1 K\n").arg(l.Px()));
        }
        else if(l.typ() == wgv_load::tempDiffrence)
        {
            text.append(QString("T = %1 K\n").arg(l.Pz()));
        }
        else
        {
            QString eh;
            if(l.typ() == wgv_load::nodeLoad)
                eh = unitSettings::instance().Peh();
            else
                eh = unitSettings::instance().Peh()+"/m";
            text.append(QString("Px = %1").arg(l.Px()*PPREFIX, 0, 'f', PRECISON)+ eh + "\n");
            text.append(QString("Pz = %1").arg(l.Pz()*PPREFIX, 0, 'f', PRECISON)+ eh + "\n");
        }
        p->drawPixmap(iconRect, QPixmap(colinIcons::icondir_ +"tooltip/load_64.png"));
        p->setFont(titleFont);
        p->drawText(titleRect, tr("load")+ " #" + QString::number(object), Qt::AlignVCenter | Qt::AlignLeft);
        p->setFont(textFont);
        p->drawText(textRect, text);
    }
}

void resultVisualWidget::paintMinimap(QPainter *p)
{


    p->setRenderHint(QPainter::Antialiasing, true);
    p->setPen(palette().color(QPalette::ToolTipText));
    wgv_tw *tw = filelist::instance().currentFile();

    QTransform t(1.0,  0.0,  0.0,
                 0.0,  1.0,  0.0,
                 0.0,  0.0,  1.0);
    QRectF boundingRect = tw->boundingRect();
    double scale = qMin((size().width()-30)*2/boundingRect.width(), (size().height()-30)*2/boundingRect.height());
    t.translate(-boundingRect.x()*scale+size().width()-boundingRect.width()*scale/2,
                -boundingRect.y()*scale+size().height()-boundingRect.height()*scale/2);
    t.scale(scale, scale);

    p->scale(0.5, 0.5);

    for(int i=0; i<tw->beam_n(); i++)
    {
        p->drawLine(t.map(tw->beam(i).toQLineF()));
    }
    for(int i=0; i<tw->node_n(); i++)
    {
        p->drawEllipse(t.map(tw->node(i).toQPointF()), 3, 3);
    }
    for(int i=0; i<tw->node_n(); i++)
    {
        if(tw->node(i).hasbearing())
        {
            switch(tw->node(i).bearing().simpleform())
            {
            case wgv_bearing::X:
                structPainter::drawHBear(p, t.map(tw->node(i).toQPointF()));
                break;
            case wgv_bearing::Z:
                structPainter::drawVBear(p, t.map(tw->node(i).toQPointF()));
                break;
            case wgv_bearing::XZ:
                structPainter::drawHVBear(p, t.map(tw->node(i).toQPointF()));
                break;
            case wgv_bearing::XPhi:
                structPainter::drawHMBear(p, t.map(tw->node(i).toQPointF()));
                break;
            case wgv_bearing::ZPhi:
                structPainter::drawVMBear(p, t.map(tw->node(i).toQPointF()));
                break;
            case wgv_bearing::Phi:
                structPainter::drawMBear(p, t.map(tw->node(i).toQPointF()));
                break;
            case wgv_bearing::XZPhi:
                structPainter::drawHVMBear(p, t.map(tw->node(i).toQPointF()));
                break;
            }
        }
    }
    QColor lBG =p->pen().color();
    lBG.setAlpha(200);
    p->setPen(lBG);
    for(int i=0; i<tw->load_n(); i++)
    {
        if(tw->load(i).typ() == wgv_load::uniformlyDistibutedLoad ||
           tw->load(i).typ() == wgv_load::decreasingLinearLoad    ||
           tw->load(i).typ() == wgv_load::increasingLinearLoad    )
        {
            p->drawPolygon(t.map(tw->load(i).shape()));

            QTransform loadT;
            loadT.rotate(atan2(tw->load(i).Pz(), tw->load(i).Px())*180.0/M_PI-90);

            QPolygonF arrow(3);

            arrow[0] = QPointF(0, 0);
            arrow[1] = QPointF(-5, -10);
            arrow[2] = QPointF(5, -10);

            arrow = loadT.map(arrow);
            QLineF beamL(t.map(tw->load(i).shape()[0]), t.map(tw->load(i).shape().last()));
            const int ac = 5;

            p->save();
            p->setClipRegion(QRegion(t.map(tw->load(i).shape()).toPolygon()));
            p->setBrush(lBG);
            for(int n=0; n<ac+1; n++)
            {
                p->save();
                p->translate((beamL.p1()*(ac-n)+beamL.p2()*n)/ac);
                p->drawLine(loadT.map(QLineF(0, 0, 0, -300)));
                p->drawPolygon(arrow);
                p->restore();
            }
            p->restore();
        }
        if(tw->load(i).typ() == wgv_load::nodeLoad)
        {
            p->save();
            p->setBrush(p->pen().color());
            QTransform loadT;
            QPointF translation = t.map(tw->node(tw->load(i).at()).toQPointF());
            loadT.translate(translation.x(), translation.y());
            loadT.rotate(atan2(tw->load(i).Pz(), tw->load(i).Px())*180.0/M_PI-90);

            QPolygonF arrow(3);

            arrow[0] = QPointF(0, 0);
            arrow[1] = QPointF(-5, -10);
            arrow[2] = QPointF(5, -10);

            arrow = loadT.map(arrow);

            p->drawPolygon(arrow);
            p->drawLine(t.map(QLineF(tw->node(tw->load(i).at()).toQPointF(),
                                     tw->node(tw->load(i).at()).toQPointF()-tw->scaleP()*QPointF(tw->load(i).Px(), tw->load(i).Pz()))));
            p->restore();
        }
    }
}

void resultVisualWidget::paintResults(QPainter *p)
{
    switch (objectType)
    {
    case catcher::CatchedNode:
        paintNodeResults(p);
        return;
    case catcher::CatchedBeam:
        paintBeamResults(p);
        return;
    default:
        return;
    }
}

void resultVisualWidget::paintNodeResults(QPainter *p)
{
    p->setPen(palette().color(QPalette::ToolTipText));
    wgv_tw *tw = filelist::instance().currentFile();
    QMap<int, double>beams;
    QList<double>beamangles;
    for(int i=0; i<tw->beam_n(); i++)
    {
        if(tw->beam(i).leftNodeI() == object)
        {
            double angle = tw->beam(i).angle();
            beams.insert(i, validAngle(angle));
            beamangles.append(validAngle(angle));
        }
        else if(tw->beam(i).rightNodeI() == object)
        {
            double angle = tw->beam(i).angle()+M_PI;
            if(angle > 2*M_PI)
                angle-=2*M_PI;
            beams.insert(i, validAngle(angle));
            beamangles.append(validAngle(angle));
        }
    }
    qSort(beamangles);
    QList<double> clipangles;
    if(!beamangles.isEmpty())
    {
        clipangles.append((beamangles.last()+beamangles.first()-2*M_PI)/2);
        for(int i=1; i<beamangles.count(); i++)
        {
            clipangles.append((beamangles.at(i-1)+beamangles.at(i))/2);
        }

    }


    //drawing

    const int iRad = 8; //the "inner" radius of the node
    const double aRad = 10; //the "outer" radius of the node
    const double length = 25;
    const double pc = 2;
    const double radc = 8;
    const double d = 5;
    const double dys = d/2 + radc;
    const double dxs = sqrt(pow(aRad+radc, 2) - pow(dys, 2));
    const double sa = atan2(dys, dxs);
    const double NStart = 40;
    const double QStart = 50;
    const double MStart = 60;
    const double MExStart = 25;
    const double RxStart = 15;
    const double RzStart = 15;
    const double RpStart = 20;

    p->setRenderHint(QPainter::Antialiasing, true);
    p->translate(height()/2, height()/2);

    QPainterPath beam;


    beam.moveTo(QPointF(dxs, d/2));
    beam.arcTo(QRectF(dxs-radc, dys-radc, 2*radc, 2*radc), 90, 90-sa*180/M_PI);
    beam.arcTo(QRectF(-aRad, -aRad, 2*aRad, 2*aRad), -sa*180/M_PI, -360+2*sa*180/M_PI);
    beam.arcTo(QRectF(dxs-radc, -dys+radc-2*radc, 2*radc, 2*radc), -180+sa*180/M_PI, 90-sa*180/M_PI);
    beam.lineTo(QPointF(aRad+length, -d/2));
    beam.cubicTo(QPointF(aRad+length+pc, -d/4), QPointF(aRad+length-pc, d/6),
                 QPointF(aRad+length, d/2));
    beam.closeSubpath();

    for(int i=0; i<clipangles.count(); i++)
    {
        if(clipangles.count()>1)
        {
            QPainterPath clipPath;
            clipPath.moveTo(0, 0);
            clipPath.lineTo(100*cos(clipangles.at(i)), 100*sin(clipangles.at(i)));
            clipPath.lineTo(100*cos(beamangles.at(i)), 100*sin((beamangles.at(i))));
            if(i==beamangles.count()-1)
                clipPath.lineTo(100*cos(clipangles.at(0)), 100*sin((clipangles.at(0))));
//              clipPath.arcTo(QRectF(-200, -200, 400, 400), clipangles.last()*180/M_PI, validAngle(clipangles.last()-clipangles.first()-2*M_PI)*180/M_PI);
            else
                clipPath.lineTo(100*cos(clipangles.at(i+1)), 100*sin((clipangles.at(i+1))));
//              clipPath.arcTo(QRectF(-200, -200, 400, 400), clipangles.at(i)*180/M_PI, validAngle(clipangles.at(i+1)-clipangles.at(i))*180/M_PI);
            clipPath.closeSubpath();
            p->setClipPath(clipPath);
        }
/*
        if(i==0)
            p->fillRect(QRect(-100, -100, 200, 200), QColor(255, 0, 0, 100));
        if(i==1)
            p->fillRect(QRect(-100, -100, 200, 200), QColor(0, 0, 255, 100));
        if(i==2)
            p->fillRect(QRect(-100, -100, 200, 200), QColor(0, 255, 0, 100));
*/

        QTransform t;
        t.rotate(beamangles.at(i)*180/M_PI);
        p->drawPath(t.map(beam));

/*      p->save();
        p->setPen(Qt::red);
        p->drawLine(0, 0, 100*cos(clipangles.at(i)), 100*sin(clipangles.at(i)));
        p->setPen(Qt::green);
        p->drawLine(0, 0, 100*cos(beamangles.at(i)), 100*sin(beamangles.at(i)));
        p->restore();
*/
    }

    p->setClipping(false);
    p->setBrush(p->pen().color());
    p->drawEllipse(QPoint(), iRad, iRad);


    if(!tw->isCalculated())
        return;

    QPolygonF arrow(3);
    double scale = viewPortSettings::instance().resultsWidgetScale();

    for(int i=0; i<beams.count(); i++)
    {
        int currentBeam = beams.keys().at(i);
        bool left = (tw->beam(currentBeam).leftNodeI() == object);



        p->save();
        p->rotate(beams.value(beams.keys().at(i))*180/M_PI);

/*******
 *  N  *
 *******/

        double stress = tw->beam(currentBeam).N(left? 0 : tw->beam(currentBeam).l());

        if(!qFuzzyIsNull(stress))
        {
            p->setPen(viewPortSettings::instance().color(Colin::C_Np));

            if(stress>0)
            {
                //p->setPen(viewPortSettings::instance().color(Colin::C_Np));
                arrow[0] = QPointF(NStart+stress*tw->scaleP()*scale+5, 0);
                arrow[1] = QPointF(NStart+stress*tw->scaleP()*scale, -3);
                arrow[2] = QPointF(NStart+stress*tw->scaleP()*scale, 3);
            }
            else
            {
                //p->setPen(viewPortSettings::instance().color(Colin::C_Nm));
                arrow[0] = QPointF(NStart-5, 0);
                arrow[1] = QPointF(NStart, -3);
                arrow[2] = QPointF(NStart, 3);
            }
            p->setBrush(p->pen().color());
            p->drawPolygon(arrow);
            p->drawLine(NStart, 0, NStart+fabs(stress*tw->scaleP()*scale), 0);
        }

/*******
 *  Q  *
 *******/

        stress = tw->beam(currentBeam).Q(left? 0 : tw->beam(currentBeam).l());

        if(!qFuzzyIsNull(stress))
        {
            arrow[0] = QPointF(QStart, stress*tw->scaleP()*scale/2+((stress>0)?5:-5));
            arrow[1] = QPointF(QStart-3, stress*tw->scaleP()*scale/2);
            arrow[2] = QPointF(QStart+3, stress*tw->scaleP()*scale/2);
            p->setPen(viewPortSettings::instance().color(Colin::C_Qp));
            //if(stress>0)
            //{
            //    p->setPen(viewPortSettings::instance().color(Colin::C_Qp));
            //}
            //else
            //{
            //    p->setPen(viewPortSettings::instance().color(Colin::C_Qm));
            //}
            p->setBrush(p->pen().color());
            p->drawPolygon(arrow);
            p->drawLine(QStart, fabs(stress*tw->scaleP()*scale)/2, QStart, -fabs(stress*tw->scaleP()*scale)/2);
        }


/*******
 *  M  *
 *******/

        stress = tw->beam(currentBeam).M(left? 0 : tw->beam(currentBeam).l())*(left?1:-1);

        if(!qFuzzyIsNull(stress))
        {
            arrow[0] = QPointF(MStart, (stress>0)?-5:5);
            arrow[1] = QPointF(MStart-3, 0);
            arrow[2] = QPointF(MStart+3, 0);
            p->setPen(viewPortSettings::instance().color(Colin::C_Mp));
            //if(stress>0)
            //{
            //    p->setPen(viewPortSettings::instance().color(Colin::C_Mp));
            //}
            //else
            //{
            //    p->setPen(viewPortSettings::instance().color(Colin::C_Mm));
            //}
            p->setBrush(p->pen().color());
            p->rotate(-stress*tw->scaleM()/2*viewPortSettings::instance().MtoRad());
            p->drawPolygon(arrow);
            p->drawArc(-MStart, -MStart, 2*MStart, 2*MStart, 0, -stress*tw->scaleM()*16*viewPortSettings::instance().MtoRad());
        }

        p->restore();
    }
/*************
 *   loads   *
 *************/

    p->setPen(viewPortSettings::instance().color(Colin::C_Load1));
    p->setBrush(p->pen().color());

    for(int i=0; i<tw->load_n(); i++)
    {
        if(tw->load(i).at()!=object)
            continue;
        if(tw->load(i).typ() == wgv_load::nodeLoad)
        {
            double nodeangle = atan2(tw->load(i).Pz(), tw->load(i).Px())*180./M_PI+180;
            double nodeabs = sqrt(pow(tw->load(i).Px(), 2)+pow(tw->load(i).Pz(), 2));
            p->rotate(nodeangle);


            arrow[0] = QPointF(RxStart-5, 0);
            arrow[1] = QPointF(RxStart, -3);
            arrow[2] = QPointF(RxStart, 3);

            p->setBrush(p->pen().color());
            p->drawPolygon(arrow);
            p->drawLine(RxStart, 0, RxStart+fabs(nodeabs*tw->scaleP()*scale), 0);

            p->rotate(-nodeangle);
        }
        else if(tw->load(i).typ() == wgv_load::moment)
        {

            arrow[0] = QPointF(MExStart, (tw->load(i).M()>0)?-5:5);
            arrow[1] = QPointF(MExStart-3, 0);
            arrow[2] = QPointF(MExStart+3, 0);

            p->save();

            p->rotate(-90-tw->load(i).M()*tw->scaleM()/2*viewPortSettings::instance().MtoRad());
            p->drawPolygon(arrow);
            p->drawArc(-MExStart, -MExStart, 2*MExStart, 2*MExStart, 0, -tw->load(i).M()*tw->scaleM()*16*viewPortSettings::instance().MtoRad());

            p->restore();
        }
    }
    if(tw->node(object).hasbearing())
    {
        p->setPen(viewPortSettings::instance().color(Colin::C_Reaction));
        p->setBrush(p->pen().color());
        const wgv_bearing &bear = tw->node(object).bearing();

/*******
 * Ax  *
 *******/

        if(!qFuzzyIsNull(bear.a_x()))
        {
            p->drawLine(-RxStart, 0, -RxStart-fabs(bear.a_x()*scale*tw->scaleP()), 0);
            if(bear.a_x()>0)
            {
                arrow[0] = QPointF(-RxStart+5, 0);
                arrow[1] = QPointF(-RxStart, -3);
                arrow[2] = QPointF(-RxStart, 3);
            }
            else
            {
                arrow[0] = QPointF(-RxStart+bear.a_x()*scale*tw->scaleP()-5, 0);
                arrow[1] = QPointF(-RxStart+bear.a_x()*scale*tw->scaleP(), -3);
                arrow[2] = QPointF(-RxStart+bear.a_x()*scale*tw->scaleP(), 3);
            }
            p->drawPolygon(arrow);
        }


/*******
 * Az  *
 *******/

        if(!qFuzzyIsNull(bear.a_z()))
        {
            p->drawLine(0, RzStart, 0, RzStart+fabs(bear.a_z()*scale*tw->scaleP()));
            if(bear.a_z()<0)
            {
                arrow[0] = QPointF(0, RzStart-5);
                arrow[1] = QPointF(-3, RzStart);
                arrow[2] = QPointF(3, RzStart);
            }
            else
            {
                arrow[0] = QPointF(0, RzStart+bear.a_z()*scale*tw->scaleP()+5);
                arrow[1] = QPointF(-3, RzStart+bear.a_z()*scale*tw->scaleP());
                arrow[2] = QPointF(3, RzStart+bear.a_z()*scale*tw->scaleP());
            }
            p->drawPolygon(arrow);
        }


/*******
 * Ap  *
 *******/

        if(!qFuzzyIsNull(bear.a_m()))
        {

            arrow[0] = QPointF(RpStart, (bear.a_m()>0)?-5:5);
            arrow[1] = QPointF(RpStart-3, 0);
            arrow[2] = QPointF(RpStart+3, 0);

            p->save();

            p->rotate(90-bear.a_m()*tw->scaleM()/2*viewPortSettings::instance().MtoRad());
            p->drawPolygon(arrow);
            p->drawArc(-RpStart, -RpStart, 2*RpStart, 2*RpStart, 0, -bear.a_m()*tw->scaleM()*16*viewPortSettings::instance().MtoRad());

            p->restore();
        }
        p->resetTransform();
        p->translate(height()+10, mode->height()+3);



        bool atleastOneBear(false);


/*******
 * Ax  *
 *******/
        if(!qFuzzyIsNull(bear.a_x()))
        {
	    p->drawText(QRect(0, 0, 300, p->font().pointSize()+3),
                        QString("Ax%1 = %2 %3").arg(object).arg(bear.a_x()*PPREFIX, 0, 'f',  PRECISON).arg(unitSettings::instance().Peh()),
                        Qt::AlignLeft | Qt::AlignVCenter);
            p->translate(0, p->font().pointSize()+3);
            atleastOneBear = true;
        }


/*******
 * Az  *
 *******/

        if(!qFuzzyIsNull(bear.a_z()))
        {
	    p->drawText(QRect(0, 0, 300, p->font().pointSize()+3),
                        QString("Az%1 = %2 %3").arg(object).arg(bear.a_z()*PPREFIX, 0, 'f',  PRECISON).arg(unitSettings::instance().Peh()),
                        Qt::AlignLeft | Qt::AlignVCenter);
            p->translate(0, p->font().pointSize()+3);
            atleastOneBear = true;
        }


/*******
 * Am  *
 *******/

        if(!qFuzzyIsNull(bear.a_m()))
        {
	    p->drawText(QRect(0, 0, 300, p->font().pointSize()+3),
                        QString("Am%1 = %2 %3").arg(object).arg(bear.a_m()*MPREFIX, 0, 'f',  PRECISON).arg(unitSettings::instance().Meh()),
                        Qt::AlignLeft | Qt::AlignVCenter);
            p->translate(0, p->font().pointSize()+3);
            atleastOneBear = true;
        }
        if(atleastOneBear)
        {
            p->translate(0, 7);
            p->setRenderHint(QPainter::Antialiasing, false);
            p->setPen(palette().color(QPalette::ToolTipText));
            p->drawLine(-5, -4, 200, -4);
            p->setRenderHint(QPainter::Antialiasing, true);
        }

    }
    else
    {
        p->resetTransform();
	p->translate(height()-10, mode->height()+3);
    }

    bool atleastOneBear = false;
    p->setPen(palette().color(QPalette::ToolTipText));


/*******
 *  u  *
 *******/

    double u = tw->node(object).u();
    if(u!=0)
    {
	p->drawText(QRect(0, 0, 300, p->font().pointSize()+3),
		    QString("u%1 = %2 m").arg(object).arg(u, 0, 'e', PRECISON),
                    Qt::AlignLeft | Qt::AlignVCenter);
        p->translate(0, p->font().pointSize()+3);
        atleastOneBear = true;
    }


/*******
 *  w  *
 *******/

    u = tw->node(object).w();
    if(u!=0)
    {
	p->drawText(QRect(0, 0, 300, p->font().pointSize()+3),
		    QString("w%1 = %2 m").arg(object).arg(u, 0, 'e', PRECISON),
                    Qt::AlignLeft | Qt::AlignVCenter);
        p->translate(0, p->font().pointSize()+3);
        atleastOneBear = true;
    }

/*******
 *  p  *
 *******/

    u=tw->node(object).phi();
    if(u!=0)
    {
	p->drawText(QRect(0, 0, 300, p->font().pointSize()+3),
		    QString("%1%2 = %3 grad").arg(QChar(0x03C6)).arg(object).arg(u*180/M_PI, 0, 'e', PRECISON),
                    Qt::AlignLeft | Qt::AlignVCenter);
        p->translate(0, p->font().pointSize()+3);
        atleastOneBear = true;
    }

//line
    if(atleastOneBear)
    {
        p->translate(0, 7);
        p->setRenderHint(QPainter::Antialiasing, false);
        p->setPen(palette().color(QPalette::ToolTipText));
        p->drawLine(-5, -4, 200, -4);
        p->setRenderHint(QPainter::Antialiasing, true);
    }


    foreach(int nr, beams.keys())
    {
        bool left = (tw->beam(nr).leftNodeI() == object);



/*******
 *  N  *
 *******/


        double stress = tw->beam(nr).N(left? 0 : tw->beam(nr).l());


        if(!qFuzzyIsNull(stress))
        {
            //if(stress>0)
                p->setPen(viewPortSettings::instance().color(Colin::C_Np));
            //else
            //    p->setPen(viewPortSettings::instance().color(Colin::C_Nm));
	    p->drawText(QRect(0, 0, 300, p->font().pointSize()+3),
                        QString("N%1%2 = %3 %4").arg(left ? "l":"r").arg(nr).arg(stress*PPREFIX, 0, 'f',  PRECISON).arg(unitSettings::instance().Peh()),
                        Qt::AlignLeft | Qt::AlignVCenter);
            p->translate(0, p->font().pointSize()+3);
        }



/*******
 *  Q  *
 *******/
        stress = tw->beam(nr).Q(left? 0 : tw->beam(nr).l());


        if(!qFuzzyIsNull(stress))
        {
            //if(stress>0)
                p->setPen(viewPortSettings::instance().color(Colin::C_Qp));
            //else
            //    p->setPen(viewPortSettings::instance().color(Colin::C_Qm));
	    p->drawText(QRect(0, 0, 300, p->font().pointSize()+3),
                        QString("Q%1%2 = %3 %4").arg(left ? "l":"r").arg(nr).arg(stress*PPREFIX, 0, 'f', PRECISON).arg(unitSettings::instance().Peh()),
                        Qt::AlignLeft | Qt::AlignVCenter);
            p->translate(0, p->font().pointSize()+3);
        }


/*******
 *  M  *
 *******/
        stress = tw->beam(nr).M(left? 0 : tw->beam(nr).l());


        if(!qFuzzyIsNull(stress))
        {
            //if(stress>0)
                p->setPen(viewPortSettings::instance().color(Colin::C_Mp));
            //else
            //    p->setPen(viewPortSettings::instance().color(Colin::C_Mm));
	    p->drawText(QRect(0, 0, 300, p->font().pointSize()+3),
                        QString("M%1%2 = %3 %4").arg(left ? "l":"r").arg(nr).arg(stress*MPREFIX, 0, 'f', PRECISON).arg(unitSettings::instance().Meh()),
                        Qt::AlignLeft | Qt::AlignVCenter);
            p->translate(0, p->font().pointSize()+3);
        }
    }
}

void resultVisualWidget::paintBeamResults(QPainter *p)
{
    const double d = 6;
    const double pc = 5;
    const double lenght = 200;
    const double dx = 70;
    const double NStart = 5;
    const double QStart = 15;
    const double MStart = -20;
    const double MRad = 45;

    wgv_tw *tw = filelist::instance().currentFile();
    p->setRenderHint(QPainter::Antialiasing, true);
    QPainterPath beam;
    beam.moveTo(QPointF(-lenght, -d/2));
    beam.lineTo(QPointF(0, -d/2));
    beam.cubicTo(QPointF(pc, -d/4), QPointF(-pc, d/6),
                 QPointF(0, d/2));
    beam.lineTo(QPointF(-lenght, d/2));

    p->setPen(palette().color(QPalette::ToolTipText));
    p->drawText(QRect(2, 2, 250, 100), QString(tr("beam") +" #%1, x = %2 m").arg(object).arg(x, 0, 'f', PRECISON), Qt::AlignLeft | Qt::AlignTop);
    p->translate(height()/2, height()/2);
    QTransform t;
    t.translate(-dx, 0);
    p->drawPath(t.map(beam));
    t.reset();
    t.rotate(180);
    t.translate(-dx, 0);
    p->drawPath(t.map(beam));

    if(!tw->isCalculated())
        return;

    double M, N, Q;
    QPolygonF arrow(3);
    M = tw->beam(object).M(x);
    N = tw->beam(object).N(x);
    Q = tw->beam(object).Q(x);

    double scale = viewPortSettings::instance().resultsWidgetScale();


    for(int i=0; i<2; i++)
    {
        p->translate(-dx, 0);

/*******
 *  N  *
 *******/
        if(!qFuzzyIsNull(N))
        {
            p->setPen(viewPortSettings::instance().color(Colin::C_Np));
            if(N>0)
            {
                //p->setPen(viewPortSettings::instance().color(Colin::C_Np));
                arrow[0] = QPointF(NStart+fabs(N)*scale*tw->scaleP()+5, 0);
                arrow[1] = QPointF(NStart+fabs(N)*scale*tw->scaleP(), 3);
                arrow[2] = QPointF(NStart+fabs(N)*scale*tw->scaleP(), -3);
            }
            else
            {
                //p->setPen(viewPortSettings::instance().color(Colin::C_Nm));
                arrow[0] = QPointF(NStart-5, 0);
                arrow[1] = QPointF(NStart, 3);
                arrow[2] = QPointF(NStart, -3);
            }
            p->setBrush(p->pen().color());
            p->drawLine(NStart, 0, NStart+fabs(N)*scale*tw->scaleP(), 0);
            p->drawPolygon(arrow);
        }

/*******
 *  Q  *
 *******/
        if(!qFuzzyIsNull(Q))
        {
            p->setPen(viewPortSettings::instance().color(Colin::C_Qp));
            if(Q>0)
            {
                //p->setPen(viewPortSettings::instance().color(Colin::C_Qp));
                arrow[0] = QPointF(QStart, Q*scale*tw->scaleP()/2+5);
            }
            else
            {
                //p->setPen(viewPortSettings::instance().color(Colin::C_Qm));
                arrow[0] = QPointF(QStart, Q*scale*tw->scaleP()/2-5);
            }
            arrow[1] = QPointF(QStart+3, Q*scale*tw->scaleP()/2);
            arrow[2] = QPointF(QStart-3, Q*scale*tw->scaleP()/2);
            p->setBrush(p->pen().color());
            p->drawLine(QStart, Q*scale*tw->scaleP()/2, QStart, -Q*scale*tw->scaleP()/2);
            p->drawPolygon(arrow);
        }
/*******
 *  M  *
 *******/
        if(!qFuzzyIsNull(M))
        {
            p->setPen(viewPortSettings::instance().color(Colin::C_Mp));
            if(M>0)
            {
                //p->setPen(viewPortSettings::instance().color(Colin::C_Mp));
                arrow[0] = QPointF(MRad, -5);
            }
            else
            {
                //p->setPen(viewPortSettings::instance().color(Colin::C_Mm));
                arrow[0] = QPointF(MRad, 5);
            }
            arrow[1] = QPointF(MRad+3, 0);
            arrow[2] = QPointF(MRad-3, 0);
            p->setBrush(p->pen().color());
            p->save();
            p->translate(MStart, 0);
            if(i)
                p->scale(1, -1);
            p->rotate(-M*tw->scaleM()/2*viewPortSettings::instance().MtoRad());
            p->drawPolygon(arrow);
            p->drawArc(-MRad, -MRad, 2*MRad, 2*MRad, 0, -M*tw->scaleM()*16*viewPortSettings::instance().MtoRad());
            p->restore();
        }
        p->translate(dx, 0);
        p->rotate(180);
    }


    p->resetTransform();
    p->translate(height()-20, mode->height()+3);


    bool atleastOneBear = false;
    p->setPen(palette().color(QPalette::ToolTipText));


/*******
 *  u  *
 *******/

    double u = tw->beam(object).u(x);
    if(u!=0)
    {
	p->drawText(QRect(0, 0, 300, p->font().pointSize()+3),
		    QString("u(%1) = %2 m").arg(x, 0, 'f',  PRECISON).arg(u, 0, 'e', PRECISON),
                    Qt::AlignLeft | Qt::AlignVCenter);
        p->translate(0, p->font().pointSize()+3);
        atleastOneBear = true;
    }


/*******
 *  w  *
 *******/

    u = tw->beam(object).w(x);
    if(u!=0)
    {
	p->drawText(QRect(0, 0, 300, p->font().pointSize()+3),
		    QString("w(%1) = %2 m").arg(x, 0, 'f',  PRECISON).arg(u, 0, 'e', PRECISON),
                    Qt::AlignLeft | Qt::AlignVCenter);
        p->translate(0, p->font().pointSize()+3);
        atleastOneBear = true;
    }

/*******
 *  p  *
 *******/

    u = tw->beam(object).phi(x);
    if(u!=0)
    {
	p->drawText(QRect(0, 0, 300, p->font().pointSize()+3),
		    QString("%1(%2) = %3 grad").arg(QChar(0x03C6)).arg(x, 0, 'f',  PRECISON).arg(u*180/M_PI, 0, 'e', PRECISON),
                    Qt::AlignLeft | Qt::AlignVCenter);
        p->translate(0, p->font().pointSize()+3);
        atleastOneBear = true;
    }

//line
    if(atleastOneBear)
    {
        p->translate(0, 7);
        p->setRenderHint(QPainter::Antialiasing, false);
        p->setPen(palette().color(QPalette::ToolTipText));
        p->drawLine(-5, -4, 200, -4);
        p->setRenderHint(QPainter::Antialiasing, true);
    }


/*******
 *  N  *
 *******/


    double stress = tw->beam(object).N(x);


    if(!qFuzzyIsNull(stress))
    {
        //if(stress>0)
            p->setPen(viewPortSettings::instance().color(Colin::C_Np));
        //else
        //    p->setPen(viewPortSettings::instance().color(Colin::C_Nm));
	p->drawText(QRect(0, 0, 300, p->font().pointSize()+3),
                    QString("N(%1) = %2 %3").arg(x, 0, 'f',  PRECISON).arg(stress*PPREFIX, 0, 'f',  PRECISON).arg(unitSettings::instance().Peh()),
                    Qt::AlignLeft | Qt::AlignVCenter);
        p->translate(0, p->font().pointSize()+3);
    }



/*******
 *  Q  *
 *******/
    stress = tw->beam(object).Q(x);


    if(!qFuzzyIsNull(stress))
    {
        //if(stress>0)
            p->setPen(viewPortSettings::instance().color(Colin::C_Qp));
        //else
        //    p->setPen(viewPortSettings::instance().color(Colin::C_Qm));
	p->drawText(QRect(0, 0, 300, p->font().pointSize()+3),
                    QString("Q(%1) = %2 %3").arg(x, 0, 'f',  PRECISON).arg(stress*PPREFIX, 0, 'f',  PRECISON).arg(unitSettings::instance().Peh()),
                    Qt::AlignLeft | Qt::AlignVCenter);
        p->translate(0, p->font().pointSize()+3);
    }


/*******
 *  M  *
 *******/
    stress = tw->beam(object).M(x);


    if(!qFuzzyIsNull(stress))
    {
        //if(stress>0)
            p->setPen(viewPortSettings::instance().color(Colin::C_Mp));
        //else
        //    p->setPen(viewPortSettings::instance().color(Colin::C_Mm));
	p->drawText(QRect(0, 0, 300, p->font().pointSize()+3),
                    QString("M(%1) = %2 %3").arg(x, 0, 'f',  PRECISON).arg(stress*MPREFIX, 0, 'f',  PRECISON).arg(unitSettings::instance().Meh()),
                    Qt::AlignLeft | Qt::AlignVCenter);
        p->translate(0, p->font().pointSize()+3);
    }


}

void resultVisualWidget::fileChanged(wgv_tw *tw)
{
    objectType = catcher::CatchedNothing;
    if(tw)
    {
        mode->show();
        if(sH != Hide)
            show();
    }
    else
    {
        hide();
        mode->hide();
    }
}

void resultVisualWidget::setElement(const catcher::CatchCases &cc, const int &index, const double &x_)
{
    Q_ASSERT((cc == catcher::CatchedNode && index>-1 && index<filelist::instance().currentFile()->node_n()) ||
             (cc == catcher::CatchedBeam && index>-1 && index<filelist::instance().currentFile()->beam_n()) ||
             (cc == catcher::CatchedLoad && index>-1 && index<filelist::instance().currentFile()->load_n()) ||
             (cc == catcher::CatchedTemp && index>-1 && index<filelist::instance().currentFile()->load_n()) ||
             (cc == catcher::CatchedNothing));
    objectType = cc;
    object = index;
    this->x = x_;
}

QSize resultVisualWidget::bSize()
{
    //if(sH == Results)
    //    return QSize(300, 300);
    return QSize(350, 220);
}
