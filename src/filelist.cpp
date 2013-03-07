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

#include "filelist.h"
#include "viewportsettings.h"
#include "wgv.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>

filelist* filelist::instance_ = NULL;

ColinStruct *filelist::currentFile()
{
    if(current_<0)
        return NULL;
    return files.at(current_).tw;
}

const int &filelist::currentIndex() const
{
    return current_;
}

QString filelist::filename(const int &i) const
{
    return files.at(i).filename;
}

QString filelist::filepath(const int &i) const
{
    return files.at(i).filepath;
}


void filelist::append(ColinStruct *f, QString name, QString path)
{
    if (name == "")
        name = QString(tr("untitled")+"_%1.cxs").arg(newfilecount++);

	f->QObject::setObjectName(name);

	cfile c = {f, name, path};
    files.append(c);
    QUndoStack *us = new QUndoStack(g);
    stacks.append(us);
    new UndoCommander(f, us, f);

    connect(f,              SIGNAL(beginS(QString)),
            this,           SLOT(beginSkript(QString)));

    connect(f,              SIGNAL(endS()),
            this,           SLOT(endSkript()));

    g->addStack(us);
    hideNew();

	emit fileOpened();
}

void filelist::closeFile(int i)
{
    if(i==-2)
        closeSettings();
    else if(i==-1)
    {
        hideNew();
    }
    else if(filecount() == 1)
    {
        showNew();
		delete files.takeAt(i).tw;
        g->removeStack(stacks.takeAt(i));
    }
    else
    {
        changeCurrentTo(i-1);
		delete files.takeAt(i).tw;
        g->removeStack(stacks.takeAt(i));
    }

    emit fileClosed();
}

int filelist::filecount() const
{
    return files.size();
}

void filelist::changeCurrentTo(ColinStruct* f)
{

    for(int i=0; i<files.size(); i++)
    {
        if(files.at(i).tw == f)
        {
            changeCurrentTo(i);
            return;
        }
    }
}

void filelist::changeCurrentTo(int i)
{
	if(i<-2 || i>=files.size())
		return;
    current_= i;


    if(i!=-1)
    {
        newVisible_ = false;
    }
    else
    {
	newVisible_ = true;
    }

	emit currentChanged(i>-1 ? files.at(i).tw: static_cast<ColinStruct*>(0));
    emit currentChanged(i);

    if(i>-1)
    {
        g->setActiveStack(stacks.at(i));
    }
    else
    {
        g->setActiveStack(NULL);
    }


}


void filelist::showNew()
{
    newVisible_ = true;
    changeCurrentTo(-1);
}

void filelist::showSettings()
{
    settingsVisible_ = true;
    newVisible_ = false;
    changeCurrentTo(-2);
}

/*
void filelist::showLib()
{
    libVisible_ = true;
    newVisible_ = false;
    changeCurrentTo(-3);
}
*/

void filelist::hideNew()
{
    if(!files.isEmpty())
    {
        newVisible_ = false;
        changeCurrentTo(files.size()-1);
    }
}

void filelist::closeSettings()
{
    settingsVisible_ = false;
    if(files.isEmpty())
        showNew();
    else
        changeCurrentTo(files.size()-1);
    emit deleteSWid();
}

/*
void filelist::closeLib()
{
    libVisible_ = false;
    if(files.isEmpty())
        showNew();
    else
        changeCurrentTo(files.size()-1);
    emit deleteLWid();
}
*/

int filelist::tabcount() const
{
    return filecount()
            +(newVisible_ ? 1 : 0)
            +(settingsVisible_ ? 1 : 0);
            //+(libVisible_ ? 1 : 0);
}

bool filelist::openT(const QString &url)
{
#ifndef QT_NO_DEBUG
	qDebug() << "filelist::open file \"" << url << "\""<< endl;
#endif
    QFile f(url);
    if(!f.exists())
    {
#ifndef QT_NO_DEBUG
		qDebug() << "filelist::file does not exist! :(" << endl;
#endif
        return false;
    }
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
#ifndef QT_NO_DEBUG
		qDebug() << "filelist::can not open file" << endl;
#endif
        return false;
    }
    foreach(cfile file, files)
    {
        if(file.filepath == url)
        {
            this->changeCurrentTo(file.tw);
#ifndef QT_NO_DEBUG
			qDebug() << "filelist::file already open! switching to workspace" << endl;
#endif
            return false;
        }
    }

	ColinStruct *tw = new ColinStruct();

    XmlReader Xr(tw);

    if(Xr.read(&f))
	{
		for(int i=0; i<tw->bls_n();i++)
			tw->setBLSColor(i, (i==0)?viewPortSettings::instance().firstStandardColor():
									  viewPortSettings::instance().nextStandardColor(tw->bls(i-1).color()));
        this->append(tw, QFileInfo(url).fileName(), url);
    }
    else
    {
        delete tw;


#ifndef QT_NO_DEBUG
		qDebug() << "erros while parsing/finished at:" << Xr.errors();
		if(Xr.errorString().isEmpty())
			qDebug() << "no errors";
#endif //QT_NO_DEBUG

        return false;
    }

    appendtoRecUsed(url);



#ifndef QT_NO_DEBUG
	qDebug() << endl << "erros while parsing/finished at:" << Xr.errors();
    if(Xr.errorString().isEmpty())
		qDebug() << "no errors";
