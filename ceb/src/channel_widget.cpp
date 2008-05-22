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
#include <QLabel>
#include <QAction>
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>

#include "version.h"
#include "profile.h"
#include "mtp_token_info.h"
#include "lua_utils.h"
#include "transfers_manager.h"
#include "paths.h"

#include "channel_widget.h"

ChannelWidget::ChannelWidget(Session *session, QWidget *parent) : SessionWidget(session, parent)
{
    init();

    connect(m_session, SIGNAL(newToken(const TokenEvent&)),
            this, SLOT(newTokenFromSession(const TokenEvent&)));
    connect(m_session, SIGNAL(connecting()), this, SLOT(sessionConnecting()));
    connect(m_session, SIGNAL(connected()), this, SLOT(sessionConnected()));
    connect(m_session, SIGNAL(logged()), this, SLOT(sessionLogged()));
    connect(m_session, SIGNAL(disconnected()), this, SLOT(sessionDisconnected()));
    connect(m_session, SIGNAL(socketError(const QString &)), this, SLOT(sessionSocketError(const QString &)));
    connect(m_session, SIGNAL(loginChanged(const QString &, const QString &)),
            this, SLOT(loginChanged(const QString &, const QString &)));
}

void ChannelWidget::init()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(2);

    // Keep alive timer
    connect(&timerKeepAlive, SIGNAL(timeout()), this, SLOT(timerTimeout()));

    // Topic
    widgetTopic = new QWidget(this);
    QHBoxLayout *topicLayout = new QHBoxLayout(widgetTopic);
    topicLayout->setMargin(0);
    mainLayout->addWidget(widgetTopic);
    QLabel *labelTopic = new QLabel("Topic: ");
    topicLayout->addWidget(labelTopic);
    lineEditTopic = new QLineEdit;
    lineEditTopic->installEventFilter(this);
    lineEditTopic->setReadOnly(true);
    QPalette palette = lineEditTopic->palette();
    palette.setColor(QPalette::Base, Profile::instance().textSkin().backgroundColor());
    lineEditTopic->setPalette(palette);
    topicLayout->addWidget(lineEditTopic);
    widgetTopic->setVisible(Profile::instance().topicWindowVisible);

    // In/Out Splitter
    splitterOutIn = new QSplitter;
    mainLayout->addWidget(splitterOutIn);
    splitterOutIn->setChildrenCollapsible(false);
    splitterOutIn->setOrientation(Qt::Vertical);
    connect(splitterOutIn, SIGNAL(splitterMoved(int, int)),
            this, SLOT(splitterInOutMoved(int, int)));

    // Output and Who
    splitterOutWho = new QSplitter;
    splitterOutIn->addWidget(splitterOutWho);
    splitterOutWho->setOrientation(Qt::Horizontal);
    connect(splitterOutWho, SIGNAL(splitterMoved(int, int)),
            this, SLOT(splitterOutWhoMoved(int, int)));
    splitterOutWho->setChildrenCollapsible(false);
    QSizePolicy sizePolicy = splitterOutWho->sizePolicy();
    sizePolicy.setVerticalStretch(1);
    splitterOutWho->setSizePolicy(sizePolicy);

    // Main output
    QWidget *outputWidget = new QWidget;
    QVBoxLayout *outputLayout = new QVBoxLayout(outputWidget);
    outputLayout->setMargin(0);
    splitterOutWho->addWidget(outputWidget);

    textEditOutput = new MyTextEdit;
    outputLayout->addWidget(textEditOutput);
    m_tokenRenderer.setTextEdit(textEditOutput);
    m_tokenRenderer.setSession(m_session);
    textEditOutput->setAllowFilters(true);
    textEditOutput->setReadOnly(true);
    textEditOutput->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    palette = textEditOutput->palette();
    palette.setColor(QPalette::Base, Profile::instance().textSkin().backgroundColor());
	palette.setColor(QPalette::Inactive, QPalette::Highlight, palette.color(QPalette::Active, QPalette::Highlight));
    textEditOutput->setPalette(palette);
    sizePolicy = outputWidget->sizePolicy();
    sizePolicy.setHorizontalStretch(1);
    outputWidget->setSizePolicy(sizePolicy);
    connect(textEditOutput, SIGNAL(myKeyPressed(const QKeyEvent &)),
            this, SLOT(outputKeyPressed(const QKeyEvent &)));
    connect(textEditOutput, SIGNAL(sendToChat(const QString &)),
            this, SLOT(outputFilterSendToChat(const QString &)));
    splitterOutIn->addWidget(splitterOutWho);
