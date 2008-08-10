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

#ifndef URL_TEXTEDIT_H
#define URL_TEXTEDIT_H

#include <QTextBrowser>
#include <QProcess>

#include "render_segment.h"

class UrlTextEdit : public QTextBrowser
{
    Q_OBJECT

public:
    bool isAway;

    UrlTextEdit(QWidget *parent = 0);

    static QColor getTextBackgroundColor() { return _textBackgroundColor; }
    static void setTextBackgroundColor(const QColor &value) { _textBackgroundColor = value; }
    static void openUrl(const QUrl &link, bool forcesystem = false);

    virtual void setText(const QString &text);

protected:
    virtual void mouseReleaseEvent(QMouseEvent *e);

    void insertLine(QTextCursor &cursor, const QString &line, const QFont *font = NULL, const QColor *color = NULL);

private:
    static QColor _textBackgroundColor;
    RenderSegmentList _segments;

private slots:
    void myAnchorClicked(const QUrl &link);

public slots:
    void setSource(const QUrl &name);
};

#endif
