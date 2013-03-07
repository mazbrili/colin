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

/***************************************
 * singletone for a list of open files *
 ***************************************/

#ifndef FILELIST_H
#define FILELIST_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtGui/QUndoGroup>
#include <QtCore/QPair>
#include <QtCore/QMap>

#include "colinstruct.h"
#include "undocommandmaker.h"
#include "xmlwriter.h"  //save and
#include "xmlreader.h"  //open
#include "scriptengine.h"


struct cfile
{
	ColinStruct *tw;
    QString filename;
	QString filepath;
};

class filelist : public QObject
{
	Q_OBJECT


public:
    static filelist& instance()
    {
        if(!instance_)
            instance_ = new filelist();
        return *instance_;
    }

	Q_INVOKABLE ColinStruct *currentFile();
	ColinStruct *file(const int &i) const {return files.at(i).tw;}
	Q_INVOKABLE const int &currentIndex() const;
	Q_INVOKABLE QString filename(const int &i) const;
	Q_INVOKABLE QString filepath(const int &i) const;

	void append(ColinStruct *f, QString name = "", QString path = "");

	Q_INVOKABLE void closeFile(int i);

	Q_INVOKABLE int filecount() const;
	Q_INVOKABLE int tabcount() const;
	Q_INVOKABLE const inline bool &newVisible() const { return newVisible_;}
	Q_INVOKABLE const inline bool &settingsVisible() const {return settingsVisible_;}

    QUndoGroup *undoGroup(){return g;}

	Q_INVOKABLE QString recUsedFileName(const int &i) const;
	Q_INVOKABLE void removeRecUsedFile(const int &i);

signals:
	void currentChanged(ColinStruct*);
    void currentChanged(int i);
    void deleteSWid();
    void recUsedCanged();
    void fileClosed();
	void fileOpened();


public slots:
	void changeCurrentTo(ColinStruct* f);
    void changeCurrentTo(int i);

    void showNew();
    void showSettings();
    //void showLib();

    void hideNew();
    void closeSettings();
    //void closeLib();

	void nextTab();
    void previousTab();

	bool saveCurrent();
	bool saveCurrent(const QString &url);
    bool openT(const QString &url);
	bool save(const int &i);
	bool saveAs(const int &i, const QString &url);
	void saveResults(const int &i, const QString &url);
	void saveCurrentResults(const QString &url);
	void calcFinished();

    void saveSettings();


private slots:
    void beginSkript(const QString &text) const { g->activeStack()->beginMacro(text);}
    void endSkript() const { g->activeStack()->endMacro();}



private:
    Q_DISABLE_COPY(filelist)
    filelist():QObject(){
        current_ = -1;
        newVisible_ = true;
        settingsVisible_ = false;
        //libVisible_ = false;
        newfilecount = 0;
        g = new QUndoGroup(this);
        loadRecUsed();}
    void loadRecUsed();
    void appendtoRecUsed(const QString &url);

    QList<cfile> files;
    QList <QUndoStack*> stacks;
    QStringList recUsed;
    QUndoGroup *g;
    int current_;
    bool newVisible_;
    bool settingsVisible_;
    //bool libVisible_;
    int newfilecount;
	QMap<QObject*, QString> resultpath;

    static filelist* instance_;


};

#endif // FILELIST_H
