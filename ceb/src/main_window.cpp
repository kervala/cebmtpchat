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

#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QToolBar>
#include <QMenuBar>
#include <QLabel>
#include <QTabWidget>
#include <QMessageBox>
#include <QCloseEvent>
#include <QWindowStateChangeEvent>
#include <QDir>
#include <QSound>
#include <QDesktopServices>

#ifdef Q_OS_WIN32
#include <windows.h>
#endif

#include "version.h"
#include "dialog_settings.h"
#include "dialog_about.h"
#include "dialog_session_config.h"
#include "session_manager.h"
#include "dialog_update.h"
#include "dialog_warningo.h"
#include "dialog_whatsnew.h"
#include "profile.h"
#include "logger.h"
#include "paths.h"
#include "event_script.h"

#include "main_window.h"
#include "my_textedit.h"

MainWindow *MainWindow::_instance = 0;

MainWindow *MainWindow::instance()
{
    if (!_instance)
        _instance = new MainWindow;

    return _instance;
}

MainWindow::MainWindow()
{
    setWindowTitle("CeB");

    trayMenu = new QMenu(this);
    trayIcon = new QSystemTrayIcon(QIcon(":/images/tray-neutral.png"), this);
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
    if (Profile::instance().trayEnabled && Profile::instance().trayAlwaysVisible)
        trayIcon->show();

    SessionManager &sessionManager = SessionManager::instance();
    connect(&sessionManager, SIGNAL(newSessionToken(Session *, const Token &)),
            this, SLOT(newSessionTokenForActivity(Session *, const Token &)));
    connect(&sessionManager, SIGNAL(newSessionToken(Session *, const Token &)),
            this, SLOT(newSessionToken(Session *, const Token &)));
    connect(&sessionManager, SIGNAL(sessionLoginChanged(Session *, const QString &, const QString &)),
            this, SLOT(sessionLoginChanged(Session *, const QString &, const QString &)));

    TransfersManager &transfersManager = TransfersManager::instance();
    connect(&transfersManager, SIGNAL(newTransferAdded(Transfer *)),
            this, SLOT(newTransfer(Transfer *)));

    layout()->setMargin(0);

    makeMenuBar();
    makeToolBar();

    if (Profile::instance().mainWidth != -1)
    {
        resize(Profile::instance().mainWidth, Profile::instance().mainHeight);
        move(Profile::instance().mainLeft, Profile::instance().mainTop);
    } else
        resize(640, 480);

    statusBar()->setVisible(Profile::instance().statusBarVisible);
    menuBar()->setVisible(Profile::instance().menuBarVisible);

    // Create and init multitabwidget
    tabWidgetMain = new MyTabWidget;
    connect(tabWidgetMain, SIGNAL(currentChanged(int)), this, SLOT(tabWidgetMainCurrentChanged(int)));
    tabWidgetMain->installEventFilter(this);
    setCentralWidget(tabWidgetMain);

    applyProfileOnTabWidget();

    // Create system dialog
    if (Profile::instance().systemLogsVisible)
        tabWidgetMain->addTab(SystemWidget::instance(), tr("system"));

    // Autoconnect connections
    foreach (SessionConfig *config, Profile::instance().sessionConfigs())
        if ((*config).autoconnect())
            connectTo(*config);

    if (Profile::instance().systemLogsVisible && tabWidgetMain->count() > 1)
        tabWidgetMain->setCurrentIndex(1);

    connectionsSignalMapper = new QSignalMapper(this);
    connect(connectionsSignalMapper, SIGNAL(mapped(const QString &)),
            this, SLOT(connectToFromMenu(const QString &)));

    // Set focus
    ChannelWidget *channelWidget = qobject_cast<ChannelWidget*>(tabWidgetMain->currentWidget());
    if (channelWidget)
        channelWidget->applyFirstShow();

    // Start autoupdate stuff
    connect(&autoUpdate, SIGNAL(newVersion(const QDate &)), this, SLOT(newProgramVersion(const QDate &)));
    autoUpdate.checkForUpdate();

    animationTimer.setInterval(10);
/*    connect(&animationTimer, SIGNAL(timeout()), this, SLOT(animationTimeout()));
      animationTimer.start();*/

    m_firstShow = true;

    actionSignalMapper = new QSignalMapper(this);
    connect(actionSignalMapper, SIGNAL(mapped(int)),
            this, SLOT(executeAction(int)));

    createActionShortcuts();
}

MainWindow::~MainWindow()
{
    // Free some singletons
    SessionManager::free();
}

void MainWindow::createActionShortcuts()
{
    qDeleteAll(actionShortcuts);
    actionShortcuts.clear();

    for (int i = 0; i < Profile::instance().actionManager.actions().count(); ++i)
    {
        const Action &action = Profile::instance().actionManager.actions()[i];
        QShortcut *shortcut = new QShortcut(action.keySequence(), this);
        actionShortcuts << shortcut;
        connect(shortcut, SIGNAL(activated()), actionSignalMapper, SLOT(map()));
        actionSignalMapper->setMapping(shortcut, action.actionType());
    }
}

QWidget *MainWindow::getTab(Session *session, const QString &category, const QString &argument)
{
    if (!category.compare("tell", Qt::CaseInsensitive))
        return getTellWidget(session, argument);
    else if (!category.compare("channel", Qt::CaseInsensitive))
        return getChannelWidget(session);
    return 0;
}

QColor MainWindow::getTabColor(QWidget *widget) const
{
    int index = tabWidgetMain->indexOf(widget);
    if (index < 0)
        return QColor();

    return tabWidgetMain->tabTextColor(index);
}

void MainWindow::setTabColor(QWidget *widget, const QColor &color)
{
    int index = tabWidgetMain->indexOf(widget);
    if (index < 0)
        return;

    tabWidgetMain->setTabTextColor(index, color);
}

bool MainWindow::isTabFocused(QWidget *widget) const
{
    return widget == tabWidgetMain->currentWidget();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // All finalization stuff must be done here
    recordCurrentProfileDatas();

    // Save current profile
    Profile::instance().save();

    // Hide tray
    trayIcon->hide();

    // Close all sessions
    SessionManager::instance().free();

    event->accept();
}

