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

#include <QKeyEvent>
#include <QTextCursor>
#include <QMessageBox>
#include <QTextDocumentFragment>
#include <QMenu>
#include <QVBoxLayout>
#include <QSplitter>
#include <QPushButton>
#include <QScrollBar>
#include <QtNetwork>
#include <QProgressDialog>
#include <QDate>

#include "profile.h"
#include "lua_utils.h"
#include "paths.h"

#include "my_textedit.h"

MyTextEdit::MyTextEdit(QWidget *parent) : UrlTextEdit(parent), m_allowFilters(false), ftp(NULL)
{
    progressDialog = new QProgressDialog(this);
    progressDialog->setCancelButtonText(tr("Cancel"));
    progressDialog->setLabelText(tr("Uploading..."));
    progressDialog->setModal(false);

    connect(progressDialog, SIGNAL(canceled()),
            this, SLOT(cancelDownload()));

    isAway = false;

    setAcceptDrops(true);
}

void MyTextEdit::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_PageUp || e->key() == Qt::Key_PageDown)
    {
        QTextEdit::keyPressEvent(e);
        return;
    }

    if (!(e->modifiers() & Qt::ControlModifier) &&
        !(e->modifiers() & Qt::AltModifier))
    {
        QTextEdit::keyPressEvent(e);
        emit myKeyPressed(*e);
    }
    else if ((e->modifiers() == Qt::ControlModifier) &&
             (e->key() == Qt::Key_V))
    {
        QTextEdit::keyPressEvent(e);
        emit myKeyPressed(*e);
    }
    else
        QTextEdit::keyPressEvent(e);
}

void MyTextEdit::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu *menu = createStandardContextMenu();

    if (m_allowFilters)
    {
        menu->addSeparator();

        QMenu *filterMenu = new QMenu(tr("Filter it!"));
        connect(filterMenu, SIGNAL(triggered(QAction*)),
                this, SLOT(filterTriggered(QAction*)));

        QDir scriptsDir(QDir(Paths::sharePath()).filePath("scripts"));

        QStringList nameFilters;
        nameFilters << "*.lua";
        foreach (QFileInfo fileInfo, scriptsDir.entryInfoList(nameFilters, QDir::Files))
        {
            QAction *action = new QAction(fileInfo.baseName(), 0);
            filterMenu->addAction(action);
        }

        menu->addMenu(filterMenu);
    }

    menu->exec(e->globalPos());
    delete menu;
}

void MyTextEdit::addNewLine(const QString &line, const QFont &font,
                            const QColor &color)
{
    QTextCursor cursor(document());
    cursor.movePosition(QTextCursor::End);
    if (!document()->isEmpty())
        cursor.insertBlock();

    insertLine(cursor, line, &font, &color);
}

void MyTextEdit::addString(const QString &line, const QFont &font, const QColor &color)
{
    QTextCursor cursor(document());
    cursor.movePosition(QTextCursor::End);

    insertLine(cursor, line, &font, &color);
}

void MyTextEdit::addNewLine(const QString &line, const QColor &color)
{
    QTextCursor cursor(document());
    cursor.movePosition(QTextCursor::End);
    if (!document()->isEmpty())
        cursor.insertBlock();
    QTextCharFormat charFormat;
    charFormat.setFont(cursor.charFormat().font());
    charFormat.setForeground(color);
    cursor.insertText(line, charFormat);
}

void MyTextEdit::addString(const QString &line, const QColor &color)
{
    QTextCursor cursor(document());
    cursor.movePosition(QTextCursor::End);
    QTextCharFormat charFormat;
    charFormat.setFont(cursor.charFormat().font());
    charFormat.setForeground(color);
    cursor.insertText(line, charFormat);
}

void MyTextEdit::addNewLine(const QString &line)
{
    QTextCursor cursor(document());
    cursor.movePosition(QTextCursor::End);
    if (!document()->isEmpty())
        cursor.insertBlock();
    cursor.insertText(line);
}

