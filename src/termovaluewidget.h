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



#ifndef TERMOVALUEWIDGET_H
#define TERMOVALUEWIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include "structpainter.h"
#include <QtGui/QLineEdit>
#include <QtGui/QMouseEvent>

#ifndef QT_NO_DEBUG
#include <QtCore/QTextStream>
#endif

class termoValueWidget : public QWidget
{
    Q_OBJECT
public:
    explicit termoValueWidget(QWidget *parent = 0);
    ~termoValueWidget();

	const static termoValueWidget &instance(){if(!instance_) instance_ = new termoValueWidget(); return *instance_;}
	static void deleteInstance(){if (instance_) delete instance_;}
	static bool hasInstance(){return(instance_?true:false);}

    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);
    void hideEvent(QHideEvent *);
    void keyPressEvent(QKeyEvent *e);

signals:
    void chosen(const double &temp, const int &val);

private slots:
    void typChanged();
    void valChanged(const QString &s);
    void valChanged(const double &d);

private:
    void adjustLimites();
    QPushButton *formBut;
    double value;
    double min;
    double max;
    double minP;
    double maxP;
    bool mousePressed;
    static termoValueWidget *instance_;
    int form;

protected:
    bool eventFilter(QObject *obj, QEvent *event){
        if(event->type() == QEvent::FocusIn)
        {
            this->setFocus();
            return true;
        }
        else
            return QObject::eventFilter(obj, event);
    }
};

#endif // TERMOVALUEWIDGET_H
