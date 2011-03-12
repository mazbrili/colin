#include "catcher.h"

#include "viewportsettings.h"

catcher *catcher::instance_ = NULL;

catcher::catcher()
{
   readSettings();

   allOff = false;
}

void catcher::setCatchOn(const catcher::CatchCases &c, const bool &on)
{
    if(on)
        isOn |= c;
    else
        isOn = isOn & (~c);
}

void catcher::setCatchRange(const int &r)
{
    catchRange_ = r;
}

bool catcher::isCatchOn(const catcher::CatchCases &c) const
{
    return ( (isOn & c) == c) ? true : false;
}

const int &catcher::catchRange() const
{
    return catchRange_;
}

catcher::CatchCases catcher::CasesForAction(const Colin::Actions &a)const
{
    if((a & Colin::drawBearing) == Colin::drawBearing)
        return catcher::CatchStdBearing;

    if((a & Colin::drawJoint) == Colin::drawJoint)
        return catcher::CatchStdJoint;

    if(a == Colin::drawNLoad    )
        return catcher::CatchStdLoad;

    if(a == Colin::drawULoad    ||
       a == Colin::drawILoad    ||
       a == Colin::drawDLoad    ||
       a == Colin::drawTemp     )
        return catcher::CatchStdSLoad;

    if(a == Colin::drawNode)
        return catcher::CatchStdNode;

    if(a == Colin::drawBeam)
        return catcher::CatchStdBeam;

    if(a == Colin::drawMove)
        return catcher::CatchStdMove;

    if(a == Colin::drawSelec)
        return catcher::CatchStdSelect;

    if(a == Colin::drawDoubleLoad)
        return catcher::CatchStdDLoad;


    return catcher::CatchedNothing;
}

