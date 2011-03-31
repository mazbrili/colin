#include "viewport.h"

#include "nodemenu.h"
#include "beammenu.h"
#include "loadmenu.h"
#include "tempmenu.h"
#include "nloadmenu.h"
#include "momentmenu.h"
#include "unitsettings.h"

//QPoint viewport::lastClick = QPoint(-1, -1);
//int drawingSteps = -1;
wgv_tw *viewport::tw = filelist::instance().currentFile();
viewPortSettings *viewport::vS = &viewPortSettings::instance();
structPainter viewport::sPainter = structPainter();
catcher::CatchCases viewport::movingObject = catcher::CatchedNothing;
int viewport::viewCounter = 0;

wgv_tw* viewport::toPaste_ = 0;


const double zoomfactor = 1.2;
const double maxZoomfactor = 100000.0;
const double minZoomfactor = 0.1;

/*
            p.setPen(viewPortSettings::instance().color(Colin::C_Mp));
viewport::viewport(QWidget *parent) :
        QGLWidget (QGLFormat(QGL::SampleBuffers), parent)*/
viewport::viewport(QWidget *parent) :
        QWidget (parent)
{

    mouseIsOverChild = false;
    id_ = viewCounter++;
    maximizer = new QPushButton("", this);
    maximizer->setIcon(colinIcons::instance().icon(Colin::drawZoomIn));
    //maximizer->setFlat(true);

    hider = new QPushButton("", this);
    hider->setIcon(colinIcons::instance().icon(Colin::Close));
    //hider->setFlat(true);

    allshower = new QPushButton("", this);
    allshower->setIcon(colinIcons::instance().icon(Colin::Windows));
    //allshower->setFlat(true);

    setMouseTracking(true);
    lastMouseButtons = Qt::NoButton;
    lastMousePosition = QPoint(-100, 100);

    selec = new drawSelectionWidget(id(), this);
    selec->move(30, 30);


    setCursor(Qt::OpenHandCursor);
    maximizer->setCursor(Qt::ArrowCursor);
    hider->setCursor(Qt::ArrowCursor);
    allshower->setCursor(Qt::ArrowCursor);
    selec->setCursor(Qt::ArrowCursor);

    tooltip = new viewPortToolTip(this);

    installEventFilter(tooltip);
    maximizer->installEventFilter(tooltip);
    hider->installEventFilter(tooltip);
    allshower->installEventFilter(tooltip);
    selec->installEventFilter(tooltip);

    connect(&filelist::instance(),              SIGNAL(currentChanged(wgv_tw*)),
            this,                               SLOT(setTw(wgv_tw*)));

    connect(filelist::instance().undoGroup(),   SIGNAL(indexChanged(int)),
            this,                               SLOT(update()));

    connect(maximizer,                          SIGNAL(clicked()),
            this,                               SLOT(emit_maximizeMe()));

    connect(hider,                              SIGNAL(clicked()),
            this,                               SLOT(emit_hideMe()));

    connect(allshower,                          SIGNAL(clicked()),
            this,                               SLOT(emit_showAll()));

    connect(&viewPortSettings::instance(),      SIGNAL(currentActionChanged(Colin::Actions)),
            this,                               SLOT(setDrawCursor(Colin::Actions)));

    connect(&viewPortSettings::instance(),      SIGNAL(clipBoardActionChanged(Colin::clipboardAction)),
            this,                               SLOT(setClipCursor(Colin::clipboardAction)));

    allshower->hide();


}

viewport::~viewport()
{
    viewCounter--;
}

const int &viewport::id() const
{
    return id_;
}

QTransform &viewport::globalMatrix()
{
    return tw->view(id());
}

void viewport::setTw(wgv_tw* t)
{
    if(t!=0)
    {
        tw = t;

        connect(tw,             SIGNAL(selectionChanged()),
                this,           SLOT(update()));

        connect(tw,             SIGNAL(changedUScale(double)),
                this,           SLOT(update()));

        connect(tw,             SIGNAL(changedPScale(double)),
                this,           SLOT(update()));

        connect(tw,             SIGNAL(changedMScale(double)),
                this,           SLOT(update()));

        update();

    }
}

void viewport::setFullyArea(bool doI)
{
    maximizer->setHidden(doI);
    hider->setHidden(doI);
    allshower->setHidden(!doI);
}

void viewport::paintEvent(QPaintEvent *)
{
    if(tw == NULL) return;



#ifndef QT_NO_DEBUG
    QTime t;
    t.start();
#endif //QT_NO_DEBUG


    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, viewPortSettings::instance().antialiasing());

    QTransform &globM = globalMatrix();


    //background && grid!

    double zf = globM.m11();

    QPixmap brushpix(grid::instance().gridmap(&zf));

    globM = QTransform(globM.m11()*zf,         0,                       0,
                       0,                      globM.m22()*zf,          0,
                       int(globM.dx()),        int(globM.dy()),         1);


    //gird looks much more better if the pixmap dont need to be scaled..
    //so scale all the rest

    p.translate(int(globM.dx())%brushpix.width(),
                int(globM.dy())%brushpix.height());

    p.fillRect(QRect(-int(globM.dx())%brushpix.width(),
                     -int(globM.dy())%brushpix.height(),
                     width(),
                     height()),
               brushpix);

    //center ...

    p.resetTransform();
    p.translate(globM.dx(), globM.dy());
    grid::instance().drawCordsys(&p);

    //and scale ...

    p.resetTransform();
    grid::instance().drawScale(&p, globM.m11());

    //draw the structur
    drawStruct(&p);

    //cursor
    drawCursorAxes(&p);


    //making this a rounded rect
    p.setPen(palette().color(QPalette::Dark));
    //if(!rect().contains(mapFromGlobal(QCursor::pos())))
        //p.setPen(palette().color(QPalette::Dark));
    //else
        //p.setPen(palette().color(QPalette::Highlight));
    p.setBrush(palette().brush(QPalette::Background));
    p.setRenderHint(QPainter::Antialiasing, false);

    /*
    int rad = 2;

    QPainterPath RPath;
    RPath.moveTo(rad, -2);
    RPath.arcTo(QRect(0, 0, 2*rad, 2*rad), 90, 90);
    RPath.lineTo(-2, rad);
    RPath.lineTo(-2, -2);
    RPath.lineTo(rad, -2);

    //"buttonbox"
    const int c = 16;
    const int dh = 4;
    QPainterPath path;
    QRect bR;

    if(maximizer->isVisible() && hider->isVisible())
    {
        bR = QRect(maximizer->geometry().x(), 0,
                   width()-maximizer->geometry().x(),
                   maximizer->geometry().bottom());
    }
    else
    {
        bR = QRect(allshower->geometry().x(), 0,
                   width()-allshower->geometry().x(),
                   allshower->geometry().bottom());
    }

    path.moveTo(bR.x()-c, 0);
    path.cubicTo(bR.x()-c/2-1, 0,
                 bR.x()-c/2-1, bR.bottom()+dh,
                 bR.x(), bR.bottom()+dh);
    path.arcTo(QRect(width()-rad*2-1, bR.bottom()+dh, 2*rad, 2*rad), 90, -90);
    path.lineTo(width()+1, bR.bottom()+dh+rad);
    path.lineTo(width()+1, -1);
    path.lineTo(bR.x()-c, -1);
    p.drawPath(path);

    //outlines
    p.drawLine(width()-1, bR.bottom()+dh+rad,
               width()-1, height()-rad);
    p.drawLine(rad, 0,
               bR.x()-c, 0);
    p.drawLine(rad, height()-1,
               width()-rad, height()-1);
    p.drawLine(0, rad,
               0, height()-rad);
    p.drawPath(RPath);
    p.translate(0, height()-1);
    p.scale(1, -1);
    p.drawPath(RPath);
    p.translate(width()-1, 0);
    p.scale(-1, 1);
    p.drawPath(RPath); */

    p.drawLine(0,       0,
               width(), 0);
    p.drawLine(0,       height()-1,
               width(), height()-1);

    p.drawLine(0, 0,
               0, height());
    p.drawLine(width()-1, 0,
               width()-1, height());




#ifndef QT_NO_DEBUG
    p.resetTransform();
    p.setPen(Qt::blue);
    p.drawText(QRect(width()-100, height()-40, 100, 40),
               QString("drawing time = %1 ms")
               .arg(t.elapsed()));
#endif //QT_NO_DEBUG

}

void viewport::drawStruct(QPainter *p)
{
    sPainter.drawStruct(*filelist::instance().currentFile(), p, &globalMatrix(), viewPortSettings::instance().elements(id()));
}

