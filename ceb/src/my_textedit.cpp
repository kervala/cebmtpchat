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
#include "profile.h"
#include "lua_utils.h"
#include "paths.h"

#include "my_textedit.h"

#ifdef Q_OS_WIN
    #include <sdkddkver.h>
    #ifdef _WIN32_WINNT_WIN7
        // only supported by Windows 7 Platform SDK
		#include <BaseTyps.h>
        #include <ShObjIdl.h>
        #define TASKBAR_PROGRESS 1
    #endif
#endif

#ifdef DEBUG_NEW
#define new DEBUG_NEW
#endif

MyTextEdit::MyTextEdit(QWidget *parent) : UrlTextEdit(parent), networkManager(NULL), lastReply(NULL), m_allowFilters(false), progressDialog(NULL)
{
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(ftpCommandFinished(QNetworkReply*)));

    isAway = false;

    setAcceptDrops(true);

#ifdef TASKBAR_PROGRESS
    pTaskbarList = NULL;
    // instanciate the taskbar control COM object
    CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pTaskbarList));
#endif // TASKBAR_PROGRESS
}

MyTextEdit::~MyTextEdit()
{
#ifdef TASKBAR_PROGRESS
    if (pTaskbarList)
    {
        pTaskbarList->Release();
        pTaskbarList = NULL;
    }
#endif // TASKBAR_PROGRESS
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
    QPoint pt(e->pos().x() + horizontalScrollBar()->value(), 
              e->pos().y() + verticalScrollBar()->value());

    QMenu *menu = createStandardContextMenu(pt);

    if (m_allowFilters)
    {
        menu->addSeparator();

        QMenu *filterMenu = new QMenu(tr("Filter it!"), menu);
        connect(filterMenu, SIGNAL(triggered(QAction*)),
                this, SLOT(filterTriggered(QAction*)));

        QDir scriptsDir(Paths::scriptsPath());

        QStringList nameFilters;
        nameFilters << "*.lua";
        foreach (QFileInfo fileInfo, scriptsDir.entryInfoList(nameFilters, QDir::Files))
        {
            QAction *action = new QAction(fileInfo.baseName(), filterMenu);
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
    QDialog *dialog = new QDialog(this, Qt::Dialog | Qt::WindowCloseButtonHint);
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
    if (lastReply) return;

    if (event->mimeData()->hasUrls())
    {
        QUrl uploadUrl(Profile::instance().uploadUrl);

        if (!uploadUrl.isValid())
        {
            QString message;

            if (uploadUrl.isEmpty())
                message = tr("To upload a file, you need to configure an upload URL");
            else
                message = tr("\"%1\" is not a valid upload URL").arg(uploadUrl.toString());

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
//      connect(networkManager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)), this, SLOT(auth(QNetworkReply*, QAuthenticator*)));

        // get the list of all files to upload
        QList<QUrl> urlList = event->mimeData()->urls();

        for (int i = 0; i < urlList.size() && i < 32; ++i)
        {
            UploadData *data = new UploadData();

            data->fileName = urlList.at(i).toLocalFile();
            data->uploadUrl =  uploadUrl.toString();
            data->fileSize = 0;

            QFile *file = new QFile(data->fileName);

            QFileInfo info(data->fileName);

            // error when opening local file
            if (!file->open(QIODevice::ReadOnly))
            {
                QMessageBox::critical(this, tr("Error"), tr("Unable to open the file %1: %2.").arg(data->fileName).arg(file->errorString()));
                delete file;
                continue;
            }

            data->fileSize = file->size();

            static const unsigned char ascii_replacement[] = "                                             -. 0123456789   =  @ABCDEFGHIJKLMNOPQRSTUVWXYZ[ ]   abcdefghijklmnopqrstuvwxyz{ }            S O        .--  s o zY  c                             AAAAAAACEEEEIIIIDNOOOOOxOUUUUYPSaaaaaaaceeeeiiiionooooo ouuuuy y";

            QString encoded;

            for(int i = 0; i < info.fileName().length(); ++i)
            {
                const unsigned char c = info.fileName().at(i).toLatin1();

                encoded += ascii_replacement[c];
            }

            encoded = encoded.simplified().replace(" ", "_");

            if (Profile::instance().prefixDate)
            {
                QDateTime date = QDateTime::currentDateTime();

                // set the final URL
                data->downloadUrl = date.toString("yyMMddhhmm") + "_" + encoded;
            }
            else
            {
                // set the final URL
                data->downloadUrl = encoded;
            }

            // a file
            if (data->uploadUrl.right(4) == ".php" ||
                data->uploadUrl.right(4) == ".cgi" ||
                data->uploadUrl.right(4) == ".asp" ||
                data->uploadUrl.right(4) == ".htm" ||
                data->uploadUrl.right(5) == ".html")
            {
                data->uploadUrl += "?filename=";
            }
            // a script with default parameter
            else if (data->uploadUrl.right(1) == "=")
            {
            }
            // a directory
            else if (data->uploadUrl.right(1) != "/")
            {
                data->uploadUrl += "/";
            }

            data->uploadUrl += data->downloadUrl;

            // send the file to FTP
            QNetworkReply *reply = networkManager->put(QNetworkRequest(QUrl(data->uploadUrl)), file);
            file->setParent(reply);

            reply->setUserData(0, data);

			progressDialog = new QProgressDialog(this, Qt::Dialog|Qt::WindowSystemMenuHint|Qt::WindowTitleHint);
			progressDialog->setWindowTitle("CeB");
			progressDialog->setCancelButtonText(tr("Cancel"));
			progressDialog->setLabelText(tr("Uploading..."));
			progressDialog->setModal(false);

			connect(progressDialog, SIGNAL(canceled()), this, SLOT(cancelUpload()));

            connect(reply, SIGNAL(uploadProgress(qint64, qint64)), this, SLOT(updateDataTransferProgress(qint64, qint64)));

            lastReply = reply;
        }

        event->acceptProposedAction();
    }
}

void MyTextEdit::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void MyTextEdit::ftpCommandFinished(QNetworkReply *reply)
{
    lastReply = NULL;

    QNetworkReply::NetworkError error = reply->error();
    QString errorStr = reply->errorString();

    UploadData *data = (UploadData*)reply->userData(0);

    if (error != QNetworkReply::NoError)
    {
        QMessageBox::critical(this, tr("Error"), tr("Canceled upload of %1: %2").arg(data->fileName).arg(errorStr));
    }
    else
    {
        QString url = Profile::instance().downloadUrl;

        if (url.right(1) != "/") url += "/";

        url += data->downloadUrl + " " + tr("(%n bytes)", "", data->fileSize);

        emit sendToChat(url);
    }

	if (progressDialog)
	{
		progressDialog->hide();
		progressDialog->deleteLater();
	}

    reply->deleteLater();
}

void MyTextEdit::updateDataTransferProgress(qint64 readBytes, qint64 totalBytes)
{
	if (progressDialog)
	{
		progressDialog->setMaximum(totalBytes);
	    progressDialog->setValue(readBytes);
	}

#ifdef TASKBAR_PROGRESS
    // update the taskbar progress
    if (pTaskbarList) pTaskbarList->SetProgressValue((HWND)progressDialog->winId(), readBytes, totalBytes);
#endif // TASKBAR_PROGRESS
}

void MyTextEdit::cancelUpload()
{
    if (lastReply) lastReply->abort();
}
