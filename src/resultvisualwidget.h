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

#ifndef RESULTVISUALWIDGET_H
#define RESULTVISUALWIDGET_H

#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QMenu>
#include <QtGui/QMouseEvent>
#include <QtGui/QActionGroup>
#include "colinstruct.h"
#include "catcher.h"

class resultVisualWidget : public QLabel
{
    Q_OBJECT
public:

    enum showHint
    {
        Hide,
        Minimap,
        Results,
        Details
    };

    resultVisualWidget(QWidget *parent = 0);
    void paintEvent(QPaintEvent *);
    static resultVisualWidget &instance(){
        return *instance_;
    }

    void setElement(const catcher::CatchCases &cc, const int &index){setElement(cc, index, 0);}
    void setElement(const catcher::CatchCases &cc, const int &index, const double &x_);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
public slots:
    void checkForMouseOver();
    void setMode(QAction *a);
    void setMode(const showHint &sH);
    void fileChanged(ColinStruct *tw);
private:
    QSize bSize();
    static resultVisualWidget *instance_;
    void paintDetails(QPainter *p);
    void paintMinimap(QPainter *p);
    void paintResults(QPainter *p);
    void paintNodeResults(QPainter *p);
    void paintBeamResults(QPainter *p);
    int object;
    catcher::CatchCases objectType;
    double x;
    void actualGeometry();
    bool mouseIsOver;
    showHint sH;
    QPushButton *mode;
    QActionGroup *aGroup;
};

#endif // RESULTVISUALWIDGET_H
