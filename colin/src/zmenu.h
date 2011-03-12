#ifndef ZMENU_H
#define ZMENU_H

#include <QtGui/QMenu>
#include <QtGui/QPushButton>
#include <QtGui/QButtonGroup>

#include "viewportsettings.h"
#include "colinicons.h"

class zMenu : public QMenu
{
    Q_OBJECT
public:
    explicit zMenu(QAction *a, QWidget *parent = 0);

    void paintEvent(QPaintEvent *){}
signals:

public slots:
    void click(int nr);
private:
    QAction *theAction;
    QButtonGroup *group;
};

#endif // ZMENU_H
