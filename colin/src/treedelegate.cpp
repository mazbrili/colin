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
