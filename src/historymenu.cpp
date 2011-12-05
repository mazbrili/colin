#include "historymenu.h"

#include <QtGui/QUndoView>

#include "filelist.h"

historyMenu::historyMenu(QWidget *parent) :
    QMenu(parent)
{
	view = new QUndoView(this);
	view->setGroup(filelist::instance().undoGroup());
}


QSize historyMenu::sizeHint() const
{
	return view->sizeHint();
}
