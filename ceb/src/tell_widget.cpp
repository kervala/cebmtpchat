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

#include <QVBoxLayout>
#include <QScrollBar>
#include <QTextCursor>
#include <QKeyEvent>
#include <QApplication>
#include <QDir>

#include "profile.h"
#include "lua_utils.h"
#include "paths.h"

#include "tell_widget.h"

void TellWidget::init()
{
    const TextSkin &textSkin = Profile::instance().textSkin();
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(1);
    QPalette palette;

    // In/Out Splitter
    QSplitter *splitterOutIn = new QSplitter;
    mainLayout->addWidget(splitterOutIn);
    splitterOutIn->setChildrenCollapsible(false);
    splitterOutIn->setOrientation(Qt::Vertical);

    // Main output
    QWidget *outputWidget = new QWidget;
    QVBoxLayout *outputLayout = new QVBoxLayout(outputWidget);
    outputLayout->setMargin(0);
    splitterOutIn->addWidget(outputWidget);

    _textEditOutput = new MyTextEdit;
    outputLayout->addWidget(_textEditOutput);
    _tokenRenderer.setTextEdit(_textEditOutput);
    _tokenRenderer.setSession(_session);
    _textEditOutput->setAllowFilters(true);
    _textEditOutput->setReadOnly(true);
    _textEditOutput->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    palette = _textEditOutput->palette();
    palette.setColor(QPalette::Base, Profile::instance().textSkin().backgroundColor());
    palette.setColor(QPalette::Inactive, QPalette::Highlight, palette.color(QPalette::Active, QPalette::Highlight));
    _textEditOutput->setPalette(palette);
    QSizePolicy sizePolicy = outputWidget->sizePolicy();
    sizePolicy.setVerticalStretch(1);
    outputWidget->setSizePolicy(sizePolicy);
    connect(_textEditOutput, SIGNAL(myKeyPressed(const QKeyEvent &)),
            this, SLOT(outputKeyPressed(const QKeyEvent &)));
    splitterOutIn->addWidget(outputWidget);
//	_textEditOutput->setCurrentFont(QFont(Profile::instance().globalFontName, Profile::instance().globalFontSize, 0));
    connect(_textEditOutput, SIGNAL(sendToChat(const QString &)),
            this, SLOT(outputFilterSendToChat(const QString &)));

    // Search widget
    _searchWidget = new SearchWidget;
    outputLayout->addWidget(_searchWidget);
    connect(_searchWidget, SIGNAL(hideMe()), this, SLOT(hideSearchWidget()));
    _searchWidget->setVisible(false);
    _searchWidget->setTextWidget(_textEditOutput);

    // Bottom input
    QWidget *inputWidget = new QWidget;
    splitterOutIn->addWidget(inputWidget);
    QHBoxLayout *inputLayout = new QHBoxLayout(inputWidget);
    inputLayout->setMargin(0);

    // Main input
    _historyWidget = new HistoryWidget;
    _historyWidget->setMinimumHeight(20);
    palette = _historyWidget->palette();
    palette.setColor(QPalette::Base, Profile::instance().textSkin().backgroundColor());
    _historyWidget->setPalette(palette);
    _historyWidget->setFont(textSkin.inputTextFont().font());
    connect(_historyWidget, SIGNAL(textValidated(const QString &)),
            this, SLOT(sendText(const QString &)));
    connect(_historyWidget, SIGNAL(pageUp()), this, SLOT(historyPageUp()));
    connect(_historyWidget, SIGNAL(pageDown()), this, SLOT(historyPageDown()));
    inputLayout->addWidget(_historyWidget);

    // Filter combobox
    _comboBoxFilter = new QComboBox;
    connect(_comboBoxFilter, SIGNAL(activated(int)), this, SLOT(filterActivated(int)));
    inputLayout->addWidget(_comboBoxFilter);

    initScriptComboBox();

    QList<int> list2;
    list2.append(0);
    list2.append(_session->config().entryHeight());
    splitterOutIn->setSizes(list2);

    _firstShow = true;
    _userAway = false;
    _chatBlock = chatBlockNoOne;
}