void viewport::drawCursorAxes(QPainter *p)
{

    if(mouseIsOverChild)
        return;
    const static QString prefixM("m");

    QPointF cursorpos = mapFromGlobal(QCursor::pos());

    if(cursorpos.x() < 0 || cursorpos.y() < 0 ||
       cursorpos.x() > width()-1 || cursorpos.y() > height()-1)
        return;



/***********************************
 *  first we have to snap things   *
 ***********************************/

    int aditional = -1;
    bool unignoreableSnap = false;

    catcher::CatchCases cC;

    if((vS->toClip() == Colin::copybasePointRequest) ||
       (vS->toClip() == Colin::cutbasePointRequest)  ||
       (vS->toClip() == Colin::pasteRequest)         )
    {
        //snaps for copy & paste
        cC = catcher::CatchedNode | catcher::CatchedGrid;
    }
    else if(vS->toClip() == Colin::pasteLoadRequestBeam)
    {
        cC = catcher::CatchedBeam;
        unignoreableSnap = true;
    }
    else if(vS->toClip() == Colin::pasteLoadRequestNode)
    {
        cC = catcher::CatchedNode;
        unignoreableSnap = true;
    }
    else if(vS->toDraw() == Colin::drawMove)
    {
        //snaps for moving stuff
        switch(movingObject)
        {
        case catcher::CatchedNode:
            cC = catcher::CatchedGrid;
            break;
        case catcher::CatchedLoadHotSpot:
            if(tw->load(tw->lastObjectLoad()).typ() == wgv_load::decreasingLinearLoad    ||
               tw->load(tw->lastObjectLoad()).typ() == wgv_load::increasingLinearLoad    ||
               tw->load(tw->lastObjectLoad()).typ() == wgv_load::uniformlyDistibutedLoad )
		cC = catcher::CatchedOrthoGlob |  catcher::CatchedOrthoLokal | catcher::CatchedGrid;
            else
		cC = catcher::CatchedOrthoGlob | catcher::CatchedGrid;
            break;
        case catcher::CatchedLoad:
            if(tw->load(tw->lastObjectLoad()).typ() == wgv_load::decreasingLinearLoad    ||
               tw->load(tw->lastObjectLoad()).typ() == wgv_load::increasingLinearLoad    ||
               tw->load(tw->lastObjectLoad()).typ() == wgv_load::uniformlyDistibutedLoad )
                cC = catcher::CatchedBeam;
            else
                cC = catcher::CatchedNode;
            break;
        case catcher::CatchedTemp:
            cC = catcher::CatchedBeam;
            break;
        default:
            cC = catcher::CatchStdMove;
            unignoreableSnap = true;
        }
    }
    else if((vS->toDraw()& Colin::drawBearing) == Colin::drawBearing)
    {
        unignoreableSnap = true;
        cC = catcher::CatchedNode;
    }
    else if((vS->toDraw()& Colin::drawJoint) == Colin::drawJoint)
    {
        unignoreableSnap = true;
        cC = catcher::instance().CasesForAction(Colin::drawJoint);
    }
    else if(vS->toDraw() == Colin::drawSelec)
    {
        unignoreableSnap = true;
        cC = catcher::instance().CasesForAction(vS->toDraw());
    }
    else if((vS->toDraw() == Colin::drawULoad ||
             vS->toDraw() == Colin::drawILoad ||
             vS->toDraw() == Colin::drawDLoad ||
             vS->toDraw() == Colin::drawTemp) &&
             tw->lastObjectBeam() == -1)
    {
        unignoreableSnap = true;
        cC=catcher::instance().CasesForAction(vS->toDraw());
    }
    else if((vS->toDraw() == Colin::drawNLoad ||
             vS->toDraw() == Colin::drawMoment) &&
            tw->lastObjectNode() == -1)
    {
        unignoreableSnap = true;
        cC=catcher::instance().CasesForAction(vS->toDraw());
    }
    else if(vS->toDraw() == Colin::drawDoubleLoad &&
            tw->lastObjectBeam() == -1)
    {
        unignoreableSnap = true;
        cC = catcher::CatchedBeamEnd;
    }
    else if((vS->toDraw() == Colin::drawMove))
    {
        switch(movingObject)
        {
        case catcher::CatchedNode:
            cC = catcher::CatchStdMoveNode;
            break;
        case catcher::CatchedTemp:
        case catcher::CatchedLoad:
            cC = catcher::CatchStdMoveLoad;
            break;
        case catcher::CatchedLoadHotSpot:
            cC = catcher::CatchStdMoveLoadHotSpot;
            break;
        default:
            break;
        }
    }
    else
    {
        //snap for normal actions
	cC = catcher::instance().CasesForAction(vS->toDraw());
    }

    if(!vS->elements(id()).testFlag(Colin::nload))
    {
        cC = cC & ~catcher::CatchedLoad;
        cC = cC & ~catcher::CatchedLoadHotSpot;
    }

    if((vS->toDraw() & (Colin::drawNLoad | Colin::drawMoment)) != Colin::drawMove)
    {
        if(tw->lastObjectNode()>0)
            cC = cC & ~(catcher::CatchedNode | catcher::CatchedBeam | catcher::CatchedMiddle);
    }
    if((vS->toDraw() & (Colin::drawULoad | Colin::drawILoad | Colin::drawDLoad | Colin::drawDoubleLoad)) != Colin::drawMove)
    {
        if(tw->lastObjectBeam()>0)
            cC = cC & ~(catcher::CatchedBeam | catcher::CatchedBeamEnd);
    }
    //snap!
    int first = catcher::instance().doYourWork(&cursorpos, &cC, globalMatrix(), &aditional, unignoreableSnap);



    /***********************************
     *          set tooltips!          *
     ***********************************/


    //tooltips for copy & paste

    if(vS->toClip() == Colin::copybasePointRequest)
    {
        tooltip->set(colinIcons::instance().icon(Colin::Copy, true),
                     QString::number(globalMatrix().inverted().map(cursorpos).x(), 'f', 2) + ", " +
                     QString::number(globalMatrix().inverted().map(cursorpos).y(), 'f', 2),
                     tr("enter base point to copy selection!"));
    }
    else if(vS->toClip() == Colin::cutbasePointRequest)
    {
        tooltip->set(colinIcons::instance().icon(Colin::Cut, true),
                     QString::number(globalMatrix().inverted().map(cursorpos).x(), 'f', 2) + ", " +
                     QString::number(globalMatrix().inverted().map(cursorpos).y(), 'f', 2),
                     tr("enter base point to cut selection!"));
    }
    else if(vS->toClip() == Colin::pasteRequest)
    {
        tooltip->set(colinIcons::instance().icon(Colin::Paste, true),
                     QString::number(globalMatrix().inverted().map(cursorpos).x(), 'f', 2) + ", " +
                     QString::number(globalMatrix().inverted().map(cursorpos).y(), 'f', 2),
                     tr("enter base point to paste from clipboard!"));
    }
    else if(vS->toClip() == Colin::pasteLoadRequestBeam ||
            vS->toClip() == Colin::pasteLoadRequestNode )
    {
        tooltip->set(colinIcons::instance().icon(Colin::Paste, true),
                     QString::number(globalMatrix().inverted().map(cursorpos).x(), 'f', 2) + ", " +
                     QString::number(globalMatrix().inverted().map(cursorpos).y(), 'f', 2),
                     tr("enter base object to paste a load from clipboard!"));
    }
    else if(vS->toDraw() == Colin::drawMove && movingObject == catcher::CatchedLoadHotSpot && cC!=catcher::CatchedOrthoLokal) //show local koords in tooltip
    {
        QPointF local = tw->lastObjectClick()-globalMatrix().inverted().map(cursorpos);
        local /= tw->scaleP();
        QString eh;
        if(tw->load(tw->lastObjectLoad()).typ() == wgv_load::uniformlyDistibutedLoad ||
           tw->load(tw->lastObjectLoad()).typ() == wgv_load::increasingLinearLoad    ||
           tw->load(tw->lastObjectLoad()).typ() == wgv_load::decreasingLinearLoad    )
            eh = unitSettings::instance().Peh()+"/m";
        else
            eh = unitSettings::instance().Peh();
        tooltip->set(colinIcons::instance().icon(tw->load(tw->lastObjectLoad()).typ(), true),
                     QString::number(local.x()*PPREFIX, 'f', 2) + eh + " , " +
                     QString::number(local.y()*PPREFIX, 'f', 2) + eh,
                     "");
    }
    else if((vS->toDraw() == Colin::drawDoubleLoad) && tw->lastObjectBeam()>-1)
    {
	QString eh;
	if(qApp->keyboardModifiers().testFlag(Qt::ControlModifier) ||
           qApp->keyboardModifiers().testFlag(Qt::ShiftModifier))
	{
	    QPointF local = tw->node(tw->lastObjectNode()).toQPointF()-globalMatrix().inverted().map(cursorpos);
	    eh = unitSettings::instance().Meh();
	    double M = sqrt(pow(local.x(), 2)+pow(local.y(), 2))/tw->scaleM();
	    if(local.x()<0)
		M=-M;
	    tooltip->set(colinIcons::instance().icon(vS->toDraw(), true),
                         QString::number(M*MPREFIX, 'f', PRECISON) + eh,
                         tr("press Shift to draw moments!"));
	}
	else
	{
	    QPointF local = globalMatrix().inverted().map(cursorpos);
	    eh = unitSettings::instance().Peh();
	    local = tw->beam(tw->lastObjectBeam()).transform().inverted().map(local);
	    if(tw->lastObjectNode() == tw->beam(tw->lastObjectBeam()).rightNodeI())
		local -= QPointF(tw->beam(tw->lastObjectBeam()).l(), 0);
	    local/=tw->scaleP();
	    tooltip->set(colinIcons::instance().icon(vS->toDraw(), true),
			 QString::number(local.x()*PPREFIX, 'f', PRECISON) + eh +", " +
			 QString::number(local.y()*PPREFIX, 'f', PRECISON) + eh +", ",
                         tr("press Shift to draw moments!"));
	}
    }
    else if((vS->toDraw() == Colin::drawNLoad || vS->toDraw() == Colin::drawMoment) && tw->lastObjectNode() > -1 && cC==catcher::CatchedNothing) //show local koords in tooltip
    {
        QPointF local = tw->node(tw->lastObjectNode()).toQPointF()-globalMatrix().inverted().map(cursorpos);
        QString eh;
        if(vS->toDraw() == Colin::drawNLoad)
        {
            eh = unitSettings::instance().Peh();
            local /= tw->scaleP();
            tooltip->set(colinIcons::instance().icon(vS->toDraw(), true),
			 QString::number(local.x()*PPREFIX, 'f', PRECISON) + eh +", " +
			 QString::number(local.y()*PPREFIX, 'f', PRECISON) + eh +", ",
                         "");
        }
        else
        {
            eh = unitSettings::instance().Meh();
            local /= tw->scaleM();

            tooltip->set(colinIcons::instance().icon(vS->toDraw(), true),
                         QString::number(sqrt(pow(local.x()*PPREFIX,2)+
					      pow(local.y()*PPREFIX,2))*(local.x()>0?1:-1), 'f', PRECISON) + eh,
                         "");
        }
    }
    else if((vS->toDraw() == Colin::drawULoad || vS->toDraw() == Colin::drawILoad || Colin::drawULoad) && tw->lastObjectBeam() > -1 && cC==catcher::CatchedNothing) //show local koords in tooltip
    {
        QPointF local = tw->lastObjectClick()-globalMatrix().inverted().map(cursorpos);
        local /= tw->scaleP();
        tooltip->set(colinIcons::instance().icon(vS->toDraw(), true),
                     QString::number(local.x()*PPREFIX, 'f', 2) + unitSettings::instance().Peh()+"/m, " +
                     QString::number(local.y()*PPREFIX, 'f', 2) + unitSettings::instance().Peh()+"/m, ",
                     "");
    }
    else
    {
	QString extend;

        //tooltips for moving (same as normal but with extra text, explainig how to move stuff...
	if(vS->toDraw() == Colin::drawMove)
	{
	    if(movingObject == catcher::CatchedNothing)
            {
		switch(cC)
		{
		case catcher::CatchedNode:
		    extend = tr("press and move mouse to move node!");
		    break;
		case catcher::CatchedLoadHotSpot:
		    extend = tr("press and move mouse to change load!");
		    break;
		case catcher::CatchedLoad:
		    extend = tr("press and mouve mouse to move load!");
		    break;
		case catcher::CatchedTemp:
		    extend = tr("press and move mouse to move temperature!");
		    break;
		default:
		    break;
		}
            }
        }

        //normal stuff
	switch(cC)
	{
	case catcher::CatchedNothing:
            if((Colin::testForNLoad(vS->toDraw()) && tw->lastObjectNode() > -1) ||
               (Colin::testForBLoad(vS->toDraw()) && tw->lastObjectBeam() > -1) )
                tooltip->set(colinIcons::instance().icon(vS->toDraw()),
                             QString::number(globalMatrix().inverted().map(cursorpos).x()-tw->lastObjectClick().x(), 'f', 2) + ", " +
                             QString::number(globalMatrix().inverted().map(cursorpos).y()-tw->lastObjectClick().y(), 'f', 2),
                             extend);
            else
                tooltip->set(QIcon(),
                             QString::number(globalMatrix().inverted().map(cursorpos).x(), 'f', 2) + ", " +
                             QString::number(globalMatrix().inverted().map(cursorpos).y(), 'f', 2),
                             extend);
	    break;
	case catcher::CatchedGrid:
	    tooltip->set(colinIcons::instance().icon(cC, true),
			 QString::number(globalMatrix().inverted().map(cursorpos).x(), 'f', 2) + ", " +
			 QString::number(globalMatrix().inverted().map(cursorpos).y(), 'f', 2),
			 extend);
	    break;
	case catcher::CatchedNode:
	    tooltip->set(colinIcons::instance().icon(cC, true),
			 tr("node # ")+ QString::number(first),
			 extend);
	    break;
	case catcher::CatchedBeam:
	    tooltip->set(colinIcons::instance().icon(cC, true),
			 tr("beam # ")+ QString::number(first),
			 extend);
	    break;
	case catcher::CatchedLoadHotSpot:
	    tooltip->set(colinIcons::instance().icon(tw->load(first).typ(), true),
			 tr("load # ")+ QString::number(first),
			 extend);
	    break;
	case catcher::CatchedLoad:
	    tooltip->set(colinIcons::instance().icon(tw->load(first).typ(), true),
			 tr("load # ")+ QString::number(first),
			 extend);
	    break;
	case catcher::CatchedCrossing:
	    tooltip->set(colinIcons::instance().icon(cC, true),
			 tr("beam # ")+ QString::number(first) +
			 "\n" + tr("beam # ")+ QString::number(aditional),
			 extend);
	    break;
	case catcher::CatchedMiddle:
	    tooltip->set(colinIcons::instance().icon(cC, true),
			 tr("beam # ")+ QString::number(first),
                         "");
            break;
        case catcher::CatchedTemp:
            tooltip->set(colinIcons::instance().icon(cC, true),
                         tr("temperature # ")+ QString::number(first),
                         extend);
            break;
        case catcher::CatchedOrthoGlob:
            if(vS->toDraw() == Colin::drawBeam)
            {
                tooltip->set(colinIcons::instance().icon(cC, true),
                             tr("orthogonal! lenght = ") +
                             QString::number(sqrt(pow(globalMatrix().inverted().map(cursorpos).x()-tw->node(tw->lastObjectNode()).x(), 2) +
                                                  pow(globalMatrix().inverted().map(cursorpos).y()-tw->node(tw->lastObjectNode()).z(), 2)), 'f', 2),
                             "");
            }
            else
            {
                tooltip->set(colinIcons::instance().icon(cC, true),
                             tr("orthogonal! ") +
                             QString::number(sqrt(pow(globalMatrix().inverted().map(cursorpos).x()-tw->lastObjectClick().x(), 2) +
                                                  pow(globalMatrix().inverted().map(cursorpos).y()-tw->lastObjectClick().y(), 2))*PPREFIX/tw->scaleP(), 'f', 2)+unitSettings::instance().Peh(),
                             "");
            }
            break;
        case catcher::CatchedOrthoLokal:
            if(vS->toDraw() == Colin::drawBeam)
            {
		tooltip->set(colinIcons::instance().icon(cC, true),
			     tr("orthogonal! ") +
			     QString::number(sqrt(pow(globalMatrix().inverted().map(cursorpos).x()-tw->lastObjectClick().x(), 2)+
						  pow(globalMatrix().inverted().map(cursorpos).y()-tw->lastObjectClick().y(), 2)), 'f', 2) + "m",
			     "");
            }
            else
            {
                tooltip->set(colinIcons::instance().icon(cC, true),
                             tr("orthogonal! ") +
                             QString::number(sqrt(pow(globalMatrix().inverted().map(cursorpos).x()-tw->lastObjectClick().x(), 2)+
                                                  pow(globalMatrix().inverted().map(cursorpos).y()-tw->lastObjectClick().y(), 2))/tw->scaleP()*PPREFIX, 'f', 2) + " " + unitSettings::instance().Peh()+"/m )",
                             "");
            }
	    break;
        case catcher::CatchedBeamEnd:
            if(vS->toDraw().testFlag(Colin::drawJoint))
            {
                tooltip->set(colinIcons::instance().icon(vS->toDraw(), true),
                             tr("add joint @ ") +
                             ((aditional == 0)? tr("left") : tr("right"))+
                             tr("side of beam #")+
                             QString::number(first),
                             "");
            }
            else
            {
                tooltip->set(colinIcons::instance().icon(vS->toDraw(), true),
                             tr("add double load @ ") +
                             ((aditional == 0)? tr("left") : tr("right"))+
                             tr("side of beam #")+
                             QString::number(first),
                             tr("press Shift to draw moments!"));
            }
            break;
	}
        //set resultVisualWidget

        if(lastMousePosition != mapFromGlobal(QCursor::pos()))
        {
            switch(cC)
            {
            case catcher::CatchedBeam:
            case catcher::CatchedBeamEnd:
            case catcher::CatchedCrossing:
            case catcher::CatchedMiddle:
                resultVisualWidget::instance().setElement(catcher::CatchedBeam, first,
                                                          tw->beam(first).transform().inverted().map(globalMatrix().inverted().map(cursorpos)).x());
                break;
            case catcher::CatchedNode:
                resultVisualWidget::instance().setElement(catcher::CatchedNode, first);
                break;
            case catcher::CatchedTemp:
                resultVisualWidget::instance().setElement(catcher::CatchedTemp, first);
            case catcher::CatchedLoad:
            case catcher::CatchedLoadHotSpot:
                resultVisualWidget::instance().setElement(catcher::CatchedLoad, first);
                break;
            default:
                resultVisualWidget::instance().setElement(catcher::CatchedNothing, -1);
                break;
            }
            resultVisualWidget::instance().update();
        }
    }
    /*
    if(first<0)
    {
        if(vS->toDraw() == Colin::drawBeam && tw->lastObjectNode()>0)
        {
	    cursorpos = globalMatrix().inverted().map(cursorpos);   //WTF???????????
            cursorpos = globalMatrix().map(cursorpos);		    //SENSELESS BULLSHIT o_O
        }                                                           //NEED SOME SLEEP
    }*/


    QPointF globalpos = globalMatrix().inverted().map(cursorpos);

    //X.XXXXe-10 should be 0.00...
    if(cC == catcher::CatchedGrid)
        fuzzyNull(&globalpos);


    //draw the object witch is being painted at the moment
    drawCurrentObject(p, cursorpos);

    if(vS->toDraw() == Colin::drawNLoad)
    {/*
        if(tw->lastObjectNode()>0)
        {
            p->setPen(Qt::black);
            p->save();
            p->translate(globalMatrix().map(tw->node(tw->lastObjectNode()).toQPointF()));
            p->drawLine(-100, 0, 100, 0);
            p->drawLine(0, -100, 0, 100);
            p->restore();
        }*/
    }
    else if(vS->toDraw() == Colin::drawULoad ||
            vS->toDraw() == Colin::drawILoad ||
            vS->toDraw() == Colin::drawDLoad )
    {

    }
    else if(vS->toDraw() == Colin::drawTemp)
    {

    }
    else if(vS->toDraw() == Colin::drawBeam ||
            vS->toDraw() == Colin::drawNode )
    {

        //cursoraxes...
        p->setPen(Qt::black);
	p->setBrush(Qt::NoBrush);
        p->setRenderHint(QPainter::Antialiasing, false);


#ifndef QT_NO_DEBUG

        p->save();
        p->resetTransform();
        p->drawText(QRect(width()-100, height()-100, 100, 25), QString("%1 %2").arg(cursorpos.x()).arg(cursorpos.y()));
        p->drawText(QRect(width()-100, height()-75, 100, 25), QString("%1").arg(cC));
        p->drawText(QRect(width()-100, height()-50, 100, 25), QString("%1 %2").arg(first).arg(aditional));
        p->restore();
#endif //QT_NO_DEBUG



        p->drawLine(0, cursorpos.y(), width(), cursorpos.y());
        p->drawLine(cursorpos.x(), 0, cursorpos.x(), height());
        p->drawRect(cursorpos.x()-3, cursorpos.y()-3, 6, 6);

        QPoint po[5];

        QRectF boundingrect = p->boundingRect(QRectF(0, 0, 500, 1),
                                            QString("%1 %2").
                                            arg(globalpos.y()).
                                            arg(prefixM));

        po[0] = QPoint(-1, cursorpos.y()-boundingrect.height()-4);
        po[1] = QPoint(boundingrect.width()+2, cursorpos.y()-boundingrect.height()-4);
        po[2] = QPoint(boundingrect.width()+4, cursorpos.y()-boundingrect.height()-2);
        po[3] = QPoint(boundingrect.width()+4, cursorpos.y());
        po[4] = QPoint(-1, cursorpos.y());

        p->setPen(QPen(QColor(150, 150, 150), 1));
        p->setBrush(QColor(255, 255, 200));

        p->drawPolygon(po, 5);
        p->setPen(Qt::black);

        p->drawText(3, cursorpos.y()-3,
                   QString("%1 %2").
                   arg(globalpos.y()).
                   arg(prefixM));

        boundingrect = p->boundingRect(QRectF(0, 0, 500, 1),
                                      QString("%1 %2").
                                      arg(globalpos.x()).
                                      arg(prefixM));

        po[0] = QPoint(cursorpos.x(), -1);
        po[1] = QPoint(cursorpos.x()+boundingrect.width()+4, -1);
        po[2] = QPoint(cursorpos.x()+boundingrect.width()+4, boundingrect.height()+2);
        po[3] = QPoint(cursorpos.x()+boundingrect.width()+2, boundingrect.height()+4);
        po[4] = QPoint(cursorpos.x(), boundingrect.height()+4);

        p->setPen(QColor(150, 150, 150));
        p->drawPolygon(po, 5);

        p->setPen(Qt::black);
        p->drawText(QPoint(cursorpos.x()+ 2, boundingrect.height()+2),
                   QString("%1 %2").
                   arg(globalpos.x()).
                   arg(prefixM));
    }
}

