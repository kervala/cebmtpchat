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
#include <QHeaderView>

#include "version.h"
#include "profile.h"
#include "token_info.h"
#include "lua_utils.h"
#include "transfers_manager.h"
#include "paths.h"
#include "event_script.h"
#include "who_model.h"

#include "channel_widget.h"

ChannelWidget::ChannelWidget(Session *session, QWidget *parent) : SessionWidget(session, parent)
{
    init();

    connect(_session, SIGNAL(newToken(const Token&)), this, SLOT(newToken(const Token&)));
    connect(_session, SIGNAL(connecting()), this, SLOT(sessionConnecting()));
    connect(_session, SIGNAL(connected()), this, SLOT(sessionConnected()));
    connect(_session, SIGNAL(logged()), this, SLOT(sessionLogged()));
    connect(_session, SIGNAL(disconnected()), this, SLOT(sessionDisconnected()));
    connect(_session, SIGNAL(socketError(const QString &)), this, SLOT(sessionSocketError(const QString &)));
    connect(_session, SIGNAL(loginChanged(const QString &, const QString &)),
            this, SLOT(loginChanged(const QString &, const QString &)));
    connect(_session, SIGNAL(cleared()), this, SLOT(sessionCleared()));
}

void ChannelWidget::init()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(2);

    // Keep alive timer
    connect(&_timerKeepAlive, SIGNAL(timeout()), this, SLOT(keepAliveTimeout()));

    // Topic
    _widgetTopic = new QWidget(this);
    QHBoxLayout *topicLayout = new QHBoxLayout(_widgetTopic);
    topicLayout->setMargin(0);
    mainLayout->addWidget(_widgetTopic);
    QLabel *labelTopic = new QLabel(tr("Topic: "));
    topicLayout->addWidget(labelTopic);
    _lineEditTopic = new QLineEdit;
    _lineEditTopic->installEventFilter(this);
    _lineEditTopic->setReadOnly(true);
    QPalette palette = _lineEditTopic->palette();
    palette.setColor(QPalette::Base, Profile::instance().textSkin().backgroundColor());
    _lineEditTopic->setPalette(palette);
    topicLayout->addWidget(_lineEditTopic);
    _widgetTopic->setVisible(Profile::instance().topicWindowVisible);

    // In/Out Splitter
    _splitterOutIn = new QSplitter;
    mainLayout->addWidget(_splitterOutIn);
    _splitterOutIn->setChildrenCollapsible(false);
    _splitterOutIn->setOrientation(Qt::Vertical);
    connect(_splitterOutIn, SIGNAL(splitterMoved(int, int)),
            this, SLOT(splitterInOutMoved(int, int)));

    // Output and Who
    _splitterOutWho = new QSplitter;
    _splitterOutIn->addWidget(_splitterOutWho);
    _splitterOutWho->setOrientation(Qt::Horizontal);
    connect(_splitterOutWho, SIGNAL(splitterMoved(int, int)),
            this, SLOT(splitterOutWhoMoved(int, int)));
    _splitterOutWho->setChildrenCollapsible(false);
    QSizePolicy sizePolicy = _splitterOutWho->sizePolicy();
    sizePolicy.setVerticalStretch(1);
    _splitterOutWho->setSizePolicy(sizePolicy);

    // Main output
    QWidget *outputWidget = new QWidget;
    QVBoxLayout *outputLayout = new QVBoxLayout(outputWidget);
    outputLayout->setMargin(0);
    _splitterOutWho->addWidget(outputWidget);

    _textEditOutput = new MyTextEdit;
    outputLayout->addWidget(_textEditOutput);
    _tokenRenderer.setTextEdit(_textEditOutput);
    _tokenRenderer.setSession(_session);
    _textEditOutput->setAllowFilters(true);
    _textEditOutput->setReadOnly(true);
    _textEditOutput->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    palette =_textEditOutput->palette();
    palette.setColor(QPalette::Base, Profile::instance().textSkin().backgroundColor());
    palette.setColor(QPalette::Inactive, QPalette::Highlight, palette.color(QPalette::Active, QPalette::Highlight));
    _textEditOutput->setPalette(palette);
    sizePolicy = outputWidget->sizePolicy();
    sizePolicy.setHorizontalStretch(1);
    outputWidget->setSizePolicy(sizePolicy);
    connect(_textEditOutput, SIGNAL(myKeyPressed(const QKeyEvent &)),
            this, SLOT(outputKeyPressed(const QKeyEvent &)));
    connect(_textEditOutput, SIGNAL(sendToChat(const QString &)),
            this, SLOT(outputFilterSendToChat(const QString &)));
    _splitterOutIn->addWidget(_splitterOutWho);

    // Search widget
    _searchWidget = new SearchWidget;
    outputLayout->addWidget(_searchWidget);
    connect(_searchWidget, SIGNAL(hideMe()), this, SLOT(hideSearchWidget()));
    _searchWidget->setVisible(false);
    _searchWidget->setTextWidget(_textEditOutput);

    // Who column
    QWidget *whoWidget = new QWidget;
    _splitterOutWho->addWidget(whoWidget);
    QVBoxLayout *whoLayout = new QVBoxLayout(whoWidget);
    whoLayout->setMargin(0);
    _splitterOutWho->addWidget(whoWidget);

    QFrame *whoTitleFrame = new QFrame;
    whoTitleFrame->setFrameShape(QFrame::Box);
    whoTitleFrame->setFrameShadow(QFrame::Plain);
    whoLayout->addWidget(whoTitleFrame);
    _labelWhoTitle = new QLabel(tr("%n user(s)", "", 0));
    _labelWhoTitle->setAlignment(Qt::AlignHCenter);
    QVBoxLayout *whoTitleLayout = new QVBoxLayout(whoTitleFrame);
    whoTitleLayout->setMargin(0);
    whoTitleLayout->addWidget(_labelWhoTitle);

    _treeViewWho = new QTreeView;
    _whoModel = new WhoModel(_session, this);
    _whoSortModel = new WhoSortModel(_session, this);
    _whoSortModel->setSourceModel(_whoModel);
    _whoSortModel->setDynamicSortFilter(true);
    _treeViewWho->setModel(_whoSortModel);
    _treeViewWho->setSortingEnabled(true);
    connect(_whoModel, SIGNAL(modelReset()),
            this, SLOT(whoModelReset()));
    connect(_whoModel, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
            this, SLOT(whoModelRowsInserted(const QModelIndex&, int, int)));
    connect(_whoModel, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
            this, SLOT(whoModelRowsRemoved(const QModelIndex&, int, int)));
    _treeViewWho->installEventFilter(this);
    _treeViewWho->setContextMenuPolicy(Qt::ActionsContextMenu);
    _treeViewWho->header()->setVisible(false);
    _treeViewWho->setSelectionMode(QAbstractItemView::SingleSelection);
    _treeViewWho->setSelectionBehavior(QAbstractItemView::SelectRows);
    _treeViewWho->setRootIsDecorated(false);

    // Who list context menu
    QAction *action = new QAction(tr("finger"), 0);
    connect(action, SIGNAL(triggered()), this, SLOT(finger()));
    _treeViewWho->addAction(action);

    action = new QAction(tr("beep"), 0);
    connect(action, SIGNAL(triggered()), this, SLOT(beep()));
    _treeViewWho->addAction(action);

    action = new QAction(tr("kick"), 0);
    connect(action, SIGNAL(triggered()), this, SLOT(kick()));
    _treeViewWho->addAction(action);

    action = new QAction(tr("initiate a tell session"), 0);
    connect(action, SIGNAL(triggered()), this, SLOT(initiateTellSession()));
    _treeViewWho->addAction(action);

