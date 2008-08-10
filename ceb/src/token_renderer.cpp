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

#include "profile.h"
#include "token_display.h"
#include "token_factory.h"
#include "render_script.h"

#include "token_renderer.h"

void TokenRenderer::displayToken(const Token &token, bool timeStamp)
{
    Q_ASSERT(_textEdit);
    Q_ASSERT(_session);

    const TextSkin &textSkin = Profile::instance().textSkin();

    // TimeStamp?
    bool lineStarted = false;

    if (timeStamp)
    {
        lineStarted = true;
        _textEdit->addNewLine(token.timeStamp().toString("hh:mm:ss "),
                               textSkin.timeStampFont().font(),
                               textSkin.timeStampFont().color());
    }

    // Compute segments
    RenderSegmentList segments;
    MtpRegExp regExp = TokenFactory::defaultFactory.tokenRegexp()[token.type()];
    const QString &allLine = token.arguments()[0];
    QFont allLineFont = textSkin.tokenFont(token.type(), 0);
    QColor allLineColor = textSkin.tokenColor(token.type(), 0);

    int iArg = 0;
    const QList<int> &arguments = regExp.arguments();

    if (!arguments.count())
        segments << RenderSegment(allLine, allLineFont, allLineColor);
    else
    {
        // Get first non-null argument
        while (iArg < arguments.count())
            if (token.positions()[arguments[iArg]] >= 0)
                break;
            else
                iArg++;

        if (iArg == arguments.count())
            // All arguments are < 0
            segments << RenderSegment(allLine, allLineFont, allLineColor);
        else
        {
            // Before first non-null argument
            int argPos = token.positions()[arguments[iArg]];
            if (argPos > 0)
                segments << RenderSegment(allLine.mid(0, argPos), allLineFont, allLineColor);
        }
    }

    // All remained aguments
    while (iArg < arguments.count())
    {
        // Token arg
        int argNum = arguments[iArg];
        int argPos = token.positions()[argNum];
        QString arg = token.arguments()[argNum];

        segments << RenderSegment(arg,
                                  textSkin.tokenFont(token.type(), iArg + 1),
                                  textSkin.tokenColor(token.type(), iArg + 1));

        // Portion after token arg
        int afterPos = argPos + arg.length();
        int limitPos = allLine.length() - 1;

        // Get first non-null argument
        iArg++;
        while (iArg < arguments.count())
            if (token.positions()[arguments[iArg]] >= 0)
                break;
            else
                iArg++;

        if (iArg < arguments.count())
            limitPos = token.positions()[arguments[iArg]] - 1;

        if (limitPos >= afterPos)
            segments << RenderSegment(allLine.mid(afterPos, limitPos - afterPos + 1),
                                      allLineFont, allLineColor);
    }

    // Execute modifier
    RenderScript::executeRenderScript(_session, token.type(), segments);

    segments.segmentByURLs();

    // Render all
    if (_grayMode)
        foreach (const RenderSegment &segment, segments)
            if (lineStarted)
                _textEdit->addString(segment.text(), segment.font(), Qt::gray);
            else
            {
                lineStarted = true;
                _textEdit->addNewLine(segment.text(), segment.font(), Qt::gray);
            }
    else
        foreach (const RenderSegment &segment, segments)
            if (lineStarted)
                _textEdit->addString(segment.text(), segment.font(), segment.color());
            else
            {
                lineStarted = true;
                _textEdit->addNewLine(segment.text(), segment.font(), segment.color());
            }
}
