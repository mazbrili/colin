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

#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include <QtGui/QApplication>
#include <QtGui/QWidget>
#include <QtGui/QPainter>
#include <QtGui/QStyle>
#include <QtGui/QStyleOptionButton>
#include <QtGui/QPaintEvent>

#include "colinicons.h"
#include "filelist.h"
#include "previewrenderer.h"


class previewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit previewWidget(QWidget *parent = 0);


    void paintEvent(QPaintEvent *e);
    void enterEvent(QEvent *){repaint();}
    void leaveEvent(QEvent *){repaint();}
    void mouseMoveEvent(QMouseEvent *){repaint(QRegion(xRect()));}
    void mousePressEvent(QMouseEvent *e);
    void resizeEvent(QResizeEvent *e);

    QSize sizeHint() const{return QSize(200, 155);}
    static previewRenderer *renderer;
protected:
    int heightForWidth(int w) const;
public slots:
    void giefPix(const QString &url, const QImage &pix);
    void actualUrl();
private:
    bool hasPreview;
    int id;
    static int count;
    QImage image;
    QString myUrl;
    QRect xRect();

};

#endif // PREVIEWWIDGET_H
