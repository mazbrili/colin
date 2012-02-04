/***********************************************************
 * $Id$
 *
 * Colin
 *
 * Copyright (C) 2011 Matthias Rauter (matthias.rauter@student.uibk.ac.at)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details."
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see
 *
 * http://www.gnu.org/licenses/.
 *
 * Author: Matthias Rauter
 *
 ***********************************************************/

#include "miscsettingswidget.h"
#include <QtGui/QComboBox>
#include <QtGui/QLabel>
#include "colinicons.h"
#include "unitsettings.h"
#include <QtGui/QGridLayout>
#include <QtGui/QComboBox>
#include <QtGui/QHBoxLayout>


miscSettingsWidget::miscSettingsWidget(QWidget *parent):
	QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this);


    QLabel *title = new QLabel(this);

	QFont titleFont = title->font();
	titleFont.setBold(true);

    title->setFont(titleFont);
    title->setText("["+tr("language")+"]");

    QLabel *langIcon = new QLabel(this);
    langIcon->setPixmap(QPixmap(colinIcons::icondir_+"lang_64.png"));
    QLabel *langLab = new QLabel(tr("language")+":", this);


    QLabel *langrestart = new QLabel(tr("requires restart!"), this);
    langrestart->setAlignment(Qt::AlignCenter);
    QComboBox *langBox = new QComboBox(this);
    langBox->addItem("guess");
    langBox->addItem("Englisch");
    langBox->addItem("Deutsch");

    QSettings settings("clazzes.org", "Colin", this);
    for(int i=0; i<4; i++)
    {
	if(langBox->itemText(i) == settings.value("language", "guess").toString())
	    langBox->setCurrentIndex(i);
    }

    connect(langBox,	    SIGNAL(currentIndexChanged(QString)),
	    this,	    SLOT(setLang(QString)));

    layout->addWidget(title, 0, 0, 1, 4);
    layout->addWidget(langIcon, 1, 0, 1, 1);
    layout->addWidget(langLab, 1, 2, 1, 1);
	layout->addWidget(langBox, 1, 4, 1, 1);
	layout->addWidget(langrestart, 1, 5, 1, 1);

    QLabel *middleM;


    for(int i=0; i<3; i++)
    {
        middleM = new QLabel(this);
        middleM->setPixmap(QPixmap(colinIcons::icondir_+"mouse/mouse_wheel_64.png"));
        layout->addWidget(middleM, i+3, 0, 1, 1);
        middleM = new QLabel(this);
        QFont f = middleM->font();
        f.setPointSize(16);
        middleM->setFont(f);
        middleM->setText("+");
        middleM->setAlignment(Qt::AlignCenter);
        layout->addWidget(middleM, i+3, 1, 1, 1);
        middleM = new QLabel(this);
        middleM->setFont(f);
        middleM->setText("=");
        middleM->setAlignment(Qt::AlignCenter);
        layout->addWidget(middleM, i+3, 3, 1, 1);
    }
    QLabel *ctrl = new QLabel(this);
    ctrl->setPixmap(colinIcons::instance().icon(tr("Ctrl", "mousewheel modificator | Control"), true));

    QLabel *shift = new QLabel(this);
    shift->setPixmap(colinIcons::instance().icon(tr("Shift", "mousewheel modificator | Shift"), true));

    QLabel *alt = new QLabel(this);
    alt->setPixmap(colinIcons::instance().icon(tr("Alt", "mousewheel modificator | Alt"), true));

	layout->addWidget(ctrl, 3, 2, 1, 1);
    layout->addWidget(shift, 4, 2, 1, 1);
    layout->addWidget(alt, 5, 2, 1, 1);
    for(int i=0;i<3; i++)
    {
	zBox[i] = new QComboBox(this);
        zBox[i]->addItem(tr("zoom deformation"), Colin::u);
        zBox[i]->addItem(tr("zoom force"), Colin::N);
        zBox[i]->addItem(tr("zoom moment"), Colin::M);
	layout->addWidget(zBox[i], 3+i, 4, 1, 1);
    }

    for(int i=0; i<3; i++)
    {
	if(viewPortSettings::instance().wheelMod(Qt::ControlModifier) == static_cast<Colin::Elements>(zBox[0]->itemData(i, 32).toInt()))
	    zBox[0]->setCurrentIndex(i);
	if(viewPortSettings::instance().wheelMod(Qt::ShiftModifier) == static_cast<Colin::Elements>(zBox[1]->itemData(i, 32).toInt()))
	    zBox[1]->setCurrentIndex(i);
	if(viewPortSettings::instance().wheelMod(Qt::AltModifier) == static_cast<Colin::Elements>(zBox[2]->itemData(i, 32).toInt()))
	    zBox[2]->setCurrentIndex(i);
    }
    connect(zBox[0],		    SIGNAL(currentIndexChanged(int)),
	    this,		    SLOT(ctrlWheelChanged(int)));

    connect(zBox[1],		    SIGNAL(currentIndexChanged(int)),
	    this,		    SLOT(shiftWheelChanged(int)));

    connect(zBox[2],		    SIGNAL(currentIndexChanged(int)),
            this,		    SLOT(altWheelChanged(int)));


    title = new QLabel(this);
    title->setFont(titleFont);
    title->setText("["+tr("mouse wheel")+"]");
    layout->addWidget(title, 2, 0, 1, 4);

    title = new QLabel(this);
    title->setFont(titleFont);
    title->setText("["+tr("units")+"]");
    layout->addWidget(title, 6, 0, 1, 4);

    title = new QLabel(this);
    title->setFont(titleFont);
    title->setText("["+tr("drawing")+"]");
    layout->addWidget(title, 11, 0, 1, 4);


    //QLabel *EIcon = new QLabel(this);
    //EIcon->setPixmap(colinIcons::icondir_ + "mat_64.png");
    QLabel *EEhLab = new QLabel(tr("material constants in"), this);
    EEhLab->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    EEhBox = new QComboBox(this);
    EEhBox->addItem(QString("N/m%1").arg(QChar(0x00B2)), unitSettings::N_m2);
    EEhBox->addItem(QString("kN/m%1").arg(QChar(0x00B2)), unitSettings::kN_m2);
    EEhBox->addItem(QString("kN/cm%1").arg(QChar(0x00B2)), unitSettings::kN_cm2);
    EEhBox->addItem(QString("N/cm%1").arg(QChar(0x00B2)), unitSettings::N_cm2);
    //layout->addWidget(EIcon, 7, 0, 1, 1);
    layout->addWidget(EEhLab, 7, 2, 1, 1);
	layout->addWidget(EEhBox, 7, 4, 1, 1);
    for(int i=0; i<4; i++){
	if(EEhBox->itemData(i, 32).toInt()==unitSettings::instance().EU())
	    EEhBox->setCurrentIndex(i);}
    connect(EEhBox,		SIGNAL(currentIndexChanged(int)),
	    this,		SLOT(Echanged(int)));

    //QLabel *IIcon = new QLabel(this);
    //IIcon->setPixmap(colinIcons::icondir_ + "prof_64.png");
    QLabel *IEhLab = new QLabel(tr("cross section in"), this);
    IEhLab->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    IEhBox = new QComboBox(this);
    IEhBox->addItem("m", unitSettings::m);
    IEhBox->addItem("cm", unitSettings::cm);
    IEhBox->addItem("mm", unitSettings::mm);
    //layout->addWidget(IIcon, 8, 0, 1, 1);
    layout->addWidget(IEhLab, 8, 2, 1, 1);
	layout->addWidget(IEhBox, 8, 4, 1, 1);
    for(int i=0; i<3; i++){
	if(IEhBox->itemData(i, 32).toInt()==unitSettings::instance().IU())
	    IEhBox->setCurrentIndex(i);}
    connect(IEhBox,		SIGNAL(currentIndexChanged(int)),
	    this,		SLOT(Ichanged(int)));

    //QLabel *PIcon = new QLabel(this);
    //PIcon->setPixmap(colinIcons::icondir_ + "load_64.png");
    QLabel *PEhLab = new QLabel(tr("force/moment in"), this);
    PEhLab->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    PEhBox = new QComboBox(this);
    PEhBox->addItem("N, N/m & Nm", unitSettings::Nm);
    PEhBox->addItem("N, N/m & Ncm", unitSettings::Ncm);
    PEhBox->addItem("kN, kN/m & kNm", unitSettings::kNm);
    PEhBox->addItem("kN, kN/m & kNcm", unitSettings::kNcm);
    //layout->addWidget(PIcon, 9, 0, 1, 1);
    layout->addWidget(PEhLab, 9, 2, 1, 1);
	layout->addWidget(PEhBox, 9, 4, 1, 1);
    for(int i=0; i<3; i++){
	if(PEhBox->itemData(i, 32).toInt()==unitSettings::instance().PU())
	    PEhBox->setCurrentIndex(i);}
    connect(PEhBox,		SIGNAL(currentIndexChanged(int)),
	    this,		SLOT(Pchanged(int)));

    QLabel *PrecLab = new QLabel(tr("precison"), this);
    PrecLab->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QDoubleSpinBox *Prec = new QDoubleSpinBox(this);
    Prec->setMaximum(8);
    Prec->setMinimum(2);
    Prec->setSingleStep(1);
    Prec->setDecimals(0);
    Prec->setValue(unitSettings::instance().precison());
    layout->addWidget(PrecLab, 10, 2, 1, 1);
	layout->addWidget(Prec, 10, 4, 1, 1);
    connect(Prec,               SIGNAL(valueChanged(double)),
            this,               SLOT(precisonChanged(double)));

    QLabel *aaLab = new QLabel(tr("Antialiasing"), this);
    QComboBox *aaBox = new QComboBox(this);
    aaBox->addItem(tr("on (slow)"));
    aaBox->addItem(tr("off (fast)"));
    aaBox->setCurrentIndex(viewPortSettings::instance().antialiasing()? 0 : 1);
   // layout->addWidget(langIcon, 12, 0, 1, 1);
    layout->addWidget(aaLab, 12, 2, 1, 1);
	layout->addWidget(aaBox, 12, 4, 1, 1);
    connect(aaBox,              SIGNAL(currentIndexChanged(int)),
            this,               SLOT(AAChanged(int)));

    QLabel *resultWSLab = new QLabel(tr("result widget scale")+QString(" [m%1pxl]").arg(QChar(0x2259)));
    QDoubleSpinBox *resultWScale = new QDoubleSpinBox(this);
    resultWScale->setMaximum(1000);
    resultWScale->setValue(viewPortSettings::instance().resultsWidgetScale());
    //layout->addWidget(langIcon, 13, 0, 1, 1);
    layout->addWidget(resultWSLab, 13, 2, 1, 1);
	layout->addWidget(resultWScale, 13, 4, 1, 1);
    connect(resultWScale,       SIGNAL(valueChanged(double)),
            this,               SLOT(resultWChanged(double)));


    QLabel *momentRad = new QLabel(tr("scale moments") +QString(" [m%1rad]").arg(QChar(0x2259)));
    QDoubleSpinBox *mRLE = new QDoubleSpinBox(this);
    mRLE->setMaximum(1000);
    mRLE->setValue(viewPortSettings::instance().MtoRad());
    //layout->addWidget(langIcon, 14, 0, 1, 1);
    layout->addWidget(momentRad, 14, 2, 1, 1);
	layout->addWidget(mRLE, 14, 4, 1, 1);
    connect(mRLE,               SIGNAL(valueChanged(double)),
            this,               SLOT(mRadChanged(double)));


    QLabel *tooltipLab = new QLabel(tr("icons in tooltips"), this);

    QComboBox *tooltip = new QComboBox(this);
    tooltip->addItem(tr("light"));
    tooltip->addItem(tr("dark"));
    tooltip->setCurrentIndex(colinIcons::instance().darkIcons());
    layout->addWidget(tooltipLab, 15, 2, 1, 1);
	layout->addWidget(tooltip, 15, 4, 1, 1);
    connect(tooltip,                SIGNAL(currentIndexChanged(int)),
            this,                   SLOT(tooltipColor(int)));

    //QLabel *resLab = new QLabel(tr("export resolution"), this);
    //QLabel *xxxx = new QLabel(("x"), this);
    //xRes = new QDoubleSpinBox(this);
    //yRes = new QDoubleSpinBox(this);
    //xRes->setRange(100, 10000);
    //yRes->setRange(100, 10000);
    //xRes->setSingleStep(100);
    //yRes->setSingleStep(100);
    //xRes->setDecimals(0);
    //yRes->setDecimals(0);
    //xRes->setValue(viewPortSettings::instance().exportSize().width());
    //yRes->setValue(viewPortSettings::instance().exportSize().height());
    //QHBoxLayout *resLay = new QHBoxLayout();
    //resLay->addWidget(xRes);
    //resLay->addWidget(xxxx);
    //resLay->addWidget(yRes);
    //layout->addWidget(resLab, 15,2, 1, 1);
	//layout->addLayout(resLay, 15,4, 1, 1);
    //connect(xRes,               SIGNAL(valueChanged(double)),
    //        this,               SLOT(resChanged()));

    //connect(yRes,               SIGNAL(valueChanged(double)),
    //        this,               SLOT(resChanged()));