void viewport::drawCurrentObject(QPainter *p, const QPointF &cursorpos)
{
    if(vS->toClip() == Colin::pasteRequest)
    {
        QTransform t(globalMatrix().m11(), 0, 0,
                     0, globalMatrix().m22(), 0,
                     cursorpos.x(), cursorpos.y(), 1);

        toPaste_->setScaleM(tw->scaleM());
        toPaste_->setScaleP(tw->scaleP());

        sPainter.drawStruct(*toPaste_, p, &t,
                            Colin::beam    |
                            Colin::node    |
                            Colin::nload   |
                            Colin::bearing |
                            Colin::joint   );
        return;
    }
    else if(vS->toClip() == Colin::copybasePointRequest ||
            vS->toClip() == Colin::cutbasePointRequest  )
    {
        return;
    }

    switch(vS->toDraw())
    {
    case Colin::drawBeam:
        if(tw->lastObjectNode()>-1)
        {
            QPointF oldpos = globalMatrix().map(tw->node(tw->lastObjectNode()).toQPointF());
            p->setPen(QPen(Qt::black, 1, Qt::DashLine));
            p->drawLine(oldpos, cursorpos);


/*
#ifdef QT_DEBUG
            QPointF xy = (cursorpos-oldpos)/globalMatrix().m11();
            double len = sqrt(pow(xy.x(),2)+pow(xy.x(), 2));
            double alp = atan2(xy.y(), xy.x());
            p->drawText(QRect(cursorpos.x()+20, cursorpos.y()+20,
                              300, 300),
                        QString("%1\n%2\n%3\n%4")
                        .arg(xy.x())
                        .arg(xy.y())
                        .arg(len)
                        .arg(alp));
            p->setPen(QPen(Qt::red));
            p->drawText(QRect(cursorpos.x()+20, cursorpos.y()+70,
                              300, 300),
                        QString("catch:%1")
                        .arg(tw->nearestNode(globalMatrix().inverted().map(cursorpos),
                                             20/globalMatrix().m11())));
#endif
*/
        }
        return;
    case Colin::drawSelec:
        if(this->lastMouseButtons == Qt::LeftButton)
        {
            QLinearGradient gradm(lastMousePosition, mapFromGlobal(QCursor::pos()));
            gradm.setColorAt(0, QColor(255, 138, 0, 100));
            gradm.setColorAt(1, QColor(120, 52, 75, 100));
            p->setPen(QColor(255, 138, 0));
            p->setBrush(QBrush(gradm));
            p->drawRect(QRectF(gradm.start(), gradm.finalStop()));
        }
        return;
    case Colin::drawZoomRect:
        if(this->lastMouseButtons == Qt::LeftButton)
        {
            QLinearGradient gradm(lastMousePosition, mapFromGlobal(QCursor::pos()));
            gradm.setColorAt(0, QColor(255, 138, 0, 100));
            gradm.setColorAt(1, QColor(120, 52, 75, 100));
            p->setPen(QColor(255, 138, 0));
            p->setBrush(QBrush(gradm));
            p->drawRect(QRectF(gradm.start(), gradm.finalStop()));
        }
        return;
    case Colin::drawDoubleLoad:
        if(tw->lastObjectBeam() >-1)
        {
            QPointF oldpos = tw->node(tw->lastObjectNode()).toQPointF();
            QPointF newpos = mapFromGlobal(QCursor::pos());
            newpos = globalMatrix().inverted().map(newpos);
	    QLineF beamL = tw->beam(tw->lastObjectBeam()).toQLineF();



	    wgv_load l;

	    if(tw->beam(tw->lastObjectBeam()).leftNodeI() == tw->lastObjectNode())
		l.setTyp(wgv_load::doubleLoadLeft);
	    else
		l.setTyp(wgv_load::doubleLoadRight);
	    l.setBeam(tw->lastObjectBeam());

	    if(qApp->keyboardModifiers().testFlag(Qt::ShiftModifier) ||
	       qApp->keyboardModifiers().testFlag(Qt::ControlModifier))
	    {
		double M = sqrt(pow(oldpos.x()-newpos.x(), 2)+pow(oldpos.y()-newpos.y(), 2))/tw->scaleM();
		if(oldpos.x()-newpos.x()>0)
		    l.setM(M);
		else
		    l.setM(-M);
	    }
	    else
	    {
		QTransform t = tw->beam(tw->lastObjectBeam()).transform();
		newpos =t.inverted().map(newpos);
		if(l.typ()==wgv_load::doubleLoadRight)
		    newpos -= QPointF(tw->beam(tw->lastObjectBeam()).l(), 0);
		newpos/=tw->scaleP();
		l.setPx(newpos.x());
		l.setPz(newpos.y());
	    }
	    sPainter.drawDoubleLoad(l, globalMatrix().map(beamL));
	    p->setPen(Qt::black);
        }
        return;
    case Colin::drawNLoad:
        if(tw->lastObjectNode()>-1)
        {
            QPointF oldpos = tw->node(tw->lastObjectNode()).toQPointF();
            QPointF newpos =  cursorpos;
            newpos = globalMatrix().inverted().map(newpos);

            wgv_load l;
            l.setBeam(tw->lastObjectNode());
            l.setPx((oldpos.x()-newpos.x())/tw->scaleP());
            l.setPz((oldpos.y()-newpos.y())/tw->scaleP());
            l.setTyp(wgv_load::nodeLoad);
            sPainter.drawLoad(l, globalMatrix().map(oldpos));
            p->setPen(Qt::black);

        }
        return;
    case Colin::drawMoment:
        if(tw->lastObjectNode()>-1)
        {
            wgv_load l;
            QPointF oldpos = tw->node(tw->lastObjectNode()).toQPointF();
            QPointF newpos = cursorpos;
            newpos = globalMatrix().inverted().map(newpos);
            double M = sqrt(pow(oldpos.x()-newpos.x(), 2)+pow(oldpos.y()-newpos.y(), 2))/tw->scaleM();
            if(oldpos.x()-newpos.x()>0)
                l.setM(M);
            else
                l.setM(-M);
            l.setTyp(wgv_load::moment);
            sPainter.drawMoment(l, globalMatrix().map(oldpos));
        }
        return;
    default:
        break;
    }
    if(vS->toDraw() == Colin::drawILoad ||
       vS->toDraw() == Colin::drawDLoad ||
       vS->toDraw() == Colin::drawULoad )
    {
        if(tw->lastObjectBeam()>-1)
        {
            QPointF load = tw->lastObjectClick()-globalMatrix().inverted().map(cursorpos);

            wgv_load l;
            l.setPx(load.x()/tw->scaleP());
            l.setPz(load.y()/tw->scaleP());
            l.setBeam(tw->lastObjectBeam());
            if(vS->toDraw() == Colin::drawILoad)
                l.setTyp(wgv_load::increasingLinearLoad);
            else if(vS->toDraw() == Colin::drawULoad)
                l.setTyp(wgv_load::uniformlyDistibutedLoad);
            else
                l.setTyp(wgv_load::decreasingLinearLoad);
            QLineF  bl = globalMatrix().map(tw->beam(tw->lastObjectBeam()).toQLineF());
	    sPainter.drawStLoad(l, &bl);
        }
    }

}

