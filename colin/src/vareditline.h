#ifndef VAREDITLINE_H
#define VAREDITLINE_H


#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QGridLayout>


#include "colinicons.h"
#include "filelist.h"


class varEditLine : public QObject
{
    Q_OBJECT
public:
    varEditLine(wgv_tw::VarName varname, QGridLayout *layout);

signals:

public slots:

    void aktual();
    void setTw(wgv_tw *tw);
private:
    QPushButton *searchB;
    QLineEdit *value;
    QLabel *name;
    wgv_tw::VarName varName;
    wgv_tw *t;

    QString varString(wgv_tw::VarName var) const;


};

#endif // VAREDITLINE_H