/* WAITING FOR FILE TRANSFER	action = new QAction("send a file...", 0);
   connect(action, SIGNAL(triggered()), this, SLOT(sendAFile()));
   _listWidgetWho->addAction(action);*/

    palette = _treeViewWho->palette();
    palette.setColor(QPalette::Base, Profile::instance().textSkin().backgroundColor());
    _treeViewWho->setPalette(palette);
    _treeViewWho->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    whoLayout->addWidget(_treeViewWho);
    connect(_treeViewWho, SIGNAL(doubleClicked(const QModelIndex&)),
            this, SLOT(whoDoubleClicked(const QModelIndex&)));

    QList<int> list1;
    list1.append(0);
    list1.append(_session->config().whoWidth());
    _splitterOutWho->setSizes(list1);

    // Input widget
    QWidget *inputWidget = new QWidget;
    QHBoxLayout *inputLayout = new QHBoxLayout(inputWidget);
    inputLayout->setMargin(0);
    _splitterOutIn->addWidget(inputWidget);

    // Main input
    _stackedWidgetEntry = new QStackedWidget;
    inputLayout->addWidget(_stackedWidgetEntry);
    _stackedWidgetEntry->setMinimumHeight(20);

    // LineEdit
    _lineEditWidget = new ChatLineWidget;
    palette = _lineEditWidget->palette();
    palette.setColor(QPalette::Base, Profile::instance().textSkin().backgroundColor());
    _lineEditWidget->setPalette(palette);
    connect(_lineEditWidget, SIGNAL(textValidated(const QString &)),
            this, SLOT(sendLineEditText(const QString &)));
    connect(_lineEditWidget, SIGNAL(pageUp()), this, SLOT(historyPageUp()));
    connect(_lineEditWidget, SIGNAL(pageDown()), this, SLOT(historyPageDown()));
    _stackedWidgetEntry->addWidget(_lineEditWidget);

    // HistoryWidget
    _historyWidget = new HistoryWidget;
    palette = _historyWidget->palette();
    palette.setColor(QPalette::Base, Profile::instance().textSkin().backgroundColor());
    _historyWidget->setPalette(palette);
    _historyWidget->setCompletionMode(true);
    connect(_historyWidget, SIGNAL(textValidated(const QString &)),
            this, SLOT(sendText(const QString &)));
    connect(_historyWidget, SIGNAL(pageUp()), this, SLOT(historyPageUp()));
    connect(_historyWidget, SIGNAL(pageDown()), this, SLOT(historyPageDown()));
    _stackedWidgetEntry->addWidget(_historyWidget);

    _stackedWidgetEntry->setCurrentIndex(0);

    // Filter combobox
    _comboBoxFilter = new QComboBox;
    connect(_comboBoxFilter, SIGNAL(activated(int)), this, SLOT(filterActivated(int)));
    inputLayout->addWidget(_comboBoxFilter);

    initScriptComboBox();

    QList<int> list2;
    list2.append(0);
    list2.append(_session->config().entryHeight());
    _splitterOutIn->setSizes(list2);

    refreshFonts();

    _firstShow = true;

    _timerWhoBlinking.setInterval(10);
    connect(&_timerWhoBlinking, SIGNAL(timeout()), this, SLOT(whoBlinking()));
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

    if (obj == _treeViewWho || obj == _lineEditTopic)
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

