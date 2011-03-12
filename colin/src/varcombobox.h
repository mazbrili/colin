#ifndef VARCOMBOBOX_H
#define VARCOMBOBOX_H
#include <QtGui/QLabel>
#include <QtGui/QComboBox>
#include <QtGui/QPushButton>
#include <QtGui/QGridLayout>


#include "colinicons.h"
#include "filelist.h"


class varComboBox : public QObject
{
    Q_OBJECT
public:
    varComboBox(wgv_tw::VarName varname, QGridLayout *layout);

signals:

public slots:

    void aktual();
private:
    QPushButton *searchB;
    QComboBox *value;
    QLabel *name;
    wgv_tw::VarName varName;

    QString varString(wgv_tw::VarName var) const;

};


#endif // VARCOMBOBOX_H
