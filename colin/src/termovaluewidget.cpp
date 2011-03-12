#include "termovaluewidget.h"

termoValueWidget *termoValueWidget::instance_ = NULL;

termoValueWidget::termoValueWidget(QWidget *parent) :
    QWidget(parent)
{
    if(instance_ != NULL)
        delete instance_;
    instance_ = this;
    setMouseTracking(true);
    setFixedSize(100, 120);
    setAttribute(Qt::WA_TranslucentBackground, true);
    form = 1;
    formBut = new QPushButton(colinIcons::instance().icon(wgv_load::tempChange), "", this);
    formBut->setIconSize(QSize(32, 32));
    formBut->setFixedSize(QSize(50, 50));
    formBut->move(100-50, 120-50);
    formBut->installEventFilter(this);
    value = 20;
    min = -40;
    max = 40;
    mousePressed = false;
    setFocusPolicy(Qt::WheelFocus);
    setFocus();

    setCursor(Qt::ArrowCursor);

    connect(formBut,            SIGNAL(clicked()),
            this,               SLOT(typChanged()));

}
termoValueWidget::~termoValueWidget()
{
    instance_ = NULL;
}


void termoValueWidget::typChanged()
{
    if (form == 1)
    {
        form = 2;
        formBut->setIcon(colinIcons::instance().icon(wgv_load::tempDiffrence));
    }
    else
    {
        form = 1;
        formBut->setIcon(colinIcons::instance().icon(wgv_load::tempChange));
    }
}

void termoValueWidget::valChanged(const QString &s)
{
    bool ok;
    double d = s.toDouble(&ok);
    if(ok)
        valChanged(d);
}

void termoValueWidget::valChanged(const double &d)
{
    value = d;
}

void termoValueWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    double dMin = min;
    double dMax = max;
    structPainter::drawThermometer(&p, QRect(0, 0, 50, 120), value, hasFocus(), &dMax, &dMin);
    minP = dMin;
    maxP = dMax;

#ifndef QT_NO_DEBUG
    QTextStream debugS(stdout);
    debugS << endl << endl << "entering termoWidget PaintEvent" << endl;
    debugS << "value = " << value << endl;
    debugS << "min = " << min << endl;
    debugS << "max = " << min << endl;
    debugS << "minP = " << minP << endl;
    debugS << "maxP = " << maxP << endl << endl << endl;
#endif
}

void termoValueWidget::mousePressEvent(QMouseEvent *e)
{
    mousePressed = true;
    mouseMoveEvent(e);
}

void termoValueWidget::mouseMoveEvent(QMouseEvent *e)
{
    if(!mousePressed)
        return;

    double yVal;
    if(e->posF().y() < maxP)
        yVal = maxP;
    else if(e->posF().y() > minP)
        yVal = minP;
    else
        yVal = e->posF().y();

#ifndef QT_NO_DEBUG
    QTextStream debugS(stdout);
    debugS << endl << endl << "entering termoWidget mouseMoveEvent" << endl;
    debugS << "value old = " << value << endl;
    debugS << "value new before snap = " << max -(yVal-maxP)*(max-min)/(minP-maxP) << endl;
#endif

    double val = max -(yVal-maxP)*(max-min)/(minP-maxP);
    double rounded = static_cast<int>(val)/10;
    rounded *= 10;
    if(fabs(val-rounded) < 2)
        val = rounded;
    else if(fabs(val-(rounded+10)) < 2)
        val = rounded +10;
    else
        val = static_cast<int>(val);

    value = val;

    repaint();
}

void termoValueWidget::mouseReleaseEvent(QMouseEvent *)
{
    mousePressed = false;

    adjustLimites();
    repaint();
}


void termoValueWidget::wheelEvent(QWheelEvent *e)
{
    double val;
    if(e->delta() > 0)
        val = value + 10;
    else
        val = value - 10;

    double rounded = static_cast<int>(val)/10;
    rounded *= 10;
    if(fabs(val-rounded) < 2)
        val = rounded;
    else if(fabs(val-(rounded+10)) < 2)
        val = rounded +10;
    else
        val = static_cast<int>(val);

    value = val;

    adjustLimites();
    repaint();
}


void termoValueWidget::adjustLimites()
{
    if(value>0)
        max = value*2;
    else
        max = -value/2;

    max = qMax(max, 10.);

    if(value > 0)
        min = -value/2;
    else
        min = value*2;

    min = qMin(min, -10.);


}


void termoValueWidget::hideEvent(QHideEvent *)
{
    emit chosen(value, form);
    deleteLater();
}

void termoValueWidget::keyPressEvent(QKeyEvent *e)
{
    double val;
    if(e->key() == Qt::Key_Up)
        val = value + 10;
    else if(e->key() == Qt::Key_Down)
        val = value - 10;

    if(e->key() == Qt::Key_Left ||
       e->key() == Qt::Key_Right)
    {
        typChanged();
        e->setAccepted(true);
        return;
    }
    if(e->key() == Qt::Key_Up ||
       e->key() == Qt::Key_Down)
    {
        double rounded = static_cast<int>(val)/10;
        rounded *= 10;
        if(fabs(val-rounded) < 2)
            val = rounded;
        else if(fabs(val-(rounded+10)) < 2)
            val = rounded +10;
        else
            val = static_cast<int>(val);

        value = val;

        e->setAccepted(true);
        adjustLimites();
        repaint();
    }
    else if(e->key() == Qt::Key_Enter ||
            e->key() == Qt::Key_Backspace ||
            e->key() == Qt::Key_Return)
    {
        emit chosen(value, form);
        deleteLater();
        e->setAccepted(true);
    }
    else if(e->key() == Qt::Key_Escape)
    {
        deleteLater();
    }
    else
        e->setAccepted(false);
}
