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

#include <QHBoxLayout>

#include "search_widget.h"

SearchWidget::SearchWidget(QWidget *parent) : QWidget(parent),
                                              _textWidget(0),
                                              _findFlags(0)
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(0);

    QPushButton *closeButton = new QPushButton;
    mainLayout->addWidget(closeButton);
    closeButton->setIcon(QIcon(":/images/clear.png"));
    QSize size = closeButton->maximumSize();
    size.setWidth(32);
    closeButton->setMaximumSize(size);
    connect(closeButton, SIGNAL(clicked()), this, SIGNAL(hideMe()));

    _lineEditSearch = new QLineEdit;
    mainLayout->addWidget(_lineEditSearch);

    connect(_lineEditSearch, SIGNAL(textChanged(const QString &)),
            this, SLOT(lineEditTextChanged(const QString &)));

    _previousButton = new QPushButton(tr("&Previous"));
    mainLayout->addWidget(_previousButton);
    _previousButton->setIcon(QIcon(":/images/up.png"));
    connect(_previousButton, SIGNAL(clicked()), this, SLOT(searchForPrevious()));

    _nextButton = new QPushButton(tr("&Next"));
    mainLayout->addWidget(_nextButton);
    _nextButton->setIcon(QIcon(":/images/down.png"));
    connect(_nextButton, SIGNAL(clicked()), this, SLOT(searchForNext()));

    _toolButtonOptions = new QToolButton;
    _toolButtonOptions->setText(tr("&Options"));
    mainLayout->addWidget(_toolButtonOptions);
    _toolButtonOptions->setAutoRaise(true);

    _menuOptions = new QMenu(this);
    _toolButtonOptions->setMenu(_menuOptions);
    _toolButtonOptions->setPopupMode(QToolButton::InstantPopup);
    QAction *action = _menuOptions->addAction(tr("Case sensitive find"));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(caseSensitiveActionTriggered(bool)));
    action = _menuOptions->addAction(tr("Match only complete words"));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(completeWordsActionTriggered(bool)));
}

void SearchWidget::setTextWidget(QTextBrowser *textWidget)
{
    _textWidget = textWidget;
}

void SearchWidget::afterShow()
{
    _lineEditSearch->setFocus();
    _lineEditSearch->selectAll();
}

void SearchWidget::lineEditTextChanged(const QString &text)
{
    if (!_textWidget)
        return;

    QTextCursor cursor = _textWidget->textCursor();
    cursor.movePosition(QTextCursor::End);
    _textWidget->setTextCursor(cursor);
    if (text != "" && !_textWidget->find(text, _findFlags | QTextDocument::FindBackward))
    {
        QPalette palette = _lineEditSearch->palette();
        palette.setColor(QPalette::Base, QColor(255, 102, 102));
        palette.setColor(QPalette::Text, Qt::white);
        _lineEditSearch->setPalette(palette);
    } else
    {
        QPalette palette = QLineEdit().palette();
        _lineEditSearch->setPalette(palette);
    }
}

void SearchWidget::searchForPrevious()
{
    if (!_textWidget->find(_lineEditSearch->text(), _findFlags | QTextDocument::FindBackward))
    {
        QTextCursor cursor = _textWidget->textCursor();
        cursor.movePosition(QTextCursor::End);
        _textWidget->setTextCursor(cursor);
        _textWidget->find(_lineEditSearch->text(), _findFlags | QTextDocument::FindBackward);
    }
}

void SearchWidget::searchForNext()
{
    if (!_textWidget->find(_lineEditSearch->text(), _findFlags))
    {
        QTextCursor cursor = _textWidget->textCursor();
        cursor.movePosition(QTextCursor::Start);
        _textWidget->setTextCursor(cursor);
        _textWidget->find(_lineEditSearch->text(), _findFlags);
    }
}

void SearchWidget::caseSensitiveActionTriggered(bool checked)
{
    if (checked)
        _findFlags |= QTextDocument::FindCaseSensitively;
    else
        _findFlags &= !QTextDocument::FindCaseSensitively;

    lineEditTextChanged(_lineEditSearch->text());
}

void SearchWidget::completeWordsActionTriggered(bool checked)
{
    if (checked)
        _findFlags |= QTextDocument::FindWholeWords;
    else
        _findFlags &= !QTextDocument::FindWholeWords;

    lineEditTextChanged(_lineEditSearch->text());
}
