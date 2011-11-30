/***********************************************************
 * $Id$
 *
 * Colin
 *
 * Copyright (C) 2011 Matthias Rauter (matthias.rauter@student.uibk.ac.at)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details."
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see
 *
 * http://www.gnu.org/licenses/.
 *
 * Author: Matthias Rauter
 *
 ***********************************************************/

#include "previewrenderer.h"

#include <QtCore/QFile>

previewRenderer::previewRenderer(const QSize &s, QObject *parent) :
    QThread(parent)
{
    size_ = s;
    killYourSelf = false;
}


previewRenderer::~previewRenderer()
{
    mutex.lock();
    killYourSelf = true;
    waitCondition.wakeOne();
    mutex.unlock();
    wait();
}


void previewRenderer::run()
{
    QString current;
    QSize size;


    forever
    {
        QMutexLocker lock(&mutex);
        if(toDo_.isEmpty())
            waitCondition.wait(&mutex);
        if(killYourSelf)
            return;
        current = toDo_.first();
        toDo_.removeFirst();
        size = size_;


#ifndef QT_NO_DEBUG
        QTextStream debugS(stdout);
        debugS  << "rendering \""
                << current << "\""
                << endl;
#endif


        lock.unlock();

/*************************
 **     RENDERING       **
 *************************/

        QImage device(size, QImage::Format_RGB32);
        device.fill(qRgb(255, 255, 255));


        ColinStruct tw;
        QFile f(current);
        if(f.exists())
        {
            if(f.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                XmlReader Xr(&tw);

                if(Xr.read(&f))
                {

					for(int i=0; i<tw.bls_n();i++)
						tw.setBLSColor(i, (i==0)?viewPortSettings::instance().firstStandardColor():
												 viewPortSettings::instance().nextStandardColor(tw.bls(i-1).color()));

                    structPainter sP;

                    QTransform t(1.0,  0.0,  0.0,
                                 0.0,  1.0,  0.0,
                                 0.0,  0.0,  1.0);


                    //get a first idea about the global scale
                    QRectF boundingRect = tw.boundingNodeRect();
                    double scale = qMin((size.width()-30)*2/boundingRect.width(), (size.height()-30)*2/boundingRect.height());


                    //scale M and P so they look good
                    double mScale = tw.mMax();
                    if(mScale != 0)
                    {
                        tw.setScaleM(120/mScale);
                    }
                    double pScale = tw.pMax();
                    if(pScale != 0)
                    {
                        tw.setScaleP(size.height()/3/scale/pScale);
                    }

                    //calculate the
                    boundingRect = tw.boundingRect();
                    scale = qMin((size.width()-30)*2/boundingRect.width(), (size.height()-30)*2/boundingRect.height());


                    t.translate(-boundingRect.x()*scale+size.width()-boundingRect.width()*scale/2,
                                -boundingRect.y()*scale+size.height()-boundingRect.height()*scale/2);
                    t.scale(scale, scale);


                    QPainter p(&device);
                    p.scale(0.5, 0.5);
                    sP.drawStruct(tw, &p, &t,
                                  Colin::beam    |
                                  Colin::node    |
                                  Colin::nload   |
                                  Colin::bearing |
                                  Colin::joint   );
                    emit finished(current, device);
                }
            }
        }
        lock.relock();


#ifndef QT_NO_DEBUG
        debugS  << "image emited"
                << endl;
        debugS  << "removed from list"
                << endl;
#endif


        lock.unlock();
    }
}


void previewRenderer::renderFile(const QString &url)
{
    QMutexLocker lock(&mutex);
    toDo_.removeAll(url);
    toDo_.append(url);
    lock.unlock();
    waitCondition.wakeOne();
}


void previewRenderer::setSize(const QSize &s)
{
    QMutexLocker lock(&mutex);
    size_ = s;
}
