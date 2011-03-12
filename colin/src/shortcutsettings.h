#ifndef SHORTCUTSETTINGS_H
#define SHORTCUTSETTINGS_H

#include <QtCore/QList>
#include <QtCore/QSettings>
#include <QtGui/QAction>
#include "colin_namespace.h"

class act{
public:
    act(QAction *a_, const QKeySequence &system_, const bool &nF)
        {a=a_;system=system_;reqFile=nF;}
    QAction *a;
    QKeySequence system;
    bool reqFile;
};

class shortcutSettings
{
public:
    static shortcutSettings &instance()
    {
        if(!instance_)
            instance_ = new shortcutSettings();
        return *instance_;
    }

    void addAction(QAction *a, const bool &needsFile = false);
    void setShortut(const int &i, const QKeySequence &c);
    void restore(const int &i);
    void loadSettings();
    void saveSettings() const;
    const act &action(const int &i)const {return actions_.at(i);}
    QAction *actionWithData(const Colin::otherAction &a)const;
    int Count() const {return actions_.size();}
    const QList<act> &actions() const {return actions_;}
    void hasFile(const bool &t);

signals:

public slots:

private:
    shortcutSettings(){}

    QList<act> actions_;
    static shortcutSettings *instance_;
};

#endif // SHORTCUTSETTINGS_H
