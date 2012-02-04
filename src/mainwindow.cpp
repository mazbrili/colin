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

#include "mainwindow.h"
#include "unitsettings.h"
#include "closedialog.h"
#include "colinversion.h"

#include <QtCore/QTimer>
#include <QtGui/QPrintDialog>
#include <QtGui/QPrinter>
#include <QtGui/QToolButton>
#include <QtGui/QWhatsThis>
#include <QtScriptTools/QScriptEngineDebugger>
#include <QtGui/QDesktopServices>
#include <QtCore/QUrl>



#include "historymenu.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{

	init();
	initMenu();
	initToolBar();

	QSettings settings("clazzes.org", "Colin");
	restoreGeometry(settings.value("MainWindow/geometry").toByteArray());
	restoreState(settings.value("MainWindow/windowState").toByteArray());

	//for(int i=0; i<5; i++)
	//{
	//	tV->setColumnWidth(i, settings.value(QString("tree/columnwidth%1").arg(i), 60).toInt());
	//}
	ColinStruct dummy;
	currentTw = &dummy;
	this->calcInfo->hide();

	setTw(NULL);
	autoOpen();

}

MainWindow::~MainWindow()
{

}

void MainWindow::init()
{
	currentTw = NULL;

	QIcon wIcon(icondir + "tooltip/node_dark_64.png");

	wIcon.addFile(icondir +"tooltip/node_dark_64.png", QSize(128, 128));

	setWindowIcon(wIcon);
	setWindowTitle(tr("Colin - 42"));

	cStack = new QStackedWidget(this);
	setCentralWidget(cStack);

	newWidget = new nWidget(this);
	centralWidget = new cWidget(this);

	settingsWidget = NULL;

	cStack->addWidget(newWidget);
	cStack->addWidget(centralWidget);

	shortcutSettings &scS = shortcutSettings::instance();

	//next and previous child

	QAction *nextTab = new QAction(tr("next tab"), this);
	nextTab->setShortcut(QKeySequence::NextChild);
	scS.addAction(nextTab);

	QAction *previousTab = new QAction(tr("previous tab"), this);
	previousTab->setShortcut(QKeySequence::PreviousChild);
	scS.addAction(previousTab);

	addAction(nextTab);
	addAction(previousTab);

	// zoom in and out with ctrl++ and ctrl+-

	QAction *zoomIn = new QAction(tr("zoom in"), this);
	zoomIn->setShortcut(QKeySequence::ZoomIn);
	zoomIn->setIcon(colinIcons::instance().icon(Colin::drawZoomIn));
	scS.addAction(zoomIn);

	QAction *zoomOut = new QAction(tr("zoom out"), this);
	zoomOut->setShortcut(QKeySequence::ZoomOut);
	zoomOut->setIcon(colinIcons::instance().icon(Colin::drawZoomOut));
	scS.addAction(zoomOut);

	addAction(zoomIn);
	addAction(zoomOut);



/*
	pSideWidget *pS = new pSideWidget(this);
	propertySideBar = new QDockWidget("propertys"), this);
	propertySideBar->setWidget(pS);
	propertySideBar->setObjectName("propertySideBar");
	addDockWidget(Qt::RightDockWidgetArea, propertySideBar);
*//*
	tV = new treeView(this);
	treeSideBar = new QDockWidget(tr("tree structure"), this);
	treeSideBar->setWidget(tV);
	treeSideBar->setObjectName("tree model");
	addDockWidget(Qt::LeftDockWidgetArea, treeSideBar);

	historySideBar = new QDockWidget(tr("history"), this);
	historySideBar->setWidget(new QUndoView(filelist::instance().undoGroup(), this));
	historySideBar->setObjectName("history");
	addDockWidget(Qt::RightDockWidgetArea, historySideBar);

	libSideBar = new QDockWidget(tr("library"), this);
	libSideBar->setWidget(new libSidebar(this));
	libSideBar->setObjectName("library");
	addDockWidget(Qt::RightDockWidgetArea, libSideBar);
*/

	calcInfo = new colinToolTip(this);
	calcInfoHider = new QTimer(this);

	connect(calcInfoHider,                      SIGNAL(timeout()),
			calcInfo,                           SLOT(hide()));

	connect(nextTab,                            SIGNAL(triggered()),
			&filelist::instance(),              SLOT(nextTab()));

	connect(previousTab,                        SIGNAL(triggered()),
			&filelist::instance(),              SLOT(previousTab()));

	connect(&filelist::instance(),              SIGNAL(currentChanged(ColinStruct*)),
			this,                               SLOT(setTw(ColinStruct*)));

	connect(&filelist::instance(),              SIGNAL(fileClosed()),
			this,                               SLOT(activateLibDelButton()));

	connect(&filelist::instance(),              SIGNAL(currentChanged(int)),
			this,                               SLOT(setCWidget(int)));

	connect(&filelist::instance(),              SIGNAL(currentChanged(int)),
			this,                               SLOT(hideSideBars(int)));

	connect(newWidget->newB,                    SIGNAL(clicked()),
			this,                               SLOT(newT()));

	connect(newWidget->openB,                   SIGNAL(clicked()),
			this,                               SLOT(openT()));

	connect(newWidget->libB,                    SIGNAL(clicked()),
			this,                               SLOT(delLib()));

	connect(newWidget->settingsB,               SIGNAL(clicked()),
			&filelist::instance(),              SLOT(showSettings()));

	connect(centralWidget,                      SIGNAL(basePoint(QPointF)),
			this,                               SLOT(copyBase(QPointF)));

	//connect(filelist::instance().undoGroup(),   SIGNAL(indexChanged(int)),
	//        this,                               SLOT(setCalculationIcon()));

	//connect(filelist::instance().undoGroup(),   SIGNAL(activeStackChanged(QUndoStack*)),
	//        this,                               SLOT(setCalculationIcon()));

	connect(zoomIn,                             SIGNAL(triggered()),
			this,                               SLOT(zoomIn()));

	connect(zoomOut,                            SIGNAL(triggered()),
			this,                               SLOT(zoomOut()));
/*
	QWidget *r = new resultVisualWidget(this);
	this->installEventFilter(r);
	r->hide();
*/

}

