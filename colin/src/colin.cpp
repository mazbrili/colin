//Dear future me. Please forgive me.
//I can't even begin to express how sorry I am.


#include "mainwindow.h"
#include "toolTipEater.h"

#include <QtGui/QApplication>
#include <QtCore/QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    toolTipEater NomNomMampfMampf;
    a.installEventFilter(&NomNomMampfMampf); //disable tooltips
    a.setAttribute(Qt::AA_DontShowIconsInMenus, false); //enable icons

#ifdef WIN32
    colinIcons::icondir_ = a.applicationDirPath() + "/../share/icons/";
#else
    QDir iconDir("./icons");
    if (iconDir.exists("./icons"))
      colinIcons::icondir_ = "./icons/";
    else
      colinIcons::icondir_ = "/usr/share/colin/icons/";
#endif

    QSettings settings("clazzes.org", "Colin");
    QString langKey = "";

    if(settings.value("language", "guess").toString()=="guess")
    {
        QLocale locale;
        if (locale.language() == QLocale::German)
        {
          langKey = "de";
        }
    }
    else
    {
        if(settings.value("language").toString()=="Deutsch")
            langKey = "de";
    }
    QTranslator translator_qt;
    QTranslator translator_colin;
    if (!langKey.isEmpty())
    {
#ifdef WIN32
          QString translations_dir_qt = a.applicationDirPath()+QDir::separator()+".."+QDir::separator()+"share"+QDir::separator()+"translations";
          QString translations_dir_colin = a.applicationDirPath()+QDir::separator()+".."+QDir::separator()+"share"+QDir::separator()+"translations";
#else
          QString translations_dir_qt = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
          QString translations_dir_colin = "/usr/share/colin/translations";
#endif
        if (translator_qt.load(QString("%1/qt_%2.qm").arg(translations_dir_qt).arg(langKey)))
        {
            a.installTranslator(&translator_qt);
            QTextStream err(stderr);
            err << "translation file\"" << QString("%1/qt_%2.qm").arg(translations_dir_qt).arg(langKey) << "\" successfully loaded!" << endl;
        }
        else
        {
            QTextStream err(stderr);
            err << "failed to load translation file\"" << QString("%1/qt_%2.qm").arg(translations_dir_qt).arg(langKey) << "\"" << endl;
        }

        if (translator_colin.load(QString("colin_%1.qm").arg(langKey),translations_dir_colin))
        {
            a.installTranslator(&translator_colin);
        }
    }

    //show GUI
    MainWindow w;
    w.showMaximized();


    //run threads and event loop
    previewWidget::renderer->start(QThread::IdlePriority);
    return a.exec();
}
