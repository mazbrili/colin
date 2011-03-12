#ifndef PREVIEWRENDERER_H
#define PREVIEWRENDERER_H

#include <QtCore/QThread>
#include "wgv_tw.h"
#include "xmlreader.h"
#include "structpainter.h"

class previewRenderer : public QThread
{
    Q_OBJECT
public:
    previewRenderer(const QSize &s, QObject *parent = 0);
    ~previewRenderer();
    void run();
signals:
    void finished(const QString &url, const QImage &pix);
public slots:
    void renderFile(const QString &url);
    void setSize(const QSize &s);

private:
    QStringList toDo_;
    QSize size_;
    QMutex mutex;
    QWaitCondition waitCondition;
    bool killYourSelf;

};

#endif // PREVIEWRENDERER_H
