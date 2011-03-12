#ifndef TREEDELEGATE_H
#define TREEDELEGATE_H

#include <QtGui/QItemDelegate>
#include <QtGui/QComboBox>
#include <QtGui/QKeyEvent>
#include <QtCore/QCoreApplication>

#include "filelist.h"


class treeDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit treeDelegate(QObject *parent = 0 );
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
    void setEditorData(QWidget *editor, const QModelIndex &index ) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const;

protected:
    bool eventFilter(QObject *obj, QEvent *event){
        if(event->type() == QEvent::KeyPress)
        {
            if(static_cast<QKeyEvent*>(event)->key() == Qt::Key_Tab )
            {
                commitData(static_cast<QWidget*>(obj));
                obj->deleteLater();
                emit openNext();
                return true;
            }
            else if(static_cast<QKeyEvent*>(event)->key() == Qt::Key_Backtab)
            {
                commitData(static_cast<QWidget*>(obj));
                obj->deleteLater();
                emit openPrevious();
                return true;
            }
            else if(static_cast<QKeyEvent*>(event)->key() == Qt::Key_Down)
            {
                commitData(static_cast<QWidget*>(obj));
                obj->deleteLater();
                emit openNextItem();
                return true;
            }
            else if(static_cast<QKeyEvent*>(event)->key() == Qt::Key_Up)
            {
                commitData(static_cast<QWidget*>(obj));
                obj->deleteLater();
                emit openPreviousItem();
                return true;
            }
            else if(static_cast<QKeyEvent*>(event)->key() == Qt::Key_Return)
            {
                commitData(static_cast<QWidget*>(obj));
                obj->deleteLater();
                return true;
            }
	    else if(static_cast<QKeyEvent*>(event)->key() == Qt::Key_Escape)
            {
                obj->deleteLater();
                return true;
            }
        }
        return QItemDelegate::eventFilter(obj, event);
    }

signals:
    void openPrevious();
    void openNext();
    void openPreviousItem();
    void openNextItem();

public slots:

private slots:
    void emitCommitData();
};

#endif // TREEDELEGATE_H