TellWidget::TellWidget(Session *session, const QString &login, QWidget *parent) : SessionWidget(session, parent)
{
    _login = login;

    init();

    connect(_session, SIGNAL(newToken(const Token&)),
            this, SLOT(newTokenFromSession(const Token&)));
}

void TellWidget::sendText(const QString &text)
{
    QString toSend = text;
    if (_comboBoxFilter->currentIndex())
        toSend = executeLuaFilter(_comboBoxFilter->currentText(), text);

    foreach (QString line, toSend.split('\n'))
        _session->sendCommand("tell " + _login + " " + line);
}

void TellWidget::newTokenFromSession(const Token &token)
{
    QString login;
    bool youTalk = false;
    switch(token.type())
    {
    case Token::SomeoneTellsYou:
    case Token::SomeoneAsksYou:
    case Token::SomeoneReplies:
        if (token.arguments()[1] != _login)
            return;

        login = _login;
        break;
    case Token::YouTellToSomeone:
    case Token::YouAskToSomeone:
    case Token::YouReply:
        if (token.arguments()[1] != _login)
            return;

        youTalk = true;
        login = _session->serverLogin();
        break;
/*	case Token::SomeoneComesIn:
        if (event.arguments()[1] != _login)
        return;
        break;
	case Token::SomeoneLeaves:
	case Token::SomeoneLeavesMsg:
	case Token::SomeoneDisconnects:
	case Token::SomeoneIsKicked:
	case Token::SomeoneIsKickedMsg:
	case Token::YouKickSomeone:
	case Token::YouKickSomeoneMsg:
        if (event.arguments()[1] != _login)
        return;
        break;*/
    default:
        return;
    }

    QScrollBar *sb = _textEditOutput->verticalScrollBar();
    bool scrollDown = sb->maximum() - sb->value() < 10;

    bool displayTimeStamp = false;

    if (Profile::instance().timeStampInTellTabs)
        switch (Profile::instance().timeStampPolicy)
        {
        case Profile::Policy_Classic:
            displayTimeStamp = _session->away();
            break;
        case Profile::Policy_Always:
            displayTimeStamp = true;
            break;
        default:
            displayTimeStamp = false;
        }

    QStringList args;
    const QString &sentence = token.arguments()[2];
    args << "<" + login + "> " + sentence << login << sentence;
    QList<int> positions;
    positions << 0 << 1 << login.length() + 3;

    Token translatedToken(Token::SomeoneSays, args, positions, 0);

    _tokenRenderer.displayToken(translatedToken, displayTimeStamp);

/*	if (youTalk && _chatBlock != chatBlockYou)
	{
        _chatBlock = chatBlockYou;
        QString middleLine = QString("<" + login + ">");
        int beforeNum = (80 - middleLine.length()) / 2;
        int afterNum = 80 - beforeNum - middleLine.length();
        QString sepLine = QString(beforeNum, '-') + middleLine + QString(afterNum, '-');
        _textEditOutput->addNewLine(sepLine, Profile::instance().textSkin().textFont().font(),
        QColor(0, 0, 150));
	}
	else if (!youTalk && _chatBlock != chatBlockHim)
	{
        _chatBlock = chatBlockHim;
        QString middleLine = QString("<" + login + ">");
        int beforeNum = (80 - middleLine.length()) / 2;
        int afterNum = 80 - beforeNum - middleLine.length();
        QString sepLine = QString(beforeNum, '-') + middleLine + QString(afterNum, '-');
        _textEditOutput->addNewLine(sepLine, Profile::instance().textSkin().textFont().font(),
        QColor(150, 0, 0));
	}

	_textEditOutput->addNewLine(sentence,
        QFont(Profile::instance().globalFontName, Profile::instance().globalFontSize, 0),
        QColor(0, 0, 0));*/

    if (scrollDown)
        scrollOutputToBottom();
}

