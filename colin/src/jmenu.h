#ifndef JMENU_H
#define JMENU_H

#include <QtGui/QMenu>
#include <QtGui/QPushButton>
#include <QtGui/QButtonGroup>

#include "viewportsettings.h"
#include "colinicons.h"

class jMenu : public QMenu
{
    Q_OBJECT
public:
    explicit jMenu(QAction *a, QWidget *parent = 0);

    void paintEvent(QPaintEvent *){}
signals:

public slots:
    void click(int nr);
private:
    QAction *theAction;
    QButtonGroup *group;
};

#endif // JMENU_H
