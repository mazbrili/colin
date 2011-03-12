/***************************************
 * singletone for a list of open files *
 ***************************************/

#ifndef FILELIST_H
#define FILELIST_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtGui/QUndoGroup>
#include <QtCore/QPair>

#include "wgv_tw.h"
#include "undocommandmaker.h"
#include "xmlwriter.h"  //save and
#include "xmlreader.h"  //open


struct cfile
{
    wgv_tw *tw;
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

    wgv_tw *currentFile();
    wgv_tw *file(const int &i) const {return files.at(i).tw;}
    const int &currentIndex() const;
    QString filename(const int &i) const;
    QString filepath(const int &i) const;

    void append(wgv_tw *f, QString name = "", QString path = "");

    void removeAt(int i);

    int filecount() const;
    int tabcount() const;
    const inline bool &newVisible() const { return newVisible_;}
    const inline bool &settingsVisible() const {return settingsVisible_;}

    QUndoGroup *undoGroup(){return g;}

    QString recUsedFileName(const int &i) const;
    void removeRecUsedFile(const int &i);

signals:
    void currentChanged(wgv_tw*);
    void currentChanged(int i);
    void deleteSWid();
    void recUsedCanged();
    void fileClosed();


public slots:
    void changeCurrentTo(wgv_tw* f);
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


    static filelist* instance_;

};

#endif // FILELIST_H