#endif //QT_NO_DEBUG


    return true;
}

bool filelist::saveCurrent()
{
	return save(currentIndex());
}

bool filelist::saveCurrent(const QString &url)
{
    files[current_].filepath = url;
    files[current_].filename = QFileInfo(url).fileName();
	files[current_].tw->setObjectName(files[current_].filename);
	return saveCurrent();
}

bool filelist::save(const int &i)
{
    QFile f(files[i].filepath);
    if(!f.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate))
        return false;

    f.flush();
    XmlWriter Xw(&f);
	Xw.writeTw(*files.at(i).tw);

    f.close();
    appendtoRecUsed(files[i].filepath);
    return true;
}

bool filelist::saveAs(const int &i, const QString &url)
{
    files[i].filepath = url;
    files[i].filename = QFileInfo(url).fileName();
    emit recUsedCanged();
	return save(i);
}

void filelist::saveResults(const int &i, const QString &url)
{
	if(filelist::instance().currentFile()->bestimmt()<0)
		return;

	if(files[i].tw->isCalculated()){

		QFile f(url);
		if(!f.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate))
			return;

		f.flush();
		XmlWriter Xw(&f);
		Xw.writeTw(*files.at(i).tw);

		f.close();
		return;
	}
	else{

		wgv *cW = new wgv(filelist::instance().currentFile());

		resultpath.insert(cW, url);
		connect(cW,                 SIGNAL(calcFinished()),
				this,               SLOT(calcFinished()));
		cW->start();
		return;
	}
}



void filelist::calcFinished()
{
	wgv *cW = dynamic_cast<wgv*>(sender());
	if(!cW)
		return;
	int calculatedTw = -1;
	for(int i=0; i<filelist::instance().filecount(); i++)
	{
		if(cW->file() == filelist::instance().file(i))
		{
			calculatedTw = i;
			break;
		}
	}
	if(calculatedTw == -1)
		return;

	QFile f(resultpath[cW]);
	if(!f.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate))
		return;

	f.flush();
	XmlWriter Xw(&f);
	Xw.writeTw(*files.at(calculatedTw).tw);

	f.close();
	return;
}

void filelist::saveCurrentResults(const QString &url)
{
	saveResults(currentIndex(), url);
}

void filelist::loadRecUsed()
{
    QSettings settings("clazzes.org", "Colin");

    recUsed.clear();
    recUsed = settings.value("recentFileList").toStringList();


    foreach(QString s, recUsed)
    {
        if(!QFileInfo(s).exists())
            recUsed.removeAll(s);
    }

}

void filelist::appendtoRecUsed(const QString &url)
{
    recUsed.removeAll(url);
    recUsed.prepend(url);
    while(recUsed.size()>20)
        recUsed.removeLast();
    emit recUsedCanged();
}


QString filelist::recUsedFileName(const int &i) const
{
    if(i<recUsed.size())
        return recUsed.at(i);
    else
        return QString("");
}

void filelist::removeRecUsedFile(const int &i)
{
    if(i<recUsed.size())
        recUsed.removeAt(i);
    emit recUsedCanged();
}

void filelist::saveSettings()
{
    QSettings settings("clazzes.org", "Colin");

    settings.setValue("recentFileList", QVariant(recUsed));
}


void filelist::nextTab()
{
    if(tabcount()<2)
        return;
    if(tabcount()<3 && newVisible())
        return;
    if(newVisible())
    {
        if(files.count()!=0)
            changeCurrentTo(0);
        else if(settingsVisible())
            changeCurrentTo(-2);
        //else if(libVisible())
        //    changeCurrentTo(-3);
        else
            return;
    }
    else if(files.size()==current_+1)
    {
        if(settingsVisible())
            changeCurrentTo(-2);
        //else if(libVisible())
        //    changeCurrentTo(-3);
        else
            changeCurrentTo(0);
    }
    else if(current_==-2)
    {
        //if(libVisible())
        //    changeCurrentTo(-3);
        //else
            changeCurrentTo(0);
    }
    //else if(current_==-3)
    //{
    //    if(files.count()!=0)
    //        changeCurrentTo(0);
    //    else if(settingsVisible())
    //        changeCurrentTo(-2);
    //    else
    //        return;
    //}
    else
    {
        changeCurrentTo(current_+1);
    }
}

void filelist::previousTab()
{
    if(tabcount()<2)
        return;
    if(tabcount()<3 && newVisible())
        return;
    if(newVisible())
    {
        //if(libVisible())
        //    changeCurrentTo(-3);
        /*else*/
        if(settingsVisible())
            changeCurrentTo(-2);
        else
            changeCurrentTo(files.size()-1);
    }
    //else if(current_ == -3)
    //{
    //    if(settingsVisible())
    //        changeCurrentTo(-2);
    //    else
    //        changeCurrentTo(files.size()-1);
    //}
    else if (current_ == -2)
    {
        if(files.count()!=0)
            changeCurrentTo(files.size()-1);
        //else if(libVisible())
        //    changeCurrentTo(-3);
        else
            return;
    }
    else if(current_ == 0)
    {
        //if(libVisible())
        //   changeCurrentTo(-3);
        /*else*/
        if(settingsVisible())
            changeCurrentTo(-2);
        else
            changeCurrentTo(files.size()-1);
    }
    else
    {
        changeCurrentTo(current_-1);
    }

}
