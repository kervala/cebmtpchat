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

#ifndef ICON_EDITOR_H
#define ICON_EDITOR_H

class IconEditor : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QColor penColor READ penColor WRITE setPenColor)
	Q_PROPERTY(QImage iconImage READ iconImage WRITE setIconImage)
	Q_PROPERTY(int zoomFactor READ zoomFactor WRITE setZoomFactor)

public:
		IconEditor(QWidget *parent = 0);

	void setPenColor(const QColor &newColor);
	QColor penColor() const { return curColor; }

	void setZoomFactor(int newZoom);
	int zoomFactor() const { return zoom; }
	void setIconImage(const QImage &newImage);
	QImage iconImage() const { return image;}
	QSize sizeHint() const;

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void paintEvent(QPaintEvent *event);

private:
	void setImagePixel(const QPoint &pos, bool opaque);
	QRect pixelRect(int i, int j) const;

	QColor curColor;
	QImage image;
	int zoom;
};

#endif
