#ifndef COLINHMULTIBUTTON_H
#define COLINHMULTIBUTTON_H

#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <QtCore/QCoreApplication>
#include <QtCore/QList>
#include <QtGui/QResizeEvent>
#include "colinpushbuttonpart.h"

class ColinHMultiButton : public QWidget
{
    Q_OBJECT

public:
    ColinHMultiButton(QWidget *parent = 0);

    ~ColinHMultiButton();


    void addButton(ColinPushButtonPart *but);
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *event);
    void adjustButtons();
    void adjustButtons(const QSize &size);

private:
    QList<ColinPushButtonPart*> buttonlist;

};

#endif // COLINHMULTIBUTTON_H