void ChannelWidget::sendLineEditText(const QString &text)
{
    if (_lineEditWidget->echoMode() == QLineEdit::Password)
        _manualPassword = text; // Password is manually sent
    sendText(text);
}

void ChannelWidget::sendText(const QString &text)
{
    // Filter?
    QString scriptedText = EventScript::newEntry(_session, text);

    if (_comboBoxFilter->currentIndex())
        _session->send(executeLuaFilter(_comboBoxFilter->currentText(), scriptedText));
    else
        _session->send(scriptedText);
}

void ChannelWidget::newToken(const Token &token)
{
    QScrollBar *sb =_textEditOutput->verticalScrollBar();
    bool scrollDown = sb->maximum() - sb->value() < 10;
    switch(token.type())
    {
    case Token::LoginAsked:
        _lineEditWidget->setEchoMode(QLineEdit::Normal);
        break;
    case Token::PasswordAsked:
        _lineEditWidget->setEchoMode(QLineEdit::Password);
        break;
    case Token::Welcome:
        // Put login and password in session config
        if (!_session->config().furtiveMode())
        {
            SessionConfig *config = Profile::instance().getSessionConfigByName(_session->config().name());
            if (config)
            {
                config->setLogin(_session->serverLogin());
                if (_manualPassword != "")
                    config->setPassword(_manualPassword);
            }
        }
        break;
    case Token::Topic:
    case Token::YouSetTopic:
        _lineEditTopic->setText(token.arguments()[2]);
        break;
    case Token::SomeoneSetTopic:
        _lineEditTopic->setText(token.arguments()[3]);
        break;
    case Token::SomeoneTellsYou:
    case Token::YouTellToSomeone:
    case Token::SomeoneAsksYou:
    case Token::YouAskToSomeone:
    case Token::SomeoneReplies:
    case Token::YouReply:
        return;
    case Token::WallBegin:
        if (Profile::instance().tabForWall)
            return;
        break;
    case Token::WallEnd:
        if (Profile::instance().tabForWall)
            return;
        break;
    case Token::WallLine:
        if (Profile::instance().tabForWall)
            return;
        break;
    case Token::WhoBegin:
        _historyWidget->clearCompletionWords();
        if (_whoTicketID == token.ticketID() && token.ticketID() >= 0)
            return;
        if (Profile::instance().tabForWho)
            return;
        break;
    case Token::WhoSeparator:
        if (_whoTicketID == token.ticketID() && token.ticketID() >= 0)
            return;
        if (Profile::instance().tabForWho)
            return;
        break;
    case Token::WhoEnd:
        if (_whoTicketID == token.ticketID() && token.ticketID() >= 0)
            return;
        if (Profile::instance().tabForWho)
            return;
        break;
    case Token::WhoEndNoUser:
        if (_whoTicketID == token.ticketID() && token.ticketID() >= 0)
            return;
        if (Profile::instance().tabForWho)
            return;
        break;
    case Token::WhoLine:
    {
        if (token.arguments()[1].toUpper() != _session->serverLogin().toUpper())
            _historyWidget->addCompletionWord(token.arguments()[1]);
        if (_whoTicketID == token.ticketID() && token.ticketID() >= 0)
            return;
        if (Profile::instance().tabForWho)
            return;
    }
    break;
    case Token::GroupsBegin:
    case Token::GroupsSeparator:
    case Token::GroupsEnd:
    case Token::GroupsLine:
        if (token.ticketID() >= 0)
            return;
        break;
    case Token::FingerBegin:
    case Token::FingerEnd:
    case Token::FingerLine:
        if (Profile::instance().tabForFinger)
            return;
        break;
    case Token::YouAway:
    {
        colorizeChatItems(Profile::instance().textSkin().awayBackgroundColor());
        if (Profile::instance().awaySeparatorLines) // Away separator
           _textEditOutput->addNewLine(Profile::instance().getAwaySeparator(), Profile::instance().textSkin().textFont().font(), Profile::instance().awaySeparatorColor);
       _textEditOutput->isAway = true;
    }
    break;
    case Token::YouBack:
    {
        colorizeChatItems(Profile::instance().textSkin().backgroundColor());
        if (Profile::instance().awaySeparatorLines) // Away separator
           _textEditOutput->addNewLine(Profile::instance().getAwaySeparator(), Profile::instance().textSkin().textFont().font(), Profile::instance().awaySeparatorColor);
       _textEditOutput->isAway = false;
    }
    break;
    case Token::SomeoneAwayWarning:
        return;
    case Token::MessageBegin:
        if (token.ticketID() >= 0)
            return;
        break;
    case Token::MessageLine:
        if (token.ticketID() >= 0)
            return;
        break;
    case Token::MessageEnd:
        if (token.ticketID() >= 0)
            return;
        break;
    case Token::HelpBegin:
        // serverlist?
        if ((_backupServersHelp = (token.arguments()[1].toUpper() == "SERVERLIST")))
            _backupServers.clear();
        if (_helpTicketID == token.ticketID() && token.ticketID() >= 0)
            return;
        break;
    case Token::HelpEndNormal:
        if (_backupServersHelp)
        {
            SessionConfig sessionConfig = _session->config();
            sessionConfig.setBackupServers(_backupServers);
            _session->setConfig(sessionConfig);

            // Record in session configs
            SessionConfig *profileConfig = Profile::instance().getSessionConfig(sessionConfig.name());
            if (profileConfig)
                profileConfig->setBackupServers(_backupServers);
        }
    case Token::HelpEndNoHelp:
        if (_helpTicketID == token.ticketID() && token.ticketID() >= 0)
            return;
        break;
    case Token::HelpLine:
    {
        QString str = token.arguments()[1].trimmed();
        if (_backupServersHelp && str != "")
        {
            QStringList strLst = str.split(":");

            _backupServers << BackupServer(strLst[0], strLst[1].toInt());
        }
    }
    if (_helpTicketID == token.ticketID() && token.ticketID() >= 0)
        return;
    break;
    case Token::SomeoneComesIn:
    {
        _historyWidget->addCompletionWord(token.arguments()[1]);
//        item->setTextColor(Qt::red);
        _userToWhoBlinkTime[token.arguments()[1]] = 5000;
        _userToWhoAscendingOrder[token.arguments()[1]] = false;
        _timerWhoBlinking.start();
    }
    break;
    case Token::SomeoneLeaves:
    case Token::SomeoneDisconnects:
    case Token::SomeoneIsKicked:
    case Token::YouKickSomeone:
        _historyWidget->removeCompletionWord(token.arguments()[1]);
        break;
    case Token::YouJoinChannel:
    case Token::YouLeaveChannel:
        _whoTicketID = _session->requestTicket(TokenFactory::Command_Who);
        _session->sendCommand("who all");
        break;
    case Token::Date:
        if (_dateTicketID == token.ticketID() && token.ticketID() >= 0)
            return;
        break;
    case Token::YourClientIs:
        if (_setClientTicketID == token.ticketID() && token.ticketID() >= 0)
            return;
        break;
    case Token::SomeoneGroup:
    case Token::UnregisteredUser:
        if (token.ticketID() >= 0)
            return;
        break;
    case Token::Data:
/*TEMP		if (TransfersManager::isCommand(token.arguments()[2]))
  return;*/
        break;
    case Token::Unknown:
        break;
    default:;
    };

    QString str = QString::number(sb->value());

    QString timeStamp = token.timeStamp().toString("hh:mm:ss ");
    QString line = token.line();

    bool setTimeStamp = false;

    if (_session->away())
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
    _tokenRenderer.displayToken(token, setTimeStamp);

    // Postfix things
    if (token.type() == Token::IndicatedActiveServer)
       _textEditOutput->addNewLine(tr("Ok, let's trust you =^^=, we move to (%1:%2)").arg(token.arguments()[0]).
                                   arg(token.arguments()[1].trimmed()),
                                   Profile::instance().textSkin().textFont().font(), QColor(255, 0, 0));

    if (scrollDown)
       _textEditOutput->scrollOutputToBottom();
}

