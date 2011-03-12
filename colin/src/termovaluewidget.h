/* widget for temperature-input

   this is a singletone

   access: termoValueWidget::instance()

*/

#ifndef TERMOVALUEWIDGET_H
#define TERMOVALUEWIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include "structpainter.h"
#include <QtGui/QLineEdit>
#include <QtGui/QMouseEvent>

#ifndef QT_NO_DEBUG
#include <QtCore/QTextStream>
#endif

class termoValueWidget : public QWidget
{
    Q_OBJECT
public:
    explicit termoValueWidget(QWidget *parent = 0);
    ~termoValueWidget();

    const static termoValueWidget &instance(){if(!instance_) instance_ = new termoValueWidget(); return *instance_;}
    static void deleteInstance(){if (instance_) delete instance_;}

    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);
    void hideEvent(QHideEvent *);
    void keyPressEvent(QKeyEvent *e);

signals:
    void chosen(const double &temp, const int &val);

private slots:
    void typChanged();
    void valChanged(const QString &s);
    void valChanged(const double &d);

private:
    void adjustLimites();
    QPushButton *formBut;
    double value;
    double min;
    double max;
    double minP;
    double maxP;
    bool mousePressed;
    static termoValueWidget *instance_;
    int form;

protected:
    bool eventFilter(QObject *obj, QEvent *event){
        if(event->type() == QEvent::FocusIn)
        {
            this->setFocus();
            return true;
        }
        else
            return QObject::eventFilter(obj, event);
    }
};

#endif // TERMOVALUEWIDGET_H
