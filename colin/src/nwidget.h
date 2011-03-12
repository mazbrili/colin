#ifndef NWIDGET_H
#define NWIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QPainter>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include "previewrenderer.h"

#include "previewwidget.h"

#define CLAZZES_DOT_ORG_ICON

class nWidget : public QWidget
{
    Q_OBJECT
public:
    explicit nWidget(QWidget *parent = 0);

    void paintEvent(QPaintEvent *);
signals:
public slots:

public:
    QPushButton *newB,
                *openB,
                *settingsB,
                *libB;
private:
    previewWidget *previews[8];
    previewRenderer *renderer;

    QPixmap colinicon;
#ifdef CLAZZES_DOT_ORG_ICON
    QPixmap clazzesicon;
#endif



};

#endif // NWIDGET_H
