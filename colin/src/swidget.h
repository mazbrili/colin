#ifndef SWIDGET_H
#define SWIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QPainter>
#include <QtGui/QIcon>
#include <QtGui/QLinearGradient>
#include <QtGui/QScrollArea>
#include <QtGui/QStackedWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QButtonGroup>
#include <QtGui/QPushButton>

#include "colorsettingswidget.h"
#include "shortcutsettingswidget.h"
#include "miscsettingswidget.h"

class sWidget : public QWidget
{
    Q_OBJECT
public:
    explicit sWidget(QWidget *parent = 0);
    //void paintEvent(QPaintEvent *);

signals:
    void closeMe();
public slots:
    void restore();
private:
    QStackedWidget *central;
    colorSettingsWidget *colors;
    shortcutSettingsWidget *shortcuts;
    miscSettingsWidget *misc;
    QButtonGroup *group;
};

#endif // SWIDGET_H