void MyTextEdit::addString(const QString &line)
{
    QTextCursor cursor(document());
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(line);
}

void MyTextEdit::addSeparatorLine()
{
    QTextDocumentFragment fragment = QTextDocumentFragment::fromHtml("<hr>");
    QTextCursor cursor(document());
    cursor.movePosition(QTextCursor::End);
    cursor.insertFragment(fragment);
}

void MyTextEdit::filterTriggered(QAction *action)
{
    QString normalText = textCursor().selection().toPlainText();
    QDialog *dialog = new QDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle(action->text() + " " + tr("(filter)"));
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setMargin(2);

    QSplitter *splitter = new QSplitter;
    splitter->setOrientation(Qt::Vertical);
    layout->addWidget(splitter);

    // Normal text
    QTextEdit *textEdit = new QTextEdit;
    textEdit->setReadOnly(true);
    splitter->addWidget(textEdit);
    textEdit->setPlainText(normalText);

    // Filtered text
    QWidget *subWidget = new QWidget;
    splitter->addWidget(subWidget);
    QVBoxLayout *subLayout = new QVBoxLayout(subWidget);
    subLayout->setMargin(2);

    filterTextEdit = new QTextEdit;
    filterTextEdit->setReadOnly(true);
    subLayout->addWidget(filterTextEdit);
    filterTextEdit->setPlainText(executeLuaFilter(action->text(), normalText));

    QPushButton *button = new QPushButton(tr("Send to chat!"));
    subLayout->addWidget(button);
    connect(button, SIGNAL(clicked()),
            this, SLOT(sendIt()));

    dialog->resize(500, 300);
    dialog->show();
}

void MyTextEdit::sendIt()
{
    QString text = filterTextEdit->toPlainText();
    emit sendToChat(text);
}

void MyTextEdit::resizeEvent(QResizeEvent *e)
{
    QTextBrowser::resizeEvent(e);

    scrollOutputToBottom();
}

void MyTextEdit::scrollOutputToBottom()
{
    QScrollBar *sb = verticalScrollBar();
    sb->setValue(sb->maximum());
}

