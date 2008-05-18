#ifndef RENDER_SEGMENT_H
#define RENDER_SEGMENT_H

#include <QString>
#include <QFont>
#include <QColor>

class RenderSegment
{
public:
    RenderSegment(const QString &text,
                  const QFont &font,
                  const QColor &color);

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

#endif