void MainWindow::initMenu()
{


	shortcutSettings &scS = shortcutSettings::instance();

	/*****************************
	 *         filemenu          *
	 *****************************/

	QMenu *filem = new QMenu(tr("&file", "menu | filemenu(don't remve the \"&\""), this);

	scS.addAction(
			filem->addAction(tr("new tab"),
									  this, SLOT(newTab()), QKeySequence::AddTab));
	scS.lastAction()->setWhatsThis("<b>"+tr("new tab")+"</b><br/>"+
								   tr("opens the starter widget<br/>")+
								   tr("<a href=\"startwidget\">more...</a><br />"));

	scS.addAction(
			filem->addAction(tr("new"),
							 this, SLOT(newT()), QKeySequence::New));
	scS.lastAction()->setWhatsThis("<b>"+tr("new")+"</b><br/>"+
								   tr("creates a new empty file"));

	scS.addAction(
			filem->addAction(colinIcons::instance().icon(Colin::Open), tr("open"),
							this, SLOT(openT()), QKeySequence::Open));
	scS.lastAction()->setWhatsThis("<b>"+tr("open")+"</b><br/>"+
								   tr("open a file saved on your disk or any other device!"));
	filem->addSeparator();

	//scS.addAction(
	//		filem->addAction(tr("print"),
	//						 this, SLOT(print()), QKeySequence::Print), true);
	//filem->addSeparator();

	scS.addAction(
			filem->addAction(colinIcons::instance().icon(Colin::Save), tr("save"),
							 this, SLOT(saveCurrent()), QKeySequence::Save), true);
	scS.lastAction()->setWhatsThis("<b>"+tr("save")+"</b><br/>"+
								   tr("save the current file to your disk or any other device!")+
								   tr("The file is saved without asking for the filename, if already named!"));

	scS.addAction(
			filem->addAction(tr("save as"),
							 this, SLOT(saveAsT()), QKeySequence::SaveAs), true);
	scS.lastAction()->setWhatsThis("<b>"+tr("save")+"</b><br/>"+
								   tr("save the current file to your disk or any other device under a new name!"));
	filem->addSeparator();

	scS.addAction(
			filem->addAction(tr("close tab"),
							 this, SLOT(closeTab()),
							 QKeySequence(tr("Ctrl+W", "action | close tab"))));
	scS.lastAction()->setWhatsThis("<b>"+tr("close tab")+"</b><br/>"+
								   tr("close the current tab!"));

	scS.addAction(
			filem->addAction(colinIcons::instance().icon(Colin::Close), tr("quit"),
							 this, SLOT(close()), QKeySequence::Quit));
	scS.lastAction()->setWhatsThis("<b>"+tr("quit")+"</b><br/>"+
								   tr("close the application!"));


	menuBar()->addMenu(filem);


	/*****************************
	 *         editmenu          *
	 *****************************/

	//undothings
	QMenu *editm = new QMenu(tr("&edit", "menu | editmenu(don't remve the \"&\""), this);

	undoA = filelist::instance().undoGroup()->createUndoAction(this);
	undoA->setIcon(colinIcons::instance().icon(Colin::Undo));
	undoA->setShortcut(QKeySequence::Undo);
	editm->addAction(undoA);
	redoA = filelist::instance().undoGroup()->createRedoAction(this);
	redoA->setIcon(colinIcons::instance().icon(Colin::Redo));
	redoA->setShortcut(QKeySequence::Redo);
	editm->addAction(redoA);
	editm->addSeparator();


	//clipboardthings
	QAction *act;
	act = editm->addAction(colinIcons::instance().icon(Colin::Copy), tr("copy"),
						   this, SLOT(copy()), QKeySequence::Copy);
	act->setData(Colin::Copy);
	act->setWhatsThis(tr("<b>copy tool</b> ")+
					  tr("<a href=\"tool/copy\">open manual</a><br /><br />")+
					  tr("copy the current selection to the clipboard!"));
	scS.addAction(act , true);

	scS.addAction(
			editm->addAction(colinIcons::instance().icon(Colin::Copy), tr("copy with basepoint"),
							 this, SLOT(copyBase()), QKeySequence(tr("Ctrl+Shift+C", "action | copy with basepoint"))), true);
	scS.lastAction()->setWhatsThis("<b>"+tr("copy with basepoint")+"</b> "+
								  tr("<a href=\"menu/copybasepoint\">open manual</a><br /><br />")+
								  tr("copy the current selection to the clipboard. ")+
								  tr("You have to specify a point in the view. ")+
								  tr("This point will be used as reference when pasting from clipboard!"));



	act = editm->addAction(colinIcons::instance().icon(Colin::Cut), tr("cut"),
					 this, SLOT(cut()), QKeySequence::Cut);
	act->setData(Colin::Cut);
	act->setWhatsThis(tr("<b>cut</b> ")+
					  tr("<a href=\"menu/cut tool\">open manual</a><br /><br />")+
					  tr("put the current selection to the clipboard and remove it from current file!"));
	scS.addAction(act, true);

	scS.addAction(
			editm->addAction(colinIcons::instance().icon(Colin::Cut), tr("cut with basepoint"),
							 this, SLOT(cutBase()), QKeySequence(tr("Ctrl+Shift+X", "action | cut with basepoint"))), true);

	scS.lastAction()->setWhatsThis(tr("<b>cut with basepoint</b> ")+
								   tr("<a href=\"menu/cut tool\">open manual</a><br /><br />")+
								   tr("put the current selection to the clipboard and remove it from current file! ")+
								   tr("You have to specify a point in the view. ")+
								   tr("This point will be used as reference when pasting from clipboard!"));

	act = editm->addAction(colinIcons::instance().icon(Colin::Paste), tr("paste"),
							 this, SLOT(paste()), QKeySequence::Paste);
	act->setData(Colin::Paste);
	scS.addAction(act, true);

	scS.lastAction()->setWhatsThis(tr("<b>paste tool</b> ")+
								   tr("<a href=\"menu/paste tool\">open manual</a><br /><br />")+
								   tr("Add content from the clipboard to your current structure. ")+
								   tr("You have to specify the position where to inert the content in one of the views."));


	editm->addSeparator();

	//editsubmenu edit->manipulate
	/*
	QMenu *manim = new QMenu(tr("manipulate"), this);

	scS.addAction(
			manim->addAction(tr("translate"),
							 this, SLOT(mov()), QKeySequence(tr("Ctrl+M", "action | translate"))));

	scS.addAction(
			manim->addAction(tr("rotate"),
							 this, SLOT(mov()), QKeySequence(tr("Ctrl+Shift+R", "action | rotate"))));

	scS.addAction(
			manim->addAction(tr("multiply"),
							 this, SLOT(mov()), QKeySequence(tr("Ctrl+*", "action | multiply"))));

	editm->addMenu(manim);
	editm->addSeparator();
	*/
	//selectthings

	scS.addAction(
			editm->addAction(tr("select all"),
							 this, SLOT(selectAll()), QKeySequence::SelectAll), true);
	scS.lastAction()->setWhatsThis(tr("<b>select all</b> ")+
								   tr("<a href=\"menu/selectall\">open manual</a><br /><br />")+
								   tr("mark all objects of the current file as selected!"));

	scS.addAction(
			editm->addAction(tr("invert selection"),
							 this, SLOT(invertSelection()),
							 QKeySequence(tr("Ctrl+I", "action | invert selection"))), true);

	scS.lastAction()->setWhatsThis(tr("<b>invert selection</b> ")+
								   tr("<a href=\"menu/invert selection\">open manual</a><br /><br />")+
								   tr("mark all unmarked objects and unmark all markes objects!"));

	scS.addAction(
			editm->addAction(tr("deselct all"),
							 this, SLOT(deselectAll()),
							 QKeySequence(tr("Ctrl+Shift+A", "action | deselect"))), true);


	scS.lastAction()->setWhatsThis(tr("<b>invert selection</b> ")+
								   tr("<a href=\"menu/deselect all\">open manual</a><br /><br />")+
								   tr("unmark all objects. No object will be selected afterwards!"));

	editm->addSeparator();

	//delete

	scS.addAction(
				  editm->addAction(tr("delete"),
								   this, SLOT(deleteSelection()), QKeySequence::Delete));

	scS.lastAction()->setWhatsThis(tr("<b>delete</b> ")+
								   tr("<a href=\"menu/delete\">open manual</a><br /><br />")+
								   tr("Removes the currently selected objects from structure!"));

	editm->addSeparator();

	//settings
	scS.addAction(
			editm->addAction(colinIcons::instance().icon(Colin::Settings), tr("settings"),
							 &filelist::instance(), SLOT(showSettings()),
							 QKeySequence(tr("Ctrl+Space", "(Ctrl+Space) action | Settings"))));

	scS.lastAction()->setWhatsThis(tr("<b>settings</b> ")+
								   tr("<a href=\"menu/settings\">open manual</a><br /><br />")+
								   tr("Opens a Window where you can edit some settings! <a href=\"settings\">more...</a><br /><br />"));


	menuBar()->addMenu(editm);

	/*****************************
	 *        windowsmenu        *
	 *****************************/
/*
	QMenu *windowm = new QMenu(tr("&windows", "menu | windowmenu(don't remve the \"&\""), this);
	QAction *action = windowm->addAction(tr("tree structure"));
	action->setCheckable(true);
	scS.addAction(action, true);
	connect(action,             SIGNAL(triggered(bool)),
			treeSideBar,        SLOT(setShown(bool)));
	connect(treeSideBar,        SIGNAL(visibilityChanged(bool)),
			action,             SLOT(setChecked(bool)));
	action = windowm->addAction(tr("library"));
	action->setCheckable(true);
	scS.addAction(action, true);
	connect(action,             SIGNAL(triggered(bool)),
			libSideBar,         SLOT(setShown(bool)));
	connect(libSideBar,         SIGNAL(visibilityChanged(bool)),
			action,             SLOT(setChecked(bool)));
	action = windowm->addAction(tr("history"));
	action->setCheckable(true);
	scS.addAction(action, true);
	connect(action,             SIGNAL(triggered(bool)),
			historySideBar,     SLOT(setShown(bool)));
	connect(historySideBar,     SIGNAL(visibilityChanged(bool)),
			action,             SLOT(setChecked(bool)));

	menuBar()->addMenu(windowm);
*/
	/*****************************st::instance().currentFile();
	QClipboard *clipboard = QApplication::clipboard();
	QByteArray data;
	QMimeData mimeData;;
	XmlWriter writer(&data);
	 *        aboutmenu          *
	 *****************************/

	QMenu *jsMenu = new QMenu("javascript");
	jsMenu->addAction(QIcon(), tr("run script"), this, SLOT(runScript()));
	jsMenu->addAction(QIcon(), tr("debug script"), this, SLOT(runDebug()));

	menuBar()->addMenu(jsMenu);

	QMenu *aboutm = new QMenu(tr("&about", "menu | aboutmenu(don't remve the \"&\""), this);
//    scS.addAction(
//            aboutm->addAction(tr("documentation"),
//                              this, SLOT(documentation()), QKeySequence::HelpContents));
//    aboutm->addSeparator();
	aboutm->addAction("manual", this, SLOT(manual()));
	aboutm->addAction("Colin", this, SLOT(aboutMe()));
	aboutm->addAction("Qt", this, SLOT(aboutQt()));
	menuBar()->addMenu(aboutm);


}