//	textEditOutput->setCurrentFont(QFont(Profile::instance().globalFontName, Profile::instance().globalFontSize, 0));

    // Search widget
    _searchWidget = new SearchWidget;
    outputLayout->addWidget(_searchWidget);
    connect(_searchWidget, SIGNAL(hideMe()), this, SLOT(hideSearchWidget()));
    _searchWidget->setVisible(false);
    _searchWidget->setTextWidget(textEditOutput);

    // Who column
    QWidget *whoWidget = new QWidget;
    splitterOutWho->addWidget(whoWidget);
    QVBoxLayout *whoLayout = new QVBoxLayout(whoWidget);
    whoLayout->setMargin(0);
    splitterOutWho->addWidget(whoWidget);

    QFrame *whoTitleFrame = new QFrame;
    whoTitleFrame->setFrameShape(QFrame::Box);
    whoTitleFrame->setFrameShadow(QFrame::Plain);
    whoLayout->addWidget(whoTitleFrame);
    labelWhoTitle = new QLabel("0 users");
    labelWhoTitle->setAlignment(Qt::AlignHCenter);
    QVBoxLayout *whoTitleLayout = new QVBoxLayout(whoTitleFrame);
    whoTitleLayout->setMargin(0);
    whoTitleLayout->addWidget(labelWhoTitle);

    listWidgetWho = new QListWidget;
    listWidgetWho->installEventFilter(this);
    listWidgetWho->setContextMenuPolicy(Qt::ActionsContextMenu);

    // Who list context menu
    QAction *action = new QAction("finger", 0);
    connect(action, SIGNAL(triggered()), this, SLOT(finger()));
    listWidgetWho->addAction(action);

    action = new QAction("beep", 0);
    connect(action, SIGNAL(triggered()), this, SLOT(beep()));
    listWidgetWho->addAction(action);

    action = new QAction("kick", 0);
    connect(action, SIGNAL(triggered()), this, SLOT(kick()));
    listWidgetWho->addAction(action);

    action = new QAction("initiate a tell session", 0);
    connect(action, SIGNAL(triggered()), this, SLOT(initiateTellSession()));
    listWidgetWho->addAction(action);

/* WAITING FOR FILE TRANSFER	action = new QAction("send a file...", 0);
   connect(action, SIGNAL(triggered()), this, SLOT(sendAFile()));
   listWidgetWho->addAction(action);*/

    palette = listWidgetWho->palette();
    palette.setColor(QPalette::Base, Profile::instance().textSkin().backgroundColor());
    listWidgetWho->setPalette(palette);
    listWidgetWho->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    whoLayout->addWidget(listWidgetWho);
    connect(listWidgetWho, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(whoItemDoubleClicked(QListWidgetItem*)));

    QList<int> list1;
    list1.append(0);
    list1.append(m_session->config().whoWidth());
    splitterOutWho->setSizes(list1);

    // Input widget
    QWidget *inputWidget = new QWidget;
    QHBoxLayout *inputLayout = new QHBoxLayout(inputWidget);
    inputLayout->setMargin(0);
    splitterOutIn->addWidget(inputWidget);

    // Main input
    stackedWidgetEntry = new QStackedWidget;
    inputLayout->addWidget(stackedWidgetEntry);
    stackedWidgetEntry->setMinimumHeight(20);

    // LineEdit
    lineEditWidget = new ChatLineWidget;
    palette = lineEditWidget->palette();
    palette.setColor(QPalette::Base, Profile::instance().textSkin().backgroundColor());
    lineEditWidget->setPalette(palette);
    connect(lineEditWidget, SIGNAL(textValidated(const QString &)),
            this, SLOT(sendText(const QString &)));
    connect(lineEditWidget, SIGNAL(pageUp()), this, SLOT(historyPageUp()));
    connect(lineEditWidget, SIGNAL(pageDown()), this, SLOT(historyPageDown()));
    stackedWidgetEntry->addWidget(lineEditWidget);

    // HistoryWidget
    historyWidget = new HistoryWidget;
    palette = historyWidget->palette();
    palette.setColor(QPalette::Base, Profile::instance().textSkin().backgroundColor());
    historyWidget->setPalette(palette);
    historyWidget->setCompletionMode(true);
    connect(historyWidget, SIGNAL(textValidated(const QString &)),
            this, SLOT(sendText(const QString &)));
    connect(historyWidget, SIGNAL(pageUp()), this, SLOT(historyPageUp()));
    connect(historyWidget, SIGNAL(pageDown()), this, SLOT(historyPageDown()));
    stackedWidgetEntry->addWidget(historyWidget);

    stackedWidgetEntry->setCurrentIndex(0);

    // Filter combobox
    comboBoxFilter = new QComboBox;
    connect(comboBoxFilter, SIGNAL(activated(int)), this, SLOT(filterActivated(int)));
    inputLayout->addWidget(comboBoxFilter);

    initScriptComboBox();

    QList<int> list2;
    list2.append(0);
    list2.append(m_session->config().entryHeight());
    splitterOutIn->setSizes(list2);

    refreshFonts();

    firstShow = true;

    timerWhoBlinking.setInterval(10);
    connect(&timerWhoBlinking, SIGNAL(timeout()), this, SLOT(whoBlinking()));
}

