#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include <QtGui/QApplication>
#include <QtGui/QWidget>
#include <QtGui/QPainter>
#include <QtGui/QStyle>
#include <QtGui/QStyleOptionButton>
#include <QtGui/QPaintEvent>

#include "colinicons.h"
#include "filelist.h"
#include "previewrenderer.h"


class previewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit previewWidget(QWidget *parent = 0);


    void paintEvent(QPaintEvent *e);
    void enterEvent(QEvent *){repaint();}
    void leaveEvent(QEvent *){repaint();}
    void mouseMoveEvent(QMouseEvent *){repaint(QRegion(xRect()));}
    void mousePressEvent(QMouseEvent *e);
    void resizeEvent(QResizeEvent *e);

    QSize sizeHint() const{return QSize(200, 155);}
    static previewRenderer *renderer;
protected:
    int heightForWidth(int w) const;
public slots:
    void giefPix(const QString &url, const QImage &pix);
    void actualUrl();
private:
    bool hasPreview;
    int id;
    static int count;
    QImage image;
    QString myUrl;
    QRect xRect();

};

#endif // PREVIEWWIDGET_H