void MainWindow::initToolBar()
{

	shortcutSettings &scS = shortcutSettings::instance();

	toolBarTip = new colinToolTip(this);
	toolBarTip->hide();

	viewm = new viewMenu(this);                      //viewoptionMenu1
	viewm2 = new viewMenu2(centralWidget, this);     //viewoptionMenu2

	toolbar = new cToolBar(tr("toolbar"), this);
	QActionGroup *aGroup = new QActionGroup(this);
	aGroup->setExclusive(true);
	toolbar->setIconSize(colinIcons::iconSize());
	//nodes
	QAction *action = new QAction(colinIcons::instance().icon(Colin::drawNode), "draw Nodes", this);
	action->setData(Colin::drawNode);
	action->setShortcut(QKeySequence(tr("Ctrl+K", "Action | drawNode")));
	toolbar->addAction(action);
	aGroup->addAction(action);
	scS.addAction(action);

	scS.lastAction()->setWhatsThis(tr("<b>node tool</b> ")+
								  tr("<a href=\"tool/node\">open manual</a><br /> <br />")+
								  tr("Use this tool to add nodes. ")+
								  tr("<a href=\"nodes\">more...</a><br />")+
								  tr("To do so click on the designated position in the view. ")+
								  tr("<a href=\"view\">more...</a><br />")+
								  tr("The <b>snap</b> will help you! ")+
								  tr("<a href=\"snap\">more...</a><br />")+
								  tr("Afterwards you can add supports to the node with the <b>support tool</b>. ")+
								  tr("<a href=\"tool/support\">more...</a><br />"));


	//beam
	action = new QAction(colinIcons::instance().icon(Colin::drawBeam), "draw beams", this);
	action->setData(Colin::drawBeam);
	action->setShortcut(QKeySequence(tr("Ctrl+F", "Action | drawBeam")));
	toolbar->addAction(action);
	aGroup->addAction(action);
	scS.addAction(action);

   scS.lastAction()->setWhatsThis(tr("<b>beam tool</b> ")+
								  tr("<a href=\"tool/beam\">open manual</a><br /> <br />")+
								  tr("Use this tool to add beams. ")+
								  tr("<a href=\"beams\">more...</a><br />")+
								  tr("To do so click on the designated positions or nodes in the view. ")+
								  tr("<a href=\"view\">more...</a><br />")+
								  tr("The <b>snap</b> will help you! ")+
								  tr("<a href=\"snap\">more...</a><br />")+
								  tr("The chosen material and cross section can be chosen in the sidebar. ")+
								  tr("<a href=\"library\">more...</a><br />"));


	//bearing
	action = new QAction("draw support",  this);
	action->setShortcut(QKeySequence(tr("Ctrl+B", "Action | drawBearing")));
	aGroup->addAction(action);
	toolbar->addAction(action);
	scS.addAction(action);
	new bMenu(aGroup->actions().at(2), this);

	scS.lastAction()->setWhatsThis(tr("<b>support tool</b> ")+
								  tr("<a href=\"tool/support\">open manual</a><br /> <br /> <br />")+
								  tr("Use this tool to add supports to existion nodes. ")+
								  tr("<a href=\"support\">more...</a><br />")+
								  tr("To do so click on the designated node in the view. ")+
								  tr("<a href=\"view\">more...</a><br />")+
								  tr("To specify the form of the support which gets added to your structure, keep the triangle beside the tool button pressed. ")+
								  tr("Use the appearing buttons to set the form to what you need! ")+
								  tr("Additional posibilities, such as rotated supports and springs as supports can be specified in the tree or view afterwards."));



	//joint
	action = new QAction("draw joints", this);
	action->setShortcut(QKeySequence(tr("Ctrl+J", "Action | drawHinge")));
	aGroup->addAction(action);
	toolbar->addAction(action);
	scS.addAction(action);
	new jMenu(aGroup->actions().at(3), this);


   scS.lastAction()->setWhatsThis(tr("<b>hinge tool</b> ")+
								  tr("<a href=\"tool/hinge\">open manual</a><br /> <br />")+
								  tr("Use this tool to add hinges to existion beams. ")+
								  tr("<a href=\"hinges\">more...</a><br />")+
								  tr("To do so click on the designated beam. ")+
								  tr("<a href=\"view\">more...</a><br />")+
								  tr("Click near a node and the hinge will be placed between the node and the beam. Otherwise it will be created at some position on the beam. ")+
								  tr("To specify the form of hinge wich gets added to your structure, keep the triangle beside the tool button pressed. ")+
								  tr("Use the appearing buttons to set the form to what you need! ")+
								  tr("Additional posibilities, such as springs between the hinge can be set in the tree afterwards!"));



	//load
	action = new QAction("draw loads", this);
	action->setShortcut(QKeySequence(tr("Ctrl+L", "Action | drawLoad")));
	aGroup->addAction(action);
	toolbar->addAction(action);
	scS.addAction(action);
	new lMenu(aGroup->actions().at(4), this);


	scS.lastAction()->setWhatsThis(tr("<b>load tool</b> ")+
								   tr("<a href=\"tool/load\">open manual</a><br /> <br />")+
								   tr("Use this tool to add loads to the structure. ")+
								   tr("To do so click on the designated beam or node an specify the force with a second on. <br />")+
								   tr("If you want to add a temperature load to the structure, click on the designated beam and specify the temperature using the appearing thermometer. ")+
								   tr("<b>Press the enter key to finish the procedure! </b>")+
								   tr("To specify the type of load, keep the triangle beside the tool button pressed. "));

	//cls
	/*
	action = new QAction("combined load sets", this);
	scS.addAction(action);
	connect(action,			SIGNAL(triggered()),
		   this,			SLOT(loadset()));
	toolbar->addAction(action);
	*/

	//undo-redo
	toolbar->addSeparator();
	toolbar->addAction(undoA);
	toolbar->addAction(redoA);
	scS.addAction(undoA);
	scS.addAction(redoA);
	toolbar->addSeparator();

	undoA->setMenu(new historyMenu());
	redoA->setMenu(new historyMenu());

	undoA->setWhatsThis(tr("<b>undo</b> ")+
					   tr("<a href=\"tool/undo\">open manual</a><br /> <br />")+
					   tr("Use this tool to revert your last modification. ")+
					   tr("Klick on the triangle beside the tool button to show the history. Here you can revert and resotre many steps in once!"));

	redoA->setWhatsThis(tr("<b>redo</b> ")+
					   tr("<a href=\"tool/redo\">open manual</a><br /> <br />")+
					   tr("Use this tool to revert your last modification. ")+
					   tr("Klick on the triangle beside the tool button to show the history. Here you can revert and resotre many steps in once!"));



	//clipboard
	toolbar->addAction(shortcutSettings::instance().actionWithData(Colin::Copy));
	toolbar->addAction(shortcutSettings::instance().actionWithData(Colin::Cut));
	toolbar->addAction(shortcutSettings::instance().actionWithData(Colin::Paste));
	toolbar->addSeparator();

	//move and selection
	aGroup->addAction(toolbar->addAction(colinIcons::instance().icon(Colin::drawMove),
										tr("move")));
	stdA = aGroup->actions().last();
	aGroup->actions().last()->setData(Colin::drawMove);

	stdA->setWhatsThis(tr("<b>move</b> ")+
					  tr("<a href=\"tool/move\">open manual</a><br /> <br />")+
					  tr("Use this tool to move stuff in the <a href=\"view\">view</a>. <br />")+
					  tr("If you click on a void place in the view you can move the visible area as long as the mouse button is pressed. <br />")+
					  tr("If you click on a node, you can move the node as long as the mouse is pressed. <br />")+
					  tr("If you click on a load, you can move the load from beam to beam or from node to node as ling as the mouse is pressed. <br />")+
					  tr("Temperatures behave the same as loads. <br />")+
					  tr("If you click on a the circle attacted to a node, you can change the values of the load as ling as the mouse is pressed. <br />"));





	aGroup->addAction(toolbar->addAction(colinIcons::instance().icon(Colin::drawSelec), tr("select")));
	aGroup->actions().last()->setData(Colin::drawSelec);
	aGroup->actions().last()->setShortcut(tr("Ctrl+D", "action | select"));
	scS.addAction(aGroup->actions().last());

	scS.lastAction()->setWhatsThis(tr("<b>select</b> ")+
								  tr("<a href=\"tool/select\">open manual</a><br /> <br />")+
								  tr("Use this tool to select objects within an area you can specify in the view.<br />")+
								  tr("You can also select single Objects be clicking them (also in the view).")+
								  tr("You will probably find also other selection tools usefull. You can find them in the edit menu: <br />")+
								  tr("<b>Select all</b> (press Ctrl+A by default),<br />")+
								  tr("<b>Invert Selection</b> (press Ctrl+I by default) and <br />")+
								  tr("<b>Deselect all</b> (press Ctrl+Shift+A by default) <br />")+
								  tr("<a href=\"menu/edit\">more...</a>"));

	//zoom
	toolbar->addSeparator();
	action = new QAction(("zoom all"), this);
	action->setIcon(QIcon(colinIcons::instance().icon(Colin::drawZoomAll)));
	action->setData(Colin::drawZoomAll);
	action->setShortcut(QKeySequence(tr("Ctrl+E", "action | zoom all")));
	toolbar->addAction(action);
	action->setMenu(viewm);
	connect(action,              SIGNAL(triggered()),
		   this,                SLOT(zoomAll()));
	scS.addAction(action, true);

	scS.lastAction()->setWhatsThis(tr("<b>auto zoom tools</b> ")+
								   tr("<a href=\"tool/setzoom\">open manual</a><br /> <br />")+
								   tr("Use this tool to automatically adjust the zoom factor<br />")+
								   tr("Keep the triangle beside the tool button pressed to get access to the zoom factors of results. ")+
								   tr("An auto zoom function for the results can be found there too!"));


	action = new QAction(tr("zoom"), this);
	aGroup->addAction(action);
	toolbar->addAction(action);
	new zMenu(action, this);

	action->setWhatsThis(tr("<b>zoom tools</b> ")+
						 tr("<a href=\"tool/zoom\">open manual</a><br /> <br />")+
						 tr("Use this tools to adjust the zoom. ")+
						 tr("Keep the triangle beside the tool button pressed to get access to more tools, such as zoom in, soom out and zoom rectangle!"));




	//spacingItemfrom filelist
	QWidget* dummy = new QWidget(toolbar);
	dummy->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	toolbar->addWidget(dummy);



	QAction *what = QWhatsThis::createAction(this);
	toolbar->addAction(what);
	what->setData(Colin::what);
	what->setIcon(colinIcons::instance().icon(Colin::what));
	what->setShortcut(QKeySequence("F1"));
	what->setWhatsThis(tr("<b>What's this?</b><br />")+
					  tr("Congratulation! You found the help system of Colin!<br /> <br />")+
					  tr("Use this tool to get descriptions of any button or other element of Colin!<br />")+
					  tr("Also, you will get links to the manual or to books.google"));
	scS.addAction(what);

	//calculate
	calcA = toolbar->addAction(colinIcons::instance().icon(Colin::Calculate), tr("calculate"));
	calcA->setData(Colin::Calculate);
	calcA->setShortcut(QKeySequence(tr("Ctrl+R", "action | calculate")));
	connect(calcA,               SIGNAL(triggered()),
		   this,                SLOT(calculate()));
	toolbar->addSeparator();
	scS.addAction(calcA, true);

	calcA->setWhatsThis(tr("<b>calculate tools</b> ")+
						tr("<a href=\"tool/calculate\">open manual</a><br /> <br />")+
						tr("Press to calculate stress and dispalement of your structure"));

	//snapoptions
	snapA = new QAction(tr("snap"), this);
	snapA->setIcon(colinIcons::instance().icon(Colin::Snap));
	snapA->setShortcut(QKeySequence(tr("Ctrl+U", "action | snap on-off")));
	scS.addAction(snapA);

	snapA->setWhatsThis(tr("<b>snap tools</b> ")+
						tr("<a href=\"tool/snap\">open manual</a><br /> <br />")+
						tr("When active, this tool helps you to draw. ")+
						tr("For some settings, keep the triangle beside the tool button pressed"));

	toolbar->addAction(snapA);
	new cMenu(snapA, this);

	//viewoptions
	action =toolbar->addAction(colinIcons::instance().icon(Colin::Windows), tr("view"),
					  this, SLOT(launchViewMenu2()));
	action->setData(Colin::Windows);

	action->setWhatsThis(tr("<b>windows</b> ")+
						 tr("<a href=\"tool/windows\">open manual</a><br /> <br />")+
						 tr("Press to get access to a menu where you can specify the arrangement of views."));


	//making the group work
	foreach(QAction *a, aGroup->actions())
	   a->setCheckable(true);
	aGroup->actions().at(aGroup->actions().count()-2)->setChecked(true);

	//...and add to the mainwindow
	toolbar->setMovable(false);
	addToolBar(toolbar);


	//widgetswitcher!
	tab = new cTabBar(this);
	tab->setMovable(false);
	tab->setAllowedAreas(Qt::BottomToolBarArea | Qt::TopToolBarArea);
	addToolBar(Qt::BottomToolBarArea, tab);

	tab->setObjectName("tabbar");
	toolbar->setObjectName("toolbar");


	//make sure the tooltip will launched!


	foreach(QAction *a, toolbar->actions())
	{
	   connect(a,							SIGNAL(hovered()),
			   this,						SLOT(launchToolTip()));

	   a->setToolTip("");
	}



	connect(toolbar,						SIGNAL(actionTriggered(QAction*)),
		   toolBarTip,						SLOT(hide()));

	connect(toolbar,						SIGNAL(mouseLeft()),
		   toolBarTip,						SLOT(hide()));

	connect(aGroup,							SIGNAL(selected(QAction*)),
		   this,							SLOT(actionSelected(QAction*)));

	connect(viewm2,							SIGNAL(fullScreenRequested(bool)),
	   this,								SLOT(fullScreen(bool)));

	connect(&shortcutSettings::instance(),	SIGNAL(menusBesideChanged(bool)),
		   this,							SLOT(setActionMenus(bool)));

	stdA->toggle();

	this->setActionMenus(shortcutSettings::instance().menuBeside());
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
	if(filelist::instance().currentIndex() < 0)
	{
		QMainWindow::keyPressEvent(e);
		return;
	}


	if(e->key()==Qt::Key_Escape)
	{

		if(viewPortSettings::instance().toClip() == Colin::pasteRequest)
		{
			viewport::removeToPaste();
			return;
		}
		if(viewPortSettings::instance().toClip() == Colin::copybasePointRequest ||
		   viewPortSettings::instance().toClip() == Colin::cutbasePointRequest  )
		{
			viewPortSettings::instance().setClipBoard(Colin::noRequest);
			return;
		}

		const Colin::Actions toD = viewPortSettings::instance().toDraw();


		if(!filelist::instance().currentFile()->isSelectionEmpty())
		{
			filelist::instance().currentFile()->deselectAll();
			return;
		}
		if(toD == Colin::drawMove)
			return;
		if( toD == Colin::drawBeam  ||   //everything
			toD == Colin::drawNLoad ||   //drawable with
			toD == Colin::drawMoment)    //two clicks
		{
			if(filelist::instance().currentFile()->lastObjectNode()!=-1)
			{
				filelist::instance().currentFile()->setlastObjectNode(-1);
				centralWidget->repaintyourChildren();
				//restores the first drawing step
			}
			else
			{
				stdA->trigger();    //Escape + Escape triggers the standardaction
			}
			return;
		}
		else if( toD == Colin::drawULoad ||  //everything
				 toD == Colin::drawDLoad ||  //drawable with
				 toD == Colin::drawILoad ||  //two clicks
				 toD == Colin::drawTemp  )   //on a beam
		{
			if(filelist::instance().currentFile()->lastObjectBeam()!=-1)
			{
				filelist::instance().currentFile()->setlastObjectBeam(-1);
				centralWidget->repaintyourChildren();
				//restores the first drawing step
			}
			else
			{
				stdA->trigger();
			}
			return;
		}
		else if( toD == Colin::drawDoubleLoad)
		{
			if(filelist::instance().currentFile()->lastObjectBeam()!=-1)
			{
				filelist::instance().currentFile()->setlastObjectBeam(-1);
				filelist::instance().currentFile()->setlastObjectNode(-1);
				centralWidget->repaintyourChildren();
			}
			else
			{
				stdA->trigger();
			}
			return;
		}
		else
		{
			stdA->trigger();
			return;
		}
	}

	QMainWindow::keyPressEvent(e);
}