bool ChannelWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_F &&
            keyEvent->modifiers() & Qt::ControlModifier)
            toggleSearchWidgetVisibility();
        if (keyEvent->key() == Qt::Key_Escape && _searchWidget->isVisible())
            toggleSearchWidgetVisibility();
    }

    if (obj == listWidgetWho || obj == lineEditTopic)
    {
        if (event->type() == QEvent::KeyPress)
        {
            outputKeyPressed(*static_cast<QKeyEvent*>(event));
            return true;
        } else if (event->type() == QEvent::ShortcutOverride)
            return true;
        else
            return false;
    }
    return SessionWidget::eventFilter(obj, event);
}

void ChannelWidget::sendText(const QString &text)
{
    // Filter?
    if (comboBoxFilter->currentIndex())
        m_session->send(executeLuaFilter(comboBoxFilter->currentText(), text));
    else
        m_session->send(text);
}

QListWidgetItem *ChannelWidget::getWhoItemByNickname(const QString &nickname)
{
    QList<QListWidgetItem*> items =
        listWidgetWho->findItems(nickname, Qt::MatchWildcard);
    if (items.count() > 0)
        return items[0];
    else
        return 0;
}

void ChannelWidget::newTokenFromSession(const TokenEvent &event)
{
    QColor color(0, 0, 0);
/*	QFont normalFont(Profile::instance().globalFontName, Profile::instance().globalFontSize, 0);
	QFont timeStampFont(Profile::instance().globalFontName, 6, 0);*/
    QScrollBar *sb = textEditOutput->verticalScrollBar();
    bool scrollDown = sb->maximum() - sb->value() < 10;
    switch(event.token())
    {
    case Token_LoginAsked:
        lineEditWidget->setEchoMode(QLineEdit::Normal);
        break;
    case Token_PasswordAsked:
        lineEditWidget->setEchoMode(QLineEdit::Password);
        break;
    case Token_Topic:
    case Token_YouSetTopic:
        lineEditTopic->setText(event.arguments()[2]);
        break;
    case Token_SomeoneSetTopic:
        lineEditTopic->setText(event.arguments()[3]);
        break;
    case Token_SomeoneTellsYou:
    case Token_YouTellToSomeone:
    case Token_SomeoneAsksYou:
    case Token_YouAskToSomeone:
    case Token_SomeoneReplies:
    case Token_YouReply:
        return;
    case Token_WallBegin:
        if (Profile::instance().tabForWall)
            return;
        break;
    case Token_WallEnd:
        if (Profile::instance().tabForWall)
            return;
        break;
    case Token_WallLine:
        if (Profile::instance().tabForWall)
            return;
        break;
    case Token_WhoBegin:
        listWidgetWho->clear();
        historyWidget->clearCompletionWords();
        if (whoTicketID == event.ticketID() && event.ticketID() >= 0)
            return;
        if (Profile::instance().tabForWho)
            return;
        break;
    case Token_WhoSeparator:
        if (whoTicketID == event.ticketID() && event.ticketID() >= 0)
            return;
        if (Profile::instance().tabForWho)
            return;
        break;
    case Token_WhoEnd:
        labelWhoTitle->setText(event.arguments()[2] + tr(" users"));
        if (whoTicketID == event.ticketID() && event.ticketID() >= 0)
            return;
        if (Profile::instance().tabForWho)
            return;
        break;
    case Token_WhoEndNoUser:
        if (whoTicketID == event.ticketID() && event.ticketID() >= 0)
            return;
        if (Profile::instance().tabForWho)
            return;
        break;
    case Token_WhoLine:
    {
        QListWidgetItem *item = new QListWidgetItem(listWidgetWho);
        item->setText(event.arguments()[1]);

        if (event.arguments()[3] != m_session->channel())
            item->setIcon(QIcon(":/images/yellow-led.png"));
        else if (event.arguments()[4] == "*Away*")
            item->setIcon(QIcon(":/images/away.png"));
        else
            item->setIcon(QIcon(":/images/here.png"));
        if (event.arguments()[1].toUpper() != m_session->serverLogin().toUpper())
            historyWidget->addCompletionWord(event.arguments()[1]);
        if (whoTicketID == event.ticketID() && event.ticketID() >= 0)
            return;
        if (Profile::instance().tabForWho)
            return;
    }
    break;
    case Token_FingerBegin:
    case Token_FingerEnd:
    case Token_FingerLine:
        if (Profile::instance().tabForFinger)
            return;
        break;
    case Token_SomeoneAway:
    {
        QListWidgetItem *item = getWhoItemByNickname(event.arguments()[1]);
        if (item)
            item->setIcon(QIcon(":/images/away.png"));
    }
    break;
    case Token_SomeoneBack:
    {
        QListWidgetItem *item = getWhoItemByNickname(event.arguments()[1]);
        if (item)
            item->setIcon(QIcon(":/images/here.png"));
    }
    break;
    case Token_YouAway:
    {
        QListWidgetItem *item = getWhoItemByNickname(m_session->serverLogin());
        if (item)
            item->setIcon(QIcon(":/images/away.png"));
        colorizeChatItems(Profile::instance().textSkin().awayBackgroundColor());
        if (Profile::instance().awaySeparatorLines) // Away separator
            textEditOutput->addNewLine(Profile::instance().getAwaySeparator(), Profile::instance().textSkin().textFont().font(), Profile::instance().awaySeparatorColor);
        textEditOutput->isAway = true;
    }
    break;
    case Token_YouBack:
    {
        QListWidgetItem *item = getWhoItemByNickname(m_session->serverLogin());
        if (item)
            item->setIcon(QIcon(":/images/here.png"));
        colorizeChatItems(Profile::instance().textSkin().backgroundColor());
        if (Profile::instance().awaySeparatorLines) // Away separator
            textEditOutput->addNewLine(Profile::instance().getAwaySeparator(), Profile::instance().textSkin().textFont().font(), Profile::instance().awaySeparatorColor);
        textEditOutput->isAway = false;
    }
    break;
    case Token_SomeoneAwayWarning:
        return;
    case Token_MessageBegin:
        if (event.ticketID() >= 0)
            return;
        break;
    case Token_MessageLine:
        if (event.ticketID() >= 0)
            return;
        break;
    case Token_MessageEnd:
        if (event.ticketID() >= 0)
            return;
        break;
    case Token_HelpBegin:
        // serverlist?
        if ((m_backupServersHelp = (event.arguments()[1].toUpper() == "SERVERLIST")))
            m_backupServers.clear();
        if (helpTicketID == event.ticketID() && event.ticketID() >= 0)
            return;
        break;
    case Token_HelpEndNormal:
        if (m_backupServersHelp)
        {
            SessionConfig sessionConfig = m_session->config();
            sessionConfig.setBackupServers(m_backupServers);
            m_session->setConfig(sessionConfig);

            // Record in session configs
            SessionConfig *profileConfig = Profile::instance().getSessionConfig(sessionConfig.name());
            if (profileConfig)
                profileConfig->setBackupServers(m_backupServers);
        }
    case Token_HelpEndNoHelp:
        if (helpTicketID == event.ticketID() && event.ticketID() >= 0)
            return;
        break;
    case Token_HelpLine:
    {
        QString str = event.arguments()[1].trimmed();
        if (m_backupServersHelp && str != "")
        {
            QStringList strLst = str.split(":");

            m_backupServers << BackupServer(strLst[0], strLst[1].toInt());
        }
    }
    if (helpTicketID == event.ticketID() && event.ticketID() >= 0)
        return;
    break;
    case Token_UserLoginRenamed:
        changeLoginInWhoColumn(event.arguments()[1], event.arguments()[2]);
        break;
    case Token_SomeoneComesIn:
    {
        QListWidgetItem *item = new QListWidgetItem(listWidgetWho);
        item->setText(event.arguments()[1]);
        item->setIcon(QIcon(":/images/here.png"));
        labelWhoTitle->setText(QString::number(listWidgetWho->count()) + " users");
        historyWidget->addCompletionWord(event.arguments()[1]);
        item->setTextColor(Qt::red);
        userToWhoBlinkTime[event.arguments()[1]] = 5000;
        userToWhoAscendingOrder[event.arguments()[1]] = false;
        timerWhoBlinking.start();
    }
    break;
    case Token_SomeoneLeaves:
    case Token_SomeoneDisconnects:
    case Token_SomeoneIsKicked:
    case Token_YouKickSomeone:
        // Refresh who column
    {
        QListWidgetItem *item = getWhoItemByNickname(event.arguments()[1]);
        if (item)
        {
            listWidgetWho->takeItem(listWidgetWho->row(item));
            delete item;
        }

        // Who title
        labelWhoTitle->setText(QString::number(listWidgetWho->count()) + tr(" users"));
    }

    // History widget
    historyWidget->removeCompletionWord(event.arguments()[1]);
    break;
    case Token_YouJoinChannel:
    case Token_YouLeaveChannel:
        whoTicketID = m_session->requestTicket(MtpAnalyzer::Command_Who);
        m_session->send("who all");
        break;
    case Token_SomeoneJoinChannel:
    case Token_SomeoneFadesIntoTheShadows:
    {
        QListWidgetItem *item = getWhoItemByNickname(event.arguments()[1]);
        if (item)
            item->setIcon(QIcon(":/images/yellow-led.png"));
    }
    break;
    case Token_SomeoneLeaveChannel:
    case Token_SomeoneAppearsFromTheShadows:
    {
        QListWidgetItem *item = getWhoItemByNickname(event.arguments()[1]);
        if (item)
            item->setIcon(QIcon(":/images/here.png"));
    }
    break;
    case Token_Date:
        if (dateTicketID == event.ticketID() && event.ticketID() >= 0)
            return;
        break;
    case Token_YourClientIs:
        if (setClientTicketID == event.ticketID() && event.ticketID() >= 0)
            return;
        break;
    case Token_SomeoneSays:
    {
        // Someone talks about you?
        QString sentence = event.arguments()[2];
        if (sentence.indexOf(regExpAboutMe) >= 0)
            color = QColor(200, 0, 200);
        else
            color = QColor(0, 0, 0);
    }
    break;
    case Token_Data:
/*TEMP		if (TransfersManager::isCommand(event.arguments()[2]))
  return;*/
        break;
    default:;
    };

    QString str = QString::number(sb->value());

    QString timeStamp = event.timeStamp().toString("hh:mm:ss ");
    QString line = event.line();

    bool setTimeStamp = false;

    if (m_session->away())
        switch (Profile::instance().timeStampPolicy)
        {
        case Profile::Policy_Classic:
        case Profile::Policy_Always:
            setTimeStamp = true;
            break;
        default:;
        }
    else
        switch (Profile::instance().timeStampPolicy)
        {
        case Profile::Policy_Always:
            setTimeStamp = true;
            break;
        default:;
        }

    // Display it!
    m_tokenRenderer.displayToken(event, setTimeStamp);

    // Postfix things
    if (event.token() == Token_IndicatedActiveServer)
        textEditOutput->addNewLine(QString("Ok, let's trust you =^^=, we move to (%1:%2)").arg(event.arguments()[0]).
                                   arg(event.arguments()[1].trimmed()),
                                   Profile::instance().textSkin().textFont().font(), QColor(255, 0, 0));

    if (scrollDown)
        textEditOutput->scrollOutputToBottom();

    sendHighlightSignal(MinorHighlight);
}

