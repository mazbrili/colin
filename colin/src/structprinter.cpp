#include "structprinter.h"
#include "colinicons.h"
#include "unitsettings.h"


int headerSize = 120;
int marging = 30;
int fontBig = 18;
int fontNorm = 9;

structPrinter::structPrinter(wgv_tw *t):
        QPrinter()
{
    tw=t;
    pageCount = 1;
}

void structPrinter::print()
{
    dx = 30;
    this->setResolution(resolution()*2);
    QPainter p(this);
    QFont f = p.font();
    f.setPointSize(6);
    p.setFont(f);
    addHeader(&p);
    p.save();

    structPainter sP;
    sP.ignoreHotSpots(true);
    sP.ignoreSelection(true);
    QRectF source = tw->boundingRect();
    QRectF target = QRectF(marging, marging, width()/2-4*marging, height()/2-headerSize-4*marging);
    QTransform t;
    double scale = qMin((target.width())/source.width(), (target.height()-headerSize)/source.height());
    t.translate(-source.x()*scale+target.width()/2-source.width()*scale/2+marging,
                -source.y()*scale+target.height()/2-source.height()*scale/2+marging);
    t.scale(scale, scale);

    p.setPen(QPen(QColor(0, 0, 0), 1));
    p.setBrush(Qt::NoBrush);
    p.drawRect(2, 2, width()/2-4, height()/2-headerSize-4);
    sP.drawStruct(*tw, &p, &t, viewPortSettings::instance().elements(0));

    t.translate((width()/2+2*marging)/scale, 0);
    p.setPen(QPen(QColor(0, 0, 0), 1));
    p.setBrush(Qt::NoBrush);
    p.drawRect(width()/2+2, 2, width()/2-4, height()/2-headerSize-4);
    sP.drawStruct(*tw, &p, &t, viewPortSettings::instance().elements(1));

    t.translate(-(width()/2+2*marging)/scale, (height()/2-headerSize+2*marging)/scale);
    p.setPen(QPen(QColor(0, 0, 0), 1));
    p.setBrush(Qt::NoBrush);
    p.drawRect(2, height()/2-headerSize+2, width()/2-4, height()/2-headerSize-4);
    sP.drawStruct(*tw, &p, &t, viewPortSettings::instance().elements(2));

    t.translate((width()/2+2*marging)/scale, 0);
    p.setPen(QPen(QColor(0, 0, 0), 1));
    p.setBrush(Qt::NoBrush);
    p.drawRect(width()/2+2, height()/2-headerSize+2, width()/2-4, height()/2-headerSize-4);
    sP.drawStruct(*tw, &p, &t, viewPortSettings::instance().elements(3));

    p.setPen(QPen(QColor(0, 0, 0), 1));
    p.setBrush(Qt::NoBrush);
    p.resetTransform();
    p.translate(0, (height()-headerSize-marging));
    p.drawLine(50, -100, scale+50, -100);
    p.drawText(QRect(50+scale+10, -100-20, 500, 40), Qt::AlignVCenter | Qt::AlignLeft, "1m");
    p.setPen(viewPortSettings::instance().color(Colin::C_Np));
    p.drawLine(50, -120, scale*tw->scaleP()/PPREFIX+50, -120);
    p.drawText(QRect(50+scale*tw->scaleP()/PPREFIX+10, -120-20, 500, 40), Qt::AlignVCenter | Qt::AlignLeft, "1 "+unitSettings::instance().Peh());
    p.setPen(viewPortSettings::instance().color(Colin::C_Mp));
    p.drawLine(50, -140, scale*tw->scaleM()/MPREFIX+50, -140);
    p.drawText(QRect(50+scale*tw->scaleM()/MPREFIX+10, -140-20, 500, 40), Qt::AlignVCenter | Qt::AlignLeft, "1 "+unitSettings::instance().Peh());


    p.restore();
    f.setPointSize(8);
    p.setFont(f);


    writeMats(&p);
    writeNodes(&p);
    writeBeams(&p);
    writeLoads(&p);
    if(!tw->isCalculated())
        return;
    writeResultsNodes(&p);
    writeResultsBeams(&p);

}

