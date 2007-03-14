/*
    This file is part of Max.

    Max is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    Max is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Max; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "history_widget.h"

#include <QApplication>
#include <QKeyEvent>
#include <QTextCursor>
#include <QTextBlock>

HistoryWidget::HistoryWidget(QWidget *parent) : QTextEdit(parent)
{
    historyIt = history.end();
    _completionMode = false;
	setAcceptRichText(false);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void HistoryWidget::smashText(const QString &text)
{
    if (text.isEmpty())
        document()->clear();
    else
        document()->setPlainText(text);
}

void HistoryWidget::historyUp()
{
    if (!history.empty() &&
        historyIt != history.begin())
    {
        // If last item, we record in editing text
        if (historyIt == history.end())
            editingText = toPlainText();

        // Replace with up text
        historyIt--;
        smashText(*historyIt);
    }
}

void HistoryWidget::historyDown()
{
    if (historyIt != history.end())
    {
        // Replace with down text
        historyIt++;
        if (historyIt == history.end())
            smashText(editingText);
        else
            smashText(*historyIt);
    }
}

void HistoryWidget::validateText()
{
    if (toPlainText().isEmpty())
        return;

    lastValidatedText = toPlainText();

    // Record in history
    if (history.empty() ||
        lastValidatedText != history.last())
        history.append(lastValidatedText);

    // Reset the iterator
    historyIt = history.end();
    editingText = "";

    clear();
    emit textValidated(lastValidatedText);
}

void HistoryWidget::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Return ||
		e->key() == Qt::Key_Enter)
	{
		if (e->modifiers() & Qt::ControlModifier)
			textCursor().insertBlock();
		else
			validateText();
	}
	else if (e->key() == Qt::Key_Up)
	{
		if (isCursorAtFirstLine())
			historyUp();
		else
			QTextEdit::keyPressEvent(e);
	}
	else if (e->key() == Qt::Key_Down)
	{
		if (isCursorAtLastLine())
			historyDown();
		else
			QTextEdit::keyPressEvent(e);
	}
	else if (e->key() == Qt::Key_PageUp)
	{
		if (e->modifiers() == Qt::NoModifier)
			emit pageUp();
		else
			QTextEdit::keyPressEvent(e);
	}
	else if (e->key() == Qt::Key_PageDown)
	{
		if (e->modifiers() == Qt::NoModifier)
			emit pageDown();
		else
			QTextEdit::keyPressEvent(e);
	}
	else if (e->key() == Qt::Key_Tab)
	{
		if (_completionMode && e->modifiers() == Qt::NoModifier)
			runCompletion();
		else if (e->modifiers() == Qt::ControlModifier)
			emit moveRight();
		else if (e->modifiers() & Qt::ControlModifier &&
			e->modifiers() & Qt::ShiftModifier)
			emit moveLeft();
	}	
	else
		QTextEdit::keyPressEvent(e);
}

bool HistoryWidget::isCursorAtFirstLine()
{
	QTextCursor cursor = textCursor();
	int oldPosition = cursor.position();
	cursor.movePosition(QTextCursor::Up);
	return oldPosition == cursor.position();
}

bool HistoryWidget::isCursorAtLastLine()
{
	QTextCursor cursor = textCursor();
	int oldPosition = cursor.position();
	cursor.movePosition(QTextCursor::Down);
	return oldPosition == cursor.position();
}

bool HistoryWidget::isCursorAtLineStart()
{
	QTextCursor cursor = textCursor();
	int oldPosition = cursor.position();
	cursor.movePosition(QTextCursor::StartOfLine);
	return oldPosition == cursor.position();
}

bool HistoryWidget::completionMode() const
{
	return _completionMode;
}

void HistoryWidget::setCompletionMode(bool completionMode)
{
	_completionMode = completionMode;
}

void HistoryWidget::addCompletionWord(const QString &word)
{
	if (completionWords.find(word.toLower()) == completionWords.end())
		completionWords.insert(word.toLower(), word);
}

void HistoryWidget::removeCompletionWord(const QString &word)
{
	completionWords.remove(word.toLower());
}

void HistoryWidget::clearCompletionWords()
{
	completionWords.clear();
}

void HistoryWidget::runCompletion()
{
	// Get the word starting...
	int lettersToRemove;
	QString word = getWordBefore(textCursor().block().text(),
		textCursor().position(), lettersToRemove);
	QString wordLower = word.toLower();
	
	// Find the word in completion words
	QString wordToInsert;
	QMap<QString, QString>::iterator it = completionWords.find(wordLower);
	if (it != completionWords.end() && it.value() == word)
	{
		// Exact string => next string (rotation)
		it++;
		if (it == completionWords.end())
			wordToInsert = completionWords.begin().value();
		else
			wordToInsert = it.value();
	}
	else
	{
		// Substring?
		for (QMap<QString, QString>::iterator it = completionWords.begin();
			it != completionWords.end(); it++)
		{
			QString valueLower = it.value().toLower();
			if (valueLower.startsWith(wordLower))
			{
				wordToInsert = it.value();
				break;
			}
		}		
	}

	if (!wordToInsert.isEmpty()) 
	{
		// Is at first column?
		for (int i = 0; i < lettersToRemove; i++)
			textCursor().deletePreviousChar();
			
		if (isCursorAtLineStart())
			wordToInsert.append(": ");

		textCursor().joinPreviousEditBlock();
		textCursor().insertText(wordToInsert);
		textCursor().endEditBlock();
	}
}

QString HistoryWidget::getWordBefore(const QString &text, int position, int &lettersToRemove)
{
	QString word;
	
	QTextCursor cursor = textCursor();
	cursor.movePosition(QTextCursor::StartOfLine);
	int startLinePosition = cursor.position();
	for (int i = startLinePosition; i < position; i++)
		word += text[i];
	QRegExp loginRegexp("^([a-zA-Z][a-zA-Z\\d]{0,7}): *$");

	if (loginRegexp.exactMatch(word))
	{
		lettersToRemove = position - startLinePosition + 1;
		return loginRegexp.cap(1);
	}
	
	word = "";
	for (int i = position - 1; i >= 0; i--)
	{
		if (text[i].isLetterOrNumber())
			word = text[i] + word;
		else
			break;
	}
	lettersToRemove = word.length();
	
	return word;
}
