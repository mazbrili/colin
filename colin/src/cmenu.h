#ifndef CMENU_H
#define CMENU_H

#include <QtGui/QMenu>
#include <QtGui/QPushButton>
#include <QtGui/QButtonGroup>
#include <QtGui/QSlider>

#include "catcher.h"
#include "colinicons.h"

class cMenu : public QMenu
{
    Q_OBJECT
public:
    explicit cMenu(QAction *a, QWidget *parent = 0);

    void paintEvent(QPaintEvent *);
signals:

private slots:
    void click();
    void setRange(int i);
    void allOn(bool on);
private:
    QButtonGroup *group;
    QAction *theAction;
    QSlider *range;
};

#endif // CMENU_H
