#ifndef RESULTVISUALWIDGET_H
#define RESULTVISUALWIDGET_H

#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QMenu>
#include <QtGui/QMouseEvent>
#include <QtGui/QActionGroup>
#include "wgv_tw.h"
#include "catcher.h"

class resultVisualWidget : public QLabel
{
    Q_OBJECT
public:

    enum showHint
    {
        Hide,
        Minimap,
        Results,
        Details
    };

    resultVisualWidget(QWidget *parent = 0);
    void paintEvent(QPaintEvent *);
    static resultVisualWidget &instance(){
        return *instance_;
    }

    void setElement(const catcher::CatchCases &cc, const int &index){setElement(cc, index, 0);}
    void setElement(const catcher::CatchCases &cc, const int &index, const double &x_);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
public slots:
    void checkForMouseOver();
    void setMode(QAction *a);
    void setMode(const showHint &sH);
    void fileChanged(wgv_tw *tw);
private:
    QSize bSize();
    static resultVisualWidget *instance_;
    void paintDetails(QPainter *p);
    void paintMinimap(QPainter *p);
    void paintResults(QPainter *p);
    void paintNodeResults(QPainter *p);
    void paintBeamResults(QPainter *p);
    int object;
    catcher::CatchCases objectType;
    double x;
    void actualGeometry();
    bool mouseIsOver;
    showHint sH;
    QPushButton *mode;
    QActionGroup *aGroup;
};

#endif // RESULTVISUALWIDGET_H
