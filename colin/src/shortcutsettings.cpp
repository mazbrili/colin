#include "shortcutsettings.h"


shortcutSettings *shortcutSettings::instance_ = NULL;



void shortcutSettings::addAction(QAction *a, const bool &needsFile)
{

    QSettings settings("clazzes.org", "Colin");
    QKeySequence key = a->shortcut();
    a->setShortcut(QKeySequence(settings.value(QString("shortcuts/%1").arg(a->text()),
					       QVariant(key.toString())).toString()));

    actions_.append(act(a, key, needsFile));
}

void shortcutSettings::setShortut(const int &i, const QKeySequence &c)
{
    actions_.at(i).a->setShortcut(c);
}

void shortcutSettings::restore(const int &i)
{
    actions_.at(i).a->setShortcut(actions_.at(i).system);
}

void shortcutSettings::loadSettings()
{
    for(int i=0; i<actions_.size(); i++)
        restore(i);
}

QAction *shortcutSettings::actionWithData(const Colin::otherAction &a)const
{
    foreach(act ac, actions_)
    {
        Colin::otherAction  data = static_cast<Colin::otherAction>(ac.a->data().toInt());
        if(data == a)
        {
            return ac.a;
        }
    }
    return 0;
}

void shortcutSettings::saveSettings() const
{

    QSettings settings("clazzes.org", "Colin");
    for(int i=0; i<actions_.size(); i++)
    {
        settings.setValue(QString("shortcuts/%1").arg(actions_.at(i).a->text()),QVariant(actions_.at(i).a->shortcut().toString(QKeySequence::PortableText)));
    }
}

void shortcutSettings::hasFile(const bool &t)
{
    foreach(act a, actions_)
    {
        if(a.reqFile)
            a.a->setDisabled(!t);
    }
}