void structPrinter::addPage(QPainter *p)
{
    newPage();
    addHeader(p);
    dx = 30;
}

void structPrinter::addHeader(QPainter *p)
{
    p->save();
    p->translate(0, p->device()->height());
    p->setPen(QPen(QColor(0, 0, 0), 1.5));
    p->drawLine(20, -headerSize, p->device()->width()-20, -headerSize);
    QFont f = p->font();
    f.setPointSize(18);
    p->setFont(f);
    p->drawText(QRectF(22, -headerSize+12, 1000, 60), Qt::AlignBottom | Qt::AlignLeft, "created with [Colin]");
    p->drawText(QRectF(width()-1022, -headerSize+12, 1000, 60), Qt::AlignBottom | Qt::AlignRight, "clazzes.org");
    f.setPointSize(6);
    p->setFont(f);
    p->drawText(QRectF(22, -headerSize+72, 1000, 28), Qt::AlignBottom | Qt::AlignLeft, "("+QString(QChar(0x2184))+") 2011 matti");
    f.setPointSize(8);
    p->setFont(f);
    p->drawText(QRectF(22, -headerSize+105, 1000, 28), Qt::AlignBottom | Qt::AlignLeft, filelist::instance().filename(filelist::instance().currentIndex()));
    p->drawText(QRectF(500-22, -headerSize+105, this->width()-500, 28), Qt::AlignBottom | Qt::AlignRight, tr("page")+" "+QString::number(pageCount++));


    p->restore();


}

