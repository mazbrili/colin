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

//Dear future me. Please forgive me.
//I can't even begin to express how sorry I am.


#warning this is the working branch for Colin 0.2.
#warning please use Colin 0.1.10 from trunk
#warning contains unfinshed features
#warning contains untestes features


#include "mainwindow.h"
#include "toolTipEater.h"

#include <QtGui/QApplication>
#include <QtCore/QDir>
#include <QtCore/QTranslator>
#include <QtCore/QLibraryInfo>
#include <QtCore/QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    toolTipEater NomNomMampfMampf;
    a.installEventFilter(&NomNomMampfMampf); //disable tooltips
    a.setAttribute(Qt::AA_DontShowIconsInMenus, false); //enable icons

#ifdef WIN32
	colinIcons::icondir_ = a.applicationDirPath() + "/../share/icons/";
	qDebug() << "using ../share/icons as icon directory";

#else
	QDir iconDir("./../");
	if (iconDir.exists("./icons")){
		colinIcons::icondir_ = "../icons/";
		qDebug() << "using ../icons as icon directory";
	}
		else{
		colinIcons::icondir_ = "/usr/share/colin/icons/";
#ifndef QT_NO_DEBUG
		qDebug() << "unable to find icon directory in " << iconDir.absolutePath();
		qDebug() << "using /usr/share/colin/icons as icon directory!";
#endif
	}

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