void ChannelWidget::colorizeChatItems(const QColor &color)
{
    // Topic
    QPalette palette = lineEditTopic->palette();
    palette.setColor(QPalette::Base, color);
    lineEditTopic->setPalette(palette);
    // Output
    palette = textEditOutput->palette();
    palette.setColor(QPalette::Base, color);
    textEditOutput->setPalette(palette);
    // Who column
    palette = listWidgetWho->palette();
    palette.setColor(QPalette::Base, color);
    listWidgetWho->setPalette(palette);
    // Input
    palette = historyWidget->palette();
    palette.setColor(QPalette::Base, color);
    historyWidget->setPalette(palette);
}

bool ChannelWidget::topicWindowVisible()
{
    return widgetTopic->isVisible();
}

void ChannelWidget::showTopicWindow()
{
    widgetTopic->show();
}

void ChannelWidget::hideTopicWindow()
{
    widgetTopic->hide();
}

void ChannelWidget::sessionConnecting()
{
    textEditOutput->addNewLine(tr("Attempt to connect ") + m_session->serverAddress() +
                               ":" + QString::number(m_session->serverPort()) + ".........",
                               Profile::instance().textSkin().textFont().font(),
                               QColor(0, 0, 200));
    textEditOutput->scrollOutputToBottom();
}