void MainWindow::recordCurrentProfileDatas()
{
    // Pos and size
    QPoint p = pos();
    QSize s = size();
    Profile::instance().mainWidth = s.width();
    Profile::instance().mainHeight = s.height();
    Profile::instance().mainLeft = p.x();
    Profile::instance().mainTop = p.y();

    // Bars
    Profile::instance().statusBarVisible = statusBar()->isVisible();
    Profile::instance().menuBarVisible = menuBar()->isVisible();

    // System logs
    Profile::instance().systemLogsVisible = tabWidgetMain->indexOf(SystemWidget::instance()) >= 0;
}

void MainWindow::makeMenuBar()
{
    mbMain = new MyMenuBar;
    setMenuBar(mbMain);
    connect(mbMain, SIGNAL(iconClicked()), this, SLOT(menuIconClicked()));

    // Connections
    QMenu *mConnections = mbMain->addMenu(tr("&Connections"));
    mConnectTo = mConnections->addMenu(tr("&Connect to"));
    connect(mConnectTo, SIGNAL(aboutToShow()), this, SLOT(aboutToShowConnectionsActions()));

    // Reconnect action
    actionReconnect = mConnections->addAction(tr("&Reconnect"));
    actionReconnect->setStatusTip(tr("Reconnect on the current connection"));
    actionReconnect->setEnabled(false);
    connect(actionReconnect, SIGNAL(triggered()), this, SLOT(reconnect()));

    // Close connection action
    actionCloseConnection = mConnections->addAction(tr("C&lose connection"));
    actionCloseConnection->setStatusTip(tr("Close current connection"));
    actionCloseConnection->setEnabled(false);
    connect(actionCloseConnection, SIGNAL(triggered()), this, SLOT(closeConnection()));

    mConnections->addSeparator();
    QAction *action = mConnections->addAction(tr("&Quit"));
    connect(action, SIGNAL(triggered()), this, SLOT(close()));

    // Edit menu
    QMenu *menuEdit = mbMain->addMenu(tr("&Edit"));
    connect(menuEdit, SIGNAL(aboutToShow()), this, SLOT(aboutToShowEditMenu()));
    QAction *actionSearch = menuEdit->addAction(tr("&Search"));
    connect(actionSearch, SIGNAL(triggered()), this, SLOT(searchActionTriggered()));
    menuEdit->addSeparator();
    _actionToggleTopicVisibility = menuEdit->addAction("");
    connect(_actionToggleTopicVisibility, SIGNAL(triggered()), this, SLOT(toggleTopicVisibility()));
    _actionToggleUsersVisibility = menuEdit->addAction("");
    connect(_actionToggleUsersVisibility, SIGNAL(triggered()), this, SLOT(toggleUsersVisibility()));

    // Configuration menu
    QMenu *menuConfiguration = mbMain->addMenu(tr("Confi&guration"));
    connect(menuConfiguration, SIGNAL(aboutToShow()), this, SLOT(aboutToShowConfigurationMenu()));
    _actionToggleMenuBarVisibility = menuConfiguration->addAction("");
    _actionToggleMenuBarVisibility->setShortcutContext(Qt::WidgetShortcut);
    connect(_actionToggleMenuBarVisibility, SIGNAL(triggered()), this, SLOT(toggleMenuBarVisibility()));
    _actionToggleStatusBarVisibility = menuConfiguration->addAction("");
    _actionToggleStatusBarVisibility->setShortcutContext(Qt::WidgetShortcut);
    connect(_actionToggleStatusBarVisibility, SIGNAL(triggered()), this, SLOT(toggleStatusBarVisibility()));
    menuConfiguration->addSeparator();
    actionEditConnectionConfig = menuConfiguration->addAction(tr("&Connection configuration..."));
    actionEditConnectionConfig->setEnabled(false);
    connect(actionEditConnectionConfig, SIGNAL(triggered()), this, SLOT(editConnectionConfig()));
    connect(menuConfiguration->addAction(tr("&General settings...")), SIGNAL(triggered()), this, SLOT(editSettings()));

    // Windows menu
    QMenu *menuWindows = mbMain->addMenu(tr("&Windows"));
    connect(menuWindows, SIGNAL(aboutToShow()), this, SLOT(aboutToShowWindowsMenu()));
    QAction *actionPreviousTab = menuWindows->addAction(tr("&Previous tab"));
    connect(actionPreviousTab, SIGNAL(triggered()), this, SLOT(previousTab()));
    QAction *actionNextTab = menuWindows->addAction(tr("&Next tab"));
    connect(actionNextTab, SIGNAL(triggered()), this, SLOT(nextTab()));
    menuWindows->addSeparator();
    _actionToggleSystemLogsVisibility = menuWindows->addAction(tr(""));
    connect(_actionToggleSystemLogsVisibility, SIGNAL(triggered()), this, SLOT(showSystemLogs()));

    // View menu
    QMenu *menuView = mbMain->addMenu(tr("&View"));

    connect(menuView->addAction(tr("Open &logs directory")), SIGNAL(triggered()), this, SLOT(showLogsDir()));
    connect(menuView->addAction(tr("&Messages")), SIGNAL(triggered()), this, SLOT(showMessages()));
// TEMP	connect(menuView->addAction(tr("&File transfers")), SIGNAL(triggered()), this, SLOT(showTransfers()));

    // Help menu
    QMenu *menuHelp = mbMain->addMenu(tr("&Help"));
#ifdef Q_OS_WIN32
    connect(actionCheckForUpdate = menuHelp->addAction(tr("&Check for update...")), SIGNAL(triggered()), this, SLOT(checkForUpdate()));
	actionCheckForUpdate->setVisible(Profile::instance().checkForUpdate);
#endif
    connect(menuHelp->addAction(tr("&What's new?")), SIGNAL(triggered()), this, SLOT(launchWhatsNew()));
    menuHelp->addSeparator();
    connect(menuHelp->addAction(tr("&Report a bug")), SIGNAL(triggered()), this, SLOT(launchBugReport()));
    connect(menuHelp->addAction(tr("&Report a wanted feature")), SIGNAL(triggered()), this, SLOT(launchFeatureReport()));
    menuHelp->addSeparator();
    connect(menuHelp->addAction(tr("&About...")), SIGNAL(triggered()), this, SLOT(about()));
    connect(menuHelp->addAction(tr("About &Qt...")), SIGNAL(triggered()), this, SLOT(aboutQt()));
}