void MainWindow::setTw(ColinStruct *tw)
{
	if(tw == NULL)
	{

		if(currentTw != NULL)
		{
			//if(propertySideBar->isHidden())
			//    propertysShown = false;
			//else
			//    propertysShown = true;
			//if(treeSideBar->isHidden())
			//    treeShown = false;
			//else
			//    treeShown = true;
			//if(historySideBar->isHidden())
			//    historyShown  = false;
			//else
			//    historyShown = true;
			//if(libSideBar->isHidden())
			//    libShown = false;
			//else
			//	libShown = true;



		}
		//propertySideBar->hide();
		//treeSideBar->hide();
		//historySideBar->hide();
		//libSideBar->hide();
	}
	else
	{
		cStack->setCurrentIndex(1);
		//if(propertysShown)
		//    propertySideBar->show();
		//if(historyShown)
		//    historySideBar->show();
		//if(treeShown)
		//    treeSideBar->show();
		//if(libShown)
		//    libSideBar->show();
	}

	shortcutSettings::instance().hasFile(tw==0?false:true);
	currentTw = tw;

	activateLibDelButton();
}

void MainWindow::newT()
{
	ColinStruct *tw = new ColinStruct();
	filelist::instance().append(tw);
}

void MainWindow::newTab()
{
	filelist::instance().showNew();
}

