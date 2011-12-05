#ifndef HISTORYMENU_H
#define HISTORYMENU_H

#include <QtGui/QMenu>

class QUndoView;

class historyMenu : public QMenu
{
    Q_OBJECT
public:
    explicit historyMenu(QWidget *parent = 0);
	QSize sizeHint() const;
signals:

public slots:

private:
	QUndoView *view;
};

#endif // HISTORYMENU_H