void MainWindow::makeToolBar()
{
    // TODO: populate
}

#if defined(Q_OS_WIN32)

bool MainWindow::winEvent(MSG *message, long *result)
{
    if (message->message == WM_SYSCOMMAND && message->wParam == SC_MINIMIZE)
    {
        if (Profile::instance().trayEnabled && Profile::instance().trayHideFromTaskBar)
            hide();
        if (Profile::instance().trayEnabled && !Profile::instance().trayAlwaysVisible)
            trayIcon->show();
        trayTalkAboutMe = false;
        if (Profile::instance().trayEnabled && Profile::instance().trayHideFromTaskBar)
        {
            (*result) = false;
            return true;
        }
    }
    return QMainWindow::winEvent(message, result);
}

#endif

void MainWindow::makeConnectionsActions()
{
    mConnectTo->clear();

    foreach (SessionConfig *config, Profile::instance().sessionConfigs())
    {
        QString caption = config->name() + " (" + config->address() + ")";
        QAction *action = mConnectTo->addAction(caption);
        action->setStatusTip(config->address() + ":" + QString::number(config->port()));
        connectionsSignalMapper->setMapping(action, config->name());
        connect(action, SIGNAL(triggered()), connectionsSignalMapper, SLOT(map()));
    }

    // New connection action
    if (mConnectTo->actions().count() > 0)
        mConnectTo->addSeparator();
    QAction *actionNewConnection = mConnectTo->addAction(tr("&New connection..."));
    actionNewConnection->setStatusTip(tr("Create a new connection"));
    connect(actionNewConnection, SIGNAL(triggered()), this, SLOT(newConnection()));
}

void MainWindow::newConnection()
{
    SessionConfig templateConfig = SessionConfig::getTemplate();
    templateConfig.setName(Profile::instance().getUniqSessionConfigName());
    DialogSessionConfig dialogSessionConfig(templateConfig, this);
    if (dialogSessionConfig.exec() == QDialog::Accepted)
    {
        SessionConfig newConfig;
        dialogSessionConfig.get(newConfig);
        Profile::instance().addSessionConfig(newConfig);

        Profile::instance().save();

        // Connect to
        connectTo(newConfig);
    }
}

void MainWindow::reconnect()
{
    // Get current session config
    Session *session = getCurrentSession();
    if (session)
    {
        if (session->isConnected() &&
            QMessageBox::question(this, tr("Confirmation"), tr("You seem to be already connected, do you really want to force a reconnection?"),
                                  QMessageBox::Yes | QMessageBox::Cancel) != QMessageBox::Yes)
            return;

        session->resetBackupServers();
        session->start();
    }
}

void MainWindow::closeConnection()
{
    closeCurrentSession();
}

void MainWindow::editConnectionConfig()
{
    SessionConfig *pConfig =
        Profile::instance().getSessionConfig(getCurrentSession()->config().name());

    DialogSessionConfig dialogSessionConfig(*pConfig, this);
    if (dialogSessionConfig.exec() == QDialog::Accepted)
    {
        QString oldSessionConfigName = pConfig->name();
        dialogSessionConfig.get(*pConfig);
        getCurrentSession()->setConfig(*pConfig);

        Profile::instance().save();

        // Change super label
// TODO        mtwMain->renameSuperLabel(oldSessionConfigName, pConfig->name());
    }
}

void MainWindow::editSettings()
{
    DialogSettings dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        // Save profile
        Profile::instance().save();

        refreshProfileSettings();
    }
}

void MainWindow::showSystemLogs()
{
    int index = tabWidgetMain->indexOf(SystemWidget::instance());
    if (index >= 0)
    {
        tabWidgetMain->removeTab(index);
        QWidget *widget = tabWidgetMain->currentWidget();
        if (widget)
            widget->focusWidget()->setFocus();
    }
    else
    {
        tabWidgetMain->insertTab(0, SystemWidget::instance(), tr("system"));
        tabWidgetMain->setCurrentIndex(0);
    }
}

void MainWindow::about()
{
    DialogAbout dialog(this);
    dialog.exec();
}

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this);
}

ChannelWidget *MainWindow::connectTo(SessionConfig &config)
{
    // Existing config?
    ChannelWidget *channelWidget = getChannelWidget(config);
    if (channelWidget)
    {
        tabWidgetMain->setCurrentWidget(channelWidget);
        channelWidget->session()->start();
        return 0;
    }

    // Create a session
    Session *session = SessionManager::instance().newSession(config);

    // Create a new ChannelWidget and add it
    channelWidget = new ChannelWidget(session);
    connect(channelWidget, SIGNAL(whoUserDoubleClicked(const QString&)),
            this, SLOT(whoUserDoubleClicked(const QString&)));
    connect(channelWidget, SIGNAL(tellSessionAsked(const QString&)),
            this, SLOT(tellSessionAsked(const QString&)));
    connect(channelWidget, SIGNAL(showFileTransfers()), this, SLOT(showFileTransfers()));
    connect(channelWidget, SIGNAL(captionChanged()), this, SLOT(captionChanged()));

    tabWidgetMain->addTab(channelWidget, channelWidget->caption());

    // Start the session
    session->start();

    // Menu actions
    actionCloseConnection->setEnabled(true);
    actionReconnect->setEnabled(true);
    actionEditConnectionConfig->setEnabled(true);

    return channelWidget;
}

void MainWindow::connectTo(const QString &configName)
{
    connectTo(*Profile::instance().getSessionConfig(configName));
}

void MainWindow::connectToFromMenu(const QString &configName)
{
    ChannelWidget *widget = connectTo(*Profile::instance().getSessionConfig(configName));
    if (widget)
        widget->applyFirstShow();
}