void viewport::zoomClick(const QPointF &p)
{
        QWheelEvent e(p.toPoint(), 2*(15.*8)*((vS->toDraw() == Colin::drawZoomIn)?1:-1), QApplication::mouseButtons(), QApplication::keyboardModifiers());

        wheelEvent(&e);
}
/*
void viewport::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Left)
    {

    }
    if(e->key() == Qt::Key_Right)
    {

    }
    if(e->key() == Qt::Key_Up)
    {

    }
    if(e->key() == Qt::Key_Down)
    {

    }

    QWidget::keyPressEvent(e);
}*/

void viewport::wheelEvent(QWheelEvent *e)
{

    if(viewPortSettings::instance().wheelMod(e->modifiers()) == Colin::u)
    {
        if(e->delta()>0)
            tw->setScaleU(zoomfactor*double(e->delta()/(15.*8))*tw->scaleU());
        else
            tw->setScaleU(-1.0/(zoomfactor*double(e->delta()/(15.*8)))*tw->scaleU());

        return;
    }
    if(viewPortSettings::instance().wheelMod(e->modifiers()) == Colin::N)
    {
        if(e->delta()>0)
            tw->setScaleP(zoomfactor*double(e->delta()/(15.*8))*tw->scaleP());
        else
            tw->setScaleP(-1.0/(zoomfactor*double(e->delta()/(15.*8)))*tw->scaleP());

        return;
    }
    if(viewPortSettings::instance().wheelMod(e->modifiers()) == Colin::M)
    {
        if(e->delta()>0)
            tw->setScaleM(zoomfactor*double(e->delta()/(15.*8))*tw->scaleM());
        else
            tw->setScaleM(-1.0/(zoomfactor*double(e->delta()/(15.*8)))*tw->scaleM());

        return;
    }
    if((globalMatrix().m11()<minZoomfactor && e->delta()<0) ||
       (globalMatrix().m11()>maxZoomfactor && e->delta()>0))
        return;

    QPointF po = globalMatrix().inverted().map(QPointF(e->pos()));

    int d = e->delta();
    d/=(15*8);
    if(e->delta()>0)
    {
        globalMatrix().scale(zoomfactor*double(e->delta()/(15*8)),
                             zoomfactor*double(e->delta()/(15*8)));
    }
    else
    {
        globalMatrix().scale(-1.0/(zoomfactor*double(e->delta()/(15*8))),
                             -1.0/(zoomfactor*double(e->delta()/(15*8))));
    }

    po = globalMatrix().inverted().map(QPointF(e->pos())) - po;
    globalMatrix().translate(po.x(),po.y());

    update();

}