int catcher::doYourWork(QPointF *mouse, CatchCases *c,
                        const QTransform &t, int *adi, bool ignoreOff)
{
    const wgv_tw &tw = *filelist::instance().currentFile();
    if(!ignoreOff)
    {
        if(allOff)
        {
            *c = catcher::CatchedNothing;
            return -1;
        }
        *c = (*c & isOn);
    }
    if(*c == catcher::CatchedNothing)
    {
        *c = catcher::CatchedNothing;
        return -1;
    }
    int i;

    //nodes
    if((*c & CatchedNode) == CatchedNode)
    {
        i = tw.nearestNode(t.inverted().map(*mouse), catchRange_/t.m11());
        if(i>-1)
        {
            *c = CatchedNode;
            *mouse = QPointF(t.map(tw.node(i).toQPointF()));
            return i;
        }
    }

    //crossing beams
    if(c->testFlag(CatchedCrossing))
    {
	int i,
	    j;
	QPointF crossing;
        tw.crossingBeams(t.inverted().map(*mouse), catchRange_/t.m11(), &i, &j, &crossing);
	if(i>-1)
	{
	    *mouse = t.map(crossing);
	    *c = CatchedCrossing;

	    Q_ASSERT(adi != NULL);

	    *adi = j;
	    return i;
	}
    }

    //load hotspot
    if((*c & CatchedLoadHotSpot) == CatchedLoadHotSpot)
    {
	QPointF dp;
	for(int m=0; m<tw.load_n(); m++)
	{
	    if(tw.load(m).typ()==wgv_load::nodeLoad)
	    {
		dp = tw.node(tw.load(m).at()).toQPointF()
		     -QPointF(tw.load(m).Px(), tw.load(m).Pz())*tw.scaleP();
	    }
	    else if(tw.load(m).typ()==wgv_load::decreasingLinearLoad)
	    {
		dp = tw.load(m).shape().at(1);
	    }
	    else if(tw.load(m).typ()==wgv_load::uniformlyDistibutedLoad)
	    {
		dp = tw.load(m).shape().at(1)/2+tw.load(m).shape().at(2)/2;
	    }
	    else if(tw.load(m).typ()==wgv_load::increasingLinearLoad)
	    {
		dp = tw.load(m).shape().at(2);
	    }

	    if(fabs((dp-t.inverted().map(*mouse)).manhattanLength()) < catchRange_/t.m11())
	    {
                *mouse = dp;
		*c = CatchedLoadHotSpot;
		return m;
	    }
	}
    }

    //beams and middle of beams
    if((*c & CatchedBeam) == CatchedBeam     ||
       (*c & CatchedMiddle) == CatchedMiddle )
    {
        QPointF n;
        i = tw.nearestBeam(t.inverted().map(*mouse), catchRange_/t.m11(), &n);
        if(i>-1)
        {
            if((*c & CatchedMiddle) == CatchedMiddle)
            {
                QPointF middle = t.map((tw.beam(i).leftNode().toQPointF()+tw.beam(i).rightNode().toQPointF())/2);
                if((middle-*mouse).manhattanLength()<catchRange_)
                {
                    *mouse = middle;
                    *c = CatchedMiddle;
                    return i;
                }
            }
            if((*c & CatchedBeam) == CatchedBeam)
            {
                *mouse = t.map(n);
                *c = CatchedBeam;
                return i;
            }
        }
    }

    //end of beams (for joints)
    if((*c & CatchedBeamEnd) == CatchedBeamEnd)
    {
        Q_ASSERT(adi != NULL);
        i = tw.nearestNode(t.inverted().map(*mouse), catchRange_/t.m11()*2);
        if(i>-1)
        {
            double angle = atan2(t.inverted().map(*mouse).y()-tw.node(i).toQPointF().y(),
                                 t.inverted().map(*mouse).x()-tw.node(i).toQPointF().x());
            double dangle = M_PI*2;
            int nearestbeamend = -1;
            bool left = true;
            for(int j=0; j<tw.beam_n(); j++)
            {
                if(tw.beam(j).leftNodeI() == i)
                {
                    if(fabs((tw.beam(j).angle()-angle))<dangle)
                    {
                        dangle = fabs(tw.beam(j).angle()-angle);
                        nearestbeamend = j;
                        left = true;
                    }
                }
                if(tw.beam(j).rightNodeI() == i)
                {
                    double angle_ = tw.beam(j).angle()+M_PI;
                    if(angle_>2*M_PI)
                        angle -= 2*M_PI;
                    if(fabs(angle_-angle)<dangle)
                    {
                        dangle = fabs(angle_-angle);
                        nearestbeamend = j;
                        left = false;
                    }
                }
            }
            if(nearestbeamend>-1)
            {
                *c = CatchedBeamEnd;
                if(left)
                    *adi = 0;
                else
                    *adi = 1;
                return nearestbeamend;
            }
        }
    }

    //grid
    if((*c & CatchedGrid) == CatchedGrid)
    {
        if(!(viewPortSettings::instance().toDraw() == Colin::drawNLoad      ||
             viewPortSettings::instance().toDraw() == Colin::drawMoment     ||
             viewPortSettings::instance().toDraw() == Colin::drawULoad      ||
             viewPortSettings::instance().toDraw() == Colin::drawDLoad      ||
             viewPortSettings::instance().toDraw() == Colin::drawILoad      ||
             viewPortSettings::instance().toDraw() == Colin::drawDoubleLoad ||
             (viewPortSettings::instance().toDraw() == Colin::drawMove && (tw.lastObjectLoad() > -1))))
        {
            if(grid::instance().gridCatch(mouse, t, catchRange_))
            {
                *c = CatchedGrid;
                return 1;
            }
        }
    }

    //temp
    if((*c & CatchedTemp) == CatchedTemp)
    {
	for(i=0; i < tw.load_n(); i++)
        {
	    if(tw.load(i).typ() != wgv_load::tempChange    &&
	       tw.load(i).typ() != wgv_load::tempDiffrence )
                continue;
            QLineF line = tw.beam(tw.load(i).at()).toQLineF();
            line = t.map(line);
            QRect boundingRect((line.p1()/2+line.p2()/2).toPoint()-QPoint(20, 100), QSize(40, 100));
            if(boundingRect.contains(mouse->x(), mouse->y()))
            {
                *c = CatchedTemp;
                return i;
            }
        }
    }

    //load
    if((*c & CatchedLoad) == CatchedLoad)
    {
        i = tw.nearestLoad(t.inverted().map(*mouse), catchRange_/t.m11());
        if(i>-1)
        {
            *c = CatchedLoad;
            return i;
        }
    }

    //Ortho
    if((*c & CatchedOrthoGlob) == CatchedOrthoGlob   ||
       (*c & CatchedOrthoLokal) == CatchedOrthoLokal )
    {
        QPointF oldpos;
        QPointF newpos;
        int catchedB(-1);
        if(viewPortSettings::instance().toDraw() == Colin::drawBeam)
        {
            if(tw.lastObjectNode() > -1)
            {
                oldpos = t.map(tw.node(tw.lastObjectNode()).toQPointF());
                newpos = (*mouse);
                catchedB = orthoCatch(t, oldpos, &newpos, c);
            }
        }
        else if(viewPortSettings::instance().toDraw() == Colin::drawNLoad)
        {
            if(tw.lastObjectNode() > -1)
            {
                oldpos = t.map(tw.node(tw.lastObjectNode()).toQPointF());
                newpos = (*mouse);
                catchedB = orthoCatch(t, oldpos, &newpos, c);
            }
        }
        else if((viewPortSettings::instance().toDraw() == Colin::drawILoad ||
                 viewPortSettings::instance().toDraw() == Colin::drawDLoad ||
                 viewPortSettings::instance().toDraw() == Colin::drawULoad ||
                 viewPortSettings::instance().toDraw() == Colin::drawDoubleLoad) &&
                 tw.lastObjectBeam() > -1)
        {
            oldpos = t.map(tw.lastObjectClick());
            newpos = (*mouse);
            catchedB = orthoCatch(t, oldpos, &newpos, c);
        }
        else if(viewPortSettings::instance().toDraw() == Colin::drawMove && tw.lastObjectLoad()>-1)
        {
                oldpos = t.map(tw.lastObjectClick());
                newpos = (*mouse);
                catchedB = orthoCatch(t, oldpos, &newpos, c);
        }
        if(catchedB > -1)
        {
            *mouse = newpos;
            if(*c == CatchedOrthoGlob)
                return 1;
            else
                return catchedB;
        }
    }

    *c = CatchedNothing;
    return -1;
}