void MainWindow::closeTab()
{
	if(filelist::instance().currentFile() != 0)
	{
		if(filelist::instance().undoGroup()->activeStack()->canUndo())
		{
			QMessageBox msgBox;
			msgBox.setText(tr("The file has been modified."));
			msgBox.setInformativeText(tr("Do you want to save your changes?"));
			msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
			msgBox.setDefaultButton(QMessageBox::Save);
			int ret = msgBox.exec();
			switch(ret)
			{
			case QMessageBox::Save:
				saveCurrent();
				break;
			case QMessageBox::Discard:
				break;
			case QMessageBox::Cancel:
				return;
			default:
				return;
			}
		}
	}
	filelist::instance().removeAt(filelist::instance().currentIndex());
}

void MainWindow::settings()
{
	if(settingsWidget == 0)
	{
		settingsWidget = new sWidget(this);
		cStack->addWidget(settingsWidget);
	}
	cStack->setCurrentWidget(settingsWidget);

	connect(&filelist::instance(),  SIGNAL(deleteSWid()),
			this,                   SLOT(closeSettings()));

	connect(settingsWidget,            SIGNAL(closeMe()),
			this,                      SLOT(closeSettings()));
}

void MainWindow::closeSettings()
{
	delete settingsWidget;
	settingsWidget = 0;
	setCWidget(filelist::instance().currentIndex());
}