void viewport::resizeEvent(QResizeEvent *)
{
    const int s = 20;
    maximizer->setGeometry(width()-s*2-4, 2, s, s);
    hider->setGeometry(width()-s-2, 2, s, s);
    allshower->setGeometry(width()-s-2, 2, s, s);

    /*
    double d = e->size().height()/static_cast<double>(e->oldSize().height())/2+
               e->size().width()/static_cast<double>(e->oldSize().width())/2;

    if(!(d > 100 || d< 0.01))
        globalMatrix().scale(d, d);
    update();
    */
}

void viewport::mousePressEvent(QMouseEvent *e)
{
    QPointF p = e->posF();
    if(vS->toClip() == Colin::pasteRequest)
    {
        vS->setClipBoard(Colin::noRequest);

        catcher::CatchCases cC = catcher::CatchedNode | catcher::CatchedGrid;
        int catched = catcher::instance().doYourWork(&p, &cC, globalMatrix(), 0, true);

        if(cC == catcher::CatchedNode)
        {
            tw->mergeWith(toPaste_, tw->node(catched).toQPointF());
        }
        else if(cC == catcher::CatchedGrid)
        {
            p = globalMatrix().inverted().map(p);
            if(qFuzzyCompare(p.x(), static_cast<int>(p.x())/1))
                p.setX(static_cast<int>(p.x())/1);
            if(qFuzzyCompare(p.y(), static_cast<int>(p.y())/1))
                p.setY(static_cast<int>(p.y())/1);
            tw->mergeWith(toPaste_, p);
        }
        else
        {
            tw->mergeWith(toPaste_, globalMatrix().inverted().map(p));
        }
        delete toPaste_;
        toPaste_ = 0;
        return;
    }
    else if(vS->toClip() == Colin::pasteLoadRequestBeam ||
            vS->toClip() == Colin::pasteLoadRequestNode)
    {
        catcher::CatchCases cC;

        if(vS->toClip() == Colin::pasteLoadRequestBeam)
            cC = catcher::CatchedBeam;
        else
            cC = catcher::CatchedNode;
        int catched = catcher::instance().doYourWork(&p, &cC, globalMatrix(), 0, true);

        if(cC != catcher::CatchedNothing)
        {
            wgv_load l = toPaste_->load(0);
            l.setBeam(catched);
            tw->addLoad(l);
            vS->setClipBoard(Colin::noRequest);
            delete toPaste_;
        }
        return;
    }
    else if(vS->toClip() == Colin::copybasePointRequest ||
            vS->toClip() == Colin::cutbasePointRequest)
    {

        //need a basePoint to copy or cut something
        catcher::CatchCases cC = catcher::CatchedNode | catcher::CatchedGrid;
        int catched = catcher::instance().doYourWork(&p, &cC, globalMatrix(), 0, true);

        if(cC == catcher::CatchedNode)
        {
            emit basePoint(tw->node(catched).toQPointF());
        }
        else if(cC == catcher::CatchedGrid)
        {
            p = globalMatrix().inverted().map(p);
            if(qFuzzyCompare(p.x(), static_cast<int>(p.x())/1))
                p.setX(static_cast<int>(p.x())/1);
            if(qFuzzyCompare(p.y(), static_cast<int>(p.y())/1))
                p.setY(static_cast<int>(p.y())/1);
            emit basePoint(p);
        }
        vS->setClipBoard(Colin::noRequest);
        return;
    }

    //if no copy&paste action is goning on do the normal stuff:



    lastMousePosition = e->pos();
    lastMouseButtons = e->button();



    //launch menus on rightclick:
    if(e->buttons().testFlag(Qt::RightButton))
    {
        catcher::CatchCases cC = catcher::CatchStdSelect;
        QPointF p = e->posF();
        int catched = catcher::instance().doYourWork(&p, &cC, globalMatrix(), 0, true);
        switch(cC)
        {
        case catcher::CatchedNode:
            launchMenuNode(catched);
            return;
        case catcher::CatchedBeam:
            launchMenuBeam(catched);
            return;
        case catcher::CatchedTemp:
            launchMenuTemp(catched);
            return;
        case catcher::CatchedLoad:
            launchMenuLoad(catched);
            return;
        default:
            return;
        }
    }


    //on leftmouseclick draw and move and do stuff...

    if((vS->toDraw() & Colin::drawBearing) == Colin::drawBearing)
    {
        addBearing(p);
        return;
    }
    switch(vS->toDraw())
    {
    case Colin::drawMove:
        setCursor(Qt::ClosedHandCursor);
        findMoveObject(p);
        return;
    case Colin::drawDoubleLoad:
        addDoubleLoad(p);
        return;
    case Colin::drawNode:
        addNode(p);
        return;
    case Colin::drawBeam:
        addBeam(p);
        return;
    case Colin::drawNLoad:
        addNLoad(p);
        return;
    case Colin::drawMoment:
        addNLoad(p);
        return;
    case Colin::drawILoad:
        addSLoad(p);
        return;
    case Colin::drawULoad:
        addSLoad(p);
        return;
    case Colin::drawDLoad:
        addSLoad(p);
        return;
    case Colin::drawTemp:
        addTemp(p);
        return;
    case Colin::drawZoomIn:
        zoomClick(p);
        return;
    case Colin::drawZoomOut:
        zoomClick(p);
        return;
    case Colin::drawJointN:
        addJoint(p);
        return;
    case Colin::drawJointQ:
        addJoint(p);
        return;
    case Colin::drawJointM:
        addJoint(p);
        return;
    default:
        return;
    }
}

void viewport::mouseReleaseEvent(QMouseEvent *e)
{
    lastMouseButtons = Qt::NoButton;

    QRect r;
    switch(vS->toDraw())
    {
    case Colin::drawMove:
        tw->setlastObjectBeam(-1);
        tw->setlastObjectNode(-1);
        tw->setlastObjectLoad(-1);
	movingObject = catcher::CatchedNothing;
        setCursor(Qt::OpenHandCursor);
	maximizer->setCursor(Qt::ArrowCursor);
	hider->setCursor(Qt::ArrowCursor);
	allshower->setCursor(Qt::ArrowCursor);
	selec->setCursor(Qt::ArrowCursor);
        return;
    case Colin::drawSelec:
        r.setLeft(qMin(lastMousePosition.x(), e->pos().x()));
        r.setRight(qMax(lastMousePosition.x(), e->pos().x()));
        r.setTop(qMin(lastMousePosition.y(), e->pos().y()));
        r.setBottom(qMax(lastMousePosition.y(), e->pos().y()));
        if(r.height() < 3 && r.width() < 3)
            selectNearest(e->posF());
        else
            selectRect(r);
        return;
    case Colin::drawZoomRect:
        r.setLeft(qMin(lastMousePosition.x(), e->pos().x()));
        r.setRight(qMax(lastMousePosition.x(), e->pos().x()));
        r.setTop(qMin(lastMousePosition.y(), e->pos().y()));
        r.setBottom(qMax(lastMousePosition.y(), e->pos().y()));
        zoomRect(r);
    default:
        return;
    }
}

