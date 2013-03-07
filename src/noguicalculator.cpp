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

#include "noguicalculator.h"

#include <iostream>

#include <QtGui/QApplication>
#include <QtCore/QStringList>
#include <QtCore/QFileInfo>

#include "filelist.h"

noguicalculator::noguicalculator(QObject *parent) :
    QObject(parent)
{
	connect(&filelist::instance(),		SIGNAL(allCalcFinished()),
			qApp,						SLOT(quit()));
	for(int i=0; i<qApp->argc(); i++)
	{
		QString arg = qApp->arguments()[i];
		if(arg.isEmpty())
			continue;
		if(filelist::instance().openT(QFileInfo(arg).canonicalFilePath())){
			if(qApp->argc() > i+2){
				if(qApp->arguments()[i+1] == "-o"){
					std::cout << "calculating and saving as " << qApp->arguments()[i+2].toStdString();
					i+=2;
					filelist::instance().saveCurrentResults(qApp->arguments()[i]);
				}
			}
		}
	}
}