void MainWindow::setCWidget(int i)
{
	if(i==-1)
	{
		cStack->setCurrentWidget(newWidget);
	}
	else if(i==-2)
	{
		settings();
	}
	else
	{
		cStack->setCurrentWidget(centralWidget);
	}
}

void MainWindow::launchViewMenu2()
{
	QRect p;
	p = toolbar->actionGeometry(toolbar->actions().at(toolbar->actions().size()-1));
	viewm2->show();
	viewm2->move(mapToGlobal(p.bottomRight()).x()-viewm2->width(),
				 mapToGlobal(toolbar->geometry().bottomLeft()).y());
}

void MainWindow::actionSelected(QAction *a)
{
	bool hasEnum;
	for(int i=0; i<filelist::instance().filecount(); i++)
	{
		filelist::instance().file(i)->setlastObjectBeam(-1);
		filelist::instance().file(i)->setlastObjectNode(-1);
	}
	Colin::Actions cA = static_cast<Colin::Actions>(a->data().toUInt(&hasEnum));
	if(hasEnum)
	{
		viewPortSettings::instance().setCurrentAction(cA);
	}
}

void MainWindow::openT()
{
	QString fileName = QFileDialog::getOpenFileName(
			this, tr("open file"), QDir::homePath(),
			tr("colin structs (*.xcs)"));
	if(!fileName.isEmpty())
		filelist::instance().openT(fileName);
}

void MainWindow::autoOpen()
{
	foreach(QString s, qApp->arguments())
	{
		filelist::instance().openT(QFileInfo(s).canonicalFilePath());
	}
}

void MainWindow::saveCurrent()
{
	if(filelist::instance().filepath(filelist::instance().currentIndex())!="")
		filelist::instance().saveCurrent();
	else
		saveAsT();
}

void MainWindow::saveAsT()
{
	QString fileName;
	fileName = QFileDialog::getSaveFileName(
				this, tr("save file"), QDir::homePath(),
				tr("colin structs")+" (*.xcs)");

	if(fileName.isEmpty())
		return;
	if(fileName.right(4) != QString(".xcs"))
	{
		if(!QFileInfo(fileName+".xcs").exists())
			fileName += QString(".xcs");
	}
	filelist::instance().saveCurrent(fileName);

}

void MainWindow::closeEvent(QCloseEvent *e)
{
	bool somethingToSave(false);

	filelist &fL = filelist::instance();
	for(int i=0; i<fL.filecount(); i++)
	{
		if(fL.undoGroup()->stacks().at(i)->canUndo())
		{
			somethingToSave = true;
		}
	}
	if(somethingToSave)
	{
		closeDialog *dia = new closeDialog(this);
		dia->exec();
		if(dia->result() == QDialog::Rejected)
		{
			e->setAccepted(false);
			delete dia;
			return;
		}
		delete dia;
	}
	grid::instance().saveSettings();
	filelist::instance().saveSettings();
	viewPortSettings::instance().saveSettings();
	catcher::instance().saveSettings();
	unitSettings::instance().saveSettings();
	shortcutSettings::instance().saveSettings();
	colinIcons::instance().saveSettings();

	if(this->isFullScreen())
		fullScreen(false);
	QSettings settings("clazzes.org", "Colin");
	settings.setValue("MainWindow/geometry", saveGeometry());
	settings.setValue("MainWindow/windowState", saveState());


	//for(int i=0; i<5; i++)
	//{
	//    settings.setValue(QString("tree/columnwidth%1").arg(i), tV->columnWidth(i));
	//}
}

void MainWindow::launchToolTip()
{

	if(menuBar()->actions().at(1)->menu()->isVisible())
		return;
	//undo and redo actions can also be triggered from the menu
	//in this case colin doesn't show the tooltip.

	QString dark;

	if(colinIcons::instance().darkIcons())
		dark ="_dark_64";
	else
		dark ="_64";
	QAction *senderA = static_cast<QAction*>(QObject::sender());
	QString title;
	QString description;
	QPixmap icon;
	QPoint p;
	bool alignright = false;
	QKeySequence shortcut = senderA->shortcut();


	p = toolbar->actionGeometry(senderA).bottomLeft()+QPoint(0, 24);

	if(senderA == undoA)
	{
		title = tr("undo");
		description = filelist::instance().undoGroup()->undoText();
		if(description == "")
			description = tr("nothing to undo");
		icon = QPixmap(icondir +"tooltip/undo"+dark+".png");
	}
	else if(senderA == redoA)
	{
		title = tr("redo");
		description = filelist::instance().undoGroup()->redoText();
		if(description == "")
			description = tr("nothing to redo");
		icon = QPixmap(icondir +"tooltip/redo"+dark+".png");
	}
	else if (senderA == snapA)
	{
		title = tr("snap");
		description = tr("object-, grid- and orthogonalsnap. "
						 "For more options keep pressed left mouse button.");
		icon = QPixmap(icondir +"tooltip/magnet"+dark+".png");
		alignright = true;
	}
	else if(senderA->data().toInt()>Colin::startsAt)
	{
		Colin::otherAction cA = static_cast<Colin::otherAction>(senderA->data().toInt());
		if(cA == Colin::what)
		{
			title = tr("What's this?");
			description = tr("Click on any element of Colin to get a description of it");
			icon = QPixmap(icondir +"tooltip/whatsThis"+dark+".png");
			shortcut = senderA->shortcut();
			alignright = true;
		}
		else if(cA == Colin::Copy)
		{
			title = tr("copy");
			description = tr("copy selection into clipboard.");
			icon = QPixmap(icondir +"tooltip/copy"+dark+".png");
			shortcut = QKeySequence::Copy;
		}
		else if(cA == Colin::Cut)
		{
			title = tr("cut");
			description = tr("move selection into clipboard.");
			icon = QPixmap(icondir +"tooltip/cut"+dark+".png");
			shortcut = QKeySequence::Cut;
		}
		else if(cA == Colin::Paste)
		{
			title = tr("paste");
			description = tr("paste from clipboard");
			icon = QPixmap(icondir +"tooltip/paste"+dark+".png");
			shortcut = QKeySequence::Paste;
		}
		else if(cA == Colin::Calculate)
		{
			title = tr("calculate");
			description = tr("ben thinks this is self-explanatory! if you don't understand it, please write to ben.jacobs@student.uibk.ac.at.");
			if(filelist::instance().currentFile()!=0)
			{
				int statBestimmt = filelist::instance().currentFile()->bestimmt();
				if(statBestimmt==0)
				{
					description.append("\n\n"+tr("system is statically determinate.")+" (n = "+QString::number(statBestimmt)+" )");
				}
				else if(statBestimmt>0)
				{
					 description.append("\n\n"+tr("structure is statically indeterminate.")+" (n = "+QString::number(statBestimmt)+" )");
				}
				else
				{
					description.append("\n\n"+tr("sry, but this is a kinematic problem.")+" (n = "+QString::number(statBestimmt)+" )");
				}
			}

			icon = QPixmap(icondir +"tooltip/calc"+dark+".png");
			alignright = true;
		}
		else if(cA == Colin::Windows)
		{
			title = tr("viewports");
			description = tr("adjust viewports.");
			icon = QPixmap(icondir +"tooltip/windows"+dark+".png");
			alignright = true;
		}
		else
		{
			toolBarTip->hide();
			return;
		}
	}
	else
	{
		Colin::Actions cA = static_cast<Colin::Actions>(senderA->data().toInt());
		if(cA == Colin::drawNode)
		{
			title = tr("nodes");
			description = tr("Create new nodes by clicking on the designated position. "
							 "Furthermore you might use this action to split a beam "
							 "or to unite two crossing beams.");
			icon = QPixmap(icondir +"tooltip/node"+dark+".png");
		}
		else if(cA == Colin::drawBeam)
		{
			title = tr("beams");
			description = tr("Connect nodes with beams or simply create new beams.");
			icon = QPixmap(icondir +"tooltip/beam"+dark+".png");
		}
		else if((cA & Colin::drawJoint)== Colin::drawJoint)
		{
			title = tr("hinges");
			description = tr("define hinges.");
			icon = QPixmap(icondir +"tooltip/joint"+dark+".png");
		}
		else if((cA & Colin::drawBearing) == Colin::drawBearing)
		{
			title = tr("supports");
			description = tr("Add supports to existing nodes. "
							 "For more options keep mouse button pressed. ");
			icon = QPixmap(icondir +"tooltip/bearing"+dark+".png");
		}
		else if(cA == Colin::drawNLoad ||
				cA == Colin::drawULoad ||
				cA == Colin::drawILoad ||
				cA == Colin::drawDLoad ||
				cA == Colin::drawTemp  ||
				cA == Colin::drawDoubleLoad)
		{
			title = tr("loads");
			description = tr("Add loads to existing beams and nodes. "
							 "For more options keep mouse button pressed.");
			icon = QPixmap(icondir +"tooltip/load"+dark+".png");
		}
		else if(cA == Colin::drawMove)
		{
			title = tr("move");

			description = tr("move everything you want and even sometimes things you don't want! (beta, you know..!?)");

			icon = QPixmap(icondir +"tooltip/move"+dark+".png");
			shortcut = QKeySequence(Qt::Key_Escape);
		}
		else if(cA == Colin::drawSelec)
		{
			title = tr("selection");
			description = tr("select single objects or create a selection area. "
							 "Keep Ctrl pressed to add an object to the selection.");
			icon = QPixmap(icondir +"tooltip/select"+dark+".png");
		}
		else if(cA == Colin::drawZoomAll)
		{
			title = tr("zoom all");
			description = tr("Adjusts the scaling of all viewports so you can see the entire structure. "
							 "To adjust the viusal scaling of loads, forces and deformations keep mouse button pressed.");
			icon = QPixmap(icondir +"tooltip/zoom_all"+dark+".png");
		}
		else if(cA == Colin::drawZoomRect ||
				cA == Colin::drawZoomIn   ||
				cA == Colin::drawZoomOut)
		 {
			 title = tr("zoom");
			 description = tr("Zoom in, zoom out, and specify visible area.");
			 icon = QPixmap(icondir +"tooltip/zoom"+dark+".png");
		 }
		else
		{
			toolBarTip->hide();
			return;
		}
	}


	toolBarTip->popup(title, description, icon, shortcut,
					  p, alignright);


}