void MainWindow::refreshProfileSettings()
{
    // Actions shortcut stuffs
    createActionShortcuts();

    // Tray stuffs
    if (Profile::instance().trayEnabled)
    {
        if (Profile::instance().trayHideFromTaskBar && isMinimized())
            hide();
        else if (!Profile::instance().trayHideFromTaskBar && !isVisible())
            trayIcon->show();
        else
        {
            if (Profile::instance().trayAlwaysVisible)
                trayIcon->show();
            else
                trayIcon->hide();
        }
    }
    else
    {
        show();
        trayIcon->hide();
    }

    // Update stuffs
#ifdef Q_OS_WIN32
    mbMain->update();
    actionCheckForUpdate->setVisible(Profile::instance().checkForUpdate);
#endif // Q_OS_WIN32

    // Tabs stuffs
    applyProfileOnTabWidget();

    // Refresh widgets
    refreshWidgets();
}

void MainWindow::refreshWidgets()
{
    for (int i = 0; i < tabWidgetMain->count(); ++i)
    {
        ChannelWidget *channelWidget = qobject_cast<ChannelWidget*>(tabWidgetMain->widget(i));
        if (channelWidget)
        {
            channelWidget->refreshKeepAlivePolicy();
            channelWidget->refreshFonts();
        }
        else
        {
            TellWidget *tellWidget = qobject_cast<TellWidget*>(tabWidgetMain->widget(i));
            if (tellWidget)
                tellWidget->refreshFonts();
        }
    }
}

void MainWindow::aboutToShowConnectionsActions()
{
    makeConnectionsActions();
}

void MainWindow::newSessionTokenForActivity(Session *session, const Token &token)
{
    if (token.ticketID() >= 0)
        return;

    if ((Profile::instance().trayHideFromTaskBar && !isVisible()) || QApplication::activeWindow() != this)
    {
        bool showWarningo = false;
        bool changeTray = false;
        switch(token.type())
        {
        case Token::SomeoneTellsYou:
        case Token::SomeoneAsksYou:
        case Token::SomeoneReplies:
            showWarningo = Profile::instance().warningoPrivate;
            changeTray = true;
            trayTalkAboutMe = true;
            break;
        case Token::YouAreKicked:
            showWarningo = true;
            changeTray = true;
            trayTalkAboutMe = true;
            break;
        case Token::SomeoneSays:
        {
            // Someone talks about you?
            QString sentence = token.arguments()[2];
            if (sentence.indexOf(session->regExpAboutMe()) >= 0)
            {
                showWarningo = Profile::instance().warningoHighlight;
                changeTray = true;
                trayTalkAboutMe = true;
            }
        }
        break;
        case Token::SomeoneComesIn:
        case Token::SomeoneLeaves:
        case Token::SomeoneDisconnects:
        case Token::YouLeave:
        case Token::YouJoinChannel:
        case Token::YouLeaveChannel:
        case Token::SomeoneJoinChannel:
        case Token::SomeoneFadesIntoTheShadows:
        case Token::SomeoneLeaveChannel:
        case Token::SomeoneAppearsFromTheShadows: // No Tray, No Warningo
            return;
        default:;
        }

        if (showWarningo || changeTray)
        {
            if (changeTray)
                trayIcon->setIcon(QIcon(":/images/tray-myself.png"));
            if (showWarningo && Profile::instance().warningoEnabled)
				(new DialogWarningo(session->config().name(), token.line(), this))->show();
        }
        else if (!trayTalkAboutMe)
            trayIcon->setIcon(QPixmap(":/images/tray-new.png"));
    }
}

void MainWindow::newSessionToken(Session *session, const Token &token)
{
    switch(token.type())
    {
    case Token::LoginAsked:
        if (!session->config().furtiveMode() && !session->config().login().isEmpty())
            session->send(session->config().login());
        break;
    case Token::InvalidLogin:
    case Token::OnlyRegisteredUsers:
    {
        SessionConfig newConfig = session->config();
        newConfig.setLogin("");
        newConfig.setPassword("");
        session->setConfig(newConfig);
    }
    break;
    case Token::PasswordAsked:
        if (!session->config().furtiveMode() && !session->config().password().isEmpty())
            session->send(session->config().password());
        break;
    case Token::IncorrectPassword:
    {
        SessionConfig newConfig = session->config();
        newConfig.setLogin("");
        newConfig.setPassword("");
        session->setConfig(newConfig);
    }
    break;
    case Token::SomeoneTellsYou:
    case Token::SomeoneAsksYou:
    case Token::SomeoneReplies:
        if (Profile::instance().soundAboutMeEnabled)
        {
            QSound s(Profile::instance().getAboutMeFileName());
            s.play();
        }
    case Token::YouTellToSomeone:
    case Token::YouAskToSomeone:
    case Token::YouReply:
    {
        QString login = token.arguments()[1];
        TellWidget *widget = getTellWidget(session, login);
        if (!widget)
        {
            widget = newTellWidget(session, login);
            widget->newTokenFromSession(token);
        }
    }
    break;
    case Token::SomeoneBeepsYou:
        if (Profile::instance().soundBeepEnabled)
        {
            QSound s(Profile::instance().getBeepFileName());
            s.play();
        }
        break;
    case Token::WallBegin:
        // New tab?
        if (token.ticketID() == -1 && Profile::instance().tabForWall)
        {
            CmdOutputWidget *widget = getCmdOutputWidget(session, "wall");
            if (!widget)
            {
                widget = newCmdOutputWidget(session, "wall");
                widget->newTokenFromSession(token);
            }
            tabWidgetMain->setCurrentWidget(widget);
        }
        break;
    case Token::FingerBegin:
        // New tab?
        if (token.ticketID() == -1 && Profile::instance().tabForFinger)
        {
            CmdOutputWidget *widget = getCmdOutputWidget(session, "finger");
            if (!widget)
            {
                widget = newCmdOutputWidget(session, "finger");
                widget->newTokenFromSession(token);
            }
            tabWidgetMain->setCurrentWidget(widget);
        }
        break;
    case Token::WhoBegin:
        // New tab?
        if (token.ticketID() == -1 && Profile::instance().tabForWho)
        {
            CmdOutputWidget *widget = getCmdOutputWidget(session, "who");
            if (!widget)
            {
                widget = newCmdOutputWidget(session, "who");
                widget->newTokenFromSession(token);
            }
            tabWidgetMain->setCurrentWidget(widget);
        }
        break;
    case Token::WhoLine:
    {
        QString login = token.arguments()[1];
        TellWidget *tellWidget = getTellWidget(session, login);
        if (tellWidget)
        {
            if (!token.arguments()[4].compare("*Away*", Qt::CaseInsensitive))
                renameWidget(tellWidget, login + " " + tr("(away)"));
            else
                renameWidget(tellWidget, login);
        }
    }
    break;
    case Token::WhoEnd:
        // Check for tell tabs (MAYBE NOT NEEDED ANYMORE)
        for (int i = 0; i < tabWidgetMain->count(); ++i)
        {
            TellWidget *tellWidget = qobject_cast<TellWidget*>(tabWidgetMain->widget(i));
            if (tellWidget && tellWidget->session() == session)
            {
                QString tellLogin = tellWidget->login();
                if (!session->whoPopulation().userForLogin(tellLogin).isValid())
                    renameWidget(tellWidget);
            }
        }
        break;
    case Token::UserLoginRenamed:
        sessionLoginChanged(session, token.arguments()[1], token.arguments()[2]);
        break;
    case Token::SomeoneSays:
    {
        // Someone talks about you?
        QString sentence = token.arguments()[2];
        if (sentence.indexOf(session->regExpAboutMe()) >= 0)
        {
            if (Profile::instance().soundAboutMeEnabled)
            {
                QSound s(Profile::instance().getAboutMeFileName());
                s.play();
            }
        }
    }
    break;
    default:;
    }
}