void viewport::mouseMoveEvent(QMouseEvent *e)
{
    if(!hasFocus())
        setFocus(Qt::MouseFocusReason);

    if(lastMouseButtons == Qt::LeftButton)
    {
        if(vS->toDraw() == Colin::drawMove)
        {
            catcher::CatchCases cC;
            QPointF p = e->posF();
            int catched;
            switch(movingObject)
            {
            case catcher::CatchedNothing:
                translate(e->pos()-lastMousePosition);
                break;
            case catcher::CatchedLoad:
		if(tw->load(tw->lastObjectLoad()).typ() == wgv_load::uniformlyDistibutedLoad ||
		   tw->load(tw->lastObjectLoad()).typ() == wgv_load::increasingLinearLoad    ||
		   tw->load(tw->lastObjectLoad()).typ() == wgv_load::decreasingLinearLoad    )
                {
                    cC = catcher::CatchedBeam;
		    catched = catcher::instance().doYourWork(&p, &cC, globalMatrix(), 0, true);
                    if(cC == catcher::CatchedBeam)
                    {
			if(tw->load(tw->lastObjectLoad()).at() != catched)
			    tw->setPos(tw->lastObjectLoad(), catched);
                    }
                }
		else if(tw->load(tw->lastObjectLoad()).typ() == wgv_load::nodeLoad ||
			tw->load(tw->lastObjectLoad()).typ() == wgv_load::moment   )
                {
                    cC = catcher::CatchedNode;
		    catched = catcher::instance().doYourWork(&p, &cC, globalMatrix(), 0, true);
                    if(cC == catcher::CatchedNode)
                    {
			if(tw->load(tw->lastObjectLoad()).at() != catched)
			    tw->setPos(tw->lastObjectLoad(), catched);
                    }
                }
                break;
	    case catcher::CatchedLoadHotSpot:
		switch(tw->load(tw->lastObjectLoad()).typ())
                {
		case wgv_load::nodeLoad:
		    cC = catcher::CatchedOrthoGlob | catcher::CatchedGrid;
		    catcher::instance().doYourWork(&p, &cC, globalMatrix());
		    p = globalMatrix().inverted().map(p)
			-tw->node(tw->load(tw->lastObjectLoad()).at()).toQPointF();
		    break;
		case wgv_load::uniformlyDistibutedLoad:
                    cC = catcher::CatchedOrthoGlob | catcher::CatchedOrthoLokal | catcher::CatchedGrid;
		    catcher::instance().doYourWork(&p, &cC, globalMatrix());
		    p = globalMatrix().inverted().map(p)
                        //-tw->beam(tw->load(tw->lastObjectLoad()).at()).middleP();
                        -tw->lastObjectClick();
                    break;
                case wgv_load::increasingLinearLoad:
                    cC = catcher::CatchedOrthoGlob | catcher::CatchedOrthoLokal | catcher::CatchedGrid;
		    catcher::instance().doYourWork(&p, &cC, globalMatrix());
                    p = globalMatrix().inverted().map(p)
                        //-tw->beam(tw->load(tw->lastObjectLoad()).at()).rightNode().toQPointF();
                        -tw->lastObjectClick();
                        break;
                case wgv_load::decreasingLinearLoad:
                    cC = catcher::CatchedOrthoGlob | catcher::CatchedOrthoLokal | catcher::CatchedGrid;
		    catcher::instance().doYourWork(&p, &cC, globalMatrix());
                    p = globalMatrix().inverted().map(p)
                        //-tw->beam(tw->load(tw->lastObjectLoad()).at()).leftNode().toQPointF();
                        -tw->lastObjectClick();
                    break;
                default:
                    break;
		}
		p/= -tw->scaleP();
		tw->setPx(tw->lastObjectLoad(), p.x());
		tw->setPz(tw->lastObjectLoad(), p.y());
		break;
            case catcher::CatchedNode:
                cC = catcher::CatchedGrid;
                catcher::instance().doYourWork(&p, &cC, globalMatrix());
                fuzzyNull(&p);
                tw->moveNode(tw->lastObjectNode(), globalMatrix().inverted().map(p));
                break;
	    case catcher::CatchedTemp:
		cC = catcher::CatchedBeam;
		catched = catcher::instance().doYourWork(&p, &cC, globalMatrix(), 0, true);
		if(cC == catcher::CatchedBeam)
		{
		    if(tw->load(tw->lastObjectLoad()).at() != catched)
			tw->setPos(tw->lastObjectLoad(), catched);
		}
            default:
                break;
            }
        }
    }
    else
    {
        //whatever...
    }

    if(!vS->toDraw() == Colin::drawSelec && this->lastMouseButtons == Qt::LeftButton)
        lastMousePosition = e->pos();
    update();
}

void viewport::translate(const QPoint &p)
{
    globalMatrix().translate(p.x()/globalMatrix().m11(), p.y()/globalMatrix().m22());
    update();
}

void viewport::setDrawCursor(Colin::Actions a)
{
    if(a == Colin::drawMove)
    {
        setCursor(QCursor(Qt::OpenHandCursor));
    }
    else if(a == Colin::drawSelec)
    {
        setCursor(QCursor(Qt::ArrowCursor));
    }
    else if(a == Colin::drawBeam || a == Colin::drawNode)
    {
        QPixmap p(1, 1);
        p.fill(QColor(0, 0, 0, 0));
        setCursor(QCursor(p));
    }
    else if((a & Colin::drawBearing) == Colin::drawBearing ||
	    (a & Colin::drawJoint) == Colin::drawJoint     ||
	    a == Colin::drawNLoad  ||
	    a == Colin::drawILoad  ||
	    a == Colin::drawULoad  ||
	    a == Colin::drawDLoad  ||
	    a == Colin::drawMoment ||
            a == Colin::drawTemp   ||
            a == Colin::drawDoubleLoad)
    {
        int hotspot = 15;
        QPixmap cpix(hotspot+32, hotspot+32);
        cpix.fill(QColor(0, 0, 0, 0));
        QPainter p(&cpix);
        p.drawLine(hotspot, 0, hotspot, hotspot*2);
        p.drawLine(0, hotspot, hotspot*2, hotspot);
        p.drawRect(hotspot-3, hotspot-3, 6, 6);
        colinIcons::instance().icon(a).paint(&p, hotspot, hotspot, 32, 32);
	setCursor(QCursor(cpix, 15, 15));
    }
    else if(a == Colin::drawSelec)
    {
        setCursor(QCursor(Qt::ArrowCursor));
    }
    else if(a == Colin::drawZoomIn)
    {
        setCursor(QCursor(colinIcons::instance().icon(Colin::drawZoomIn).pixmap(QSize(32, 32)), 22, 10));
    }
    else if(a == Colin::drawZoomOut)
    {
        setCursor(QCursor(colinIcons::instance().icon(Colin::drawZoomOut).pixmap(QSize(32, 32)), 22, 10));
    }
    else if(a.testFlag(Colin::drawZoom))
    {
        setCursor(QCursor(colinIcons::instance().icon(Colin::drawZoom).pixmap(QSize(32, 32)), 22, 10));
    }
#ifndef QT_NO_DEBUG
    else
    {
        QTextStream debugS(stdout);
        debugS << "cursorChange! enumeration not handled: " << a << endl;
    }
#endif
    maximizer->setCursor(Qt::ArrowCursor);
    hider->setCursor(Qt::ArrowCursor);
    allshower->setCursor(Qt::ArrowCursor);
    selec->setCursor(Qt::ArrowCursor);

    if(a != Colin::drawTemp)
        termoValueWidget::deleteInstance();

    update();
}


void viewport::setClipCursor(Colin::clipboardAction a)
{
    if(a == Colin::noRequest)
    {
        setDrawCursor(vS->toDraw());
    }
    else if(a == Colin::copybasePointRequest ||
            a == Colin::cutbasePointRequest  ||
            a == Colin::pasteRequest         ||
            a == Colin::pasteLoadRequestBeam ||
            a == Colin::pasteLoadRequestNode )
    {
        setCursor(QCursor(Qt::CrossCursor));
        update();
    }
    maximizer->setCursor(Qt::ArrowCursor);
    hider->setCursor(Qt::ArrowCursor);
    allshower->setCursor(Qt::ArrowCursor);
    selec->setCursor(Qt::ArrowCursor);

}

void viewport::findMoveObject(QPointF p)
{
    catcher::CatchCases cC(catcher::CatchStdMove);
    if((vS->elements(id()) & Colin::nload) != Colin::nload)  //dont move around loads when they aren't visible
    {
        cC = cC & (~catcher::CatchedLoad);
        cC = cC & (~catcher::CatchedLoadHotSpot);
        cC = cC & (~catcher::CatchedTemp);
    }
    int catched = catcher::instance().doYourWork(&p, &cC, globalMatrix(), 0, true);
    if(cC != catcher::CatchedNothing)
                tw->setlastObjectClick(globalMatrix().inverted().map(p));
    movingObject = cC;
    QPixmap movingPix(QSize(3*32, 32));
    movingPix.fill(QColor(0, 0, 0, 0));
    QPainter painter(&movingPix);
    colinIcons::instance().icon(Colin::drawMove).paint(&painter, 32, 0, 32, 32);
    switch(cC)
    {
    case catcher::CatchedNode:
        tw->setlastObjectNode(catched);
        colinIcons::instance().icon(Colin::drawNode).paint(&painter, 64, 0, 32, 32);
        break;
    //case catcher::CatchedBeam:
    //    tw->setlastObjectBeam(catched);
    //    colinIcons::instance().icon(Colin::drawBeam).paint(&painter, 64, 0, 32, 32);
    //    break;
    case catcher::CatchedLoad:
	tw->setlastObjectLoad(catched);
	colinIcons::instance().icon(tw->load(catched).typ()).paint(&painter, 64, 0, 32, 32);
        break;
    case catcher::CatchedLoadHotSpot:
	tw->setlastObjectLoad(catched);
	if(tw->load(catched).typ()==wgv_load::nodeLoad)
	{
	    tw->setlastObjectClick(tw->node(tw->load(catched).at()).toQPointF());
	    tw->setlastObjectNode(tw->load(catched).at());
	}
	else if(tw->load(catched).typ()==wgv_load::uniformlyDistibutedLoad)
	{
	    tw->setlastObjectClick(tw->load(tw->lastObjectLoad()).shape().at(0)/2+
				   tw->load(tw->lastObjectLoad()).shape().at(3)/2);
	    tw->setlastObjectBeam(tw->load(catched).at());
	}
	else if(tw->load(catched).typ()==wgv_load::increasingLinearLoad)
	{
	    tw->setlastObjectClick(tw->load(tw->lastObjectLoad()).shape().at(3));
	    tw->setlastObjectBeam(tw->load(catched).at());
	}
	else if(tw->load(catched).typ()==wgv_load::decreasingLinearLoad)
	{
	    tw->setlastObjectClick(tw->load(tw->lastObjectLoad()).shape().at(0));
	    tw->setlastObjectBeam(tw->load(catched).at());
	}
	colinIcons::instance().icon(tw->load(catched).typ()).paint(&painter, 64, 0, 32, 32);
        break;
    case catcher::CatchedTemp:
	tw->setlastObjectLoad(catched);
	colinIcons::instance().icon(tw->load(catched).typ()).paint(&painter, 64, 0, 32, 32);
	break;
    default:
        Q_ASSERT(true);
        break;
    }
    if(cC!=catcher::CatchedNothing)
    {
        //QCursor cursor(Qt::ClosedHandCursor);
        //painter.drawPixmap(0, 0, cursor.pixmap().height(), cursor.pixmap().height(), cursor.pixmap());
        setCursor(QCursor(movingPix));
    }
}