void TellWidget::scrollOutputToBottom()
{
    QScrollBar *sb = _textEditOutput->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void TellWidget::setTextColor(int r, int g, int b)
{
    _textEditOutput->setTextColor(QColor(r, g, b));
}

void TellWidget::applyFirstShow()
{
    if (_firstShow)
    {
        _firstShow = false;
        _historyWidget->setFocus();
    }
}

const QString &TellWidget::login() const
{
    return _login;
}

void TellWidget::setLogin(const QString &newLogin)
{
    _login = newLogin;
}

void TellWidget::outputKeyPressed(const QKeyEvent &e)
{
    if (e.key() == Qt::Key_Escape ||
        e.key() == Qt::Key_F11 ||
        e.key() == Qt::Key_F12)
        return;

    QKeyEvent event(QEvent::KeyPress, e.key(), e.modifiers(), e.text(), e.isAutoRepeat(), e.count());
    QApplication::sendEvent(_historyWidget, &event);
    _historyWidget->setFocus();
}

void TellWidget::historyPageUp()
{
    // Send the event to the output text
    QKeyEvent event(QEvent::KeyPress, Qt::Key_PageUp, Qt::NoModifier);
    QApplication::sendEvent(_textEditOutput, &event);
}

void TellWidget::historyPageDown()
{
    // Send the event to the output text
    QKeyEvent event(QEvent::KeyPress, Qt::Key_PageDown, Qt::NoModifier);
    QApplication::sendEvent(_textEditOutput, &event);
}

QString TellWidget::widgetCaption() const
{
    if (_userAway)
        return _login + " (away)";
    return _login;
}

bool TellWidget::userAway() const
{
    return _userAway;
}

void TellWidget::setUserAway(bool userAway)
{
    _userAway = userAway;
}

void TellWidget::refreshFonts()
{
    _historyWidget->setFont(Profile::instance().textSkin().inputTextFont().font());

    QPalette palette = _historyWidget->palette();
    palette.setColor(QPalette::Text, Profile::instance().textSkin().inputTextFont().color());
    _historyWidget->setPalette(palette);
}

void TellWidget::outputFilterSendToChat(const QString &text)
{
    foreach (QString line, text.split('\n'))
        _session->send("tell " + _login + " " + line);
}

void TellWidget::initScriptComboBox()
{
    _comboBoxFilter->clear();
    _comboBoxFilter->addItem(tr("<no filter>"));
    QDir scriptsDir(QDir(Paths::sharePath()).filePath("scripts"));

    QStringList nameFilters;
    nameFilters << "*.lua";
    foreach (QFileInfo fileInfo, scriptsDir.entryInfoList(nameFilters, QDir::Files))
        _comboBoxFilter->addItem(fileInfo.baseName());
}

void TellWidget::filterActivated(int)
{
    _historyWidget->setFocus();
}

void TellWidget::toggleSearchWidgetVisibility()
{
    _searchWidget->setVisible(!_searchWidget->isVisible());

    if (_searchWidget->isVisible())
        _searchWidget->afterShow();
    else
        _historyWidget->setFocus();
}

void TellWidget::hideSearchWidget()
{
    toggleSearchWidgetVisibility();
}

bool TellWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if ((keyEvent->key() == Qt::Key_F &&
            keyEvent->modifiers() & Qt::ControlModifier) ||
            (keyEvent->key() == Qt::Key_Escape && _searchWidget->isVisible()))
        {
            toggleSearchWidgetVisibility();
            return true;
        }
    }

    return SessionWidget::eventFilter(obj, event);
}

void TellWidget::search()
{
    if (!_searchWidget->isVisible())
        toggleSearchWidgetVisibility();
}