void ChannelWidget::sessionConnected()
{
    colorizeChatItems(Profile::instance().textSkin().backgroundColor());
    stackedWidgetEntry->setCurrentIndex(0);

    textEditOutput->addString(tr("successful."), Profile::instance().textSkin().textFont().font(),
                              QColor(0, 200, 0));
    textEditOutput->addNewLine("-", Profile::instance().textSkin().textFont().font(),
                               QColor(200, 0, 0));

    textEditOutput->scrollOutputToBottom();
}

void ChannelWidget::sessionLogged()
{
    // Send "set client"
    setClientTicketID = m_session->requestTicket(MtpAnalyzer::Command_SetClient);
    m_session->send("set client CeB Alpha " + QString(VERSION));

    // Send first who
    whoTicketID = m_session->requestTicket(MtpAnalyzer::Command_Who);
    m_session->send("who all");
    helpTicketID = m_session->requestTicket(MtpAnalyzer::Command_Help);
    m_session->send("help serverlist");

    // Switch entry widget
    stackedWidgetEntry->setCurrentIndex(1);

    // Start (or not) the keep alive timer
    if (Profile::instance().keepAlive)
    {
        timerKeepAlive.start();
        timerKeepAlive.setInterval(Profile::instance().keepAlive * 1000);
    }

    regExpAboutMe = QRegExp("(^|\\W)" + m_session->serverLogin() + "(\\W|$)", Qt::CaseInsensitive);
}