void MainWindow::aboutMe()
{
	QMessageBox::about(this, tr("about Colin"),
		QString( tr("Colin %1\n"

					"named after Colin from \"mostly harmless\" by Douglas Adams.\n\n"
					"Build on %2.\n\n"
#ifndef QT_NO_DEBUG
					"WARNING: this is the DEBUG build!\n\n"
#endif
					"Copyright (C) 2011 Matthias Rauter(matthias.rauter@student.uibk.ac.at)\n\n"

					"This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License "
					"as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.\n\n"

					"This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty "
					"of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.\n\n"

					"You should have received a copy of the GNU General Public License along with this program; if not, see http://www.gnu.org/licenses/.\n\n\n"

					"The icons are licensed under a Creative Commons Attribution-Noncommercial-Share Alike 3.0 License.\n\n"
					"See http://creativecommons.org/ for more information.\n\n"

#ifdef WINDOWS_IS_BUILD_SYS
			  )).arg("COLINVERSION").arg("BUILDDATE"));
#else
			  )).arg(ColinVersion()).arg(BuildDate()));
#endif
}

void MainWindow::aboutQt()
{
	QApplication::aboutQt();
}

void MainWindow::manual()
{
	QDesktopServices::openUrl(QUrl(tr("../manual/en/colin.pdf")));

}

void MainWindow::hideSideBars(const int &i)
{
	if(i<0)
	{
		//propertySideBar->hide();
		//libSideBar->hide();
		//treeSideBar->hide();
		//historySideBar->hide();
	}
}

void MainWindow::selectAll()
{
	filelist::instance().currentFile()->selectAll();
	centralWidget->repaintyourChildren();
}

void MainWindow::invertSelection()
{
	filelist::instance().currentFile()->invertSelection();
	centralWidget->repaintyourChildren();
}

void MainWindow::deselectAll()
{
	filelist::instance().currentFile()->deselectAll();
	centralWidget->repaintyourChildren();
}

void MainWindow::zoomAll()
{
	if(filelist::instance().currentFile() == NULL)
		return;
	QRectF bR = filelist::instance().currentFile()->boundingRect();

	for(int i=0; i<centralWidget->viewCount(); i++)
		centralWidget->zoomRect(i, bR);
}


void MainWindow::deleteSelection()
{
	filelist::instance().currentFile()->deleteSelection();
}

void MainWindow::fullScreen(const bool &on)
{
	if(on)
	{
	menuBar()->hide();
	geometryBeforeFullScreen = geometry();
	wasMaximized = isMaximized();
	showFullScreen();
	}
	else
	{
	menuBar()->show();
		if(wasMaximized)    //valgrind warning here but it's ok
		showMaximized();
	else
	{
		showNormal();
		setGeometry(geometryBeforeFullScreen);
	}
	}

}

void MainWindow::calculate()
{
	if(filelist::instance().currentFile()->bestimmt()<0)
		return;

	wgv *cW = new wgv(filelist::instance().currentFile());

	connect(cW,                 SIGNAL(calcFinished()),
			this,               SLOT(calcFinished()));
	cW->start();
}

void MainWindow::calcFinished()
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

	QString title;
	QString description;
	QPixmap icon;
	QPoint p;
	QString dark;

	if(colinIcons::instance().darkIcons())
		dark ="_dark_64";
	else
		dark ="_64";

	p = toolbar->actionGeometry(calcA).bottomLeft()+QPoint(0, 24);

	icon = QPixmap(icondir+"tooltip/calc"+dark+".png");
	if(filelist::instance().file(calculatedTw)->isCalculated())
		title.append(tr("successfully calculated!"));
	else
		title.append(tr("calculation failed!"));
	description = tr("calculation finished");
	description.append(" - "+filelist::instance().filename(calculatedTw));
	calcInfo->popup(title, description, icon, QKeySequence(),
					p, true);
	calcInfoHider->start(4000);

}


void MainWindow::copy()
{
	const ColinStruct &tw = *filelist::instance().currentFile();
	QClipboard *clipboard = QApplication::clipboard();
	QByteArray data;
	QMimeData *mimeData = new QMimeData();
	XmlWriter writer(&data);
	writer.writeSelection(tw);
	mimeData->setData("text/colinfile", data);
	clipboard->setMimeData(mimeData);
}

