#ifndef COLINRUBBERBAND_H
#define COLINRUBBERBAND_H

#include <QtGui/QRubberBand>

class ColinRubberBand : public QRubberBand 
{
    Q_OBJECT
public:
	explicit ColinRubberBand(QRubberBand::Shape s, QWidget *parent = 0);
	void paintEvent(QPaintEvent *e);

signals:

public slots:

};

#endif // COLINRUBBERBAND_H
