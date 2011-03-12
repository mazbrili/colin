#ifndef LMENU_H
#define LMENU_H

#include <QtGui/QMenu>
#include <QtGui/QPushButton>
#include <QtGui/QButtonGroup>

#include "viewportsettings.h"
#include "colinicons.h"

class lMenu : public QMenu
{
    Q_OBJECT
public:
    explicit lMenu(QAction *a, QWidget *parent = 0);

    void paintEvent(QPaintEvent *){}
signals:

public slots:
    void click(int nr);
private:
    QAction *theAction;
    QButtonGroup *group;
};

#endif // LMENU_H
