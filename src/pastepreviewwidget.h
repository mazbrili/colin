/***********************************************************
 * $Id$
 *
 * Colin
 *
 * Copyright (C) 2011 Matthias Rauter (matthias.rauter@student.uibk.ac.at)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details."
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see
 *
 * http://www.gnu.org/licenses/.
 *
 * Author: Matthias Rauter
 *
 ***********************************************************/

#ifndef PASTEPREVIEWWIDGET_H
#define PASTEPREVIEWWIDGET_H

#include <QtGui/QWidget>

class QPushButton;


class pastePreviewWidget : public QWidget
{
    Q_OBJECT
public:
	explicit pastePreviewWidget(int nr, QWidget *parent = 0);
	void paintEvent(QPaintEvent *e);
	int heightForWidth(int w) const;
	void mousePressEvent(QMouseEvent *e);
	void resizeEvent(QResizeEvent *e);

signals:
	void pasteRequest();
public slots:
	void giefPix(QImage image, int nr);
	void paste();
	void remove();
private:
	QPushButton *removeButton;
	int index;
	QImage image;

};

#endif // PASTEPREVIEWWIDGET_H
