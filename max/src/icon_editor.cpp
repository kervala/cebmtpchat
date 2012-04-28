/*
    This file is part of Max.

    Max is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    Max is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Max; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "common.h"
#include "icon_editor.h"

#ifdef DEBUG_NEW
#define new DEBUG_NEW
#endif

IconEditor::IconEditor(QWidget *parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_StaticContents);
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

	curColor = Qt::black;
	zoom = 8;

	image = QImage(16, 16, QImage::Format_ARGB32);
	image.fill(qRgba(0, 0, 0, 0));
}

QSize IconEditor::sizeHint() const
{
	QSize size = zoom * image.size();
	if (zoom >= 3)
		size += QSize(1, 1);
	return size;
}

void IconEditor::setPenColor(const QColor &newColor)
{
	curColor = newColor;
}

void IconEditor::setIconImage(const QImage &newImage)
{
	if (newImage != image) {
		image = newImage.convertToFormat(QImage::Format_ARGB32);
		update();
		updateGeometry();
	}
}

void IconEditor::setZoomFactor(int newZoom)
{
	if (newZoom < 1)
		newZoom = 1;

	if (newZoom != zoom) {
		zoom = newZoom;
		update();
		updateGeometry();
	}
}

void IconEditor::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);

	if (zoom >= 3) {
		painter.setPen(palette().foreground().color());
		for (int i = 0; i <= image.width(); ++i)
			painter.drawLine(zoom * i, 0,
							 zoom * i, zoom * image.height());
		for (int j = 0; j <= image.height(); ++j)
			painter.drawLine(0, zoom * j,
							 zoom * image.width(), zoom * j);
	}

	for (int i = 0; i < image.width(); ++i) {
		for (int j = 0; j < image.height(); ++j) {
			QRect rect = pixelRect(i, j);
			if (!event->region().intersect(rect).isEmpty()) {
				QColor color = QColor::fromRgba(image.pixel(i, j));
				painter.fillRect(rect, color);
			}
		}
	}
}

QRect IconEditor::pixelRect(int i, int j) const
{
	if (zoom >= 3) {
		return QRect(zoom * i + 1, zoom * j + 1, zoom - 1, zoom - 1);
	} else {
		return QRect(zoom * i, zoom * j, zoom, zoom);
	}
}

void IconEditor::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		setImagePixel(event->pos(), true);
	} else if (event->button() == Qt::RightButton) {
		setImagePixel(event->pos(), false);
	}
}

void IconEditor::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton) {
		setImagePixel(event->pos(), true);
	} else if (event->buttons() & Qt::RightButton) {
		setImagePixel(event->pos(), false);
	}
}

void IconEditor::setImagePixel(const QPoint &pos, bool opaque)
{
	int i = pos.x() / zoom;
	int j = pos.y() / zoom;

	if (image.rect().contains(i, j)) {
		if (opaque) {
			image.setPixel(i , j, penColor().rgba());
		} else {
			image.setPixel(i, j, qRgba(0, 0, 0, 0));
		}

		update(pixelRect(i, j));
	}
}
