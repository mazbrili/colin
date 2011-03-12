#include "varcombobox.h"


varComboBox::varComboBox(wgv_tw::VarName varname, QGridLayout *layout)
{

    varName = varname;

    name = new QLabel(varString(varName));
    value = new QComboBox();
    if(varname == wgv_tw::Bearing_h ||
       varname == wgv_tw::Bearing_v ||
       varname == wgv_tw::Bearing_m ||
       varname == wgv_tw::Joint     )
    {
        value->addItem(tr("free"));
        value->addItem(tr("locked"));
        value->addItem(tr("spring"));
        value->setEditable(true);
    }
    else if(varname == wgv_tw::Mat)
    {
        value->addItem(tr("mat1"));
        value->addItem(tr("mat2"));
        value->addItem(tr("etc"));
    }
    else if(varname == wgv_tw::Profile)
    {
        value->addItem(tr("qs1"));
        value->addItem(tr("qs2"));
        value->addItem(tr("etc"));
    }
    searchB = new QPushButton(colinIcons::instance().icon(Colin::drawZoomAll), "");

    int i = layout->rowCount();
    layout->addWidget(name, i, 0, 1, 1, Qt::AlignRight);
    layout->addWidget(value, i, 1, 1, 1);
    layout->addWidget(searchB, i, 2, 1, 1);

    name->setBuddy(value);
    searchB->setFixedSize(24, 24);
    name->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    value->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
}

void varComboBox::aktual()
{

}

QString varComboBox::varString(wgv_tw::VarName var) const
{
    switch(var)
    {
    case wgv_tw::Bearing_h:
        return (QObject::tr("H"));
    case wgv_tw::Bearing_v:
        return (QObject::tr("V"));
    case wgv_tw::Bearing_m:
        return (QObject::tr("Phi"));
    case wgv_tw::Mat:
        return (QObject::tr("mat"));
    case wgv_tw::Profile:
        return (QObject::tr("Qs"));
    default:
        return ("XXXXXXXXX");
    }
}