void ChannelWidget::sessionDisconnected()
{
    // Stop the keep alive timer
    if (timerKeepAlive.isActive())
        timerKeepAlive.stop();
    listWidgetWho->clear();
    colorizeChatItems(Profile::instance().textSkin().awayBackgroundColor());
    labelWhoTitle->setText("");
}

void ChannelWidget::sessionSocketError(const QString &errorStr)
{
    textEditOutput->addNewLine(errorStr,
                               Profile::instance().textSkin().textFont().font(),
                               QColor(200, 0, 0));

    textEditOutput->scrollOutputToBottom();
}

void ChannelWidget::outputKeyPressed(const QKeyEvent &e)
{
    if (e.key() == Qt::Key_Escape ||
        e.key() == Qt::Key_F11 ||
        e.key() == Qt::Key_F12)
        return;

    QKeyEvent event(QEvent::KeyPress, e.key(), e.modifiers(), e.text(), e.isAutoRepeat(), e.count());
    if (stackedWidgetEntry->currentIndex() == 0)
    {
        QApplication::sendEvent(lineEditWidget, &event);
        lineEditWidget->setFocus();
    }
    else
    {
        QApplication::sendEvent(historyWidget, &event);
        historyWidget->setFocus();
    }
}

void ChannelWidget::splitterInOutMoved(int, int)
{
    SessionConfig *config = Profile::instance().getSessionConfig(m_session->config().name());
    if (config)
        config->setEntryHeight(stackedWidgetEntry->height());
}