void MyTextEdit::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void MyTextEdit::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void MyTextEdit::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        QUrl ftpUrl(Profile::instance().uploadUrl);

        if (!ftpUrl.isValid() || ftpUrl.scheme().toLower() != QLatin1String("ftp"))
        {
            QString message;

            if (ftpUrl.isEmpty())
                message = tr("To upload a file, you need to configure an FTP URL");
            else
                message = tr("\"%1\" is not a valid FTP URL").arg(ftpUrl.toString());

            QMessageBox::critical(this, tr("Error"), message);

            return;
        }

        QUrl httpUrl(Profile::instance().downloadUrl);

        if (!httpUrl.isValid())
        {
            QString message;

            if (httpUrl.isEmpty())
                message = tr("To upload a file, you need to configure an destination URL");
            else
                message = tr("\"%1\" is not a valid URL").arg(httpUrl.toString());

            QMessageBox::critical(this, tr("Error"), message);

            return;
        }

        // if an FTP connection is already set, we close it
        if (ftp)
        {
            ftp->abort();
            ftp->deleteLater();
            ftp = NULL;
        }

        ftp = new QFtp(this);

        connect(ftp, SIGNAL(commandFinished(int, bool)),
            this, SLOT(ftpCommandFinished(int, bool)));
        connect(ftp, SIGNAL(dataTransferProgress(qint64, qint64)),
            this, SLOT(updateDataTransferProgress(qint64, qint64)));

        ftp->connectToHost(ftpUrl.host(), ftpUrl.port(21));

        if (!ftpUrl.userName().isEmpty())
            // send login and password
            ftp->login(QUrl::fromPercentEncoding(ftpUrl.userName().toLatin1()), ftpUrl.password());
        else
            ftp->login(); // anonymous connection

        // change directory
        if (!ftpUrl.path().isEmpty())
            ftp->cd(ftpUrl.path());

        // get the list of all files to upload
        QList<QUrl> urlList = event->mimeData()->urls();

        for (int i = 0; i < urlList.size() && i < 32; ++i)
        {
            FtpQueue queue;

            queue.fileName = urlList.at(i).toLocalFile();
            queue.file = new QFile(queue.fileName);

            QFileInfo info(queue.fileName);

            // error when opening local file
            if (!queue.file->open(QIODevice::ReadOnly))
            {
                QMessageBox::critical(this, tr("Error"), tr("Unable to open the file %1: %2.").arg(queue.fileName).arg(queue.file->errorString()));
                delete queue.file;
                continue;
            }

            static const unsigned char ascii_replacement[] = "                                             -. 0123456789   =  @ABCDEFGHIJKLMNOPQRSTUVWXYZ[ ]   abcdefghijklmnopqrstuvwxyz{ }            S O        .--  s o zY  c                             AAAAAAACEEEEIIIIDNOOOOOxOUUUUYPSaaaaaaaceeeeiiiionooooo ouuuuy y";

            QString encoded;

            for(int i = 0; i < info.fileName().length(); ++i)
            {
                const unsigned char c = info.fileName().at(i).toAscii();

                encoded += ascii_replacement[c];
            }

            encoded = encoded.simplified().replace(" ", "_");

            if (Profile::instance().prefixDate)
            {
                QDateTime date = QDateTime::currentDateTime();

                // set the final URL
                queue.finalUrl = date.toString("yyMMddhhmm") + "_" + encoded;
            }
            else
            {
                // set the final URL
                queue.finalUrl = encoded;
            }

            // send the file to FTP
            queue.id = ftp->put(queue.file, queue.finalUrl);

            // add details on the file to the ftp queue
            ftpQueue.append(queue);
        }

        event->acceptProposedAction();
    }
}

void MyTextEdit::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void MyTextEdit::ftpCommandFinished(int commandId, bool error)
{
    if (ftp->currentCommand() == QFtp::ConnectToHost && error)
        QMessageBox::critical(this, tr("Error"), tr("Unable to connect to %1. Please check that the hostname is correct.")
                    .arg(Profile::instance().uploadUrl));

    if (ftp->currentCommand() == QFtp::Put)
    {
        FtpQueue current;

        current.id = -1;

        int i = 0;

        // remove the just completed file from the queue
        while(i < ftpQueue.size())
        {
            if (ftpQueue.at(i).id == commandId)
            {
                current = ftpQueue.at(i);
                ftpQueue.removeAt(i);
                break;
            }

            ++i;
        }

        // we didn't find the right file
        if (current.id == -1)
        {
//          addNewLine(tr("Command id %1 not found").arg(commandId));
            return;
        }

        if (error)
        {
            QMessageBox::critical(this, tr("Error"), tr("Canceled upload of %1").arg(current.fileName));
            ftp->remove(current.finalUrl);
        }
        else
        {
            if (current.file->atEnd())
            {
                QString url = Profile::instance().downloadUrl;
                
                if (url.right(1) != "/") url += "/";
                
                url += current.finalUrl + " " + tr("(%n bytes)", "", current.file->size());

                emit sendToChat(url);
            }
            else
                ftp->remove(current.finalUrl); // partially uploaded file so we can safely delete it
        }

        progressDialog->hide();

        current.file->close();

        delete current.file;

        // if no files in ftp queue, we can safely close connection
        if (!ftp->hasPendingCommands())
        {
            ftp->abort();
            ftp->deleteLater();
            ftp = NULL;
        }
    }
}

void MyTextEdit::updateDataTransferProgress(qint64 readBytes, qint64 totalBytes)
{
    progressDialog->show();

    progressDialog->setMaximum(totalBytes);
    progressDialog->setValue(readBytes);
}

void MyTextEdit::cancelDownload()
{
    if (ftp) ftp->abort();
}