void MainWindow::cut()
{
	copy();
	deleteSelection();
}

void MainWindow::paste()
{
	QClipboard *clipboard = QApplication::clipboard();

	ColinStruct *tw = new ColinStruct();
	XmlReader reader(tw);
	reader.read(clipboard->mimeData()->data("text/colinfile"));

#ifndef QT_NO_DEBUG
	QTextStream debugS(stdout);
	debugS << "paste struct" << endl;
	debugS << "beamcount" << tw->beam_n() << endl;
	debugS << "nodecount" << tw->node_n() << endl;
	debugS << "loadcount" << tw->load_n() << endl;
	debugS << "bls" << tw->bls_n() << endl;
#endif


	if(tw->node_n()==0 && tw->load_n()>0)
	{
		if(tw->load(0).typ() == ColinLoad::nodeLoad ||
		   tw->load(0).typ() == ColinLoad::moment   )
			viewPortSettings::instance().setClipBoard(Colin::pasteLoadRequestNode);
		else
			viewPortSettings::instance().setClipBoard(Colin::pasteLoadRequestBeam);
		viewport::setToPaste(tw);
	}
	else if(tw->node_n()==0)
	{
		viewPortSettings::instance().setClipBoard(Colin::noRequest);
		delete tw;
	}
	else
	{
		viewPortSettings::instance().setClipBoard(Colin::pasteRequest);
		viewport::setToPaste(tw);
	}
}


void MainWindow::copyBase()
{
	viewPortSettings::instance().setClipBoard(Colin::copybasePointRequest);
	removeSelectionAfterCopy = false;

}

void MainWindow::cutBase()
{
	viewPortSettings::instance().setClipBoard(Colin::cutbasePointRequest);
	removeSelectionAfterCopy = true;
}

void MainWindow::copyBase(const QPointF &p)
{
	const ColinStruct &tw = *filelist::instance().currentFile();
	QClipboard *clipboard = QApplication::clipboard();
	QByteArray data;
	QMimeData *mimeData = new QMimeData();
	XmlWriter writer(&data);
	writer.writeSelection(tw, p);
	mimeData->setData("text/colinfile", data);
	clipboard->setMimeData(mimeData);
	if(removeSelectionAfterCopy)
		deleteSelection();
}

/*
void MainWindow::setCalculationIcon()
{
	if(filelist::instance().currentFile() == 0)
	{
		calcA->setIcon(colinIcons::instance().icon(Colin::Calculate, 1));
		return;
	}
	if(filelist::instance().currentFile()->isCalculated())
	{
		calcA->setIcon(colinIcons::instance().icon(Colin::Calculate, 0));
		return;
	}
	int statBestimmt = filelist::instance().currentFile()->bestimmt();
	if(statBestimmt>-1)
	{
		calcA->setIcon(colinIcons::instance().icon(Colin::Calculate, 1));
	}
	else
	{
		calcA->setIcon(colinIcons::instance().icon(Colin::Calculate, -1));
	}
}*/


void MainWindow::delLib()
{
	while(LIB.mats_n()>0)
	{
	LIB.removeMat(0);
	}
	while(LIB.profiles_n()>0)
	{
	LIB.removeProfile(0);
	}
}

void MainWindow::activateLibDelButton()
{

	if(filelist::instance().filecount()>0)
		newWidget->libB->setDisabled(true);
	else
		newWidget->libB->setDisabled(false);
}

/*
void MainWindow::exportPic()
{

	QFileDialog* filedlg = new QFileDialog(this,  tr("Bild exportieren"));
	QStringList filenames, filters;
	filedlg->setFileMode(QFileDialog::AnyFile);
	filedlg->setDirectory(QDir::homePath());
	filters <<"Scalable Network Grafics (*.SVG)";
	filters <<"Portable Network Grafics (*.PNG)";
	filedlg->setFilters(filters);
	filedlg->setAcceptMode(QFileDialog::AcceptSave);
	filedlg->setViewMode(QFileDialog::Detail);
	if (filedlg->exec())
		filenames=filedlg->selectedFiles();
	if (!filenames.size())
		return;
	QFile picfile(filenames.at(0));
	picfile.remove();
	QString testpng = filenames.at(0);
	if(testpng.right(3)=="svg")
	{
		QSvgGenerator gen;
		gen.setFileName(filenames.at(0));
		viewPortSettings::instance().exportSize();
		gen.setViewBox(QRect(QPoint(0, 0), viewPortSettings::instance().exportSize()));

		gen.setDescription("created with Colin by Matti :)");
		exportDocument::render(&gen, *filelist::instance().currentFile(), viewPortSettings::instance().elements(0));
	}
	else
	{
		QImage pix(viewPortSettings::instance().exportSize(), QImage::Format_ARGB32_Premultiplied);
		pix.fill(qRgba(0, 0, 0, 0));
		exportDocument::render(&pix, *filelist::instance().currentFile(), viewPortSettings::instance().elements(0));
		pix.save(filenames.at(0));
	}
}*/

void MainWindow::print()
{
//	this->centralWidget->showOverlay();
}

void MainWindow::zoomIn()
{
	ColinStruct *t = filelist::instance().currentFile();
	for(int i=0; i<4; i++)
	{
		t->view(i).scale(1.4,
						 1.4);
	}
	this->centralWidget->repaintyourChildren();
}

void MainWindow::zoomOut()
{
	ColinStruct *t = filelist::instance().currentFile();
	for(int i=0; i<4; i++)
	{
		t->view(i).scale(1./1.4,
						 1./1.4);
	}
	this->centralWidget->repaintyourChildren();
}
/*
void MainWindow::loadset()
{
	clswidget widget(this);
	widget.exec();
}
*/

void MainWindow::setActionMenus(bool beside)
{
	foreach(QToolButton *b, toolbar->findChildren<QToolButton *>())
	{
		if(!b->actions().isEmpty()){
			if(b->actions().at(0)->menu())
				b->setPopupMode(beside?QToolButton::MenuButtonPopup:QToolButton::DelayedPopup);
			b->actions().at(0)->setEnabled(!b->actions().at(0)->isEnabled());
			b->actions().at(0)->setEnabled(!b->actions().at(0)->isEnabled());
		}
	}
}

void MainWindow::runScript()
{
	QString scriptFile;
	scriptFile = QFileDialog::getOpenFileName(this, tr("open *js file"), QString(), "javascript (*.js)");
	QString programm;
	QFile code(scriptFile);
	code.open(QIODevice::ReadOnly);
	programm = code.readAll();
	code.close();

	scriptEngine *sE = new scriptEngine(this);

	sE->evaluate(programm);
}

void MainWindow::runDebug()
{
	QString scriptFile;
	scriptFile = QFileDialog::getOpenFileName(this, tr("open *js file"), QString(), "javascript (*.js)");
	QString programm;
	QFile code(scriptFile);
	code.open(QIODevice::ReadOnly);
	programm = code.readAll();
	code.close();

	scriptEngine *sE = new scriptEngine(this);
	QScriptEngineDebugger *debugger = new QScriptEngineDebugger(this);
	QWidget *debugWindow = debugger->standardWindow();
	debugWindow->setWindowModality(Qt::ApplicationModal);
	debugWindow->resize(1280, 704);
	debugWindow->show();
	debugger->attachTo(sE);
	debugger->action(QScriptEngineDebugger::InterruptAction)->trigger();
	sE->evaluate(programm, scriptFile);
}