void MainWindow::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason != QSystemTrayIcon::Trigger)
        return;

    if (Profile::instance().trayEnabled && Profile::instance().trayHideFromTaskBar)
        show();
    if (isMinimized())
        showNormal();
    activateWindow();
    if (Profile::instance().trayEnabled && !Profile::instance().trayAlwaysVisible)
        trayIcon->hide();
}

bool MainWindow::event(QEvent *e)
{
    if (e->type() == QEvent::WindowActivate)
    {
        trayIcon->setIcon(QIcon(":/images/tray-neutral.png"));
        trayTalkAboutMe = false;

        EventScript::focused();
    } else if (e->type() == QEvent::WindowDeactivate)
        EventScript::unfocused();

    return QMainWindow::event(e);
}

void MainWindow::showEvent(QShowEvent *)
{
    if (m_firstShow)
    {
        m_firstShow = false;

        if (VERSION != Profile::instance().clientVersion)
            launchWhatsNew();

        // Default connect to
        if (!Profile::instance().sessionConfigs().count())
            newConnection();
    }
}

void MainWindow::newProgramVersion(const QDate &)
{
    mbMain->setUpdateAvailable(true);
}

void MainWindow::checkForUpdate()
{
    DialogUpdate *dialog = new DialogUpdate(this);
    connect(dialog, SIGNAL(accepted()), this, SLOT(updateAccepted()));
    dialog->show();
}

void MainWindow::applyProfileOnTabWidget()
{
    // General
#ifdef Q_OS_WIN32
    Qt::WindowFlags flags = windowFlags();

    if (!(flags & Qt::WindowStaysOnTopHint) && Profile::instance().keepAboveOtherWindows)
    {
        flags |= Qt::WindowStaysOnTopHint;
        setWindowFlags(flags);
        show();
    } else if (flags & Qt::WindowStaysOnTopHint && !Profile::instance().keepAboveOtherWindows)
    {
        flags &= ~Qt::WindowStaysOnTopHint;
        setWindowFlags(flags);
        show();
    }
#endif

    // Tabs location
    tabWidgetMain->setTabPosition(Profile::instance().tabsPosition);
}

Session *MainWindow::getCurrentSession()
{
    SessionWidget *sessionWidget = qobject_cast<SessionWidget*>(tabWidgetMain->currentWidget());
    if (sessionWidget)
        return sessionWidget->session();
    return 0;
}

void MainWindow::closeCurrentSession()
{
    SessionWidget *sessionWidget = qobject_cast<SessionWidget*>(tabWidgetMain->currentWidget());
    if (sessionWidget)
    {
        Session *session = sessionWidget->session();
        removeSessionWidgets(session);
        if (!SessionManager::destroyed())
            SessionManager::instance().removeSession(session);
    }
}

void MainWindow::whoItemDblClicked(const QString &login)
{
    Session *session = qobject_cast<ChannelWidget*>(sender())->session();

    // Initiate a tell widget
    TellWidget *tellWidget = getTellWidget(session, login);
    if (!tellWidget)
        tellWidget = newTellWidget(session, login);

    // Just focus the tell widget
    tabWidgetMain->setCurrentWidget(tellWidget);
}

TellWidget *MainWindow::getTellWidget(Session *session, const QString &login)
{
    for (int i = 0; i < tabWidgetMain->count(); ++i)
    {
        TellWidget *tellWidget = qobject_cast<TellWidget*>(tabWidgetMain->widget(i));
        if (tellWidget && tellWidget->session() == session && tellWidget->login() == login)
            return tellWidget;
    }
    return 0;
}

TellWidget *MainWindow::newTellWidget(Session *session, const QString &login)
{
    TellWidget *tellWidget = new TellWidget(session, login);
    connect(tellWidget, SIGNAL(captionChanged()), this, SLOT(captionChanged()));

    tabWidgetMain->addTab(tellWidget, login);

    return tellWidget;
}

void MainWindow::closeTabWidget()
{
    QWidget *widget = tabWidgetMain->currentWidget();
    if (!widget ||
        qobject_cast<ChannelWidget*>(widget) ||
        qobject_cast<SystemWidget*>(widget)) // Don't close channel widget or system logs
        return;

    Session *session = 0;
    if (qobject_cast<SessionWidget*>(widget))
        session = qobject_cast<SessionWidget*>(widget)->session();

    tabWidgetMain->removeTab(tabWidgetMain->indexOf(widget));
    widget->deleteLater();

    ChannelWidget *channelWidget = getChannelWidget(session);

    if (channelWidget)
    {
        tabWidgetMain->setCurrentWidget(channelWidget);
        channelWidget->focusWidget()->setFocus();
    }
}

