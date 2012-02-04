#include "colinmanual.h"

#include <QtGui/QDesktopServices>
#include <QtCore/QDebug>
#include <QtCore/QUrl>
#include <QtCore/QProcess>
#include <QtGui/QApplication>
#include <QtGui/QWhatsThis>

void colinManual::launch(const QString &what)
{

#ifdef MANUAL_VERBOSE
	qDebug() << "help: url request: " << what;
#endif

	if(what.contains("http"))
	{
#ifdef LIBRARY_VERBOSE
		qDebug() << "launching webbrowser!";
#endif
		QDesktopServices::openUrl(what);
		QWhatsThis::hideText();
		return;
	}

	if(what.contains("cite"))
	{
#ifdef LIBRARY_VERBOSE
		qDebug() << "cite in launch!";
#endif
		QStringList args = what.split("/");
#ifdef LIBRARY_VERBOSE
		qDebug() << "... with arguments " << args;
#endif
		if(args[1]=="hofstetter")
			QDesktopServices::openUrl(QString("http://books.google.de/books?hl=de&id=3QcaavV-ENUC&q=%1#v=snippet&q=%2&f=false").arg(args[2]).arg(args[2]));
		QWhatsThis::hideText();
		return;
	}


	QProcess *pdfreader = new QProcess(QApplication::instance());

	pdfreader->start("evince", QStringList() << QObject::tr("../manual/en/colin.pdf") << QString("--page-index=%1").arg(pageForUrl(what)));
	pdfreader->waitForStarted();

	if(pdfreader->error()!=QProcess::FailedToStart){
		QObject::connect(pdfreader,				SIGNAL(finished(int)),
						 pdfreader,				SLOT(deleteLater()));
		QWhatsThis::hideText();
		return;
	}

	QDesktopServices::openUrl(QUrl(QObject::tr("../manual/en/colin.pdf")));
	QWhatsThis::hideText();

}


QString colinManual::pageForUrl(const QString &url)
{
	if(url=="graphicsview")			return QObject::tr("5", "page of manual: graphicsview");
	if(url=="tabbar")				return QObject::tr("5", "Page of manual: tabbar");
	if(url=="treeview")				return QObject::tr("3", "page of manual: treeview");
	if(url=="sidebar")				return QObject::tr("5", "page of manual: sidebar");
	if(url=="startwidget")			return QObject::tr("5", "page of manual: startwidget");
	if(url=="settingswidget")		return QObject::tr("5", "page of manual: settingswidget");

	if(url=="javascript")			return QObject::tr("4", "page of manual: javascript");

	if(url=="node")					return QObject::tr("1", "page of manual: node");
	if(url=="beam")					return QObject::tr("2", "page of manual: beam");
	if(url=="load")					return QObject::tr("3", "page of manual: load");
	if(url=="temp")					return QObject::tr("4", "page of manual: temp");
	if(url=="hinge")				return QObject::tr("5", "page of manual: hinge");
	if(url=="support")				return QObject::tr("6", "page of manual: support");
	if(url=="bls")					return QObject::tr("6", "page of manual: bls");
	if(url=="cls")					return QObject::tr("6", "page of manual: cls");
	if(url=="material")				return QObject::tr("5", "page of manual: material");
	if(url=="cross section")		return QObject::tr("5", "page of manual: cross section");

	if(url=="tool/node")			return QObject::tr("5", "page of manual: tool/node");
	if(url=="tool/beam")			return QObject::tr("6", "page of manual: tool/beam");
	if(url=="tool/load")			return QObject::tr("7", "page of manual: tool/load");
	if(url=="tool/support")			return QObject::tr("7", "page of manual: tool/support");
	if(url=="tool/hinge")			return QObject::tr("7", "page of manual: tool/hinge");
	if(url=="tool/copy")			return QObject::tr("8", "page of manual: tool/copy");
	if(url=="tool/cut")				return QObject::tr("8", "page of manual: tool/cut");
	if(url=="tool/paste")			return QObject::tr("2", "page of manual: tool/paste");
	if(url=="tool/undo")			return QObject::tr("2", "page of manual: tool/undo");
	if(url=="tool/redo")			return QObject::tr("2", "page of manual: tool/redo");
	if(url=="tool/move")			return QObject::tr("2", "page of manual: tool/move");
	if(url=="tool/select")			return QObject::tr("2", "page of manual: tool/select");
	if(url=="tool/setzoom")			return QObject::tr("2", "page of manual: tool/setzoom");
	if(url=="tool/zoom")			return QObject::tr("2", "page of manual: tool/zoom");
	if(url=="tool/calculate")		return QObject::tr("2", "page of manual: tool/calculate");
	if(url=="tool/widgets")			return QObject::tr("2", "page of manual: tool/widgets");

	if(url=="rightclick/node")		return QObject::tr("4", "page of manual: rightclick/node");
	if(url=="rightclick/beam")		return QObject::tr("4", "page of manual: rightclick/beam");
	if(url=="rightclick/load")		return QObject::tr("4", "page of manual: rightclick/load");
	if(url=="rightclick/temp")		return QObject::tr("4", "page of manual: rightclick/temp");

	if(url=="menu/file")			return QObject::tr("4", "page of manual: menu/file");
	if(url=="menu/edit")			return QObject::tr("4", "page of manual: menu/edit");
	if(url=="menu/about")			return QObject::tr("2", "page of manual: menu/about");

	if(url=="tree/node")			return QObject::tr("3", "page of manual: tree/node");
	if(url=="tree/support")			return QObject::tr("3", "page of manual: tree/support");
	if(url=="tree/beam")			return QObject::tr("3", "page of manual: tree/beam");
	if(url=="tree/hinge")			return QObject::tr("3", "page of manual: tree/hinge");
	if(url=="tree/load")			return QObject::tr("3", "page of manual: tree/load");
	if(url=="tree/bls")				return QObject::tr("3", "page of manual: tree/bls");
	if(url=="tree/cls")				return QObject::tr("3", "page of manual: tree/cls");
	if(url=="tree/blscls")			return QObject::tr("3", "page of manual: tree/blscls");


	else				return "1";
}
