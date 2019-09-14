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

#include "closedialog.h"
#include <QGridLayout>
#include <QScrollArea>
#include <QFileDialog>
#include "colinicons.h"
#include "filelist.h"


closeDialog::closeDialog(QWidget *parent) :
    QDialog(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    QPushButton *ok = new QPushButton(colinIcons::instance().icon(Colin::Ok), tr("save selected"), this);
	QPushButton *sA = new QPushButton(colinIcons::instance().icon(Colin::Save), tr("save all"), this);
    QPushButton *wS = new QPushButton(QIcon(colinIcons::instance().icon(Colin::Close).pixmap(32, QIcon::Normal, QIcon::On)), tr("exit without saving"), this);
	QPushButton *escape = new QPushButton(colinIcons::instance().icon(Colin::Close), tr("don't exit"), this);
    QScrollArea *scrollArea = new QScrollArea(this);
    QWidget *area = new QWidget(this);
    layout->addWidget(scrollArea, 0, 0, 1, 4);
    layout->addWidget(ok, 1, 0, 1, 1);
    layout->addWidget(sA, 1, 1, 1, 1);
    layout->addWidget(wS, 1, 2, 1, 1);
    layout->addWidget(escape, 1, 3, 1, 1);

    layout = new QGridLayout(area);
    layout->setVerticalSpacing(0);
    layout->setContentsMargins(2, 0, 2, 0);
    area->setMinimumWidth(200);
    scrollArea->setWidget(area);
    QPalette palette(area->palette());
    QLinearGradient backGrad(0, 0, 0, 32);
    backGrad.setColorAt(0, palette.color(QPalette::Base).lighter(120));
    backGrad.setColorAt(1, palette.color(QPalette::Base).darker(120));

    //QLinearGradient backGrad(0, 0, 0, 64);
    //backGrad.setColorAt(0, palette.color(QPalette::Base).lighter(120));
    //backGrad.setColorAt(0.4999999, palette.color(QPalette::Base).darker(120));
    //backGrad.setColorAt(0.5, palette.color(QPalette::AlternateBase).lighter(110));
    //backGrad.setColorAt(1, palette.color(QPalette::AlternateBase).darker(140));

    backGrad.setSpread(QGradient::RepeatSpread);
    QBrush backGround(backGrad);
    palette.setBrush(QPalette::Background, backGround);
    scrollArea->setPalette(palette);
    filelist &fL = filelist::instance();

    for(int i=0; i<fL.filecount(); i++)
    {
        checker.append(new QCheckBox(fL.filename(i)));
        if(!fL.undoGroup()->stacks().at(i)->canUndo())
        {
            checker[i]->setDisabled(true);
            checker[i]->setText(checker[i]->text()+" - "+tr("unchanged"));
        }
        else
        {
            checker[i]->setChecked(true);
        }
        layout->addWidget(checker[i], i, 0, 1, 1);
        layout->setRowMinimumHeight(i, 32);

    }
    area->setFixedHeight(checker.count()*32);
    scrollArea->setMaximumHeight(checker.count()*32+2);
    setWindowIcon(colinIcons::instance().icon(Colin::Save));
    setWindowTitle(tr("save changed files?"));

    connect(ok,         SIGNAL(clicked()),
            this,       SLOT(oke()));
    connect(sA,         SIGNAL(clicked()),
            this,       SLOT(saveAll()));
    connect(escape,     SIGNAL(clicked()),
            this,       SLOT(esc()));
    connect(wS,         SIGNAL(clicked()),
            this,       SLOT(closewS()));
}


void closeDialog::oke()
{
    filelist &fL = filelist::instance();
    for(int i=0; i<fL.filecount(); i++)
    {
        if(checker[i]->isChecked())
        {
            if(filelist::instance().filepath(i)=="")
            {
                fL.saveAs(i, getFileName());
            }
            else
                fL.save(i);
        }
    }
    accept();
}

void closeDialog::esc()
{
    reject();
}

void closeDialog::saveAll()
{
    filelist &fL = filelist::instance();
    for(int i=0; i<fL.filecount(); i++)
    {
        if(filelist::instance().filepath(i)=="")
        {
            fL.saveAs(i, getFileName());
        }
        else
            fL.save(i);
    }
    accept();
}

void closeDialog::closewS()
{
    accept();
}

QString closeDialog::getFileName()
{
    QString fileName;
    fileName = QFileDialog::getSaveFileName(
                this, tr("save file"), QDir::homePath(),
                tr("colin structs (*.xcs)"));

    if(fileName.isEmpty())
        return fileName;
    if(fileName.right(4) != QString(".xcs"))
    {
        if(!QFileInfo(fileName+".xcs").exists())
            fileName += QString(".xcs");
    }
    return fileName;
}