void ChannelWidget::colorizeChatItems(const QColor &color)
{
    // Topic
    QPalette palette = _lineEditTopic->palette();
    palette.setColor(QPalette::Base, color);
    _lineEditTopic->setPalette(palette);

    // Output
    palette =_textEditOutput->palette();
    palette.setColor(QPalette::Base, color);
   _textEditOutput->setPalette(palette);

    // Who column
    palette = _treeViewWho->palette();
    palette.setColor(QPalette::Base, color);
    _treeViewWho->setPalette(palette);

    // Input
    palette = _historyWidget->palette();
    palette.setColor(QPalette::Base, color);
    _historyWidget->setPalette(palette);
}

bool ChannelWidget::topicWindowVisible()
{
    return _widgetTopic->isVisible();
}

void ChannelWidget::showTopicWindow()
{
    _widgetTopic->show();
}

void ChannelWidget::hideTopicWindow()
{
    _widgetTopic->hide();
}

void ChannelWidget::sessionConnecting()
{
    _textEditOutput->addNewLine(tr("Attempt to connect %1:%2.........").arg(_session->serverAddress())
                               .arg(_session->serverPort()),
                               Profile::instance().textSkin().textFont().font(),
                               QColor(0, 0, 200));
   _textEditOutput->scrollOutputToBottom();
}