int catcher::orthoCatch(const QTransform &t, const QPointF &oldPos, QPointF *newPos, CatchCases *c)
{

    if(allOff)
        return -1;
#ifndef QT_NO_DEBUG
    QTextStream debugS(stdout);
#endif

    bool testForGrid(c->testFlag(CatchedGrid));
    const wgv_tw &tw = *filelist::instance().currentFile();
    double scale = t.m11();
    if(viewPortSettings::instance().toDraw() == Colin::drawNLoad ||
       viewPortSettings::instance().toDraw() == Colin::drawILoad ||
       viewPortSettings::instance().toDraw() == Colin::drawDLoad ||
       viewPortSettings::instance().toDraw() == Colin::drawULoad ||
       viewPortSettings::instance().toDraw() == Colin::drawDoubleLoad ||
       (viewPortSettings::instance().toDraw() == Colin::drawMove && tw.lastObjectLoad() > -1))
    {
        scale*=tw.scaleP();
    }

    if(isOn.testFlag(catcher::CatchedOrthoGlob))
    {
        if(fabs(oldPos.x()-newPos->x())<catchRange_)
        {
            newPos->setX(oldPos.x());
            if(testForGrid)
            {
                double lenght((newPos->y()-oldPos.y()));
                if(grid::instance().gridCatch(&lenght, scale, catchRange_))
                {
                    newPos->setY(oldPos.y()+lenght);
                }
            }
            *c = catcher::CatchedOrthoGlob;
            return 1;
        }
        if(fabs(oldPos.y()-newPos->y())<catchRange_)
        {
            newPos->setY(oldPos.y());
            if(testForGrid)
            {
                double lenght((newPos->x()-oldPos.x()));
                if(grid::instance().gridCatch(&lenght, scale, catchRange_))
                {
                    newPos->setX(oldPos.x()+lenght);
                }
            }
            *c = catcher::CatchedOrthoGlob;
            return 1;
        }
    }
    if(isOn.testFlag(catcher::CatchedOrthoLokal))
    {
        if(viewPortSettings::instance().toDraw() == Colin::drawNLoad)
        {
            return -1;
        }
        else if(viewPortSettings::instance().toDraw() == Colin::drawILoad ||
                viewPortSettings::instance().toDraw() == Colin::drawDLoad ||
                viewPortSettings::instance().toDraw() == Colin::drawULoad ||
                viewPortSettings::instance().toDraw() == Colin::drawDoubleLoad ||
		viewPortSettings::instance().toDraw() == Colin::drawMove  )
        {
            if(tw.lastObjectBeam() > -1)
            {
                double length = sqrt(pow(oldPos.x()-newPos->x(), 2)+pow(oldPos.y()-newPos->y(), 2));    //abs(newLoad)
                double angleRange = atan2(catchRange_, length);                                 //]0, Pi/2[
                double angle = atan2(newPos->y()-oldPos.y(), newPos->x()-oldPos.x());                   //angle(newLoad)

                if(angle < 0)                                                                           //-->
                    angle += M_PI*2.;                                                                   //[0, 2Pi[

                double beamangle = tw.beam(tw.lastObjectBeam()).angle();                                //angle(beam)

                forever {
                    if(beamangle<0)                 beamangle+=M_PI/2.;
                    else if(beamangle>=M_PI/2.)     beamangle-=M_PI/2.;                                 //-->
                    else                            break; }                                            //[0, Pi/2[

                for(int m = 0; m < 4; m++)
                {
                    if(fabs(beamangle - angle) < angleRange)
                    {
                        if(testForGrid)
                            grid::instance().gridCatch(&length, scale, catchRange_);
                        *newPos = oldPos + QPointF(cos(beamangle)*length, sin(beamangle)*length);
                        *c = catcher::CatchedOrthoLokal;
                        return tw.lastObjectBeam();
                    }
                    beamangle += M_PI/2.;
                }
            }
        }
        else if(viewPortSettings::instance().toDraw() == Colin::drawBeam)
        {

#ifndef QT_NO_DEBUG
            debugS << "/////////////////////////////////////////////" << endl;
            debugS << "local ortho snap!" << endl;
#endif

            if(tw.lastObjectNode() > 0)
            {
                double length = sqrt(pow(oldPos.x()-newPos->x(), 2)+pow(oldPos.y()-newPos->y(), 2));
                double angleRange = atan2(catchRange_, length); //]0, Pi/2[
                double angle = atan2(newPos->y()-oldPos.y(), newPos->x()-oldPos.x());
                if(angle < 0)
                    angle += M_PI*2.;//[0, 2Pi[


#ifndef QT_NO_DEBUG
                debugS << "lenght = " << length << endl;
                debugS << "angleRange = " << angleRange << endl;
                debugS << "angle = " << angle << endl;
#endif

                for(int i =0 ; i<tw.beam_n(); i++)
                {

#ifndef QT_NO_DEBUG
                        debugS << "beam nr = " << i << endl;
#endif


                    if(tw.beam(i).leftNodeI() == tw.lastObjectNode() ||
                       tw.beam(i).rightNodeI() == tw.lastObjectNode() )
                    {


#ifndef QT_NO_DEBUG
                        debugS << "beam nr = " << i << endl;
                        debugS << "beamangle = " << tw.beam(i).angle() << endl;
#endif



                        double beamangle = tw.beam(i).angle();

                        forever {
                            if(beamangle<0)                 beamangle+=M_PI/2.;
                            else if(beamangle>=M_PI/2.)     beamangle-=M_PI/2.;
                            else                            break; }        //[0, Pi/2[


#ifndef QT_NO_DEBUG
                        debugS << "pos. beamangle = " << beamangle << endl;
#endif

                        for(int m = 0; m < 4; m++)
                        {

#ifndef QT_NO_DEBUG
                            debugS << "angle = " << angle << endl;
#endif

                            if(fabs(beamangle - angle) < angleRange)
                            {

#ifndef QT_NO_DEBUG
                                debugS << "using beam nr " << i << endl;
                                debugS << "oldPosition = P(" << newPos->x() << " / " << newPos->y() << " )" << endl;

#endif
                                if(testForGrid)
                                    grid::instance().gridCatch(&length, scale, catchRange_);
                                *newPos = oldPos + QPointF(cos(beamangle)*length, sin(beamangle)*length);




#ifndef QT_NO_DEBUG
                                debugS << "newPosition = P(" << newPos->x() << " / " << newPos->y() << " )" << endl;                                debugS << "newPosition = P(" << newPos->x() << " / " << newPos->y() << " )" << endl;
#endif

                                *c = catcher::CatchedOrthoLokal;
                                return i;
                            }
#ifndef QT_NO_DEBUG
                            else
                            {
                                debugS << "ignored" << endl;
                            }
#endif
                            beamangle += M_PI/2.;
                        }
                    }
                }
            }
            return -1;
        }
    }
    return -1;
}

void catcher::readSettings()
{
    QSettings settings("clazzes.org", "Colin");

    catchRange_ = settings.value("catch/catchRange", 15).toInt();

    isOn = static_cast<CatchCases>(settings.value("catch/isOn", catcher::CatchPreSettings).toUInt());

}

void catcher::saveSettings()
{
    QSettings settings("clazzes.org", "Colin");

    settings.setValue("catch/catchRange", catchRange_);

    settings.setValue("catch/isOn", static_cast<int>(isOn));
}
