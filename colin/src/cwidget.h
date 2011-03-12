#ifndef CWIDGET_H
#define CWIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtCore/QByteArray>

#include "viewport.h"


class cWidget : public QWidget
{
    Q_OBJECT
public:
    explicit cWidget(QWidget *parent = 0);
    void repaintyourChildren();

    inline void zoomRect(const int &i, const QRectF &r) {if(views.at(i)->isVisible())
        views[i]->zoomRect(filelist::instance().currentFile()->view(i).mapRect(r), true);}

signals:
    void viewChanged();
    void basePoint(QPointF p);

public slots:
    void idrequest(int* id);
    void setViewCount(int i);
    void setVisibleViews(const QByteArray &vis);
    void setTw(wgv_tw* tw);
    void maximizeView();
    void hideView();
    void showAll();
    void emitBasePoint(QPointF p){emit basePoint(p);}

    inline int viewCount() const {return views.size();}
    inline int rowmax() const {return rowmax_;}
    inline bool isVisible(const int &i) const {return views.at(i)->isVisible();}

private:
    QHBoxLayout *layout;
    QList<viewport*> views;
    int rowmax_;
    int hasFullArea;
    int shown;

};


#endif // CWIDGET_H