void ChannelWidget::sessionConnected()
{
    colorizeChatItems(Profile::instance().textSkin().backgroundColor());
    _manualPassword = "";
    _stackedWidgetEntry->setCurrentIndex(0);

   _textEditOutput->addString(tr("successful."), Profile::instance().textSkin().textFont().font(),
                              QColor(0, 200, 0));
   _textEditOutput->addNewLine("-", Profile::instance().textSkin().textFont().font(),
                               QColor(200, 0, 0));

   _textEditOutput->scrollOutputToBottom();
}

void ChannelWidget::sessionLogged()
{
    // Send "set client"
    _setClientTicketID = _session->requestTicket(TokenFactory::Command_SetClient);
    _session->sendCommand("set client CeB Alpha " + QString(VERSION));

    // Send first groups command
    _groupsTicketID = _session->requestTicket(TokenFactory::Command_Groups);
    _session->sendCommand("groups");

    // Send first who
    _whoTicketID = _session->requestTicket(TokenFactory::Command_Who);
    _session->sendCommand("who all");

    // Get the server list
    _helpTicketID = _session->requestTicket(TokenFactory::Command_Help);
    _session->sendCommand("help serverlist");

    // Switch entry widget
    _stackedWidgetEntry->setCurrentIndex(1);

    // Start (or not) the keep alive timer
    if (Profile::instance().keepAlive)
    {
        _timerKeepAlive.start();
        _timerKeepAlive.setInterval(Profile::instance().keepAlive * 1000);
    }

    _regExpAboutMe = QRegExp("(^|\\W)" + _session->serverLogin() + "(\\W|$)", Qt::CaseInsensitive);
}

