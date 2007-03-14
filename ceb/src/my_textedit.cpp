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

#if defined(Q_OS_WIN32)
#include <windows.h>
#endif

#include <QApplication>
#include <QKeyEvent>
#include <QTextCursor>
#include <QMessageBox>
#include <QTextDocumentFragment>
#include <QAction>
#include <QMenu>
#include <QDir>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QSplitter>
#include <QPushButton>
#include <QScrollBar>
#include <QDesktopServices>

#include "my_textedit.h"
#include "profile_manager.h"
#include "lua_utils.h"

MyTextEdit::MyTextEdit(QWidget *parent) : QTextBrowser(parent), m_allowFilters(false)
{
	urlRegexp << QRegExp("http:\\/\\/\\S*");
	urlRegexp << QRegExp("http:\\/\\/\\S*");
	urlRegexp << QRegExp("https:\\/\\/\\S*");
	urlRegexp << QRegExp("www\\.\\S*");
	urlRegexp << QRegExp("mailto:\\S*");

	connect(this, SIGNAL(anchorClicked(const QUrl &)),
		this, SLOT(myAnchorClicked(const QUrl &)));

	isAway = false;
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

		QMenu *filterMenu = new QMenu("Filter it!");
		connect(filterMenu, SIGNAL(triggered(QAction*)),
				this, SLOT(filterTriggered(QAction*)));

		QDir profilesDir(QApplication::applicationDirPath());

		if (profilesDir.cd("scripts"))
		{
			QStringList nameFilters;
			nameFilters << "*.lua";
			foreach (QFileInfo fileInfo, profilesDir.entryInfoList(nameFilters, QDir::Files))
				{
					QAction *action = new QAction(fileInfo.baseName(), 0);
					filterMenu->addAction(action);
				}
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

	insertLine(cursor, line, font, color);
}

void MyTextEdit::addString(const QString &line, const QFont &font, const QColor &color)
{
	QTextCursor cursor(document());
	cursor.movePosition(QTextCursor::End);

	insertLine(cursor, line, font, color);
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

void MyTextEdit::myAnchorClicked(const QUrl &link)
{
	Profile &profile = *ProfileManager::instance().currentProfile();

	// Custom browser?
	if (profile.linksCustomBrowser.isEmpty())
		QDesktopServices::openUrl(link);
    else
	{
		QStringList args;
		QProcess process;
		args << link.toString();
		process.startDetached(profile.linksCustomBrowser, args);
	}
}

void MyTextEdit::setSource(const QUrl &)
{
}

void MyTextEdit::insertLine(QTextCursor &cursor, const QString &line, const QFont &font, const QColor &color)
{
	// Normal format
	QTextCharFormat charFormat;
	charFormat.setFont(font);
/*	if (isAway)
		charFormat.setBackground(QColor(215, 215, 200));
	else
	charFormat.setBackground(QColor(255, 255, 240));*/
	charFormat.setForeground(color);

	// Url format
	QTextCharFormat urlFormat;
	urlFormat.setFont(font);
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

void MyTextEdit::deleteProcLater(int exitCode)
{
	if (exitCode)
		QMessageBox::warning(this, "Warning", "Failed to open URL");

	delete urlProcess;
}

void MyTextEdit::addSeparatorLine()
{
	QTextDocumentFragment fragment = QTextDocumentFragment::fromHtml("<hr>");
	QTextCursor cursor(document());
	cursor.movePosition(QTextCursor::End);
	cursor.insertFragment(fragment);
}

void MyTextEdit::mouseReleaseEvent(QMouseEvent *e)
{
	Profile &profile = *ProfileManager::instance().currentProfile();

	QTextBrowser::mouseReleaseEvent(e);
	if (profile.copyOnSelection)
		copy();
}

void MyTextEdit::filterTriggered(QAction *action)
{
	QString normalText = textCursor().selection().toPlainText();
	QDialog *dialog = new QDialog(this);
	dialog->setAttribute(Qt::WA_DeleteOnClose);
	dialog->setWindowTitle(action->text() + tr(" (filter)"));
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
