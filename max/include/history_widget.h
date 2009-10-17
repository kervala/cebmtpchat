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

#ifndef HISTORY_WIDGET_H
#define HISTORY_WIDGET_H

#include <QTextEdit>
#include <QLinkedList>
#include <QMap>

class HistoryWidget : public QTextEdit
{
    Q_OBJECT

private:
    QString lastValidatedText;
    QLinkedList<QString> history;
    QLinkedList<QString>::iterator historyIt;
    QString editingText;
    bool _completionMode;
    QMap<QString, QString> completionWords;

    void historyUp();
    void historyDown();
    void validateText();
    bool isCursorAtFirstLine();
    bool isCursorAtLastLine();
	bool isCursorAtLineStart();
    void smashText(const QString &text);
    void runCompletion();
    QString getWordBefore(const QString &text, int position, int &lettersToRemove);

protected:
    void keyPressEvent(QKeyEvent *e);

public:
    HistoryWidget(QWidget *parent = 0);

    bool completionMode() const;
    void setCompletionMode(bool completionMode);
    void addCompletionWord(const QString &word);
    void removeCompletionWord(const QString &word);
    void clearCompletionWords();

signals:
    void textValidated(const QString &text);
    void moveLeft();
    void moveRight();
    void pageUp();
    void pageDown();
};

#endif
