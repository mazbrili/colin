#include "vareditline.h"

varEditLine::varEditLine(wgv_tw::VarName varname, QGridLayout *layout)
{

    varName = varname;

    name = new QLabel(varString(varName));
    value = new QLineEdit();
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

QString varEditLine::varString(wgv_tw::VarName var) const
{
    switch(var)
    {
    case wgv_tw::x:
        return (QObject::tr("x"));
    case wgv_tw::z:
        return (QObject::tr("z"));
    case wgv_tw::leftNode:
        return (QObject::tr("left"));
    case wgv_tw::rightNode:
        return (QObject::tr("right"));
    default:
        return ("XXXXXXXXX");
    }
}

void varEditLine::aktual()
{
    if(varName == wgv_tw::x || varName == wgv_tw::z)
    {
        double val(0);
	bool set = false;
	bool allthesame = true;
	for(int i=0; i<t->node_n(); i++)
	{
	    if(t->node(i).isSelected())
	    {
		if(set == false)
		{
		    val = (varName == wgv_tw::x) ? t->node(i).x() : t->node(i).z();
		    set = true;
		}
		else if(val != ((varName == wgv_tw::x) ? t->node(i).x() : t->node(i).z()))
		{
		    allthesame = false;
		    break;
		}
	    }
	}
	if(set && allthesame)
	{
	    this->value->setText(QString("%1").arg(val));
	}
	else
	{
	    value->setText(QString(tr("-")));
	}
    }

}

void varEditLine::setTw(wgv_tw *tw)
{

    disconnect(this,        SLOT(aktual()));

    if(tw == NULL)	return;

    connect(tw,             SIGNAL(selectionChanged()),
            this,           SLOT(aktual()));
    t = tw;
    aktual();
}
