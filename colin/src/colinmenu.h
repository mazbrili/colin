#ifndef COLINMENU_H
#define COLINMENU_H

#include <QtGui/QMenu>
#include <QtCore/QList>
#include <QtCore/QPair>
#include <QtGui/QPainter>
#include <QtGui/QStyleOptionMenuItem>
#include <QtGui/QPaintEvent>

typedef QPair<QAction *, QWidget *> ActionWidgetPair;

class ColinMenu : public QMenu
{
    Q_OBJECT
public:
    explicit ColinMenu(QWidget *parent = 0) : QMenu(parent) {}
    ColinMenu(const QString &title, QWidget *parent = 0) : QMenu(title, parent) {}

    inline bool addActionWidgetPair(ActionWidgetPair pair){
        addAction(pair.first); return addWidgetForAction(pair.first, pair.second);}
    bool addWidgetForAction(QAction *action, QWidget *widget);
    void removeWidgetFromAction(QAction *action);

    void paintEvent(QPaintEvent *e);
    void showEvent(QShowEvent *);

private:
    void setChildrenGeometry();
    int mWidth;

public slots:
    inline void removeWidgetFromAction(QObject *action){
        removeWidgetFromAction(dynamic_cast<QAction*>(action)); }

private:
    QList<QPair<QAction *, QWidget *> > actionWidgets;

};

#endif // COLINMENU_H