void viewport::addNode(QPointF p)
{
    catcher::CatchCases cC(catcher::CatchStdNode);
    int aditional;
    int catched = catcher::instance().doYourWork(&p, &cC, globalMatrix(), &aditional);
    p = globalMatrix().inverted().map(p);
    if(cC == catcher::CatchedGrid)
    fuzzyNull(&p);


    if(cC == catcher::CatchedGrid)
    {
        tw->addNode(wgv_node(p.x(), p.y()));
    }
    else if(cC == catcher::CatchedBeam || cC == catcher::CatchedMiddle)
    {
        tw->beginScript(tr("insert node into beam"));           //Script BEGIN
        int newNode = tw->addNode(wgv_node(p.x(), p.y()));      //Script
        wgv_beam s(tw->beam(catched));                          //Script
        tw->setRight(catched, newNode);                         //Script
        s.setLeftNode(newNode);                                 //Script
        tw->setJoint(catched, 3, false);                        //Script
        tw->setJoint(catched, 4, false);                        //Script
        tw->setJoint(catched, 5, false);                        //Script
        s.setJoint(0, false);                                   //Script
        s.setJoint(1, false);                                   //Script
        s.setJoint(2, false);                                   //Script
        tw->addBeam(s);                                         //Script
        tw->endScript();                                        //Script END
    }
    else if(cC == catcher::CatchedNothing)
    {
        tw->addNode(wgv_node(p.x(), p.y()));
    }
    else if(cC == catcher::CatchedCrossing)
    {
        tw->beginScript(tr("insert node into beam"));           //Script BEGIN
        int newNode = tw->addNode(wgv_node(p.x(), p.y()));      //Script
        wgv_beam s(tw->beam(catched));                          //Script
        wgv_beam s2(tw->beam(aditional));                       //Script
        tw->setRight(catched, newNode);                         //Script
        tw->setRight(aditional, newNode);                       //Script
        s.setLeftNode(newNode);                                 //Script
        s2.setLeftNode(newNode);                                //Script
        tw->setJoint(catched, 3, false);                        //Script
        tw->setJoint(catched, 4, false);                        //Script
        tw->setJoint(catched, 5, false);                        //Script
        tw->setJoint(aditional, 3, false);                      //Script
        tw->setJoint(aditional, 4, false);                      //Script
        tw->setJoint(aditional, 5, false);                      //Script
        s.setJoint(0, false);                                   //Script
        s.setJoint(1, false);                                   //Script
        s.setJoint(2, false);                                   //Script
        s2.setJoint(0, false);                                  //Script
        s2.setJoint(1, false);                                  //Script
        s2.setJoint(2, false);                                  //Script
        tw->addBeam(s);                                         //Script
        tw->addBeam(s2);                                        //Script
        tw->endScript();                                        //Script END
    }
}

void viewport::addBeam(QPointF p)
{
    int aditional;
    catcher::CatchCases cC = catcher::CatchStdBeam;
    int nextObject = catcher::instance().doYourWork(&p, &cC, globalMatrix(), &aditional);
    p = globalMatrix().inverted().map(p);
    if(cC == catcher::CatchedGrid)
        fuzzyNull(&p);
    if(tw->lastObjectNode() == -1)
    {
        if(cC == catcher::CatchedNode)
            tw->setlastObjectNode(nextObject);
        else if(cC == catcher::CatchedBeam || cC == catcher::CatchedMiddle)
        {
            tw->beginScript(tr("insert node into beam"));         //Script BEGIN
            int newNode = tw->addNode(wgv_node(p.x(), p.y()));
            wgv_beam s(tw->beam(nextObject));
            tw->setRight(nextObject, newNode);
            s.setLeftNode(newNode);
            tw->setJoint(nextObject, 3, false);
            tw->setJoint(nextObject, 4, false);
            tw->setJoint(nextObject, 5, false);
            s.setJoint(0, false);
            s.setJoint(1, false);
            s.setJoint(2, false);
            tw->addBeam(s);
            tw->setlastObjectNode(newNode);
            tw->endScript();                                        //Script END
	}
	else if(cC == catcher::CatchedCrossing)
	{
	    tw->beginScript(tr("insert node into beam"));
            int newNode = tw->addNode(wgv_node(p.x(), p.y()));      //Script BEGIN
	    wgv_beam s(tw->beam(nextObject));
	    wgv_beam s2(tw->beam(aditional));
	    tw->setRight(nextObject, newNode);
	    tw->setRight(aditional, newNode);
	    s.setLeftNode(newNode);
            s2.setLeftNode(newNode);
            tw->setJoint(nextObject, 3, false);
            tw->setJoint(nextObject, 4, false);
            tw->setJoint(nextObject, 5, false);
            tw->setJoint(aditional, 3, false);
            tw->setJoint(aditional, 4, false);
            tw->setJoint(aditional, 5, false);
            s.setJoint(0, false);
            s.setJoint(1, false);
            s.setJoint(2, false);
            s2.setJoint(0, false);
            s2.setJoint(1, false);
            s2.setJoint(2, false);
            tw->addBeam(s);
	    tw->addBeam(s2);
	    tw->setlastObjectNode(newNode);
            tw->endScript();                                        //Script END
	}
        else
            tw->setlastObjectNode(tw->addNode(wgv_node(p.x(), p.y())));
    }
    else
    {
        int lastObject = tw->lastObjectNode();

        if(cC == catcher::CatchedBeam || cC == catcher::CatchedMiddle)
        {
            tw->beginScript(tr("insert node into beam"));                      //Script BEGIN
            wgv_beam s(tw->beam(nextObject));
            int newNode = tw->addNode(wgv_node(p.x(), p.y()));
            tw->setRight(nextObject, newNode);
            s.setLeftNode(newNode);
            tw->addBeam(s);
            nextObject = newNode;
	}
	else if(cC == catcher::CatchedCrossing)
	{
            tw->beginScript(tr("insert node into beam"));                           //Script BEGIN
	    int newNode = tw->addNode(wgv_node(p.x(), p.y()));
	    wgv_beam s(tw->beam(nextObject));
	    wgv_beam s2(tw->beam(aditional));
	    tw->setRight(nextObject, newNode);
	    tw->setRight(aditional, newNode);
	    s.setLeftNode(newNode);
	    s2.setLeftNode(newNode);
            tw->setJoint(nextObject, 3, false);
            tw->setJoint(nextObject, 4, false);
            tw->setJoint(nextObject, 5, false);
            tw->setJoint(aditional, 3, false);
            tw->setJoint(aditional, 4, false);
            tw->setJoint(aditional, 5, false);
            s.setJoint(0, false);
            s.setJoint(1, false);
            s.setJoint(2, false);
            s2.setJoint(0, false);
            s2.setJoint(1, false);
            s2.setJoint(2, false);
	    tw->addBeam(s);
	    tw->addBeam(s2);
            nextObject = newNode;
	}
        else if(cC != catcher::CatchedNode)
        {

            tw->beginScript(tr("add node and beam"));                           //Script BEGIN
            nextObject = tw->addNode(wgv_node(p.x(), p.y()));
        }
        tw->addBeam(wgv_beam(tw,
                               lastObject,
                               nextObject,
                               LIB.currentMaterial(), LIB.currentProfile()));

        if(cC != catcher::CatchedNode)
            tw->endScript();                                                    //Script END
        tw->setlastObjectNode(nextObject);
    }
}

void viewport::addBearing(QPointF p)
{
    p = globalMatrix().inverted().map(p);
    int i = tw->nearestNode(p, 20/globalMatrix().m11());

    if(i!=-1)
    {
        wgv_node n = tw->node(i);
        n.setBearing(wgv_bearing(vS->toDraw()));
        tw->editNode(i, n);
    }
}

void viewport::addNLoad(QPointF p)
{
    if(tw->lastObjectNode()<0)
    {
        catcher::CatchCases cC = catcher::CatchStdLoad;
        int nextObject = catcher::instance().doYourWork(&p, &cC, globalMatrix(), 0, true);
        if(cC == catcher::CatchedNode)
        {
            tw->setlastObjectNode(nextObject);
            tw->setlastObjectClick(tw->node(tw->lastObjectNode()).toQPointF());
        }
        else if(cC == catcher::CatchedBeam || cC == catcher::CatchedMiddle)
        {
            p = globalMatrix().inverted().map(p);
            tw->beginScript(tr("add nodal force on beam"));                           //Script BEGIN
            int newNode = tw->addNode(wgv_node(p.x(), p.y()));
            wgv_beam s(tw->beam(nextObject));
            tw->setRight(nextObject, newNode);
            s.setLeftNode(newNode);

            tw->setJoint(nextObject, 3, false);                        //Script
            tw->setJoint(nextObject, 4, false);                        //Script
            tw->setJoint(nextObject, 5, false);                        //Script
            s.setJoint(0, false);                                   //Script
            s.setJoint(1, false);                                   //Script
            s.setJoint(2, false);                                   //Script

            tw->addBeam(s);
            tw->setlastObjectNode(newNode);
            tw->setlastObjectClick(tw->node(tw->lastObjectNode()).toQPointF());
            tw->endScript();                                                //Script END
        }
    }
    else
    {
        catcher::CatchCases cC = catcher::CatchedOrthoGlob | catcher::CatchedGrid;
        catcher::instance().doYourWork(&p, &cC, globalMatrix());
        QPointF oldpos = tw->node(tw->lastObjectNode()).toQPointF();
        p = globalMatrix().inverted().map(p);
        wgv_load l;
        l.setBeam(tw->lastObjectNode());
        if(vS->toDraw() == Colin::drawNLoad)
        {
            l.setPx((oldpos.x()-p.x())/tw->scaleP());
            l.setPz((oldpos.y()-p.y())/tw->scaleP());
            l.setTyp(wgv_load::nodeLoad);
        }
        else
        {
	    double M = sqrt(pow(oldpos.x()-p.x(), 2)+pow(oldpos.y()-p.y(), 2))/tw->scaleM();
            if(oldpos.x()-p.x()>0)
                l.setM(M);
            else
                l.setM(-M);
            l.setTyp(wgv_load::moment);
        }
        tw->setlastObjectNode(-1);
        tw->addLoad(l);
    }
}

