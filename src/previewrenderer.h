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

#ifndef PREVIEWRENDERER_H
#define PREVIEWRENDERER_H

#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include "wgv_tw.h"
#include "xmlreader.h"
#include "structpainter.h"

class previewRenderer : public QThread
{
    Q_OBJECT
public:
    previewRenderer(const QSize &s, QObject *parent = 0);
    ~previewRenderer();
    void run();
signals:
    void finished(const QString &url, const QImage &pix);
public slots:
    void renderFile(const QString &url);
    void setSize(const QSize &s);

private:
    QStringList toDo_;
    QSize size_;
    QMutex mutex;
    QWaitCondition waitCondition;
    bool killYourSelf;

};

#endif // PREVIEWRENDERER_H
