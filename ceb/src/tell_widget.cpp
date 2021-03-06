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

#include "tell_widget.h"
#include "event_script.h"

#ifdef DEBUG_NEW
#define new DEBUG_NEW
#endif

void TellWidget::init()
{
    installEventFilter(this);
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
    if (Profile::instance().textSkin().isForcedBackgroundColor())
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
    if (Profile::instance().textSkin().isForcedBackgroundColor())
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
    _chatBlock = chatBlockNoOne;

    refreshFonts();
}

TellWidget::TellWidget(Session *session, const QString &login, QWidget *parent) : SessionWidget(session, parent)
{
    _login = login;

    init();

    replayPreviousConversation();

    connect(_session, SIGNAL(newToken(const Token&)),
            this, SLOT(newTokenFromSession(const Token&)));
}

void TellWidget::sendText(const QString &text)
{
    // Filter?
    QString toSend = EventScript::newEntry(_session, text);

    if (_comboBoxFilter->currentIndex())
        toSend = executeLuaFilter(_comboBoxFilter->currentText(), toSend);

    foreach (QString line, toSend.split('\n'))
        _session->sendCommand("tell " + _login + " " + line);
}

void TellWidget::displayToken(const Token &token, bool old)
{
    QString login;
//    bool youTalk = false;
    bool privateToken = false;
    switch(token.type())
    {
    case Token::SomeoneTellsYou:
    case Token::SomeoneAsksYou:
    case Token::SomeoneReplies:
        if (token.arguments()[1] != _login)
            return;

        login = _login;
        privateToken = true;
        break;
    case Token::YouTellToSomeone:
    case Token::YouAskToSomeone:
    case Token::YouReply:
        if (token.arguments()[1] != _login)
            return;

//        youTalk = true;
        login = _session->serverLogin();
        privateToken = true;
        break;
    case Token::SomeoneAway:
    case Token::SomeoneBack:
    case Token::SomeoneAwayWarning:
        if (!token.arguments()[1].compare(_login, Qt::CaseInsensitive))
            emit captionChanged();
        return;
    case Token::YouAway:
    case Token::YouBack:
        if (!_login.compare(_session->serverLogin(), Qt::CaseInsensitive))
            emit captionChanged();
        return;
    case Token::SomeoneComesIn:
    case Token::SomeoneLeaves:
    case Token::SomeoneDisconnects:
    case Token::SomeoneIsKicked:
    case Token::YouKickSomeone:
        if (token.arguments()[1].compare(_login, Qt::CaseInsensitive))
            return;

        emit captionChanged();
        break;
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

    if (old)
        _tokenRenderer.setGrayMode(true);

    if (privateToken)
    {
        QStringList args;
        QString sentence = token.arguments()[2];

        args << "<" + login + "> " + sentence << login << sentence;
        QList<int> positions;
        positions << 0 << 1 << login.length() + 3;

        Token translatedToken(Token::SomeoneSays, args, positions, 0);

        _tokenRenderer.displayToken(translatedToken, displayTimeStamp);
    } else
        _tokenRenderer.displayToken(token, displayTimeStamp);

    if (old)
        _tokenRenderer.setGrayMode(false);
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

void TellWidget::newTokenFromSession(const Token &token)
{
    displayToken(token);
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
    if (!_session->whoPopulation().userForLogin(_login).isValid())
        return _login + " " + tr("(quit)");
    if (_session->whoPopulation().userForLogin(_login).isAway())
        return _login + " " + tr("(away)");
    return _login;
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
        _session->sendCommand("tell " + _login + " " + line);
}

void TellWidget::initScriptComboBox()
{
    _comboBoxFilter->clear();
    _comboBoxFilter->addItem(tr("<no filter>"));
    QDir scriptsDir(Paths::scriptsPath());

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
    toggleSearchWidgetVisibility();
}

void TellWidget::replayPreviousConversation()
{
    const QList<Token> &tokens = _session->privateConversations()[_login];
    foreach (const Token &token, tokens)
        displayToken(token, true);
    if (tokens.count())
    {
        QString text = "*** " + tr("Previous conversation ended at %1").arg(tokens[tokens.count()-1].timeStamp().toString());
        _textEditOutput->addNewLine(text, Profile::instance().textSkin().timeStampFont().font(), Qt::darkGreen);
    }
}

