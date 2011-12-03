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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QToolBar>
#include <QtGui/QApplication>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStackedWidget>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QDockWidget>
#include <QtGui/QUndoView>
#include <QtGui/QClipboard>

#include "nwidget.h"    //startpage
#include "swidget.h"    //settings
#include "ctabbar.h"    //tabbar
#include "ctoolbar.h"   //toolbar
#include "viewmenu.h"   //zoom of u, N, Q, M
#include "viewmenu2.h"  //paging, etc
#include "lmenu.h"      //Menu for load action
#include "bmenu.h"      //Menu for bearing action
#include "jmenu.h"      //Menu for joint action
#include "cmenu.h"      //Menu for catch action
#include "zmenu.h"      //Menu for zoom action
#include "colintooltip.h"
#include "shortcutsettings.h"
//#include "psidewidget.h"
#include "treeview.h"
#include "libsidebar.h"
#include "wgv.h"
#include "structprinter.h"
//#include "clswidget.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void keyPressEvent(QKeyEvent *e);
	void closeEvent(QCloseEvent *e);


    void autoOpen();



private:
    void init();                //init :)
    void initToolBar();
    void initMenu();

public slots:
	void setTw(ColinStruct *tw);          //change current editable file
    void newT();                     //creates a new file
    void newTab();                   //ne tab. shows the startertab
    void closeTab();                 //closes the current tab
    void settings();                 //creates a settingswidget and pushs it on cStack
    void closeSettings();            //removes the settingswidget and destroys it
    void setCWidget(int i);          //sets the widget of the central stackedWidget
    void launchViewMenu2();          //viewports(hide, show, etc)
    void actionSelected(QAction *a); //connected with the actionGroup in the toolbar.
                                     //sets viewPortSettings::toDraw

    //void setCalculationIcon();

    void calculate();
    void calcFinished();

    void saveCurrent();
    void openT();
    void saveAsT();

    void launchToolTip();
    void aboutMe();
    void aboutQt();

    void selectAll();
    void invertSelection();
    void deselectAll();

    void zoomAll();

    void deleteSelection();

    void hideSideBars(const int &i);
    void fullScreen(const bool &on);

    void copy();
    void cut();
    void paste();

    void copyBase();
    void cutBase();

    void copyBase(const QPointF &p);

    void delLib();
    void activateLibDelButton();

    void print();

    void zoomIn();
    void zoomOut();

	void setActionMenus(bool beside);

//	void loadset();

private:
    QStackedWidget *cStack;     //will become the centralwidget
    cToolBar *toolbar;          //the only toolbar with all actions and menus
    cTabBar *tab;               //the tabbar, displaying the open files

    cWidget *centralWidget;     //centralWidget, contains the viewports
    nWidget *newWidget;         //starterWidget
    sWidget *settingsWidget;    //these 4 will added to the central stackedwidget


    viewMenu *viewm;            //zomm and stuff
    viewMenu2 *viewm2;          //viewportoption (count, hidden, etc..)
    QAction *undoA,             //actions generated by the undostack
            *redoA,
            *stdA,
            *snapA,
            *calcA;

    colinToolTip *toolBarTip,
                 *calcInfo;

    QTimer *calcInfoHider;
    //QDockWidget *propertySideBar;
	//QDockWidget *treeSideBar;
	//QDockWidget *historySideBar;
	//QDockWidget *libSideBar;

	//treeView *tV;

	ColinStruct *currentTw;

    QRect geometryBeforeFullScreen;
    bool wasMaximized;

	//bool treeShown;
	//bool propertysShown;
	//bool historyShown;
	//bool libShown;

    bool removeSelectionAfterCopy;

};

#endif // MAINWINDOW_H