/*void MainWindow::highlightSessionWidget()
{
    SessionWidget *widget = qobject_cast<SessionWidget*>(sender());

    switch (widget->highlightType())
    {
    case SessionWidget::NoHighlight:
        if (!Profile::instance().tabsIcons)
        {
            widget->setStared(false);
            mtwMain->renameLabel(widget, widget->caption());
        }
        tabToBlinkTime[widget] = 0;
        mtwMain->changeTabTextColor(widget, mtwMain->palette().color(QPalette::WindowText));
        break;
    case SessionWidget::MinorHighlight:
        if (widget == mtwMain->focusedWidget())
            return;

        if (!Profile::instance().tabsIcons)
        {
            widget->setStared(true);
            mtwMain->renameLabel(widget, widget->caption());
        }
        tabToBlinkTime[widget] = 5000;
        tabToAscendingOrder[widget] = false;
        mtwMain->changeTabTextColor(widget, Qt::blue);
        break;
    case SessionWidget::MajorHighlight:
        if (widget == mtwMain->focusedWidget())
            return;

        if (!Profile::instance().tabsIcons)
        {
            widget->setStared(true);
            mtwMain->renameLabel(widget, widget->caption());
        }
        tabToBlinkTime[widget] = 5000;
        tabToAscendingOrder[widget] = false;
        mtwMain->changeTabTextColor(widget, Qt::red);
        break;
    }
    }*/

CmdOutputWidget *MainWindow::getCmdOutputWidget(Session *session, const QString &cmdName)
{
    for (int i = 0; i < tabWidgetMain->count(); ++i)
    {
        CmdOutputWidget *w = qobject_cast<CmdOutputWidget*>(tabWidgetMain->widget(i));
        if (w && w->session() == session && !w->cmdName().compare(cmdName, Qt::CaseInsensitive))
            return w;
    }
    return 0;
}

CmdOutputWidget *MainWindow::newCmdOutputWidget(Session *session, const QString &cmdName)
{
    CmdOutputWidget *w = new CmdOutputWidget(session, cmdName);

    tabWidgetMain->addTab(w, cmdName);

    return w;
}

TransfersWidget *MainWindow::getTransfersWidget(Session *session)
{
    for (int i = 0; i < tabWidgetMain->count(); ++i)
    {
        TransfersWidget *w = qobject_cast<TransfersWidget*>(tabWidgetMain->widget(i));
        if (w && w->session() == session)
            return w;
    }
    return 0;
}

TransfersWidget *MainWindow::newTransfersWidget(Session *session)
{
    TransfersWidget *w = new TransfersWidget(session);

    tabWidgetMain->addTab(w, tr("File transfers"));

    return w;
}

void MainWindow::sessionLoginChanged(Session *session, const QString &oldLogin, const QString &newLogin)
{
    TellWidget *tellWidget = getTellWidget(session, oldLogin);
    if (tellWidget)
    {
        renameWidget(tellWidget, newLogin);
        tellWidget->setLogin(newLogin);
    }
}

void MainWindow::tabWidgetMainCurrentChanged(int index)
{
    SessionWidget *sessionWidget = qobject_cast<SessionWidget*>(tabWidgetMain->widget(index));
    if (!sessionWidget)
        return;
    sessionWidget->setStared(false);
    renameWidget(sessionWidget, sessionWidget->caption());
    changeWidgetColor(sessionWidget, tabWidgetMain->palette().color(QPalette::WindowText));
    sessionWidget->applyFirstShow();
}

ChannelWidget *MainWindow::getChannelWidget(Session *session)
{
    for (int i = 0; i < tabWidgetMain->count(); ++i)
    {
        ChannelWidget *w = qobject_cast<ChannelWidget*>(tabWidgetMain->widget(i));
        if (w && w->session() == session)
            return w;
    }
    return 0;
}

ChannelWidget *MainWindow::getChannelWidget(const SessionConfig &config) const
{
    for (int i = 0; i < tabWidgetMain->count(); ++i)
    {
        ChannelWidget *channelWidget = qobject_cast<ChannelWidget*>(tabWidgetMain->widget(i));
        if (channelWidget &&
            channelWidget->session()->config().ID() == config.ID())
            return channelWidget;
    }
    return 0;
}

void MainWindow::whoUserDoubleClicked(const QString &login)
{
    ChannelWidget *channelWidget = qobject_cast<ChannelWidget*>(sender());
    Session *session = channelWidget->session();

    // Initiate a tell widget
    TellWidget *widget = getTellWidget(session, login);
    if (!widget)
        widget = newTellWidget(session, login);

    // Just focus it
    tabWidgetMain->setCurrentWidget(widget);
}

void MainWindow::tellSessionAsked(const QString &login)
{
    whoUserDoubleClicked(login);
}

void MainWindow::showLogsDir()
{
    // Determine the QUrl of the logs path
    QUrl url;
    if (Profile::instance().logsDefaultDir)
        url = QUrl::fromLocalFile(Logger::getDefaultLogsDir());
    else
        url = QUrl::fromLocalFile(Profile::instance().logsDir);

    // Launch it
    QDesktopServices::openUrl(url);
}

void MainWindow::showMessages()
{
    Session *session = getCurrentSession();

    if (!session) return;

    // Initiate a message widget
    MessageWidget *widget = getMessageWidget(session);
    if (!widget)
        widget = newMessageWidget(session);

    // Just focus it
    tabWidgetMain->setCurrentWidget(widget);
}

void MainWindow::showTransfers()
{
    Session *session = getCurrentSession();

    // Initiate a message widget
    TransfersWidget *widget = getTransfersWidget(session);
    if (!widget)
        widget = newTransfersWidget(session);

    // Just focus it
    tabWidgetMain->setCurrentWidget(widget);
}

MessageWidget *MainWindow::getMessageWidget(Session *session)
{
    for (int i = 0; i < tabWidgetMain->count(); ++i)
    {
        MessageWidget *w = qobject_cast<MessageWidget*>(tabWidgetMain->widget(i));
        if (w && w->session() == session)
            return w;
    }
    return 0;
}

MessageWidget *MainWindow::newMessageWidget(Session *session)
{
    MessageWidget *w = new MessageWidget(session);

    tabWidgetMain->addTab(w, tr("Messages"));

    return w;
}