//    for(int i=0; i<7; i++)
//	layout->setRowMinimumHeight(i, 64);

    layout->setContentsMargins(100, 50, 10, 10);


}


void miscSettingsWidget::Echanged(const int &i)
{
    unitSettings::instance().setEEh(static_cast<unitSettings::EEH>(EEhBox->itemData(i, 32).toInt()));
}

void miscSettingsWidget::Pchanged(const int &i)
{
    unitSettings::instance().setPEh(static_cast<unitSettings::PMEH>(PEhBox->itemData(i, 32).toInt()));
}

void miscSettingsWidget::Ichanged(const int &i)
{
    unitSettings::instance().setIEh(static_cast<unitSettings::ProfEH>(IEhBox->itemData(i, 32).toInt()));
}

void miscSettingsWidget::setLang(const QString &lang)
{
    QSettings settings("clazzes.org", "Colin", this);
    settings.setValue("language", QVariant(lang));
}

void miscSettingsWidget::ctrlWheelChanged(const int &i)
{
    viewPortSettings::instance().setCtrlMod(static_cast<Colin::Elements>(zBox[0]->itemData(i, 32).toInt()));
}

void miscSettingsWidget::shiftWheelChanged(const int &i)
{
    viewPortSettings::instance().setShiftMod(static_cast<Colin::Elements>(zBox[1]->itemData(i, 32).toInt()));
}

void miscSettingsWidget::altWheelChanged(const int &i)
{
    viewPortSettings::instance().setAltMod(static_cast<Colin::Elements>(zBox[2]->itemData(i, 32).toInt()));
}

void miscSettingsWidget::AAChanged(const int &i)
{
    viewPortSettings::instance().setAntiAliasing(i==0);
}

void miscSettingsWidget::mRadChanged(const double &mR)
{
    viewPortSettings::instance().setMtoRad(mR);
}

void miscSettingsWidget::resultWChanged(const double &s)
{
    viewPortSettings::instance().setResultsWidgetScale(s);
}

//void miscSettingsWidget::resChanged()
//{
//    viewPortSettings::instance().setExportSize(QSize(xRes->value(), yRes->value()));
//}

void miscSettingsWidget::precisonChanged(const double &prec)
{
    unitSettings::instance().setPrecison(prec);
}

void miscSettingsWidget::tooltipColor(const int &row)
{
    colinIcons::instance().setDark(row);
}
