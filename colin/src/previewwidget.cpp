#include "previewwidget.h"

int previewWidget::count = 0;
previewRenderer *previewWidget::renderer = 0;

previewWidget::previewWidget(QWidget *parent) :
    QWidget(parent)
{

    Q_ASSERT(renderer);


    setMouseTracking(true);


    id = count++;
    hasPreview = false;


    myUrl = filelist::instance().recUsedFileName(id);

    QSizePolicy policy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    policy.setHeightForWidth(true);

    setSizePolicy(policy);

    renderer->renderFile(myUrl);

}

void previewWidget::resizeEvent(QResizeEvent *)
{
    renderer->setSize(size());
    renderer->renderFile(myUrl);
    repaint();
}

void previewWidget::actualUrl()
{
    if(myUrl != filelist::instance().recUsedFileName(id))
    {
        myUrl = filelist::instance().recUsedFileName(id);
        renderer->renderFile(myUrl);
        hasPreview = false;
        repaint();
    }
}

void previewWidget::paintEvent(QPaintEvent *)
{
    if(!hasPreview)
        return;

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.drawImage(QRect(QPoint(2,18), QSize(size().width()-4, size().height()-20)), image, image.rect());
    QLinearGradient grad(0, 0, 0, height());
    if(rect().contains(mapFromGlobal(QCursor::pos())))
    {
        grad.setColorAt(0,palette().color(QPalette::Light));
        grad.setColorAt(1,palette().color(QPalette::Midlight));
    }
    else
    {
        grad.setColorAt(0,palette().color(QPalette::Mid));
        grad.setColorAt(1,palette().color(QPalette::Dark));
    }
    QRect r = QRect(1, 1, width()-2, height()-2);
    QPainterPath p1, p2;
    p1.addRoundedRect(r, 5, 5);
    r.setTop(r.top()+20);
    r.setLeft(r.x()+3);
    r.setRight(r.right()-3);
    r.setBottom(r.bottom()-3);
    p2.addRoundedRect(r, 5, 5);
    p1 = p1 - p2;
    p.fillPath(p1, grad);
    if(rect().contains(mapFromGlobal(QCursor::pos())))
    {
        p.setPen(QPen(palette().color(QPalette::Highlight), 1));
        p.setBrush(Qt::NoBrush);
        p.drawRoundedRect(QRect(1, 1, width()-2, height()-2), 5, 5);
    }

    if(xRect().contains(mapFromGlobal(QCursor::pos())))
        colinIcons::instance().icon(Colin::Close).paint(&p, xRect(),
                                                        Qt::AlignCenter, QIcon::Active);
    else
        colinIcons::instance().icon(Colin::Close).paint(&p, xRect());

    p.setPen(palette().color(QPalette::Dark));
    p.drawText(QRect(4, 2, xRect().x()-8, 20-4),
               Qt::AlignCenter,
               QFileInfo(myUrl).fileName());
    p.setPen(palette().color(QPalette::Text));
    p.drawText(QRect(2, 0, xRect().x()-8, 20-4),
               Qt::AlignCenter,
               QFileInfo(myUrl).fileName());

}

QRect previewWidget::xRect()
{
    return QRect(width()-16, 4, 12, 12);
}


void previewWidget::mousePressEvent(QMouseEvent *e)
{
    if(xRect().contains(e->pos()))
        filelist::instance().removeRecUsedFile(id);
    else if(myUrl != "")
        filelist::instance().openT(myUrl);
}

void previewWidget::giefPix(const QString &url, const QImage &pix)
{
    if(url != myUrl)
        return;
    image = pix;
    hasPreview = true;
    repaint();
}


int previewWidget::heightForWidth(int w) const
{
    int h = (w-4)/196.0*135.0+20;
    return h;
}

