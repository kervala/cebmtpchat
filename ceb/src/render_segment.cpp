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

#include "common.h"
#include "render_segment.h"

#ifdef DEBUG_NEW
#define new DEBUG_NEW
#endif

RenderSegmentList::RenderSegmentList()
{
    urlRegexp << QRegExp("http:\\/\\/\\S*");
    urlRegexp << QRegExp("mtpchat:\\/\\/\\S*");
    urlRegexp << QRegExp("ftp:\\/\\/\\S*");
    urlRegexp << QRegExp("https:\\/\\/\\S*");
    urlRegexp << QRegExp("www\\.\\S*");
    urlRegexp << QRegExp("[a-zA-Z0-9._-]+@[a-zA-Z0-9._-]+\\.[a-zA-Z.]+");
}

QList<RenderSegmentList::TextRange> RenderSegmentList::urlRanges(const QString &text) const
{
    QList<TextRange> urlRanges;

    foreach (const QRegExp &regexp, urlRegexp)
    {
        int index = 0;
        while ((index = regexp.indexIn(text, index)) != -1)
        {
            TextRange range;
            range.start = index;
            range.length = regexp.matchedLength();

            urlRanges << range;

            index += range.length;
        }
    }

    // Bubble sort by <range.start>
    for (int i = urlRanges.count() - 1; i > 0; i--)
        for (int j = 0; j < i; j++)
        {
            TextRange range1 = urlRanges[j];
            TextRange range2 = urlRanges[j + 1];
            if (range1.start >= range2.start)
            {
                // Exchange
                urlRanges[j] = range2;
                urlRanges[j + 1] = range1;
            }
        }

    // Remove cross URLs
    if (urlRanges.count() > 1)
        for (int i = urlRanges.count() - 1; i > 0; i--)
        {
            TextRange iRange = urlRanges[i];
            TextRange beforeRange = urlRanges[i - 1];

            if (iRange.start >= beforeRange.start &&
                iRange.start < (beforeRange.start + beforeRange.length))
                // Cross-over => remove iRange
                urlRanges.removeAt(i);
        }

    return urlRanges;
}

void RenderSegmentList::segmentByURLs()
{
    RenderSegmentList newSegments = *this;

    QString text;
    foreach (const RenderSegment &segment, newSegments)
        text += segment.text();

    // Search for URLs
    QList<TextRange> urlRanges = newSegments.urlRanges(text);

    foreach (const TextRange &range, urlRanges)
    {
        int startIndex = segmentIndex(newSegments, range.start);
        int endIndex = segmentIndex(newSegments, range.end());

        if (startIndex == endIndex)
            continue;

        RenderSegment &startSegment = newSegments[startIndex];
        RenderSegment &endSegment = newSegments[endIndex];
        int toRemove = 0;
        QString addedString;
        QString cutString;
        for (int index = startIndex + 1; index <= endIndex; ++index)
        {
            RenderSegment &segment = newSegments[index];
            TextRange segRange = segmentRange(newSegments, index);
            if (segRange.end() <= range.end()) // Totally absorbed by the first segment
            {
                addedString += segment.text();
                toRemove++;
            } else // Just cut
            {
                addedString += segment.text().left(range.end() - segRange.start + 1);
                cutString = segment.text().right(segRange.end() - range.end());
            }
        }
        while (toRemove)
        {
            newSegments.removeAt(startIndex + 1);
            toRemove--;
        }
        startSegment.setText(startSegment.text() + addedString);
        if (!cutString.isEmpty())
            endSegment.setText(cutString);
    }

    (*this) = newSegments;
}

int RenderSegmentList::segmentIndex(const RenderSegmentList &segments, int charIndex) const
{
    int offset = 0;
    for (int index = 0; index < count(); ++index)
        if (charIndex >= offset && charIndex < offset + segments[index].text().length())
            return index;
        else
            offset += segments[index].text().length();
    return -1;
}

RenderSegmentList::TextRange RenderSegmentList::segmentRange(const RenderSegmentList &segments, int segmentIndex) const
{
    TextRange range;
    int offset = 0;
    for (int i = 0; i < count(); ++i)
        if (i == segmentIndex)
        {
            range.start = offset;
            range.length = segments[i].text().length();
            return range;
        } else
            offset += segments[i].text().length();
    return TextRange();
}
