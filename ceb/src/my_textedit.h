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

#ifndef MY_TEXTEDIT_H
#define MY_TEXTEDIT_H

#include <QTextBrowser>
#include <QProcess>

class MyTextEdit : public QTextBrowser
{
    Q_OBJECT

public:
    bool isAway;

    MyTextEdit(QWidget *parent = 0);

    static QColor getTextBackgroundColor() { return _textBackgroundColor; }
    static void setTextBackgroundColor(const QColor &value) { _textBackgroundColor = value; }

    void addNewLine(const QString &line, const QFont &font, const QColor &color); // Url analyzing
    void addString(const QString &line, const QFont &font, const QColor &color);
    void addNewLine(const QString &line, const QColor &color);
    void addString(const QString &line, const QColor &color);
    void addNewLine(const QString &line);
    void addString(const QString &line);
    void addSeparatorLine();

    bool allowFilters() const { return m_allowFilters; }
    void setAllowFilters(bool value) { m_allowFilters = value; }

    void scrollOutputToBottom();

protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void contextMenuEvent(QContextMenuEvent *e);
    virtual void resizeEvent(QResizeEvent *e);

private:
    static QColor _textBackgroundColor;

    QProcess *urlProcess;
    QTextEdit *filterTextEdit;
    bool m_allowFilters;

    struct UrlRange
    {
        int start;
        int length;
    };

    QList<QRegExp> urlRegexp;

    void insertLine(QTextCursor &cursor, const QString &line, const QFont &font, const QColor &color);

signals:
    void myKeyPressed(const QKeyEvent &e);

private slots:
    void myAnchorClicked(const QUrl &link);
    void deleteProcLater(int exitCode);
    void filterTriggered(QAction *action);
    void sendIt();

public slots:
    void setSource(const QUrl &name);

signals:
    void sendToChat(const QString &string);
};

#endif
