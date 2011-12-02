#ifndef VIEWPORTLAYOUT_H
#define VIEWPORTLAYOUT_H

#include <QtGui/QLayout>
#include <QtGui/QWidget>

class ViewPortLayout : public QLayout
{
    Q_OBJECT
public:
	explicit ViewPortLayout(QWidget *parent = 0);
	~ViewPortLayout();

	void addItem(QLayoutItem *item);
	int horizontalSpacing() const {return 0;}
	int verticalSpacing() const {return 0;}
	Qt::Orientations expandingDirections() const;
	bool hasHeightForWidth() const;
	int heightForWidth(int) const;
	int count() const;
	QLayoutItem *itemAt(int index) const;
	QSize minimumSize() const;
	void setGeometry(const QRect &rect);
	QSize sizeHint() const;
	QLayoutItem *takeAt(int index);

private:
	void doLayout(const QRect &rect) const;

	QList<QLayoutItem *> itemList;
};

#endif // VIEWPORTLAYOUT_H
