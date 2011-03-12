#ifndef SHORTCUTSETTINGSWIDGET_H
#define SHORTCUTSETTINGSWIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QPushButton>
#include <QtGui/QButtonGroup>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMessageBox>
#include <QtGui/QKeyEvent>

#include "colinicons.h"
#include "shortcutsettings.h"

class shortcutSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit shortcutSettingsWidget(QWidget *parent = 0);

signals:

public slots:
    void buttonClicked(const int &i);
    void setShortCut();
    void restoreShortCut(const int &i);
private:
    QButtonGroup *setter;
    QButtonGroup *restorer;
    QLineEdit *editor;
    QGridLayout *layout;

    QList<QLabel*> shortcuts;
    int old;

};

#endif // SHORTCUTSETTINGSWIDGET_H
