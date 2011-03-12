#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QKeyEvent>

#include "filelist.h"
#include "colinicons.h"
#include "treedelegate.h"

#ifndef QT_NO_DEBUG
#include <QtCore/QTextStream>
#endif


class treeView : public QTreeWidget
{
    Q_OBJECT
public:
    explicit treeView(QWidget *parent = 0);




    void recount(QTreeWidgetItem *parent, const int &i);
    void fill();


    void keyPressEvent(QKeyEvent *e);


signals:

public slots:

    void dumpTw(QObject *o);
    void setTw(wgv_tw *t);
    void actualSelection();
    void actualEH();
    void selectionChanged();

    void popupEditor(const QModelIndex &index);
    void popupMenu(const QPoint &p);


    void addNode(const wgv_node &n, int i = -1);
    void addBeam(const wgv_beam &b, int i = -1);
    void addLoad(const wgv_load &l, int i = -1);

    void addNode(const int &i){addNode(tw->node(i), i);}
    void addBeam(const int &i){addBeam(tw->beam(i), i);}
    void addLoad(const int &i){addLoad(tw->load(i), i);}

    void removeNode(const int &i);
    void removeBeam(const int &i);
    void removeLoad(const int &i);

    void setNode(const int &i, const wgv_node &n);
    void setBeam(const int &i, const wgv_beam &b);
    void setLoad(const int &i, const wgv_load &l);

    void setNode(const int &i){setNode(i, tw->node(i));}
    void setBeam(const int &i){setBeam(i, tw->beam(i));}
    void setLoad(const int &i){setLoad(i, tw->load(i));}


    void writeData(QTreeWidgetItem *item, int column);

    void disableSelection(){selectionisUpToDate = true;}
    void enableSelection(){selectionisUpToDate = false; actualSelection();}

    //void editorClosed(QWidget *w, QAbstractItemDelegate::EndEditHint eh);

    void nextColumn();
    void previousColumn();
    void nextItem();
    void previousItem();

private:

    void setHeader();
    QTreeWidgetItem *nodeParent;
    QTreeWidgetItem *beamParent;
    QTreeWidgetItem *loadParent;

    int opencolumn;
    QTreeWidgetItem *openitem;

    bool selectionisUpToDate;
    wgv_tw *tw;
    treeDelegate *delegate;
    bool isChanging;


};

#endif // TREEVIEW_H
