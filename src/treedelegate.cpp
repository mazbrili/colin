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

#include "viewportsettings.h"
#include "colinicons.h"
#include "unitsettings.h"

treeDelegate::treeDelegate(QObject *parent) :
	QItemDelegate(parent)
{
	lastIndex = QModelIndex();
	commitOnFocusLoss = false;
}

QWidget *treeDelegate::createEditor( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
	qDebug() << "treeDelegate: Editor requested for Index: " << index;
	lastIndex = index;
	commitOnFocusLoss = true;
	if(index.data(Qt::UserRole).toInt() == ColinStruct::NodalLoadTyp)
	{
		QComboBox *combo = new QComboBox(parent);
		combo->addItem(colinIcons::instance().icon(ColinLoad::nodeLoad), tr("nodal load"));
		combo->addItem(colinIcons::instance().icon(ColinLoad::moment), tr("moment"));

		connect(combo,              SIGNAL(activated(int)),
				this,               SLOT(emitCommitData()));

		combo->setFocus(Qt::PopupFocusReason);
		return combo;
	}
	if(index.data(Qt::UserRole).toInt() == ColinStruct::BeamLoadTyp)
	{
		QComboBox *combo = new QComboBox(parent);
		combo->addItem(colinIcons::instance().icon(ColinLoad::uniformlyDistibutedLoad), tr("uniform beam load"));
		combo->addItem(colinIcons::instance().icon(ColinLoad::decreasingLinearLoad), tr("decreasing beam load"));
		combo->addItem(colinIcons::instance().icon(ColinLoad::increasingLinearLoad), tr("increasing beam load"));

		connect(combo,              SIGNAL(activated(int)),
				this,               SLOT(emitCommitData()));

		combo->setFocus(Qt::PopupFocusReason);
		return combo;
	}
	if(index.data(Qt::UserRole).toInt() == ColinStruct::TempLoadTyp)
	{
		QComboBox *combo = new QComboBox(parent);
		combo->addItem(colinIcons::instance().icon(ColinLoad::tempChange), tr("temperature change"));
		combo->addItem(colinIcons::instance().icon(ColinLoad::tempDiffrence), tr("temperature difference"));

		connect(combo,              SIGNAL(activated(int)),
				this,               SLOT(emitCommitData()));

		combo->setFocus(Qt::PopupFocusReason);
		return combo;
	}
	if(index.data(Qt::UserRole).toInt() == ColinStruct::LoadTyp)
	{
		QComboBox *combo = new QComboBox(parent);
		combo->addItem(colinIcons::instance().icon(ColinLoad::nodeLoad), tr("nodal load"));
		combo->addItem(colinIcons::instance().icon(ColinLoad::moment), tr("moment"));
		combo->addItem(colinIcons::instance().icon(ColinLoad::uniformlyDistibutedLoad), tr("uniform beam load"));
		combo->addItem(colinIcons::instance().icon(ColinLoad::decreasingLinearLoad), tr("decreasing beam load"));
		combo->addItem(colinIcons::instance().icon(ColinLoad::increasingLinearLoad), tr("increasing beam load"));
		combo->addItem(colinIcons::instance().icon(ColinLoad::tempChange), tr("temperature change"));
		combo->addItem(colinIcons::instance().icon(ColinLoad::tempDiffrence), tr("temperature difference"));
		combo->addItem(colinIcons::instance().icon(ColinLoad::doubleLoadLeft), tr("double load left side"));
		combo->addItem(colinIcons::instance().icon(ColinLoad::doubleLoadRight), tr("double load right side"));

		connect(combo,              SIGNAL(activated(int)),
				this,               SLOT(emitCommitData()));

		combo->setFocus(Qt::PopupFocusReason);
		return combo;
	}
	if(index.data(32).toInt() == ColinStruct::Mat) //column for material
	{
		QComboBox *combo = new QComboBox(parent);
		for(int i=0; i<LIB.mats_n(); i++)
		{
			combo->addItem(LIB.mat(i).name());
		}

		connect(combo,              SIGNAL(activated(int)),
				this,               SLOT(emitCommitData()));

		combo->setFocus(Qt::PopupFocusReason);
		return combo;
	}

	if(index.data(32).toInt() == ColinStruct::CrossSection) //column for Qs
	{
		QComboBox *combo = new QComboBox(parent);
		for(int i=0; i<LIB.profiles_n(); i++)
		{
			combo->addItem(LIB.Profile(i).name());
		}

		connect(combo,              SIGNAL(activated(int)),
				this,               SLOT(emitCommitData()));
		combo->setFocus(Qt::PopupFocusReason);
		return combo;
	}


	if(index.data(32).toInt() == ColinStruct::Bearing_h ||
	   index.data(32).toInt() == ColinStruct::Bearing_v ||
	   index.data(32).toInt() == ColinStruct::Bearing_m )
	{
		QComboBox *combo = new QComboBox(parent);
		combo->addItem(tr("free"));
		combo->addItem(tr("locked"));
		QString s = index.data(0).toString();
		if(s.contains(UNIT.Feh()))
		{
			s.remove(UNIT.Feh());
			combo->addItem(s);
			combo->setCurrentIndex(combo->count()-1);
		}
		else if(s.contains(UNIT.FMeh()))
		{
			s.remove(UNIT.FMeh());
			combo->addItem(s);
			combo->setCurrentIndex(combo->count()-1);
		}
		combo->setEditable(true);

		connect(combo,              SIGNAL(activated(int)),
				this,               SLOT(emitCommitData()));
		combo->setFocus(Qt::PopupFocusReason);
		return combo;
	}

	if(index.data(32).toInt() == ColinStruct::Spring)
	{
		QComboBox *combo = new QComboBox(parent);
		combo->addItem(tr("free"));
		combo->addItem(tr("locked"));
		combo->setEditable(true);

		connect(combo,              SIGNAL(activated(int)),
				this,               SLOT(emitCommitData()));
		combo->setFocus(Qt::PopupFocusReason);
		return combo;
	}

	if(index.data(32).toInt() == ColinStruct::Joint)
	{
		QComboBox *combo = new QComboBox(parent);
		combo->addItem(tr("hinge"));
		combo->addItem(tr("no hinge"));
		combo->setEditable(true);

		connect(combo,              SIGNAL(activated(int)),
				this,               SLOT(emitCommitData()));
		combo->setFocus(Qt::PopupFocusReason);
		return combo;
	}

	if(((index.data(32).toInt() & (~ColinStruct::creation)) == ColinStruct::CLSBLS))
	{
		QComboBox *combo = new QComboBox(parent);
		ColinStruct *tw = filelist::instance().currentFile();
		for(int i=0; i<tw->bls_n(); i++){
			combo->addItem(tw->bls(i).name());
		}

		connect(combo,				SIGNAL(activated(int)),
				this,				SLOT(emitCommitData()));
		combo->setFocus(Qt::PopupFocusReason);
		return combo;
	}

	if(((index.data(32).toInt() & (~ColinStruct::creation)) == ColinStruct::LoadSet))
	{
		QComboBox *combo = new QComboBox(parent);
		ColinStruct *tw = filelist::instance().currentFile();
		combo->addItem(tr("none"));
		for(int i=0; i<tw->bls_n(); i++){
			combo->addItem(tw->bls(i).name());
		}

		connect(combo,				SIGNAL(activated(int)),
				this,				SLOT(emitCommitData()));
		combo->setFocus(Qt::PopupFocusReason);
		return combo;
	}

	if(((index.data(32).toInt() & (~ColinStruct::creation)) == ColinStruct::BLScolor))
	{
		QComboBox *combo = new QComboBox(parent);

		foreach(QColor color, viewPortSettings::instance().standardColors()){
			QIcon icon = colinIcons::instance().icon(color);
			combo->addItem(icon, color.name());
		}
		combo->addItem(tr("more..."));

		connect(combo,				SIGNAL(activated(int)),
				this,				SLOT(emitCommitData()));
		combo->setFocus(Qt::PopupFocusReason);
		return combo;
	}
	if(((index.data(32).toInt() & (~ColinStruct::creation)) == ColinStruct::Mat))
	{
		QComboBox *combo = new QComboBox(parent);

		ColinStruct *tw = filelist::instance().currentFile();

		for(int i = 0; i< LIB.mats_n(); i++)
		{
			combo->addItem(colinIcons::instance().icon(Colin::Material), LIB.mat(i).name());
		}

		combo->setCurrentIndex(tw->beam(index.row()).MatI());
		connect(combo,				SIGNAL(activated(int)),
				this,				SLOT(emitCommitData()));
		combo->setFocus(Qt::PopupFocusReason);
		return combo;
	}
	if(((index.data(32).toInt() & (~ColinStruct::creation)) == ColinStruct::CrossSection))
	{
		QComboBox *combo = new QComboBox(parent);

		ColinStruct *tw = filelist::instance().currentFile();

		for(int i = 0; i< LIB.profiles_n(); i++)
		{
			combo->addItem(colinIcons::instance().icon(Colin::Profile), LIB.Profile(i).name());
		}

		combo->setCurrentIndex(tw->beam(index.row()).ProfileI());
		connect(combo,				SIGNAL(activated(int)),
				this,				SLOT(emitCommitData()));
		combo->setFocus(Qt::PopupFocusReason);
		return combo;
	}

	QWidget *temp = QItemDelegate::createEditor( parent, option, index );
	temp->setFocus(Qt::PopupFocusReason);
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


bool treeDelegate::eventFilter(QObject *obj, QEvent *event){
	if(event->type() == QEvent::FocusOut)
	{
		QFocusEvent *e = static_cast<QFocusEvent*>(event);
		if(e->reason() != Qt::PopupFocusReason &&
		   e->reason() != Qt::OtherFocusReason){
			qDebug() << "commiting Data on FocusReason " << e->reason();
			if(commitOnFocusLoss){
				commitData(static_cast<QWidget*>(obj));
				obj->deleteLater();
				return true;
			}
		}
	}
	if(event->type() == QEvent::KeyPress)
	{
		if(static_cast<QKeyEvent*>(event)->key() == Qt::Key_Tab )
		{
			qDebug() << "commiting Data on keyPressEvent Tab";
			commitOnFocusLoss = false;
			commitData(static_cast<QWidget*>(obj));
			emit openNext(lastIndex);
			return true;
		}
		else if(static_cast<QKeyEvent*>(event)->key() == Qt::Key_Backtab)
		{
			qDebug() << "commiting Data on keyPressEvent Backtab";
			commitOnFocusLoss = false;
			commitData(static_cast<QWidget*>(obj));
			emit openPrevious(lastIndex);
			return true;
		}
		else if(static_cast<QKeyEvent*>(event)->key() == Qt::Key_Down)
		{
			qDebug() << "commiting Data on keyPressEvent Down";
			commitOnFocusLoss = false;
			commitData(static_cast<QWidget*>(obj));
			emit openNextItem(lastIndex);
			return true;
		}
		else if(static_cast<QKeyEvent*>(event)->key() == Qt::Key_Up)
		{
			qDebug() << "commiting Data on keyPressEvent Up";
			commitOnFocusLoss = false;
			commitData(static_cast<QWidget*>(obj));
			emit openPreviousItem(lastIndex);
			return true;
		}
		else if(static_cast<QKeyEvent*>(event)->key() == Qt::Key_Return)
		{
			qDebug() << "commiting Data on keyPressEvent Return";
			commitOnFocusLoss = false;
			commitData(static_cast<QWidget*>(obj));
			emit openFirstColumn(lastIndex);
			return true;
		}
	else if(static_cast<QKeyEvent*>(event)->key() == Qt::Key_Escape)
		{
			qDebug() << "returning from Delegate on Escape press";
			commitOnFocusLoss = false;
//#warning restore in treeView
			obj->deleteLater();
			return true;
		}
	}
		return QItemDelegate::eventFilter(obj, event);
}
