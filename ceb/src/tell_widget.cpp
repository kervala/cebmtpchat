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

#include "tell_widget.h"
#include "profile_manager.h"
#include "lua_utils.h"

void TellWidget::init()
{
    Profile &profile = *ProfileManager::instance().currentProfile();
    const TextSkin &textSkin = profile.textSkin();
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

    m_textEditOutput = new MyTextEdit;
    outputLayout->addWidget(m_textEditOutput);
    m_tokenRenderer.setTextEdit(m_textEditOutput);
    m_tokenRenderer.setSession(m_session);
    m_textEditOutput->setAllowFilters(true);
    m_textEditOutput->setReadOnly(true);
    m_textEditOutput->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    palette = m_textEditOutput->palette();
    palette.setColor(QPalette::Base, profile.textSkin().backgroundColor());
	palette.setColor(QPalette::Inactive, QPalette::Highlight, palette.color(QPalette::Active, QPalette::Highlight));
    m_textEditOutput->setPalette(palette);
    QSizePolicy sizePolicy = outputWidget->sizePolicy();
    sizePolicy.setVerticalStretch(1);
    outputWidget->setSizePolicy(sizePolicy);
    connect(m_textEditOutput, SIGNAL(myKeyPressed(const QKeyEvent &)),
            this, SLOT(outputKeyPressed(const QKeyEvent &)));
    splitterOutIn->addWidget(outputWidget);
//	m_textEditOutput->setCurrentFont(QFont(profile.globalFontName, profile.globalFontSize, 0));
    connect(m_textEditOutput, SIGNAL(sendToChat(const QString &)),
            this, SLOT(outputFilterSendToChat(const QString &)));

    // Search widget
    _searchWidget = new SearchWidget;
    outputLayout->addWidget(_searchWidget);
    connect(_searchWidget, SIGNAL(hideMe()), this, SLOT(hideSearchWidget()));
    _searchWidget->setVisible(false);
    _searchWidget->setTextWidget(m_textEditOutput);

    // Bottom input
    QWidget *inputWidget = new QWidget;
    splitterOutIn->addWidget(inputWidget);
    QHBoxLayout *inputLayout = new QHBoxLayout(inputWidget);
    inputLayout->setMargin(0);

    // Main input
    m_historyWidget = new HistoryWidget;
    m_historyWidget->setMinimumHeight(20);
    palette = m_historyWidget->palette();
    palette.setColor(QPalette::Base, profile.textSkin().backgroundColor());
    m_historyWidget->setPalette(palette);
    m_historyWidget->setFont(textSkin.inputTextFont().font());
    connect(m_historyWidget, SIGNAL(textValidated(const QString &)),
            this, SLOT(sendText(const QString &)));
    connect(m_historyWidget, SIGNAL(pageUp()), this, SLOT(historyPageUp()));
    connect(m_historyWidget, SIGNAL(pageDown()), this, SLOT(historyPageDown()));
    inputLayout->addWidget(m_historyWidget);

    // Filter combobox
    comboBoxFilter = new QComboBox;
    connect(comboBoxFilter, SIGNAL(activated(int)), this, SLOT(filterActivated(int)));
    inputLayout->addWidget(comboBoxFilter);

    initScriptComboBox();

    QList<int> list2;
    list2.append(0);
    list2.append(m_session->config().entryHeight());
    splitterOutIn->setSizes(list2);

    m_firstShow = true;
    m_userAway = false;
    m_chatBlock = chatBlockNoOne;
}

TellWidget::TellWidget(Session *session, const QString &login, QWidget *parent) : SessionWidget(session, parent)
{
    m_login = login;

    init();

    connect(m_session, SIGNAL(newToken(const TokenEvent&)),
            this, SLOT(newTokenFromSession(const TokenEvent&)));
}

void TellWidget::sendText(const QString &text)
{
    QString toSend = text;
    if (comboBoxFilter->currentIndex())
        toSend = executeLuaFilter(comboBoxFilter->currentText(), text);

    foreach (QString line, toSend.split('\n'))
    {

        m_session->send("tell " + m_login + " " + line);
    }
}

