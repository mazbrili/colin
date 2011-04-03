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

#include "treedelegate.h"

treeDelegate::treeDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}

QWidget *treeDelegate::createEditor( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{


    if(index.data(32).toInt() == wgv_tw::Mat) //column for material
    {
        QComboBox *combo = new QComboBox(parent);
        for(int i=0; i<LIB.mats_n(); i++)
        {
            combo->addItem(LIB.mat(i).name());
        }
        connect(combo,              SIGNAL(activated(int)),
                this,               SLOT(emitCommitData()));

        combo->setFocus();
        return combo;
    }

    if(index.data(32).toInt() == wgv_tw::Profile) //column for Qs
    {
        QComboBox *combo = new QComboBox(parent);
        for(int i=0; i<LIB.profiles_n(); i++)
        {
            combo->addItem(LIB.Profile(i).name());
        }
        connect(combo,              SIGNAL(activated(int)),
                this,               SLOT(emitCommitData()));
        combo->setFocus();
        return combo;
    }


    if(index.data(32).toInt() == wgv_tw::Bearing_h ||
       index.data(32).toInt() == wgv_tw::Bearing_v ||
       index.data(32).toInt() == wgv_tw::Bearing_m )
    {
        QComboBox *combo = new QComboBox(parent);
        combo->addItem(tr("free"));
        combo->addItem(tr("locked"));
        QString s = index.data(0).toString();
        if(s.contains("c = "))
        {
            s.remove("c =");
            combo->addItem(s);
            combo->setCurrentIndex(combo->count()-1);
        }
        combo->setEditable(true);

	connect(combo,              SIGNAL(activated(int)),
                this,               SLOT(emitCommitData()));
        combo->setFocus();
        return combo;
    }

    if(index.data(32).toInt() == wgv_tw::Spring)
    {
        QComboBox *combo = new QComboBox(parent);
        combo->addItem(tr("free"));
        combo->addItem(tr("locked"));
        combo->setEditable(true);

        connect(combo,              SIGNAL(activated(int)),
                this,               SLOT(emitCommitData()));
        combo->setFocus();
        return combo;
    }

    QWidget *temp = QItemDelegate::createEditor( parent, option, index );
    temp->setFocus();
    return  temp;
}

void treeDelegate::setEditorData( QWidget *editor, const QModelIndex &index ) const
{
    QComboBox *combo = qobject_cast<QComboBox *>( editor );
    if ( !combo )
    {
        QItemDelegate::setEditorData( editor, index );
        return;
    }
    int pos = combo -> findText( index.model() -> data( index ).toString(), Qt::MatchExactly );
    combo -> setCurrentIndex( pos );
}

void treeDelegate::setModelData( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const
{
    QComboBox *combo = qobject_cast<QComboBox *>( editor );
    if ( !combo )
    {
        QItemDelegate::setModelData( editor, model, index );
        return;
    }
    model -> setData( index, combo -> currentText() );
}

void treeDelegate::emitCommitData()
{
    emit commitData( qobject_cast<QWidget *>( sender() ) );
}
