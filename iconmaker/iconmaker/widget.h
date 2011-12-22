#ifndef WIDGET_H
#define WIDGET_H

#include <QtGui/QWidget>

class QComboBox;
class QPushButton;

class widget : public QWidget
{
    Q_OBJECT
public:
    explicit widget(QWidget *parent = 0);

signals:

public slots:
	void setTargetSize(QString s);
	void setTargetLineWidth(QString s);
	void setTargetColor(QString s);
	void setTargetName(QString s);
	void saveIcon();
private:
	QWidget *paintTarget;
	QComboBox *sizing,
			  *lineWidth,
			  *coloring,
			  *name;
	QPushButton *save;

};

#endif // WIDGET_H
