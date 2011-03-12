#include "colorsettingswidget.h"

colorSettingsWidget::colorSettingsWidget(QWidget *parent) :
    QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    group = new QButtonGroup(this);

    QPushButton *button;
    int i;
    for(i=0; ; i++)
    {
        QString name = Colin::colorToString(static_cast<Colin::Color>(i));
        if(name == "")
        {
            break;
        }
        QIcon icon = colinIcons::instance().icon(static_cast<Colin::Color>(i));
        button = new QPushButton(icon, "    "+name, this);
        button->setFixedWidth(200);
        group->addButton(button, i);
        layout->addWidget(button, i, 0, 1, 1, Qt::AlignLeft);
    }

    group->setExclusive(true);
    foreach(QAbstractButton *b, group->buttons())
    {
        b->setCheckable(true);
    }
    group->button(0)->setChecked(true);

    dialog = new QColorDialog(this);
    dialog->setWindowFlags(Qt::Widget);
    dialog->setOption(QColorDialog::ShowAlphaChannel, true);
    dialog->setOption(QColorDialog::NoButtons, true);
    dialog->setModal(false);
    dialog->setCurrentColor(viewPortSettings::instance().color(Colin::C_Node));
    dialog->move(300, 100);
    connect(group,          SIGNAL(buttonClicked(int)),
            this,           SLOT(setColor(int)));

    connect(dialog,         SIGNAL(currentColorChanged(QColor)),
            this,           SLOT(changeCurrentColor(QColor)));

}

void colorSettingsWidget::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Down && group->checkedId()<group->buttons().size()-1)
        {
            group->button(group->checkedId()+1)->setChecked(true);
            setColor(group->checkedId());
        }
    else if(e->key() == Qt::Key_Up && group->checkedId()>0)
    {
        group->button(group->checkedId()-1)->setChecked(true);
        setColor(group->checkedId());
    }
    else
    {
        e->ignore();
    }

}

void colorSettingsWidget::setColor(const int &i)
{
    Colin::Color color = static_cast<Colin::Color>(i);
    dialog->setCurrentColor(viewPortSettings::instance().color(color));

    QWidget* par = static_cast<QWidget*>(parent());

    int ypos = par->height()/2-dialog->height()/2;
    dialog->move(300,
                 mapFromGlobal(par->mapToGlobal(par->pos())+QPoint(0, ypos)).y());

}

void colorSettingsWidget::changeCurrentColor(const QColor &c)
{
    Colin::Color colornr = static_cast<Colin::Color>(group->checkedId());
    viewPortSettings::instance().setColor(colornr,
                                          c);
    group->checkedButton()->setIcon(colinIcons::instance().icon(colornr));
    if(dialog->isHidden())
        dialog->show();
}

