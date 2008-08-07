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

#include <QTextCursor>
#include <QMessageBox>
#include <QTextDocumentFragment>
#include <QDesktopServices>

#include "profile.h"
#include "main_window.h"

#include "url_textedit.h"

QColor UrlTextEdit::_textBackgroundColor = QColor(0, 0, 0, 0);


UrlTextEdit::UrlTextEdit(QWidget *parent) : QTextBrowser(parent)
{
    urlRegexp << QRegExp("http:\\/\\/\\S*");
    urlRegexp << QRegExp("mtpchat:\\/\\/\\S*");
    urlRegexp << QRegExp("ftp:\\/\\/\\S*");
    urlRegexp << QRegExp("https:\\/\\/\\S*");
    urlRegexp << QRegExp("www\\.\\S*");
    urlRegexp << QRegExp("[a-zA-Z0-9._-]+@[a-zA-Z0-9._-]+");

    connect(this, SIGNAL(anchorClicked(const QUrl &)),
            this, SLOT(myAnchorClicked(const QUrl &)));

    isAway = false;
}

void UrlTextEdit::openUrl(const QUrl &link, bool forcesystem)
{
    // Custom browser?
    if (Profile::instance().linksCustomBrowser.isEmpty() || forcesystem)
        QDesktopServices::openUrl(link);
    else
    {
        QStringList args;
        QProcess process;
        args << link.toString();
        process.startDetached(Profile::instance().linksCustomBrowser, args);
    }
}

void UrlTextEdit::myAnchorClicked(const QUrl &link)
{
    if (link.scheme() == "mtpchat")
    {
        SessionConfig config = Profile::instance().addSessionUrl(link);
        if (!config.address().isEmpty())
            MainWindow::instance()->connectTo(config);
    }
    else if (link.scheme().isEmpty() && link.host().isEmpty() && link.toString().contains('@'))
        openUrl(QUrl("mailto:" + link.toString()), true);
    else
        openUrl(link);
}

void UrlTextEdit::setSource(const QUrl &)
{
}

void UrlTextEdit::insertLine(QTextCursor &cursor, const QString &line, const QFont *font, const QColor *color)
{
    // Normal format
    QTextCharFormat charFormat;
    if (font) charFormat.setFont(*font);
    if (color) charFormat.setForeground(*color);
    if (Profile::instance().textSkin().isForcedBackgroundColor())
        charFormat.setBackground(_textBackgroundColor);

    // Url format
    QTextCharFormat urlFormat;
    if (font) urlFormat.setFont(*font);
    urlFormat.setForeground(Qt::blue);
    urlFormat.setFontUnderline(true);
    urlFormat.setAnchor(true);
    urlFormat.setAnchorHref("");

    // Search for URLs
    QList<UrlRange> urlRanges;

    foreach (const QRegExp &regexp, urlRegexp)
    {
        int index = line.indexOf(regexp);
        while (index >= 0)
        {
            UrlRange range;
            range.start = index;
            range.length = regexp.matchedLength();

            urlRanges << range;

            index = line.indexOf(regexp, index + regexp.matchedLength());
        }
    }

    // Bubble sort by <range.start>
    for (int i = urlRanges.count() - 1; i > 0; i--)
        for (int j = 0; j < i; j++)
        {
            UrlRange range1 = urlRanges[j];
            UrlRange range2 = urlRanges[j + 1];
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
            UrlRange iRange = urlRanges[i];
            UrlRange beforeRange = urlRanges[i - 1];

            if (iRange.start >= beforeRange.start &&
                iRange.start < (beforeRange.start + beforeRange.length))
                // Cross-over => remove iRange
                urlRanges.removeAt(i);
        }

    // Format work
    int pos = 0;
    for (int i = 0; i < urlRanges.count(); i++)
    {
        UrlRange range = urlRanges[i];

        // Normal format
        if (pos < range.start)
            cursor.insertText(line.mid(pos, range.start - pos), charFormat);

        // Url format
        urlFormat.setAnchorHref(line.mid(range.start, range.length));
        cursor.insertText(line.mid(range.start, range.length), urlFormat);

        pos = range.start + range.length;
    }

    if (pos < line.length()) // remains a piece of normal format
        cursor.insertText(line.mid(pos, line.length() - pos), charFormat);
}

void UrlTextEdit::deleteProcLater(int exitCode)
{
    if (exitCode)
        QMessageBox::warning(this, tr("Warning"), tr("Failed to open URL"));

    delete urlProcess;
}

void UrlTextEdit::mouseReleaseEvent(QMouseEvent *e)
{
    QTextBrowser::mouseReleaseEvent(e);
    if (Profile::instance().copyOnSelection)
        copy();
}

void UrlTextEdit::setText(const QString &text)
{
    clear();

    QTextCursor cursor(document());
    cursor.movePosition(QTextCursor::Start);

    insertLine(cursor, text);
}
