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

#include "url_textedit.h"

QT_BEGIN_NAMESPACE
class QMimeData;
class QFile;
class QFtp;
class QProgressDialog;
QT_END_NAMESPACE

class MyTextEdit : public UrlTextEdit
{
    Q_OBJECT

public:
    bool isAway;

    MyTextEdit(QWidget *parent = 0);
    virtual ~MyTextEdit();

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
    virtual void contextMenuEvent(QContextMenuEvent *e);
    virtual void resizeEvent(QResizeEvent *e);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

private:

    QTextEdit *filterTextEdit;
    QProgressDialog *progressDialog;
    QFtp *ftp;
    
    bool m_allowFilters;

    struct FtpQueue
    {
        int id;
        QString fileName;
        QString finalUrl;
        QFile *file;
    };

    QList<FtpQueue> ftpQueue;

signals:
    void myKeyPressed(const QKeyEvent &e);

private slots:
    void filterTriggered(QAction *action);
    void sendIt();

    // ftp methods
    void cancelDownload();
    void ftpCommandFinished(int commandId, bool error);
    void updateDataTransferProgress(qint64 readBytes,
                                    qint64 totalBytes);

signals:
    void sendToChat(const QString &string);
};

#endif