void ChannelWidget::splitterOutWhoMoved(int, int)
{
    SessionConfig *config = Profile::instance().getSessionConfig(m_session->config().name());
    if (config)
        config->setWhoWidth(listWidgetWho->width());
}

void ChannelWidget::historyPageUp()
{
    // Send the event to the output text
    QKeyEvent event(QEvent::KeyPress, Qt::Key_PageUp, Qt::NoModifier);
    QApplication::sendEvent(textEditOutput, &event);
}

void ChannelWidget::historyPageDown()
{
    // Send the event to the output text
    QKeyEvent event(QEvent::KeyPress, Qt::Key_PageDown, Qt::NoModifier);
    QApplication::sendEvent(textEditOutput, &event);
}

void ChannelWidget::timerTimeout()
{
    if (m_session && m_session->isLogged())
    {
        dateTicketID = m_session->requestTicket(MtpAnalyzer::Command_Date);
        m_session->send("date", false);
    }
}

void ChannelWidget::refreshKeepAlivePolicy()
{
    if (!Profile::instance().keepAlive)
    {
        if (timerKeepAlive.isActive())
            timerKeepAlive.stop();
    }
    else
    {
        if (!timerKeepAlive.isActive())
        {
            timerKeepAlive.setInterval(Profile::instance().keepAlive * 1000);
            if (m_session && m_session->isLogged())
                timerKeepAlive.start();
        }
        else if (timerKeepAlive.interval() != Profile::instance().keepAlive * 1000)
        {
            timerKeepAlive.stop();
            timerKeepAlive.setInterval(Profile::instance().keepAlive * 1000);
            timerKeepAlive.start();
        }
    }
}

void ChannelWidget::refreshWhoColumn()
{
    whoTicketID = m_session->requestTicket(MtpAnalyzer::Command_Who);
    m_session->send("who all");
}

void ChannelWidget::refreshFonts()
{
    const TextSkin &textSkin = Profile::instance().textSkin();
    QPalette palette;
    lineEditTopic->setFont(textSkin.topicTextFont().font());
    palette = lineEditTopic->palette();
    palette.setColor(QPalette::Text, textSkin.topicTextFont().color());
    lineEditTopic->setPalette(palette);

    lineEditWidget->setFont(textSkin.inputTextFont().font());
    palette = lineEditWidget->palette();
    palette.setColor(QPalette::Text, textSkin.inputTextFont().color());
    lineEditWidget->setPalette(palette);

    historyWidget->setFont(textSkin.inputTextFont().font());
    palette = historyWidget->palette();
    palette.setColor(QPalette::Text, textSkin.inputTextFont().color());
    historyWidget->setPalette(palette);

    listWidgetWho->setFont(textSkin.whoTextFont().font());
    palette = listWidgetWho->palette();
    palette.setColor(QPalette::Text, textSkin.whoTextFont().color());
    listWidgetWho->setPalette(palette);
}

void ChannelWidget::whoItemDoubleClicked(QListWidgetItem *item)
{
    if (item) // Get the nickname
        emit whoUserDoubleClicked(item->text());
}

void ChannelWidget::loginChanged(const QString &oldLogin, const QString &newLogin)
{
    regExpAboutMe = QRegExp("(^|\\W)" + newLogin + "(\\W|$)", Qt::CaseInsensitive);
    changeLoginInWhoColumn(oldLogin, newLogin);
}

void ChannelWidget::changeLoginInWhoColumn(const QString &oldLogin, const QString &newLogin)
{
    QListWidgetItem *item = getWhoItemByNickname(oldLogin);
    if (item)
        item->setText(newLogin);
}

