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

#ifndef SHORTCUTSETTINGSWIDGET_H
#define SHORTCUTSETTINGSWIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QPushButton>
#include <QtGui/QButtonGroup>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMessageBox>
#include <QtGui/QKeyEvent>

#include "colinicons.h"
#include "shortcutsettings.h"
#include "colinboolslider.h"

class shortcutSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit shortcutSettingsWidget(QWidget *parent = 0);

signals:

public slots:
    void buttonClicked(const int &i);
    void setShortCut();
    void restoreShortCut(const int &i);
private:
    QButtonGroup *setter;
    QButtonGroup *restorer;
    QLineEdit *editor;
    QGridLayout *layout;

    QList<QLabel*> shortcuts;
    int old;

	ColinBoolSlider *tools;
	QLabel *toolLabel;
};

#endif // SHORTCUTSETTINGSWIDGET_H
