#ifndef COLORSETTINGSWIDGET_H
#define COLORSETTINGSWIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QColorDialog>
#include <QtGui/QPushButton>
#include <QtGui/QButtonGroup>
#include <QtGui/QKeyEvent>

#include "viewportsettings.h"
#include "colinicons.h"
#include "grid.h"

class colorSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit colorSettingsWidget(QWidget *parent = 0);
    void keyPressEvent(QKeyEvent *e);

signals:

public slots:

private slots:
    void setColor(const int &i);
    void changeCurrentColor(const QColor &c);

private:
    QColorDialog *dialog;
    QButtonGroup *group;


};

#endif // COLORSETTINGSWIDGET_H