void viewport::addSLoad(QPointF p)
{
    catcher::CatchCases cC = catcher::CatchStdSLoad;
    int nextObject = catcher::instance().doYourWork(&p, &cC, globalMatrix(), 0, true);
    if(tw->lastObjectBeam()<0)
    {
        if(cC == catcher::CatchedBeam)
            tw->setlastObjectBeam(nextObject);

        tw->setlastObjectClick(globalMatrix().inverted().map(p));
    }
    else
    {
        QPointF load = tw->lastObjectClick()-globalMatrix().inverted().map(p);
        fuzzyNull(&load);

        wgv_load l;
        l.setPx(load.x()/tw->scaleP());
        l.setPz(load.y()/tw->scaleP());
        l.setBeam(tw->lastObjectBeam());
        if(vS->toDraw() == Colin::drawILoad)
            l.setTyp(wgv_load::increasingLinearLoad);
        else if(vS->toDraw() == Colin::drawULoad)
            l.setTyp(wgv_load::uniformlyDistibutedLoad);
        else
            l.setTyp(wgv_load::decreasingLinearLoad);
        tw->addLoad(l);
        tw->setlastObjectBeam(-1);
    }
}


void viewport::addTemp(QPointF p)
{
    catcher::CatchCases cC = catcher::CatchStdSLoad;
    int nextObject = catcher::instance().doYourWork(&p, &cC, globalMatrix(), 0, true);
    if(cC == catcher::CatchedBeam)
    {
        tw->setlastObjectBeam(nextObject);
        termoValueWidget *termo = new termoValueWidget(this);
        termo->show();
        termo->move(globalMatrix().map(
                                        tw->beam(tw->lastObjectBeam()).leftNode().toQPointF()/2+
                                        tw->beam(tw->lastObjectBeam()).rightNode().toQPointF()/2
                                        ).toPoint()-
                                QPoint(25, 120));
        connect(termo,          SIGNAL(chosen(double,int)),
                this,           SLOT(newTemp(double,int)));
    }
}


void viewport::addJoint(QPointF p)
{
    catcher::CatchCases cC = catcher::CatchStdJoint;
    int position;
    int beamnr = catcher::instance().doYourWork(&p, &cC, globalMatrix(), &position, true);
    if(cC == catcher::CatchedBeamEnd)
    {
        position *= 3;
        if(vS->toDraw() == Colin::drawJointQ)
            position+=1;
        if(vS->toDraw() == Colin::drawJointM)
            position+=2;
        tw->setJoint(beamnr, position, true);
    }
    else if( cC == catcher::CatchedMiddle || cC == catcher::CatchedBeam)
    {
        p = globalMatrix().inverted().map(p);
        tw->beginScript(tr("added hinge on beam"));                         //Script BEGIN
        int newNode = tw->addNode(wgv_node(p.x(), p.y()));
        wgv_beam s(tw->beam(beamnr));
        tw->setRight(beamnr, newNode);
        s.setLeftNode(newNode);

        tw->setJoint(beamnr, 3, false);                        //Script
        tw->setJoint(beamnr, 4, false);                        //Script
        tw->setJoint(beamnr, 5, false);                        //Script
        if(vS->toDraw() != Colin::drawJointN)
            s.setJoint(0, false);                                   //Script
        else
            s.setJoint(0, true);
        if(vS->toDraw() != Colin::drawJointQ)
            s.setJoint(1, false);                                   //Script
        else
            s.setJoint(1, true);
        if(vS->toDraw() != Colin::drawJointM)
            s.setJoint(2, false);                                   //Script
        else
            s.setJoint(2, true);
        tw->addBeam(s);
        tw->endScript();

    }
}

void viewport::addDoubleLoad(QPointF p)
{
    if(tw->lastObjectBeam()<0)
    {
        catcher::CatchCases cC = catcher::CatchStdDLoad;
        int position = 0;
        int beamNr = catcher::instance().doYourWork(&p, &cC, globalMatrix(), &position, true);
        if(cC == catcher::CatchedBeamEnd)
        {
            tw->setlastObjectBeam(beamNr);
            tw->setlastObjectNode((position == 0)? tw->beam(beamNr).leftNodeI() : tw->beam(beamNr).rightNodeI());
            tw->setlastObjectClick(tw->node(tw->lastObjectNode()).toQPointF());
        }
    }
    else
    {
	QPointF oldpos = tw->node(tw->lastObjectNode()).toQPointF();
	p = globalMatrix().inverted().map(p);



	wgv_load l;

	if(tw->beam(tw->lastObjectBeam()).leftNodeI() == tw->lastObjectNode())
	    l.setTyp(wgv_load::doubleLoadLeft);
	else
	    l.setTyp(wgv_load::doubleLoadRight);
	l.setBeam(tw->lastObjectBeam());

	if(qApp->keyboardModifiers().testFlag(Qt::ShiftModifier) ||
	   qApp->keyboardModifiers().testFlag(Qt::ControlModifier))
	{
	    double M = sqrt(pow(oldpos.x()-p.x(), 2)+pow(oldpos.y()-p.y(), 2))/tw->scaleM();
	    if(oldpos.x()-p.x()>0)
		l.setM(M);
	    else
		l.setM(-M);
	}
	else
	{
	    QTransform t = tw->beam(tw->lastObjectBeam()).transform();
	    p =t.inverted().map(p);
	    if(l.typ()==wgv_load::doubleLoadRight)
		p -= QPointF(tw->beam(tw->lastObjectBeam()).l(), 0);
	    p/=tw->scaleP();
	    l.setPx(p.x());
	    l.setPz(p.y());
	}
	tw->addLoad(l);
	tw->setlastObjectBeam(-1);
	tw->setlastObjectNode(-1);
    }
}

void viewport::launchMenuNode(const int &i)
{
    nodeMenu::instance().set(i);
    nodeMenu::instance().show();
    nodeMenu::instance().move(QCursor::pos());
}

void viewport::launchMenuBeam(const int &i)
{
    beamMenu::instance().set(i);
    beamMenu::instance().show();
    beamMenu::instance().move(QCursor::pos());
}

void viewport::launchMenuLoad(const int &i)
{
    if(filelist::instance().currentFile()->load(i).typ() == wgv_load::uniformlyDistibutedLoad ||
       filelist::instance().currentFile()->load(i).typ() == wgv_load::decreasingLinearLoad    ||
       filelist::instance().currentFile()->load(i).typ() == wgv_load::increasingLinearLoad    )
    {
        loadMenu::instance().set(i);
        loadMenu::instance().show();
        loadMenu::instance().move(QCursor::pos());
    }
    else if(filelist::instance().currentFile()->load(i).typ() == wgv_load::nodeLoad)
    {
        nloadMenu::instance().set(i);
        nloadMenu::instance().popup(QCursor::pos());
    }/* not possible to click till now
    else if(filelist::instance().currentFile()->load(i).typ() == wgv_load::moment)
    {
        momentMenu::instance().set(i);
        momentMenu::instance().popup(QCursor::pos());
    }*/
}

void viewport::launchMenuTemp(const int &i)
{
    tempMenu::instance().set(i);
    tempMenu::instance().show();
    tempMenu::instance().move(QCursor::pos());
}

void viewport::selectNearest(QPointF p)
{
    catcher::CatchCases cC = catcher::CatchStdSelect;
    int toSelect = catcher::instance().doYourWork(&p, &cC, globalMatrix(), 0, true);
    if((QApplication::keyboardModifiers() & Qt::ControlModifier) != Qt::ControlModifier ||
       (QApplication::keyboardModifiers() & Qt::ShiftModifier) != Qt::ShiftModifier     )
    {
        blockSignals(true);
        tw->deselectAll();
        blockSignals(false);
    }
    switch(cC)
    {
    case catcher::CatchedNode:
        tw->selectNode(toSelect, true);
        if(tw->node(toSelect).hasbearing())
            tw->selectBearing(toSelect, true);
        break;
    case catcher::CatchedBeam:
        tw->selectBeam(toSelect, true);
        break;
    case catcher::CatchedLoad:
        tw->selectLoad(toSelect, true);
        break;
    case catcher::CatchedTemp:
        tw->selectLoad(toSelect, true);
        break;
    default:
        break;
    }
}

void viewport::selectRect(const QRect &r)
{
    bool ctrl = false;
    if((QApplication::keyboardModifiers() & Qt::ControlModifier) == Qt::ControlModifier||
       (QApplication::keyboardModifiers() & Qt::ShiftModifier) == Qt::ShiftModifier     )
        ctrl = true;
    tw->selectRect(globalMatrix().inverted().mapRect(QRectF(r)), ctrl);
}

void viewport::zoomRect(const QRectF &r, bool keepSpace)
{
    QRectF globR = globalMatrix().inverted().mapRect(r);
    double zoom;
    if(keepSpace)
    {
        zoom = qMin((width()-60)/globR.width(),
                    (height()-60)/globR.height());
    }
    else
    {
        zoom = qMin((width())/globR.width(),
                    (height())/globR.height());
    }
    QPointF dP = -globR.topLeft()*zoom;
    dP += QPointF(width()-globR.width()*zoom, height()-globR.height()*zoom)/2+
          (keepSpace ? QPointF(0, 0) : QPointF(30, 30));

    if (zoom > minZoomfactor && zoom <maxZoomfactor)
    {
        globalMatrix() = QTransform(zoom,      0,             0,
                                    0,         zoom,          0,
                                    dP.x(),    dP.y(),        1);
    }
    update();
}


void viewport::newTemp(double val, int type)
{
    Q_ASSERT(tw->lastObjectBeam() > -1);

    wgv_load temp;
    if(type == 1)
    {
        temp.setPx(val);
        temp.setTyp(wgv_load::tempChange);
    }
    else
    {
        temp.setPz(val);
        temp.setTyp(wgv_load::tempDiffrence);
    }
    temp.setBeam(tw->lastObjectBeam());
    tw->addLoad(temp);
    tw->setlastObjectBeam(-1);
}