void ChannelWidget::sessionDisconnected()
{
    // Stop the keep alive timer
    if (_timerKeepAlive.isActive())
        _timerKeepAlive.stop();
//    _tableWidgetWho->clear();
    colorizeChatItems(Profile::instance().textSkin().awayBackgroundColor());
    _labelWhoTitle->setText("");
}

void ChannelWidget::sessionSocketError(const QString &errorStr)
{
   _textEditOutput->addNewLine(errorStr,
                               Profile::instance().textSkin().textFont().font(),
                               QColor(200, 0, 0));

   _textEditOutput->scrollOutputToBottom();
}

void ChannelWidget::outputKeyPressed(const QKeyEvent &e)
{
    if (e.key() == Qt::Key_Escape ||
        e.key() == Qt::Key_F11 ||
        e.key() == Qt::Key_F12)
        return;

    QKeyEvent event(QEvent::KeyPress, e.key(), e.modifiers(), e.text(), e.isAutoRepeat(), e.count());
    if (_stackedWidgetEntry->currentIndex() == 0)
    {
        QApplication::sendEvent(_lineEditWidget, &event);
        _lineEditWidget->setFocus();
    }
    else
    {
        QApplication::sendEvent(_historyWidget, &event);
        _historyWidget->setFocus();
    }
}

void ChannelWidget::splitterInOutMoved(int, int)
{
    SessionConfig *config = Profile::instance().getSessionConfig(_session->config().name());
    if (config)
        config->setEntryHeight(_stackedWidgetEntry->height());
}

void ChannelWidget::splitterOutWhoMoved(int, int)
{
    SessionConfig *config = Profile::instance().getSessionConfig(_session->config().name());
    if (config)
        config->setWhoWidth(_treeViewWho->width());
}

void ChannelWidget::historyPageUp()
{
    // Send the event to the output text
    QKeyEvent event(QEvent::KeyPress, Qt::Key_PageUp, Qt::NoModifier);
    QApplication::sendEvent(_textEditOutput, &event);
}

void ChannelWidget::historyPageDown()
{
    // Send the event to the output text
    QKeyEvent event(QEvent::KeyPress, Qt::Key_PageDown, Qt::NoModifier);
    QApplication::sendEvent(_textEditOutput, &event);
}

