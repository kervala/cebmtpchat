#include "render_segment.h"

RenderSegment::RenderSegment(const QString &text,
                             const QFont &font,
                             const QColor &color)
{
    m_text = text;
    m_font = font;
    m_color = color;
}
