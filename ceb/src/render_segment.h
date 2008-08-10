/* This file is part of CeB.
 * Copyright (C) 2005  Guillaume Denry
 *
 * CeB is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * CeB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with CeB; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef RENDER_SEGMENT_H
#define RENDER_SEGMENT_H

#include <QString>
#include <QFont>
#include <QColor>
#include <QList>
#include <QRegExp>

class RenderSegment
{
public:
    RenderSegment(const QString &text,
                  const QFont &font,
                  const QColor &color)
        : m_text(text),
          m_font(font),
          m_color(color) {}

    const QString &text() const { return m_text; }
    void setText(const QString &value) { m_text = value; }
    const QFont &font() const { return m_font; }
    void setFont(const QFont &value) { m_font = value; }
    const QColor &color() const { return m_color; }
    void setColor(const QColor &value) { m_color = value; }

private:
    QString m_text;
    QFont m_font;
    QColor m_color;
};

class RenderSegmentList : public QList<RenderSegment>
{
public:
    struct TextRange
    {
        int start;
        int length;
        int end() const { return start + length - 1; }
    };

public:
    RenderSegmentList();

    QList<TextRange> urlRanges(const QString &text) const;

    void segmentByURLs();

private:
    QList<QRegExp> urlRegexp;

    int segmentIndex(const RenderSegmentList &segments, int charIndex) const;
    TextRange segmentRange(const RenderSegmentList &segments, int segmentIndex) const;
};

#endif