void ChannelWidget::keepAliveTimeout()
{
    if (_session && _session->isLogged())
    {
        _dateTicketID = _session->requestTicket(TokenFactory::Command_Date);
        _session->sendCommand("date", false);
    }
}

void ChannelWidget::refreshKeepAlivePolicy()
{
    if (!Profile::instance().keepAlive)
    {
        if (_timerKeepAlive.isActive())
            _timerKeepAlive.stop();
    }
    else
    {
        if (!_timerKeepAlive.isActive())
        {
            _timerKeepAlive.setInterval(Profile::instance().keepAlive * 1000);
            if (_session && _session->isLogged())
                _timerKeepAlive.start();
        }
        else if (_timerKeepAlive.interval() != Profile::instance().keepAlive * 1000)
        {
            _timerKeepAlive.stop();
            _timerKeepAlive.setInterval(Profile::instance().keepAlive * 1000);
            _timerKeepAlive.start();
        }
    }
}

void ChannelWidget::refreshWhoColumn()
{
    _whoTicketID = _session->requestTicket(TokenFactory::Command_Who);
    _session->sendCommand("who all");
}

void ChannelWidget::refreshFonts()
{
    const TextSkin &textSkin = Profile::instance().textSkin();
    QPalette palette;
    _lineEditTopic->setFont(textSkin.topicTextFont().font());
    palette = _lineEditTopic->palette();
    palette.setColor(QPalette::Text, textSkin.topicTextFont().color());
    _lineEditTopic->setPalette(palette);

    _lineEditWidget->setFont(textSkin.inputTextFont().font());
    palette = _lineEditWidget->palette();
    palette.setColor(QPalette::Text, textSkin.inputTextFont().color());
    _lineEditWidget->setPalette(palette);

    _historyWidget->setFont(textSkin.inputTextFont().font());
    palette = _historyWidget->palette();
    palette.setColor(QPalette::Text, textSkin.inputTextFont().color());
    _historyWidget->setPalette(palette);

    _treeViewWho->setFont(textSkin.whoTextFont().font());
    palette = _treeViewWho->palette();
    palette.setColor(QPalette::Text, textSkin.whoTextFont().color());
    _treeViewWho->setPalette(palette);
}

void ChannelWidget::whoDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    QModelIndex sourceModelIndex = _whoSortModel->mapToSource(index);
    const WhoUser &user = _session->whoPopulation().users()[sourceModelIndex.row()];
    emit whoUserDoubleClicked(user.login());
}

void ChannelWidget::loginChanged(const QString &oldLogin, const QString &newLogin)
{
    _regExpAboutMe = QRegExp("(^|\\W)" + newLogin + "(\\W|$)", Qt::CaseInsensitive);
}

void ChannelWidget::finger()
{
    QModelIndex index = _treeViewWho->selectionModel()->currentIndex();
    if (!index.isValid())
        return;

    QModelIndex sourceModelIndex = _whoSortModel->mapToSource(index);
    _session->sendCommand("finger " + _session->whoPopulation().users()[sourceModelIndex.row()].login());
}

void ChannelWidget::beep()
{
    QModelIndex index = _treeViewWho->selectionModel()->currentIndex();
    if (!index.isValid())
        return;

    QModelIndex sourceModelIndex = _whoSortModel->mapToSource(index);
    _session->sendCommand("beep " + _session->whoPopulation().users()[sourceModelIndex.row()].login());
}

void ChannelWidget::kick()
{
    QModelIndex index = _treeViewWho->selectionModel()->currentIndex();
    if (!index.isValid())
        return;

    QModelIndex sourceModelIndex = _whoSortModel->mapToSource(index);
    _session->sendCommand("kick " + _session->whoPopulation().users()[sourceModelIndex.row()].login());
}

void ChannelWidget::initiateTellSession()
{
    QModelIndex index = _treeViewWho->selectionModel()->currentIndex();
    if (!index.isValid())
        return;

    QModelIndex sourceModelIndex = _whoSortModel->mapToSource(index);
    emit tellSessionAsked(_session->whoPopulation().users()[sourceModelIndex.row()].login());
}