void MainWindow::launchWhatsNew()
{
    DialogWhatsNew *dialog = new DialogWhatsNew(this);
    dialog->show();
}

void MainWindow::launchBugReport()
{
    QString body = tr("Hi!\n\nI'm happy (can be ironic) with your <Mtp> Chat! client but sometimes, it really sucks.\n\nHere are the problems:\n\n* ");

    MyTextEdit::openUrl(QUrl("http://code.google.com/p/cebmtpchat/issues/entry?labels=Type-Defect&comment=" + body));
}

void MainWindow::launchFeatureReport()
{
    QString body = tr("Hi!\n\nI'm happy (can be ironic) with your <Mtp> Chat! client but sometimes, it really sucks.\n\nHere are the missing features I'd like to see in the next version:\n\n* ");

    MyTextEdit::openUrl(QUrl("http://code.google.com/p/cebmtpchat/issues/entry?labels=Type-Enhancement&comment=" + body));
}

void MainWindow::menuIconClicked()
{
    checkForUpdate();
}

void MainWindow::updateAccepted()
{
    QMessageBox::warning(0, tr("Warning"), tr("You must close every CeB instance to continue installation"));
    QProcess::startDetached(qobject_cast<DialogUpdate*>(sender())->fileToLaunch());
}

/*void MainWindow::animationTimeout()
{
    static const int increment = 20;

    // Go through all tab animations
    foreach (QWidget *wi, mtwMain->getWidgets())
    {
        SessionWidget *w = qobject_cast<SessionWidget*>(wi);
        int blinkTime = tabToBlinkTime[w];
        if (blinkTime > 0)
        {
            // It remains time to blink
            tabToBlinkTime[w] = blinkTime - animationTimer.interval();

            QColor currentColor = mtwMain->tabTextColor(w);
            int colorCompo;
            bool isBlue = w->highlightType() == SessionWidget::MinorHighlight;
            if (isBlue)
                colorCompo = currentColor.blue();
            else
                colorCompo = currentColor.red();

            if (tabToAscendingOrder[w])
            {
                if (colorCompo < 255)
                {
                    if (colorCompo + increment <= 255)
                    {
                        if (isBlue)
                            currentColor.setBlue(colorCompo + increment);
                        else
                            currentColor.setRed(colorCompo + increment);
                    }
                    else
                    {
                        if (isBlue)
                            currentColor.setBlue(255);
                        else
                            currentColor.setRed(255);
                    }
                }
                else
                    tabToAscendingOrder[w] = false;
            } else
            {
                if (colorCompo > 0)
                {
                    if (colorCompo - increment >= 0)
                    {
                        if (isBlue)
                            currentColor.setBlue(colorCompo - increment);
                        else
                            currentColor.setRed(colorCompo - increment);
                    }
                    else
                    {
                        if (isBlue)
                            currentColor.setBlue(0);
                        else
                            currentColor.setRed(0);
                    }
                }
                else
                    tabToAscendingOrder[w] = true;
            }
            if (w->highlightType() == SessionWidget::NoHighlight)
                mtwMain->changeTabTextColor(w, mtwMain->palette().color(QPalette::WindowText));
            else if (tabToBlinkTime[w] <= 0)
            {
                if (isBlue)
                    mtwMain->changeTabTextColor(w, Qt::blue);
                else
                    mtwMain->changeTabTextColor(w, Qt::red);
            }
            else
                mtwMain->changeTabTextColor(w, currentColor);
        }
    }
    }*/

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == tabWidgetMain)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            QWidget *widget = tabWidgetMain->widgetByTabPosition(mouseEvent->pos());
            if (qobject_cast<ChannelWidget*>(widget)) // Don't remove channel widget
                return true;
            SessionWidget *sessionWidget = qobject_cast<SessionWidget*>(widget);
            if (sessionWidget)
            {
                removeWidget(sessionWidget);
                return true;
            }
        } else if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if ((keyEvent->key() == Qt::Key_W && keyEvent->modifiers() == Qt::ControlModifier) ||
                (keyEvent->key() == Qt::Key_Escape))
            {
                closeTabWidget();
                return true;
            }
        } else if (event->type() == QEvent::ShortcutOverride)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_F11 ||
                (keyEvent->key() == Qt::Key_Left && keyEvent->modifiers() == Qt::AltModifier))
            {
                previousTab();
                return true;
            }
            else if (keyEvent->key() == Qt::Key_F12 ||
                     (keyEvent->key() == Qt::Key_Right && keyEvent->modifiers() == Qt::AltModifier) ||
                     (keyEvent->key() == Qt::Key_Tab && keyEvent->modifiers() == Qt::ControlModifier))
            {
                nextTab();
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::showFileTransfers()
{
    ChannelWidget *channelWidget = qobject_cast<ChannelWidget*>(sender());
    Session *session = channelWidget->session();

    // Initiate a transfers widget
    TransfersWidget *widget = getTransfersWidget(session);
    if (!widget)
        widget = newTransfersWidget(session);

    // Just focus it
    tabWidgetMain->setCurrentWidget(widget);
}

void MainWindow::newTransfer(Transfer *transfer)
{
    // Initiate a transfers widget
    TransfersWidget *widget = getTransfersWidget(transfer->session());
    if (!widget)
        widget = newTransfersWidget(transfer->session());

    // Just focus it
    tabWidgetMain->setCurrentWidget(widget);
}

void MainWindow::executeAction(int action)
{
    // Get current session
    SessionWidget *sessionWidget = qobject_cast<SessionWidget*>(tabWidgetMain->currentWidget());
    if (!sessionWidget)
         return;

    Session *session = sessionWidget->session();

    switch ((Action::ActionType) action)
    {
    case Action::Action_ToggleMenuBar:
        menuBar()->setVisible(!menuBar()->isVisible());
        break;
    case Action::Action_ToggleStatusBar:
        statusBar()->setVisible(!statusBar()->isVisible());
        break;
    case Action::Action_RefreshWhoColumn:
        if (!session->isLogged())
            return;
        getChannelWidget(session)->refreshWhoColumn();
        break;
    case Action::Action_ToggleAway:
        if (!session->isLogged())
            return;

        session->deactivateAutoAway();
        if (session->away())
            session->sendCommand("set away off");
        else
            session->sendCommand("set away on");
        break;
    case Action::Action_Reconnect:
        reconnect();
        break;
    default:;
    }
}

void MainWindow::searchActionTriggered()
{
    SessionWidget *w = qobject_cast<SessionWidget*>(tabWidgetMain->currentWidget());
    if (w)
    {
        w->search();
        return;
    }
}

void MainWindow::renameWidget(QWidget *widget, const QString &text)
{
    QString caption = text;
    if (text == "")
    {
        SessionWidget *sessionWidget = qobject_cast<SessionWidget*>(widget);
        if (sessionWidget)
            caption = sessionWidget->caption();
    }

    int index = tabWidgetMain->indexOf(widget);
    if (index >= 0)
        tabWidgetMain->setTabText(index, caption);
}

void MainWindow::changeWidgetColor(QWidget *widget, const QColor &color)
{
    int index = tabWidgetMain->indexOf(widget);
    if (index >= 0)
        tabWidgetMain->setTabTextColor(index, color);
}

void MainWindow::removeSessionWidgets(Session *session)
{
    for (int i = tabWidgetMain->count() - 1; i > 0; --i)
    {
        SessionWidget *sessionWidget = qobject_cast<SessionWidget*>(tabWidgetMain->widget(i));
        if (sessionWidget->session() == session)
        {
            tabWidgetMain->removeTab(i);
            delete sessionWidget;
        }
    }
}

void MainWindow::removeWidget(QWidget *widget)
{
    int index = tabWidgetMain->indexOf(widget);
    if (index >= 0)
    {
        tabWidgetMain->removeTab(index);
        delete widget;
    }
}

void MainWindow::previousTab()
{
    if (tabWidgetMain->currentIndex())
        tabWidgetMain->setCurrentIndex(tabWidgetMain->currentIndex() - 1);
    else if (tabWidgetMain->count())
        tabWidgetMain->setCurrentIndex(tabWidgetMain->count() - 1);
}

void MainWindow::nextTab()
{
    if (tabWidgetMain->currentIndex() < tabWidgetMain->count() - 1)
        tabWidgetMain->setCurrentIndex(tabWidgetMain->currentIndex() + 1);
    else if (tabWidgetMain->count())
        tabWidgetMain->setCurrentIndex(0);
}

void MainWindow::captionChanged()
{
    renameWidget(qobject_cast<QWidget*>(sender()));
}

void MainWindow::aboutToShowConfigurationMenu()
{
    if (menuBar()->isVisible())
        _actionToggleMenuBarVisibility->setText(tr("Hide &menu bar"));
    else
        _actionToggleMenuBarVisibility->setText(tr("Show &menu bar"));
    QShortcut *shortcut = shortcutByActionType(Action::Action_ToggleMenuBar);
    if (shortcut)
        _actionToggleMenuBarVisibility->setShortcut(shortcut->key());
    if (statusBar()->isVisible())
        _actionToggleStatusBarVisibility->setText(tr("Hide &status bar"));
    else
        _actionToggleStatusBarVisibility->setText(tr("Show &status bar"));
    shortcut = shortcutByActionType(Action::Action_ToggleStatusBar);
    if (shortcut)
        _actionToggleStatusBarVisibility->setShortcut(shortcut->key());
}

void MainWindow::toggleMenuBarVisibility()
{
    menuBar()->setVisible(!menuBar()->isVisible());
}

void MainWindow::toggleStatusBarVisibility()
{
    statusBar()->setVisible(!statusBar()->isVisible());
}

void MainWindow::aboutToShowWindowsMenu()
{
    if (tabWidgetMain->indexOf(SystemWidget::instance()) >= 0)
        _actionToggleSystemLogsVisibility->setText(tr("Hide &system logs"));
    else
        _actionToggleSystemLogsVisibility->setText(tr("Show &system logs"));
}

void MainWindow::aboutToShowEditMenu()
{
    if (Profile::instance().topicWindowVisible)
        _actionToggleTopicVisibility->setText(tr("Hide &topic"));
    else
        _actionToggleTopicVisibility->setText(tr("Show &topic"));
    if (Profile::instance().usersWindowVisible)
        _actionToggleUsersVisibility->setText(tr("Hide &users"));
    else
        _actionToggleUsersVisibility->setText(tr("Show &users"));
}

Session *MainWindow::getCurrentSession() const
{
    SessionWidget *sessionWidget = qobject_cast<SessionWidget*>(tabWidgetMain->currentWidget());
    if (sessionWidget)
        return sessionWidget->session();
    return 0;
}

ChannelWidget *MainWindow::getChannelWidget(Session *session) const
{
    for (int i = 0; i < tabWidgetMain->count(); ++i)
    {
        ChannelWidget *channelWidget = qobject_cast<ChannelWidget*>(tabWidgetMain->widget(i));
        if (channelWidget && channelWidget->session() == session)
            return channelWidget;
    }
    return 0;
}

void MainWindow::toggleTopicVisibility()
{
    Profile::instance().topicWindowVisible = !Profile::instance().topicWindowVisible;

    for (int i = 0; i < tabWidgetMain->count(); ++i)
    {
        ChannelWidget *channelWidget = qobject_cast<ChannelWidget*>(tabWidgetMain->widget(i));
        if (channelWidget)
            channelWidget->setTopicVisible(Profile::instance().topicWindowVisible);
    }
}

void MainWindow::toggleUsersVisibility()
{
    Profile::instance().usersWindowVisible = !Profile::instance().usersWindowVisible;

    for (int i = 0; i < tabWidgetMain->count(); ++i)
    {
        ChannelWidget *channelWidget = qobject_cast<ChannelWidget*>(tabWidgetMain->widget(i));
        if (channelWidget)
            channelWidget->setUsersVisible(Profile::instance().usersWindowVisible);
    }
}

QShortcut *MainWindow::shortcutByActionType(Action::ActionType type) const
{
    QShortcut *shortcut = dynamic_cast<QShortcut*>(actionSignalMapper->mapping(type));
    return shortcut;
}