void structPrinter::writeMats(QPainter *p)
{
    addPage(p);
    QRect br;
    QFont f = p->font();
    f.setPointSize(fontBig);
    p->setFont(f);
    p->setPen(QColor(0, 0, 0));
    p->drawText(QRect(20, dx, 1200, 100), Qt::AlignTop | Qt::AlignLeft, "["+tr("cross sections") +" & " +tr("materials")+"]", &br);
    usedSpace(br.height()+5, p);
    f.setPointSize(fontNorm);
    p->setFont(f);
    p->drawText(QRect(30, dx, 400, 100), Qt::AlignTop | Qt::AlignLeft, tr("materials"), &br);
    p->drawLine(30, br.bottom()+2, 630, br.bottom()+2);
    usedSpace(br.height()+5, p);
    p->drawText(QRect(330, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, "E["+unitSettings::instance().Eeh()+"]", &br);
    p->drawText(QRect(530, dx, 100, 100), Qt::AlignTop | Qt::AlignHCenter, QString(QChar(0x03B1))+"T[1/K]");
    p->drawLine(30, br.bottom()+4, 630, br.bottom()+4);
    usedSpace(br.height()+5, p);
    QList<int> mats;
    for(int i=0; i<tw->beam_n(); i++)
    {
        if(!mats.contains(tw->beam(i).MatI()))
        {
            mats.append(tw->beam(i).MatI());
            if(!(mats.count()%2))
                p->fillRect(QRect(30, dx, 630, br.height()),QColor(70, 70, 110, 50));
            p->drawText(QRect(30, dx, 300, 100), Qt::AlignTop | Qt::AlignRight, tw->beam(i).Mat().name(), &br);
            p->drawText(QRect(330, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->beam(i).Mat().E()*EPREFIX, 'g', PRECISON));
            p->drawText(QRect(530, dx, 100, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->beam(i).Mat().alphaT(), 'g', PRECISON));
            usedSpace(br.height(), p);
        }
    }
    usedSpace(40, p);
    p->drawText(QRect(30, dx, 400, 100), Qt::AlignTop | Qt::AlignLeft, tr("cross sections"), &br);
    p->drawLine(30, br.bottom()+2, 930, br.bottom()+2);
    usedSpace(br.height()+5, p);
    p->drawText(QRect(330, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, "A["+unitSettings::instance().Aeh()+"]", &br);
    p->drawText(QRect(530, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, "I["+unitSettings::instance().Ieh()+"]");
    p->drawText(QRect(730, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, "h["+unitSettings::instance().heh()+"]");
    p->drawLine(30, br.bottom()+4, 930, br.bottom()+4);
    usedSpace(br.height()+5, p);
    QList<int> profs;
    for(int i=0; i<tw->beam_n(); i++)
    {
        if(!profs.contains(tw->beam(i).ProfileI()))
        {
            profs.append(tw->beam(i).ProfileI());
            if(!(profs.count()%2))
                p->fillRect(QRect(30, dx, 930, br.height()),QColor(70, 70, 110, 50));
            p->drawText(QRect(30, dx, 300, 100), Qt::AlignTop | Qt::AlignRight, tw->beam(i).Profile().name(), &br);
            p->drawText(QRect(330, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->beam(i).Profile().A()*APREFIX, 'g', PRECISON), &br);
            p->drawText(QRect(530, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->beam(i).Profile().I()*IPREFIX, 'g', PRECISON), &br);
            p->drawText(QRect(730, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->beam(i).Profile().h()*HPREFIX, 'g', PRECISON), &br);
            usedSpace(br.height(), p);
        }
    }
}

void structPrinter::writeNodes(QPainter *p)
{
    addPage(p);
    QRect br;
    QFont f = p->font();
    f.setPointSize(fontBig);
    p->setFont(f);
    p->setPen(QColor(0, 0, 0));
    p->drawText(QRect(20, dx, 1200, 100), Qt::AlignTop | Qt::AlignLeft, "["+tr("nodes")+"]", &br);
    usedSpace(br.height()+5, p);
    f.setPointSize(fontNorm);
    p->setFont(f);
    p->drawText(QRect(130, dx, 400, 100), Qt::AlignTop | Qt::AlignLeft, tr("coordinates"));
    p->drawText(QRect(550, dx, 400, 100), Qt::AlignTop | Qt::AlignLeft, tr("support"), &br);
    usedSpace(br.height()+5, p);
    p->drawLine(130, br.bottom()+2, 530, br.bottom()+2);
    p->drawLine(550, br.bottom()+2, 1330, br.bottom()+2);
    p->drawText(QRect(130, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, "x[m]", &br);
    p->drawText(QRect(330, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, "z[m]");
    p->drawText(QRect(530, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, QString(QChar(0x03B1))+"[grad]");
    p->drawText(QRect(730, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, "c_x["+unitSettings::instance().Feh()+"]");
    p->drawText(QRect(930, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, "c_y["+unitSettings::instance().Feh()+"]");
    p->drawText(QRect(1130, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, QString("c_")+QChar(0x03C6)+"["+unitSettings::instance().FMeh()+"]");
    p->drawLine(130, br.bottom()+4, 1330, br.bottom()+4);
    usedSpace(br.height()+5, p);
    for(int i=0; i<tw->node_n(); i++)
    {
        if(!(i%2))
            p->fillRect(QRect(30, dx, 1330, br.height()),QColor(70, 70, 110, 50));
        p->drawText(QRect(30, dx, 100, 100), Qt::AlignTop | Qt::AlignRight, "("+QString::number(i)+")", &br);
        p->drawText(QRect(130, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->node(i).x(), 'f', PRECISON));
        p->drawText(QRect(330, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->node(i).z(), 'f', PRECISON));
        if(tw->node(i).hasbearing())
        {
            p->drawText(QRect(530, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, QString::number(-tw->node(i).bearing().angle()*ANGLEPREFIX, 'f', PRECISON));

            if(tw->node(i).bearing().x())
                p->drawText(QRect(730, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, tr("locked"));
            else if(tw->node(i).bearing().c_x() != 0)
                p->drawText(QRect(730, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->node(i).bearing().c_x()*FPREFIX, 'f', PRECISON));
            else
                p->drawText(QRect(730, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, tr("free"));

            if(tw->node(i).bearing().z())
                p->drawText(QRect(930, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, tr("locked"));
            else if(tw->node(i).bearing().c_z() != 0)
                p->drawText(QRect(930, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->node(i).bearing().c_z()*FPREFIX, 'f', PRECISON));
            else
                p->drawText(QRect(930, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, tr("free"));

            if(tw->node(i).bearing().phi())
                p->drawText(QRect(1130, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, tr("locked"));
            else if(tw->node(i).bearing().c_phi() != 0)
                p->drawText(QRect(1130, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->node(i).bearing().c_phi()*FMPREFIX, 'f', PRECISON));
            else
                p->drawText(QRect(1130, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, tr("free"));
        }
        usedSpace(br.height(), p);
    }
}

void structPrinter::writeBeams(QPainter *p)
{
    addPage(p);
    QRect br;
    QFont f = p->font();
    f.setPointSize(fontBig);
    p->setFont(f);
    p->setPen(QColor(0, 0, 0));
    p->drawText(QRect(20, dx, 1200, 100), Qt::AlignTop | Qt::AlignLeft, "["+tr("beams")+"]", &br);
    usedSpace(br.height()+5, p);
    f.setPointSize(fontNorm);
    p->setFont(f);
    p->drawText(QRect(130, dx, 400, 100), Qt::AlignTop | Qt::AlignLeft, tr("nodes"));
    p->drawText(QRect(350, dx, 400, 100), Qt::AlignTop | Qt::AlignLeft, tr("parameters"));
    p->drawText(QRect(750, dx, 450, 100), Qt::AlignTop | Qt::AlignLeft, tr("hinches")+" & " +tr("springs")+" ["+ unitSettings::instance().Feh() +"] & [" + unitSettings::instance().FMeh() +"]", &br);
    usedSpace(br.height()+5, p);
    p->drawLine(130, br.bottom()+2, 330, br.bottom()+2);
    p->drawLine(350, br.bottom()+2, 730, br.bottom()+2);
    p->drawLine(750, br.bottom()+2, 1420, br.bottom()+2);
    p->drawText(QRect(130, dx, 100, 100), Qt::AlignTop | Qt::AlignHCenter, tr("left"), &br);
    p->drawText(QRect(230, dx, 100, 100), Qt::AlignTop | Qt::AlignHCenter, tr("right"));
    p->drawText(QRect(330, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, tr("cross section"));
    p->drawText(QRect(530, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, tr("material"));
    p->drawText(QRect(730, dx, 120, 100), Qt::AlignTop | Qt::AlignHCenter, "cul");
    p->drawText(QRect(850, dx, 120, 100), Qt::AlignTop | Qt::AlignHCenter, "cwl");
    p->drawText(QRect(970, dx, 120, 100), Qt::AlignTop | Qt::AlignHCenter, QString("c")+QChar(0x03C6)+"l");
    p->drawText(QRect(1090, dx, 120, 100), Qt::AlignTop | Qt::AlignHCenter, "cur");
    p->drawText(QRect(1210, dx, 120, 100), Qt::AlignTop | Qt::AlignHCenter, "cwr");
    p->drawText(QRect(1330, dx, 120, 100), Qt::AlignTop | Qt::AlignHCenter, QString("c")+QChar(0x03C6)+"r");
    p->drawLine(130, br.bottom()+4, 1420, br.bottom()+4);
    usedSpace(br.height()+5, p);
    for(int i=0; i<tw->beam_n(); i++)
    {
        if(!(i%2))
            p->fillRect(QRect(30, dx, 1420, br.height()),QColor(70, 70, 110, 50));
        p->drawText(QRect(30, dx, 100, 100), Qt::AlignTop | Qt::AlignRight, "["+QString::number(i)+"]", &br);
        p->drawText(QRect(130, dx, 100, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->beam(i).leftNodeI()));
        p->drawText(QRect(230, dx, 100, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->beam(i).rightNodeI()));
        p->drawText(QRect(330, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, tw->beam(i).Profile().name());
        p->drawText(QRect(530, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, tw->beam(i).Mat().name());
        if(tw->beam(i).hasJoints())
        for(int j=0; j<6; j++)
        {
            if(tw->beam(i).joint(j))
            {
                if(tw->beam(i).spring(j) != 0)
                {
                    if(j == 2 || j == 5) //moment
                        p->drawText(QRect(730+j*120, dx, 100, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->beam(i).spring(j)*FMPREFIX, 'f', PRECISON));
                    else
                        p->drawText(QRect(730+j*120, dx, 100, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->beam(i).spring(j)*FPREFIX, 'f', PRECISON));
                }
                else
                {
                    p->drawText(QRect(730+j*120, dx, 100, 100), Qt::AlignTop | Qt::AlignRight, tr("free"));
                }
            }

            else
            {
                //p->drawText(QRect(730+j*100, dx, 100, 100), Qt::AlignTop | Qt::AlignRight, tr("locked"));
            }
        }
        usedSpace(br.height(), p);
    }
}

void structPrinter::writeLoads(QPainter *p)
{
    addPage(p);
    QRect br;
    QFont f = p->font();
    f.setPointSize(fontBig);
    p->setFont(f);
    p->setPen(QColor(0, 0, 0));
    p->drawText(QRect(20, dx, 1200, 100), Qt::AlignTop | Qt::AlignLeft, "["+tr("loads")+"]", &br);
    usedSpace(br.height()+5, p);
    f.setPointSize(fontNorm);
    p->setFont(f);
    p->drawText(QRect(130, dx, 400, 100), Qt::AlignTop | Qt::AlignLeft, tr("nodal forces"), &br);
    usedSpace(br.height()+5, p);
    p->drawLine(130, br.bottom()+2, 1130, br.bottom()+2);
    p->drawText(QRect(130, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, tr("typ"), &br);
    p->drawText(QRect(330, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, tr("on node"));
    p->drawText(QRect(530, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, tr("Px")+ " ["+unitSettings::instance().Peh()+"]");
    p->drawText(QRect(730, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, tr("Pz")+ " ["+unitSettings::instance().Peh()+"]");
    p->drawText(QRect(930, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, tr("M")+ " ["+unitSettings::instance().Meh()+"]");
    p->drawLine(130, br.bottom()+4, 1130, br.bottom()+4);
    usedSpace(br.height()+5, p);
    int j=0;
    for(int i=0; i<tw->load_n(); i++)
    {
        if(tw->load(i).typ() == wgv_load::nodeLoad ||
           tw->load(i).typ() == wgv_load::moment   )
        {
            if(!(j%2))
                p->fillRect(QRect(30, dx, 1130, br.height()),QColor(70, 70, 110, 50));
            p->drawText(QRect(30, dx, 100, 100), Qt::AlignTop | Qt::AlignRight, "<"+QString::number(i)+">", &br);
            p->drawText(QRect(330, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->load(i).at()));
            if(tw->load(i).typ() == wgv_load::moment)
            {
                p->drawText(QRect(130, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, tr("moment"));
                p->drawText(QRect(930, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->load(i).M()*MPREFIX, 'f', PRECISON));
            }
            else
            {
                p->drawText(QRect(130, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, tr("nodal force"));
                p->drawText(QRect(430, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->load(i).Px()*PPREFIX, 'f', PRECISON));
                p->drawText(QRect(630, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->load(i).Pz()*PPREFIX, 'f', PRECISON));

            }
            usedSpace(br.height(), p);
            j++;
        }
    }
    usedSpace(15, p);

    p->drawText(QRect(130, dx, 400, 100), Qt::AlignTop | Qt::AlignLeft, tr("loads on beams"), &br);
    usedSpace(br.height()+5, p);
    p->drawLine(130, br.bottom()+2, 1030, br.bottom()+2);
    p->drawText(QRect(130, dx, 300, 100), Qt::AlignTop | Qt::AlignHCenter, tr("function"), &br);
    p->drawText(QRect(430, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, tr("on beam"));
    p->drawText(QRect(630, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, "Px ["+unitSettings::instance().Peh()+"/m]");
    p->drawText(QRect(830, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, "Pz ["+unitSettings::instance().Peh()+"/m]");
    p->drawLine(130, br.bottom()+4, 1030, br.bottom()+4);
    usedSpace(br.height()+5, p);
    j=0;
    for(int i=0; i<tw->load_n(); i++)
    {
        if(tw->load(i).typ() == wgv_load::uniformlyDistibutedLoad ||
           tw->load(i).typ() == wgv_load::increasingLinearLoad    ||
           tw->load(i).typ() == wgv_load::decreasingLinearLoad    )
        {
            if(!(j%2))
                p->fillRect(QRect(30, dx, 1030, br.height()),QColor(70, 70, 110, 50));
            p->drawText(QRect(30, dx, 100, 100), Qt::AlignTop | Qt::AlignRight, "<"+QString::number(i)+">", &br);
            if(tw->load(i).typ() == wgv_load::uniformlyDistibutedLoad)
                p->drawText(QRect(130, dx, 300, 100), Qt::AlignTop | Qt::AlignRight, tr("uniformly"));
            else if(tw->load(i).typ() == wgv_load::increasingLinearLoad)
                p->drawText(QRect(130, dx, 300, 100), Qt::AlignTop | Qt::AlignRight, tr("linear, increasing"));
            else
                p->drawText(QRect(130, dx, 300, 100), Qt::AlignTop | Qt::AlignRight, tr("linear, decreasing"));

            p->drawText(QRect(430, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->load(i).at()));
            p->drawText(QRect(630, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->load(i).Px()*PPREFIX, 'f', PRECISON));
            p->drawText(QRect(830, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->load(i).Pz()*PPREFIX, 'f', PRECISON));
            usedSpace(br.height(), p);
            j++;
        }
    }
    usedSpace(15, p);

    p->drawText(QRect(130, dx, 400, 100), Qt::AlignTop | Qt::AlignLeft, tr("temperatures"), &br);
    usedSpace(br.height()+5, p);
    p->drawLine(130, br.bottom()+2, 730, br.bottom()+2);
    p->drawText(QRect(130, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, tr("typ"), &br);
    p->drawText(QRect(330, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, tr("on beam"));
    p->drawText(QRect(530, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, "T[K]");
    p->drawLine(130, br.bottom()+4, 730, br.bottom()+4);
    usedSpace(br.height()+5, p);
    j=0;
    for(int i=0; i<tw->load_n(); i++)
    {
        if(tw->load(i).typ() == wgv_load::tempDiffrence ||
           tw->load(i).typ() == wgv_load::tempChange    )
        {
            if(!(j%2))
                p->fillRect(QRect(30, dx, 730, br.height()),QColor(70, 70, 110, 50));
            p->drawText(QRect(30, dx, 100, 100), Qt::AlignTop | Qt::AlignRight, "<"+QString::number(i)+">", &br);
            p->drawText(QRect(330, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->load(i).at()));
            if(tw->load(i).typ() == wgv_load::tempDiffrence)
            {
                p->drawText(QRect(130, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, tr("gradient"));
                p->drawText(QRect(530, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->load(i).Pz()*PPREFIX, 'f', PRECISON));
            }
            else
            {
                p->drawText(QRect(130, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, tr("change"));
                p->drawText(QRect(530, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->load(i).Px()*PPREFIX, 'f', PRECISON));
            }
            usedSpace(br.height(), p);
            j++;
        }
    }
    usedSpace(15, p);

    p->drawText(QRect(130, dx, 400, 100), Qt::AlignTop | Qt::AlignLeft, tr("double loads"), &br);
    usedSpace(br.height()+5, p);
    p->drawLine(130, br.bottom()+2, 1130, br.bottom()+2);
    p->drawText(QRect(130, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, tr("side"), &br);
    p->drawText(QRect(330, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, tr("on beam"));
    p->drawText(QRect(530, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, "N["+unitSettings::instance().Peh()+"]");
    p->drawText(QRect(730, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, "Q["+unitSettings::instance().Peh()+"]");
    p->drawText(QRect(930, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, "M["+unitSettings::instance().Meh()+"]");
    p->drawLine(130, br.bottom()+4, 1130, br.bottom()+4);
    usedSpace(br.height()+5, p);
    j=0;
    for(int i=0; i<tw->load_n(); i++)
    {
        if(tw->load(i).typ() == wgv_load::doubleLoadLeft ||
           tw->load(i).typ() == wgv_load::doubleLoadRight)
        {
            if(!(j%2))
                p->fillRect(QRect(30, dx, 1130, br.height()),QColor(70, 70, 110, 50));
            p->drawText(QRect(30, dx, 100, 100), Qt::AlignTop | Qt::AlignRight, "<"+QString::number(i)+">", &br);
            if(tw->load(i).typ() == wgv_load::doubleLoadLeft)
                p->drawText(QRect(130, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, tr("right side of"));
            else
                p->drawText(QRect(130, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, tr("left side of"));
            p->drawText(QRect(330, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->load(i).at()));
            p->drawText(QRect(530, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->load(i).Px()*PPREFIX, 'f', PRECISON));
            p->drawText(QRect(730, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->load(i).Pz()*PPREFIX, 'f', PRECISON));
            p->drawText(QRect(930, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->load(i).M()*MPREFIX, 'f', PRECISON));

            usedSpace(br.height(), p);
            j++;
        }
    }
    usedSpace(15, p);
}

void structPrinter::writeResultsNodes(QPainter *p)
{

    addPage(p);
    QRect br;
    QFont f = p->font();
    f.setPointSize(fontBig);
    p->setFont(f);
    p->setPen(QColor(0, 0, 0));
    p->drawText(QRect(20, dx, 1200, 100), Qt::AlignTop | Qt::AlignLeft, "["+tr("results")+"("+tr("nodes")+")]", &br);
    usedSpace(br.height()+5, p);
    f.setPointSize(fontNorm);
    p->setFont(f);
    p->drawText(QRect(130, dx, 400, 100), Qt::AlignTop | Qt::AlignLeft, tr("deformation"));
    p->drawText(QRect(750, dx, 400, 100), Qt::AlignTop | Qt::AlignLeft, tr("reaction forces"), &br);
    usedSpace(br.height()+5, p);
    p->drawLine(130, br.bottom()+2, 730, br.bottom()+2);
    p->drawLine(750, br.bottom()+2, 1330, br.bottom()+2);
    p->drawText(QRect(130, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, "u[m]", &br);
    p->drawText(QRect(330, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, "w[m]");
    p->drawText(QRect(530, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, QString(QChar(0x03C6))+"[grad]");
    p->drawText(QRect(730, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, "Ax["+unitSettings::instance().Peh()+"]");
    p->drawText(QRect(930, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, "Az["+unitSettings::instance().Peh()+"]");
    p->drawText(QRect(1130, dx, 200, 100), Qt::AlignTop | Qt::AlignHCenter, QString("A%1[").arg(QChar(0x03C6))+unitSettings::instance().Meh()+"]");
    p->drawLine(130, br.bottom()+2, 1330, br.bottom()+2);
    usedSpace(br.height()+5, p);
    for(int i=0; i<tw->node_n(); i++)
    {
        if(!(i%2))
            p->fillRect(QRect(30, dx, 1330, br.height()),QColor(70, 70, 110, 50));
        p->drawText(QRect(30, dx, 100, 100), Qt::AlignTop | Qt::AlignRight, "("+QString::number(i)+")", &br);
        p->drawText(QRect(130, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->node(i).u(), 'e', PRECISON));
        p->drawText(QRect(330, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->node(i).w(), 'e', PRECISON));
        p->drawText(QRect(530, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->node(i).phi()*ANGLEPREFIX, 'e', PRECISON));
        if(tw->node(i).hasbearing())
        {
            p->drawText(QRect(730, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->node(i).Ax()*PPREFIX, 'f', PRECISON));
            p->drawText(QRect(930, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->node(i).Az()*PPREFIX, 'f', PRECISON));
            p->drawText(QRect(1130, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, QString::number(tw->node(i).Am()*MPREFIX, 'f', PRECISON));

        }
        usedSpace(br.height(), p);
    }
}

void structPrinter::writeResultsBeams(QPainter *p)
{
    addPage(p);
    QRect br;
    QFont f = p->font();
    f.setPointSize(fontBig);
    p->setFont(f);
    p->setPen(QColor(0, 0, 0));
    p->drawText(QRect(20, dx, 1200, 100), Qt::AlignTop | Qt::AlignLeft, "["+tr("results")+"("+tr("beams")+")]", &br);
    usedSpace(br.height()+5, p);
    f.setPointSize(fontNorm);
    p->setFont(f);
    p->drawText(QRect(130, dx, 400, 100), Qt::AlignTop | Qt::AlignLeft, tr("forces and deformation"), &br);
    usedSpace(br.height()+5, p);
    p->drawLine(130, br.bottom()+2, 830, br.bottom()+2);
    for(int i=0; i<tw->beam_n(); i++)
    {
        p->fillRect(QRect(30, dx, 100, br.height()),QColor(70, 70, 110, 100));
        p->fillRect(QRect(136, dx, 1400, br.height()),QColor(70, 70, 110, 50));
        p->drawText(QRect(30, dx, 100, 100), Qt::AlignTop | Qt::AlignRight, "("+QString::number(i)+")", &br);

        p->drawText(QRect(130, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, "u(x)[m] = ");
        p->drawText(QRect(330, dx, 1200, 100), Qt::AlignTop | Qt::AlignLeft, tw->beam(i).uconst().toString(1, 'e', PRECISON), &br);
        usedSpace(br.height()+5, p);

        p->drawText(QRect(130, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, "w(x)[m] = ");
        p->drawText(QRect(330, dx, 1200, 100), Qt::AlignTop | Qt::AlignLeft, tw->beam(i).wconst().toString(1, 'e', PRECISON), &br);
        usedSpace(br.height()+5, p);

        p->fillRect(QRect(30, dx, 1500, br.height()),QColor(70, 70, 110, 50));
        p->drawText(QRect(130, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, QString("%1(x)[grad] = ").arg(QChar(0x03C6)));
        p->drawText(QRect(330, dx, 1200, 100), Qt::AlignTop | Qt::AlignLeft, tw->beam(i).phiconst().toString(ANGLEPREFIX, 'e', PRECISON), &br);
        usedSpace(br.height()+5, p);

        p->drawText(QRect(130, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, "N(x)["+unitSettings::instance().Peh()+"] = ");
        p->drawText(QRect(330, dx, 1200, 100), Qt::AlignTop | Qt::AlignLeft, tw->beam(i).Nconst().toString(PPREFIX, 'f', PRECISON), &br);
        usedSpace(br.height()+5, p);

        p->fillRect(QRect(30, dx, 1500, br.height()),QColor(70, 70, 110, 50));
        p->drawText(QRect(130, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, "Q(x)["+unitSettings::instance().Peh()+"] = ");
        p->drawText(QRect(330, dx, 1200, 100), Qt::AlignTop | Qt::AlignLeft, tw->beam(i).Qconst().toString(PPREFIX, 'f', PRECISON), &br);
        usedSpace(br.height()+5, p);

        p->drawText(QRect(130, dx, 200, 100), Qt::AlignTop | Qt::AlignRight, "M(x)["+unitSettings::instance().Meh()+"] = ");
        p->drawText(QRect(330, dx, 1200, 100), Qt::AlignTop | Qt::AlignLeft, tw->beam(i).Mconst().toString(MPREFIX, 'f', PRECISON), &br);
        usedSpace(br.height()+5, p);


    }
}

void structPrinter::usedSpace(int h, QPainter *p)
{
    if(dx+h>height()-headerSize-40)
        addPage(p);
    else
        dx+=h+5;
}

