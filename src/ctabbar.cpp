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

#include "ctabbar.h"

#include <QtGui/QMessageBox>
#include <QtGui/QMainWindow>
#include <QtGui/QFileDialog>

#include "nwidget.h"

cTabBar::cTabBar(QWidget *parent) :
    QToolBar(parent)
{
    setFixedHeight(30);
    this->setWindowTitle(tr("tabbar"));
    this->setMouseTracking(true);

	this->setWhatsThis(tr("<b>tabbar</b> ")+
					   tr("<a href=\"tabbar\">open manual</a><br /> <br />")+
					   tr("Switch between opened files.<br /><br />")+
					   tr("<b>x</b>: Close the file.<br />")+
					   tr("<b>+</b>: Create a new empty file."));

    connect(&filelist::instance(),  SIGNAL(currentChanged(int)),
            this,                   SLOT(repaint()));

    connect(&filelist::instance(),  SIGNAL(recUsedCanged()),
            this,                   SLOT(repaint()));

    connect(&filelist::instance(),  SIGNAL(fileClosed()),
            this,                   SLOT(repaint()));
}


void cTabBar::paintEvent(QPaintEvent *)
{
    const int tabl = tablenght();
    const double tabw = 18;
    const double tabh = height()-2;
    filelist &files = filelist::instance();

    QRect crossrect;
    bool containing;

    //brush for tabs
    QPainter p(this);

    p.setPen(QColor(100, 100, 100));
    p.setRenderHint(QPainter::Antialiasing, true);
    QLinearGradient grad(0, 0, 0, height()-2);
    QLinearGradient mousegrad(0, 0, 0, height()-2);
    grad.setColorAt(0, static_cast<QWidget*>(parent())->palette().color(QPalette::Background));
    grad.setColorAt(1, static_cast<QWidget*>(parent())->palette().color(QPalette::Mid));
    mousegrad.setColorAt(0, static_cast<QWidget*>(parent())->palette().color(QPalette::Background).light(120));
    mousegrad.setColorAt(1, static_cast<QWidget*>(parent())->palette().color(QPalette::Mid).light(120));
    p.setBrush(QBrush(grad));

    //background
    QStyleOptionToolBar opt;
    opt.initFrom(this);
    p.fillRect(rect(), palette().brush(QPalette::Background));
    style()->drawControl(QStyle::CE_ToolBar, &opt, &p, this);

    QFont colinFont = p.font();
    colinFont.setPixelSize(height()-10);
    p.save();
    p.setFont(colinFont);
    QRect boundingRectColinText;
    p.drawText(rect().adjusted(0, 0, -10, 0),
               static_cast<int>(Qt::AlignRight | Qt::AlignVCenter),
               QString("[Colin]"),
               &boundingRectColinText);
    p.restore();
    colinIcons::instance().icon(Colin::drawNode).paint(&p, boundingRectColinText.x()-height(), 2,height()-4, height()-4,
                                                       Qt::AlignCenter);

    if(this->mapToGlobal(QPoint(0, 0)).y() < 200)
    {
        p.setTransform(QTransform(1,   0,         0,
                                  0,  -1,         0,
                                  0,   height(),  1));
    }


    //PainterPath for tabs
    QPainterPath tab;
    tab.cubicTo(tabw/2, 0,
                tabw/2, tabh,
                tabw, tabh);
    tab.lineTo(tabl-tabw, tabh);
    tab.cubicTo(tabl-tabw/2, tabh,
                tabl-tabw/2, 0,
                tabl, 0);

    //creating closeIcon
    p.save();
    const QIcon &cross = colinIcons::instance().icon(Colin::Close);
    const QIcon &node = colinIcons::instance().icon(Colin::Calculate); //TODO: repace with tabicon

    //filetabs
    for(int i=0; i<files.filecount(); i++)
    {
        if(i!=files.currentIndex())
        {
            if(tab.contains(p.transform().inverted().map(mapFromGlobal(QCursor::pos()))))
                p.setBrush(mousegrad);
            else
                p.setBrush(grad);
            p.drawPath(tab);
        }

        p.translate(dwtab(), 0);
    }



    //settingstab
    if(files.settingsVisible())
    {
        if(files.currentIndex()!=-2)
        {
            if(tab.contains(p.transform().inverted().map(mapFromGlobal(QCursor::pos()))))
                p.setBrush(mousegrad);
            else
                p.setBrush(grad);
            p.drawPath(tab);
        }
        p.translate(dwtab(), 0);
    }

    //libtab
    //if(files.libVisible())
    //{
    //    if(files.currentIndex()!=-3)
    //        p.drawPath(tab);
    //    p.translate(dwtab(), 0);
    //}

    //startertab
    if(files.newVisible())
	{/*
        if(files.currentIndex()!=-1)
			p.drawPath(tab);*/
        p.translate(dwtab(), 0);
    }

    p.restore();


    //current tab

    int currentfile;// = (files.currentIndex()>=0)? files.currentIndex() : files.filecount();

    if(files.currentIndex()==-1)
    {
        if(files.settingsVisible())
        {
            //if(files.libVisible())
            //    currentfile = files.filecount()+2;
            //else
            currentfile = files.filecount()+1;
        }
        else
            currentfile = files.filecount();
    }
    else if(files.currentIndex()==-2)
    {
        currentfile = files.filecount();
    }
    //else if(files.currentIndex()==-3)
    //{
    //    if(files.settingsVisible())
    //        currentfile = files.filecount() + 1;
    //    else
    //        currentfile = files.filecount();
    //}
    else
    {
        currentfile = files.currentIndex();
    }

    //modifing the tabpath
    QTransform t;
    t.translate(dwtab()*currentfile, 0);
    tab=t.map(tab);
    tab.lineTo(width()+1, 0);
    tab.lineTo(width()+1, -1);
    tab.lineTo(-1, -1);
    tab.lineTo(-1, 0);
    tab.closeSubpath();

    //shadow
    p.translate(1, 1);
    p.setBrush(static_cast<QWidget*>(parent())->palette().color(QPalette::Shadow));
    p.setPen(static_cast<QWidget*>(parent())->palette().color(QPalette::Shadow));
    p.drawPath(tab);
    p.translate(-1, -1);
    //shadowend

    p.setPen(QColor(100, 100, 100));

    grad.setColorAt(1, static_cast<QWidget*>(parent())->palette().color(QPalette::Light));
	if(files.currentIndex()<0){
		QBrush back(*nWidget::back);
		QTransform t;
		QMainWindow *mW = qobject_cast<QMainWindow*>(parentWidget());
		int aditionalOffset=0;
		if(mW)
			aditionalOffset = mW->centralWidget()->pos().y();
		t.translate(-p.transform().dx(), geometry().y()-aditionalOffset);
		t.scale(1, -1);
		back.setTransform(t);
		p.setBrush(back);
	}
	else
		p.setBrush(QBrush(grad));

    p.drawPath(tab);

    //current tab end

    p.resetTransform();
    //all closeIcons and filenames
    for(int i=0; i<files.filecount(); i++)
    {
        crossrect = closeRect(i);
        containing = crossrect.contains(mapFromGlobal(QCursor::pos()));
        cross.paint(&p, crossrect, Qt::AlignCenter, QIcon::Normal,
                    containing ? QIcon::On : QIcon::Off);
        node.paint(&p, QRect(dwtab()*i+tabw, 9,
                             height()-18, height()-18));
        p.drawText(QRect(dwtab()*i+tabw+height()-14, 0,
                          tabl-2*height()-8, height()-2),
                   Qt::AlignLeft | Qt::AlignVCenter,
                   files.filename(i));
    }

    //end closeIcons and filenames

    //plusIcon for new a tab
    crossrect = plusRect();
    containing = crossrect.contains(mapFromGlobal(QCursor::pos()));
    const QIcon &pi = colinIcons::instance().icon(Colin::New);
    pi.paint(&p, crossrect, Qt::AlignCenter, QIcon::Normal,
             containing ? QIcon::On : QIcon::Off);
    //end plusIcon

    //strings and close for settings and starter tab
    if(files.settingsVisible())
    {
        crossrect = closeRect(files.filecount());
        containing = crossrect.contains(mapFromGlobal(QCursor::pos()));
        cross.paint(&p, crossrect, Qt::AlignCenter, QIcon::Normal,
                    containing ? QIcon::On : QIcon::Off);
        p.drawText(QRect(dwtab()*files.filecount()+tabw+height()-14, 0,
                          tabl-2*height()-8, height()-2),
                   Qt::AlignLeft | Qt::AlignVCenter,
                   tr("settings"));
        p.translate(dwtab(), 0);
    }
    //if(files.libVisible())
    //{
    //    crossrect = closeRect(files.filecount());
    //    containing = crossrect.contains(mapFromGlobal(QCursor::pos()-QPoint(files.settingsVisible() ? dwtab() : 0, 0)));
    //    cross.paint(&p, crossrect, Qt::AlignCenter, QIcon::Normal,
    //                containing ? QIcon::On : QIcon::Off);
    //    p.drawText(QRect(dwtab()*files.filecount() + tabw + height()-14, 0,
    //                      tabl-2*height()-8, height()-2),
    //               Qt::AlignLeft | Qt::AlignVCenter,
    //               tr("library"));
    //    p.translate(dwtab(), 0);
    //}
    if(files.newVisible())
    {
        p.drawText(QRect(dwtab()*files.filecount()+tabw+height()-14, 0,
                          tabl-2*height()-8, height()-2),
                   Qt::AlignLeft | Qt::AlignVCenter,
                   tr("start here!"));
    }
}


