#ifndef MISCSETTINGSWIDGET_H
#define MISCSETTINGSWIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QComboBox>
#include <QtGui/QDoubleSpinBox>
#include "viewportsettings.h"
#include "grid.h"

class miscSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    miscSettingsWidget(QWidget *parent = 0);
private slots:
    void Echanged(const int &i);
    void Pchanged(const int &i);
    void Ichanged(const int &i);
    void setLang(const QString &lang);
    void ctrlWheelChanged(const int &i);
    void shiftWheelChanged(const int &i);
    void altWheelChanged(const int &i);
    void AAChanged(const int &i);
    void mRadChanged(const double &mR);
    void resultWChanged(const double &s);
    //void resChanged();
    void precisonChanged(const double &prec);
    void tooltipColor(const int &row);
private:
    QComboBox *EEhBox,
	      *IEhBox,
              *PEhBox;

    QComboBox *zBox[3];


    //QDoubleSpinBox *xRes,
    //               *yRes;
};

#endif // MISCSETTINGSWIDGET_H
