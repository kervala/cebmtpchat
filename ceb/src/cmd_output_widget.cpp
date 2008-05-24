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

#include "cmd_output_widget.h"
#include "profile.h"

#include <QVBoxLayout>
#include <QScrollBar>

CmdOutputWidget::CmdOutputWidget(Session *session, const QString &cmdName, QWidget *parent) : SessionWidget(session, parent),
                                                                                              _cmdName(cmdName)
{
    init();

    // Connect session
    connect(_session, SIGNAL(newToken(const Token&)),
            this, SLOT(newTokenFromSession(const Token&)));
}

void CmdOutputWidget::init()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(2);

    // Main output
    _textEditOutput = new MyTextEdit;
    _tokenRenderer.setTextEdit(_textEditOutput);
    _tokenRenderer.setSession(_session);
    _textEditOutput->setReadOnly(true);
    _textEditOutput->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    QPalette palette = _textEditOutput->palette();
    palette.setColor(QPalette::Base, Profile::instance().textSkin().backgroundColor());
    _textEditOutput->setPalette(palette);
    mainLayout->addWidget(_textEditOutput);
    QSizePolicy sizePolicy = _textEditOutput->sizePolicy();
    sizePolicy.setHorizontalStretch(1);
    _textEditOutput->setSizePolicy(sizePolicy);
    _textEditOutput->setCurrentFont(QFont("Bitstream Vera Sans Mono", 8, 0));
}

void CmdOutputWidget::newTokenFromSession(const Token &token)
{
    if (_cmdName == "wall")
        switch(token.type())
        {
        case Token::WallBegin:
        case Token::WallEnd:
        case Token::WallLine:
            break;
        default:
            return;
        }
    else if (_cmdName == "who")
        switch(token.type())
        {
        case Token::WhoBegin:
        case Token::WhoSeparator:
        case Token::WhoEnd:
        case Token::WhoEndNoUser:
        case Token::WhoLine:
            break;
        default:
            return;
        }
    else if (_cmdName == "finger")
        switch(token.type())
        {
        case Token::FingerBegin:
        case Token::FingerEnd:
        case Token::FingerLine:
            break;
        default:
            return;
        }

    QScrollBar *sb = _textEditOutput->verticalScrollBar();
    bool scrollDown = sb->maximum() - sb->value() < 10;

    _tokenRenderer.displayToken(token);

    if (scrollDown)
        scrollOutputToBottom();
}

void CmdOutputWidget::scrollOutputToBottom()
{
    QScrollBar *sb = _textEditOutput->verticalScrollBar();
    sb->setValue(sb->maximum());
}

QString CmdOutputWidget::widgetCaption() const
{
    return _cmdName;
}