void cTabBar::mousePressEvent(QMouseEvent *e)
{
    const double tabl = tablenght();
    const double tabw = 18;
    if(e->button() == Qt::LeftButton)
    {
        QRect crossrect;
        bool containing;
        filelist &files = filelist::instance();
        for(int i=0; i<files.filecount(); i++)
        {
            crossrect = QRect(dwtab()*i+tabl-tabw-height()+18, 9,
                              height()-18, height()-18);
            containing = crossrect.contains(mapFromGlobal(QCursor::pos()));
            if(containing)
            {
                if(files.undoGroup()->stacks().at(i)->canUndo())
                {
                    QMessageBox msgBox;
                    msgBox.setText(tr("The file \"%1\" has been modified.").arg(files.filename(i)));
                    msgBox.setInformativeText(tr("Do you want to save your changes?"));
                    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
                    msgBox.setDefaultButton(QMessageBox::Save);
                    int ret = msgBox.exec();
                    QString fileName;
                    switch(ret)
                    {
                    case QMessageBox::Save:
                        if(files.filepath(i) == "")
                        {
                            fileName = QFileDialog::getSaveFileName(
                                        this, tr("save file"), QDir::homePath(),
                                        tr("colin structs")+" (*.xcs)");

                            if(fileName.isEmpty())
                                return;
                            if(fileName.right(4) != QString(".xcs"))
                            {
                                if(!QFileInfo(fileName+".xcs").exists())
                                    fileName += QString(".xcs");
                            }
                            files.saveAs(i, fileName);
                        }
                        else
                            files.save(i);
                        break;
                    case QMessageBox::Discard:
                        break;
                    case QMessageBox::Cancel:
                        return;
                    default:
                        return;
                    }
                }
                files.closeFile(i);
                return;
            }
            crossrect = QRect(dwtab()*i, 0, dwtab(), height());
            containing = crossrect.contains(mapFromGlobal(QCursor::pos()));
            if(containing)
            {
                files.changeCurrentTo(i);
                return;
            }
        }
        if(files.settingsVisible())
        {
            crossrect = QRect(dwtab()*files.filecount()+tabl-tabw-height()+18, 9,
                              height()-18, height()-18);
            containing = crossrect.contains(mapFromGlobal(QCursor::pos()));
            if(containing)
            {
                files.closeSettings();
                return;
            }
            crossrect = QRect(dwtab()*files.filecount(), 0, dwtab(), height());
            containing = crossrect.contains(mapFromGlobal(QCursor::pos()));
            if(containing)
            {
                if(files.currentIndex()!=-2)
                    files.showSettings();
                return;
            }
        }
        //if(files.libVisible())
        //{
        //    crossrect = QRect(dwtab()*files.filecount()+tabl-tabw-height()+18, 9,
        //                      height()-18, height()-18);
        //    if(files.settingsVisible())
        //        crossrect.translate(dwtab(), 0);
        //    containing = crossrect.contains(mapFromGlobal(QCursor::pos()));
        //    if(containing)
        //    {
        //        files.closeLib();
        //        return;
        //    }
        //    crossrect = QRect(dwtab()*files.filecount(), 0, dwtab(), height());
        //    if(files.settingsVisible())
        //        crossrect.translate(dwtab(), 0);
        //    containing = crossrect.contains(mapFromGlobal(QCursor::pos()));
        //    if(containing)
        //    {
        //        if(files.currentIndex()!=-3)
        //            files.showLib();
        //        return;
        //    }
        //}
        if(files.newVisible())
        {
            crossrect = QRect(dwtab()*(files.tabcount()-1)+tabl-tabw-height()+18, 9,
                              height()-18, height()-18);
            containing = crossrect.contains(mapFromGlobal(QCursor::pos()));
            if(containing)
            {
                if(files.currentIndex()!=-1)
                    files.showNew();
                return;
            }
        }
        crossrect = plusRect();
        containing = crossrect.contains(mapFromGlobal(QCursor::pos()));
        if(containing)
        {
            files.showNew();
            return;
        }
    }

}


QRect cTabBar::closeRect(int i)
{
    return QRect(dwtab()*i+tablenght()-height(), 9,
                 height()-18, height()-18);
}

QRect cTabBar::plusRect()
{
    filelist &f = filelist::instance();
    return QRect(dwtab()*(f.tabcount()-1)+tablenght(), 9,
                 height()-18, height()-18);
}


int cTabBar::dw()
{
    return 14;
}

int cTabBar::tablenght()
{
    return qMin(double(width()-60)/filelist::instance().tabcount()+dw(), 180.0);
}

int cTabBar::dwtab()
{
    return tablenght()-dw();
}