void TellWidget::newTokenFromSession(const TokenEvent &event)
{
    QString login;
    bool highlight = false;
    bool youTalk = false;
    switch(event.token())
    {
    case Token_SomeoneTellsYou:
    case Token_SomeoneAsksYou:
    case Token_SomeoneReplies:
        if (event.arguments()[1] != m_login)
            return;

        login = m_login;
        highlight = true;
        break;
    case Token_YouTellToSomeone:
    case Token_YouAskToSomeone:
    case Token_YouReply:
        if (event.arguments()[1] != m_login)
            return;

        youTalk = true;
        login = m_session->serverLogin();
        break;
/*	case Token_SomeoneComesIn:
        if (event.arguments()[1] != m_login)
        return;
        break;
	case Token_SomeoneLeaves:
	case Token_SomeoneLeavesMsg:
	case Token_SomeoneDisconnects:
	case Token_SomeoneIsKicked:
	case Token_SomeoneIsKickedMsg:
	case Token_YouKickSomeone:
	case Token_YouKickSomeoneMsg:
        if (event.arguments()[1] != m_login)
        return;
        break;*/
    default:
        return;
    }

    QScrollBar *sb = m_textEditOutput->verticalScrollBar();
    bool scrollDown = sb->maximum() - sb->value() < 10;

    Profile &profile = *ProfileManager::instance().currentProfile();

    bool displayTimeStamp = false;

    if (profile.timeStampInTellTabs)
        switch (profile.timeStampPolicy)
        {
        case Profile::Policy_Classic:
            displayTimeStamp = m_session->away();
            break;
        case Profile::Policy_Always:
            displayTimeStamp = true;
            break;
        default:
            displayTimeStamp = false;
        }

    QStringList args;
    const QString &sentence = event.arguments()[2];
    args << "<" + login + "> " + sentence << login << sentence;
    QList<int> positions;
    positions << 0 << 1 << login.length() + 3;

    TokenEvent translatedEvent(Token_SomeoneSays, args, positions, 0);

    m_tokenRenderer.displayToken(translatedEvent, displayTimeStamp);

/*	if (youTalk && m_chatBlock != chatBlockYou)
	{
        m_chatBlock = chatBlockYou;
        QString middleLine = QString("<" + login + ">");
        int beforeNum = (80 - middleLine.length()) / 2;
        int afterNum = 80 - beforeNum - middleLine.length();
        QString sepLine = QString(beforeNum, '-') + middleLine + QString(afterNum, '-');
        m_textEditOutput->addNewLine(sepLine, profile.textSkin().textFont().font(),
        QColor(0, 0, 150));
	}
	else if (!youTalk && m_chatBlock != chatBlockHim)
	{
        m_chatBlock = chatBlockHim;
        QString middleLine = QString("<" + login + ">");
        int beforeNum = (80 - middleLine.length()) / 2;
        int afterNum = 80 - beforeNum - middleLine.length();
        QString sepLine = QString(beforeNum, '-') + middleLine + QString(afterNum, '-');
        m_textEditOutput->addNewLine(sepLine, profile.textSkin().textFont().font(),
        QColor(150, 0, 0));
	}

	m_textEditOutput->addNewLine(sentence,
        QFont(profile.globalFontName, profile.globalFontSize, 0),
        QColor(0, 0, 0));*/

    if (scrollDown)
        scrollOutputToBottom();

    if (highlight)
        sendHighlightSignal(MajorHighlight);
}

void TellWidget::scrollOutputToBottom()
{
    QScrollBar *sb = m_textEditOutput->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void TellWidget::setTextColor(int r, int g, int b)
{
    m_textEditOutput->setTextColor(QColor(r, g, b));
}

void TellWidget::applyFirstShow()
{
    if (m_firstShow)
    {
        m_firstShow = false;
        m_historyWidget->setFocus();
    }
}

const QString &TellWidget::login() const
{
    return m_login;
}

void TellWidget::setLogin(const QString &newLogin)
{
    m_login = newLogin;
}

void TellWidget::outputKeyPressed(const QKeyEvent &e)
{
    if (e.key() == Qt::Key_Escape ||
        e.key() == Qt::Key_F11 ||
        e.key() == Qt::Key_F12)
        return;

    QKeyEvent event(QEvent::KeyPress, e.key(), e.modifiers(), e.text(), e.isAutoRepeat(), e.count());
    QApplication::sendEvent(m_historyWidget, &event);
    m_historyWidget->setFocus();
}

void TellWidget::historyPageUp()
{
    // Send the event to the output text
    QKeyEvent event(QEvent::KeyPress, Qt::Key_PageUp, Qt::NoModifier);
    QApplication::sendEvent(m_textEditOutput, &event);
}

void TellWidget::historyPageDown()
{
    // Send the event to the output text
    QKeyEvent event(QEvent::KeyPress, Qt::Key_PageDown, Qt::NoModifier);
    QApplication::sendEvent(m_textEditOutput, &event);
}

QString TellWidget::widgetCaption() const
{
    if (m_userAway)
        return m_login + " (away)";
    return m_login;
}

bool TellWidget::userAway() const
{
    return m_userAway;
}

void TellWidget::setUserAway(bool userAway)
{
    m_userAway = userAway;
}

void TellWidget::refreshFonts()
{
    Profile &profile = *ProfileManager::instance().currentProfile();
    m_historyWidget->setFont(profile.textSkin().inputTextFont().font());

    QPalette palette = m_historyWidget->palette();
    palette.setColor(QPalette::Text, profile.textSkin().inputTextFont().color());
    m_historyWidget->setPalette(palette);
}

void TellWidget::outputFilterSendToChat(const QString &text)
{
    foreach (QString line, text.split('\n'))
    {
        m_session->send("tell " + m_login + " " + line);
    }
}

void TellWidget::initScriptComboBox()
{
    comboBoxFilter->clear();
    comboBoxFilter->addItem(tr("<no filter>"));
    QDir profilesDir(QApplication::applicationDirPath());

    if (profilesDir.cd("scripts"))
    {
        QStringList nameFilters;
        nameFilters << "*.lua";
        foreach (QFileInfo fileInfo, profilesDir.entryInfoList(nameFilters, QDir::Files))
            comboBoxFilter->addItem(fileInfo.baseName());
    }
}

void TellWidget::filterActivated(int)
{
    m_historyWidget->setFocus();
}

void TellWidget::toggleSearchWidgetVisibility()
{
    _searchWidget->setVisible(!_searchWidget->isVisible());

    if (_searchWidget->isVisible())
        _searchWidget->afterShow();
    else
        m_historyWidget->setFocus();
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