void ChannelWidget::sendAFile()
{
    QModelIndex index = _treeViewWho->selectionModel()->currentIndex();
    if (!index.isValid())
        return;

    QModelIndex sourceModelIndex = _whoSortModel->mapToSource(index);

    // User which we want to send a file
    QString nick = _session->whoPopulation().users()[sourceModelIndex.row()].login();

    // Pick a file
    QString fileName = QFileDialog::getOpenFileName(this);
    if (fileName == "")
        return;

    TransfersManager::instance().propose(_session, nick, fileName);
}

void ChannelWidget::applyFirstShow()
{
    if (_firstShow)
    {
        _firstShow = false;
        _textEditOutput->scrollOutputToBottom();
        if (_stackedWidgetEntry->currentWidget() == _lineEditWidget)
            _lineEditWidget->setFocus();
        else
            _historyWidget->setFocus();
    }
}

QString ChannelWidget::widgetCaption() const
{
    return _session->channel();
}

void ChannelWidget::initScriptComboBox()
{
    _comboBoxFilter->clear();
    _comboBoxFilter->addItem(tr("<no filter>"));
    QDir scriptsDir(QDir(Paths::sharePath()).filePath("scripts"));

    QStringList nameFilters;
    nameFilters << "*.lua";
    foreach (QFileInfo fileInfo, scriptsDir.entryInfoList(nameFilters, QDir::Files))
        _comboBoxFilter->addItem(fileInfo.baseName());
}

void ChannelWidget::filterActivated(int)
{
    if (_stackedWidgetEntry->currentWidget() == _lineEditWidget)
        _lineEditWidget->setFocus();
    else
        _historyWidget->setFocus();
}

void ChannelWidget::outputFilterSendToChat(const QString &text)
{
    _session->send(text);
}

void ChannelWidget::whoBlinking()
{
    bool stopAll = true;
    foreach (const QString &user, _userToWhoBlinkTime.keys())
    {
        int blinkTime = _userToWhoBlinkTime[user];
        if (blinkTime > 0)
        {
            stopAll = false;
            // It remains time to blink
            _userToWhoBlinkTime[user] = blinkTime - _timerWhoBlinking.interval();
/*            QTableWidgetItem *item = getWhoItemByNickname(user);
            if (item)
            {
                QColor currentColor = item->textColor();
                int red = currentColor.red();
                if (_userToWhoAscendingOrder[user])
                {
                    if (red < 255)
                    {
                        if (red + 10 <= 255)
                            currentColor.setRed(red + 10);
                        else
                            currentColor.setRed(255);
                    }
                    else
                        _userToWhoAscendingOrder[user] = false;
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
                        _userToWhoAscendingOrder[user] = true;
                }
                item->setTextColor(currentColor);
                }

            if (_userToWhoBlinkTime[user] <= 0 && item)
            item->setForeground(item->tableWidget()->palette().color(QPalette::WindowText));*/
        }
    }
    if (stopAll)
        _timerWhoBlinking.stop();
}

void ChannelWidget::toggleSearchWidgetVisibility()
{
    _searchWidget->setVisible(!_searchWidget->isVisible());

    if (_searchWidget->isVisible())
        _searchWidget->afterShow();
    else
    {
        if (_stackedWidgetEntry->currentIndex() == 0)
            _lineEditWidget->setFocus();
        else
            _historyWidget->setFocus();
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

void ChannelWidget::sessionCleared()
{
    _textEditOutput->clear();
}

void ChannelWidget::whoModelRowsInserted(const QModelIndex &, int, int)
{
    refreshWhoLabel();
    _treeViewWho->resizeColumnToContents(0);
}

void ChannelWidget::whoModelRowsRemoved(const QModelIndex &, int, int)
{
    refreshWhoLabel();
    _treeViewWho->resizeColumnToContents(0);
}

void ChannelWidget::whoModelReset()
{
    refreshWhoLabel();
    _treeViewWho->resizeColumnToContents(0);
}

void ChannelWidget::refreshWhoLabel()
{
    _labelWhoTitle->setText(tr("%n user(s)", "", _whoModel->rowCount()));
}