void ChannelWidget::finger()
{
    QListWidgetItem *item = listWidgetWho->currentItem();
    if (item)
        m_session->send("finger " + item->text());
}

void ChannelWidget::beep()
{
    QListWidgetItem *item = listWidgetWho->currentItem();
    if (item)
        m_session->send("beep " + item->text());
}

void ChannelWidget::kick()
{
    QListWidgetItem *item = listWidgetWho->currentItem();
    if (item)
        m_session->send("kick " + item->text());
}

void ChannelWidget::initiateTellSession()
{
    QListWidgetItem *item = listWidgetWho->currentItem();
    if (item)
        emit tellSessionAsked(item->text());
}

void ChannelWidget::sendAFile()
{
    QListWidgetItem *item = listWidgetWho->currentItem();
    if (!item)
        return;

    // User which we want to send a file
    QString nick = item->text();

    // Pick a file
    QString fileName = QFileDialog::getOpenFileName(this);
    if (fileName == "")
        return;

    TransfersManager::instance().propose(m_session, nick, fileName);
}

void ChannelWidget::applyFirstShow()
{
    if (firstShow)
    {
        firstShow = false;
        textEditOutput->scrollOutputToBottom();
        if (stackedWidgetEntry->currentWidget() == lineEditWidget)
            lineEditWidget->setFocus();
        else
            historyWidget->setFocus();
    }
}

QString ChannelWidget::widgetCaption() const
{
    return m_session->channel();
}

void ChannelWidget::initScriptComboBox()
{
    comboBoxFilter->clear();
    comboBoxFilter->addItem(tr("<no filter>"));
    QDir scriptsDir(QDir(Paths::sharePath()).filePath("scripts"));

    QStringList nameFilters;
    nameFilters << "*.lua";
    foreach (QFileInfo fileInfo, scriptsDir.entryInfoList(nameFilters, QDir::Files))
        comboBoxFilter->addItem(fileInfo.baseName());
}

void ChannelWidget::filterActivated(int)
{
    if (stackedWidgetEntry->currentWidget() == lineEditWidget)
        lineEditWidget->setFocus();
    else
        historyWidget->setFocus();
}

void ChannelWidget::outputFilterSendToChat(const QString &text)
{
    m_session->send(text);
}

void ChannelWidget::whoBlinking()
{
    bool stopAll = true;
    foreach (const QString &user, userToWhoBlinkTime.keys())
    {
        int blinkTime = userToWhoBlinkTime[user];
        if (blinkTime > 0)
        {
            stopAll = false;
            // It remains time to blink
            userToWhoBlinkTime[user] = blinkTime - timerWhoBlinking.interval();
            QListWidgetItem *item = getWhoItemByNickname(user);
            if (item)
            {
                QColor currentColor = item->textColor();
                int red = currentColor.red();
                if (userToWhoAscendingOrder[user])
                {
                    if (red < 255)
                    {
                        if (red + 10 <= 255)
                            currentColor.setRed(red + 10);
                        else
                            currentColor.setRed(255);
                    }
                    else
                        userToWhoAscendingOrder[user] = false;
                } else
                {
                    if (red > 0)
                    {
                        if (red - 10 >= 0)
                            currentColor.setRed(red - 10);
                        else
                            currentColor.setRed(0);
                    }
                    else
                        userToWhoAscendingOrder[user] = true;
                }
                item->setTextColor(currentColor);
            }

            if (userToWhoBlinkTime[user] <= 0 && item)
                item->setTextColor(item->listWidget()->palette().color(QPalette::WindowText));
        }
    }
    if (stopAll)
        timerWhoBlinking.stop();
}

void ChannelWidget::toggleSearchWidgetVisibility()
{
    _searchWidget->setVisible(!_searchWidget->isVisible());

    if (_searchWidget->isVisible())
        _searchWidget->afterShow();
    else
	{
      if (stackedWidgetEntry->currentIndex() == 0)
        lineEditWidget->setFocus();
      else
        historyWidget->setFocus();
	}
}

void ChannelWidget::hideSearchWidget()
{
    toggleSearchWidgetVisibility();
}

void ChannelWidget::search()
{
    if (!_searchWidget->isVisible())
        toggleSearchWidgetVisibility();
}
