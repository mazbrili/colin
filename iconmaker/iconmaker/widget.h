#ifndef WIDGET_H
#define WIDGET_H

#include <QtGui/QWidget>

class QComboBox;
class QPushButton;
class QCheckBox;

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
	void setShadow(bool s);
	void saveIcon();
private:
	QWidget *paintTarget;
	QComboBox *sizing,
			  *lineWidth,
			  *coloring,
			  *name;
	QCheckBox *shadow;
	QPushButton *save;

};

#endif // WIDGET_H
