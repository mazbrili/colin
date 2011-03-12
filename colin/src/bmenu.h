#ifndef BMENU_H
#define BMENU_H

#include <QtGui/QMenu>
#include <QtGui/QPushButton>
#include <QtGui/QButtonGroup>

#include "viewportsettings.h"
#include "colinicons.h"

class bMenu : public QMenu
{
    Q_OBJECT
public:
    explicit bMenu(QAction *a, QWidget *parent = 0);

    void paintEvent(QPaintEvent *){}
signals:

public slots:
    void click();
private:
    QAction *theAction;
    QButtonGroup *group;
};

#endif // BMENU_H
