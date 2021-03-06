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
#include "main_window.h"

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
#include "my_textedit.h"
#include "dialog_broadcast.h"
#include "updater.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_REVISION_H
#include "revision.h"
#endif

#ifdef DEBUG_NEW
#define new DEBUG_NEW
#endif

MainWindow *MainWindow::_instance = 0;

MainWindow *MainWindow::instance()
{
    if (!_instance)
        _instance = new MainWindow;

    return _instance;
}

void MainWindow::free()
{
    if (_instance)
        delete _instance;

    _instance = NULL;
}

MainWindow::MainWindow()
{
    _statusMessageLabel = new QLabel;
    statusBar()->addPermanentWidget(_statusMessageLabel, 1);
    _statusInfosLabel = new QLabel;
    statusBar()->addPermanentWidget(_statusInfosLabel);

    if (QSystemTrayIcon::isSystemTrayAvailable())
    {
        trayMenu = new QMenu(this);
        QAction *restoreAction = trayMenu->addAction(tr("Restore"));
        connect(restoreAction, SIGNAL(triggered()), this, SLOT(trayActivated()));
        QAction *quitAction = trayMenu->addAction(tr("Quit"));
        connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

        trayIcon = new QSystemTrayIcon(QIcon(":/images/tray-neutral.png"), this);
        trayIcon->setContextMenu(trayMenu);
        connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
        if (Profile::instance().trayEnabled && Profile::instance().trayAlwaysVisible)
            trayIcon->show();
    }
    else
    {
        trayMenu = NULL;
        trayIcon = NULL;
    }

    SessionManager &sessionManager = SessionManager::instance();
    connect(&sessionManager, SIGNAL(newSessionToken(Session *, const Token &)),
            this, SLOT(newSessionTokenForActivity(Session *, const Token &)));
    connect(&sessionManager, SIGNAL(newSessionToken(Session *, const Token &)),
            this, SLOT(newSessionToken(Session *, const Token &)));
    connect(&sessionManager, SIGNAL(sessionLoginChanged(Session *, const QString &, const QString &)),
            this, SLOT(sessionLoginChanged(Session *, const QString &, const QString &)));
    connect(&sessionManager, SIGNAL(sessionConnecting(Session *)),
            this, SLOT(sessionConnecting(Session *)));
    connect(&sessionManager, SIGNAL(sessionConnected(Session *)),
            this, SLOT(sessionConnected(Session *)));
    connect(&sessionManager, SIGNAL(sessionDisconnected(Session *)),
            this, SLOT(sessionDisconnected(Session *)));
    connect(&sessionManager, SIGNAL(sessionPrivateConversationIncoming(Session *, const QString&)),
            this, SLOT(sessionPrivateConversationIncoming(Session *, const QString&)));

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

    connect(tabWidgetMain, SIGNAL(tabBarCustomContextMenuRequested(const QPoint &)),
            this, SLOT(tabWidgetMainCustomContextMenuRequested(const QPoint &)));

    applyProfileOnTabWidget();

    // Create system dialog
    if (Profile::instance().systemLogsVisible)
        tabWidgetMain->addTab(SystemWidget::instance(), tr("System"));

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
	if (channelWidget) channelWidget->applyFirstShow();

	// check for a new version
	Updater *updater = new Updater(this);
	connect(updater, SIGNAL(newVersionDetected(QString, QString, uint, QString)), this, SLOT(onNewVersion(QString, QString, uint, QString)));
	updater->checkUpdates();

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
    SystemWidget::free();
    TransfersManager::free();
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
    if (trayIcon)
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
    QAction *actionQuit = mConnections->addAction(tr("&Quit"));
    actionQuit->setMenuRole(QAction::QuitRole);
    connect(actionQuit, SIGNAL(triggered()), this, SLOT(close()));

    // Edit menu
    QMenu *menuEdit = mbMain->addMenu(tr("&Edit"));
    connect(menuEdit, SIGNAL(aboutToShow()), this, SLOT(aboutToShowEditMenu()));
    QAction *actionSearch = menuEdit->addAction(tr("&Search"));
    connect(actionSearch, SIGNAL(triggered()), this, SLOT(searchActionTriggered()));

    // Configuration menu
    QMenu *menuConfiguration = mbMain->addMenu(tr("Confi&guration"));
    connect(menuConfiguration, SIGNAL(aboutToShow()), this, SLOT(aboutToShowConfigurationMenu()));
    _actionToggleMenuBarVisibility = menuConfiguration->addAction("");
    _actionToggleMenuBarVisibility->setShortcutContext(Qt::WidgetShortcut);
    connect(_actionToggleMenuBarVisibility, SIGNAL(triggered()), this, SLOT(toggleMenuBarVisibility()));
    _actionToggleStatusBarVisibility = menuConfiguration->addAction("");
    _actionToggleStatusBarVisibility->setShortcutContext(Qt::WidgetShortcut);
    connect(_actionToggleStatusBarVisibility, SIGNAL(triggered()), this, SLOT(toggleStatusBarVisibility()));
    _actionToggleTopicVisibility = menuConfiguration->addAction("");
    connect(_actionToggleTopicVisibility, SIGNAL(triggered()), this, SLOT(toggleTopicVisibility()));
    _actionToggleUsersVisibility = menuConfiguration->addAction("");
    connect(_actionToggleUsersVisibility, SIGNAL(triggered()), this, SLOT(toggleUsersVisibility()));
    QAction *actionEditSettings = menuConfiguration->addAction(tr("&General settings..."));
    actionEditSettings->setMenuRole(QAction::PreferencesRole);
    connect(actionEditSettings, SIGNAL(triggered()), this, SLOT(editSettings()));
    menuConfiguration->addSeparator();
    actionEditConnectionConfig = menuConfiguration->addAction(tr("&Connection configuration..."));
    actionEditConnectionConfig->setEnabled(false);
    connect(actionEditConnectionConfig, SIGNAL(triggered()), this, SLOT(editConnectionConfig()));

    // Windows menu
    QMenu *menuWindows = mbMain->addMenu(tr("&Windows"));
    connect(menuWindows, SIGNAL(aboutToShow()), this, SLOT(aboutToShowWindowsMenu()));
    _actionPreviousTab = menuWindows->addAction(tr("&Previous tab"));
    _actionPreviousTab->setShortcutContext(Qt::WidgetShortcut);
    connect(_actionPreviousTab, SIGNAL(triggered()), this, SLOT(previousTab()));
    _actionNextTab = menuWindows->addAction(tr("&Next tab"));
    _actionNextTab->setShortcutContext(Qt::WidgetShortcut);
    connect(_actionNextTab, SIGNAL(triggered()), this, SLOT(nextTab()));
    menuWindows->addSeparator();
    _actionMoveTabToPreviousPlace = menuWindows->addAction("");
    _actionMoveTabToPreviousPlace->setShortcutContext(Qt::WidgetShortcut);
    _actionMoveTabToPreviousPlace->setVisible(false);
    connect(_actionMoveTabToPreviousPlace, SIGNAL(triggered()), this, SLOT(moveTabToPreviousPlace()));
    _actionMoveTabToNextPlace = menuWindows->addAction("");
    _actionMoveTabToNextPlace->setShortcutContext(Qt::WidgetShortcut);
    _actionMoveTabToNextPlace->setVisible(false);
    connect(_actionMoveTabToNextPlace, SIGNAL(triggered()), this, SLOT(moveTabToNextPlace()));
    _actionCloseCurrentTab = menuWindows->addAction(tr("&Close the tab"));
    _actionCloseCurrentTab->setShortcutContext(Qt::WidgetShortcut);
    connect(_actionCloseCurrentTab, SIGNAL(triggered()), this, SLOT(closeCurrentTab()));

    menuWindows->addSeparator();
    _actionToggleSystemLogsVisibility = menuWindows->addAction(tr("Toggle system logs visibility"));
    connect(_actionToggleSystemLogsVisibility, SIGNAL(triggered()), this, SLOT(showSystemLogs()));

    // View menu
    QMenu *menuView = mbMain->addMenu(tr("&View"));

    connect(menuView->addAction(tr("Open &logs directory")), SIGNAL(triggered()), this, SLOT(showLogsDir()));
    connect(menuView->addAction(tr("Open &profile directory")), SIGNAL(triggered()), this, SLOT(showProfileDir()));
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
    QAction *actionHelpAbout = menuHelp->addAction(tr("&About..."));
    actionHelpAbout->setMenuRole(QAction::AboutRole);
    connect(actionHelpAbout, SIGNAL(triggered()), this, SLOT(about()));
    QAction *actionHelpAboutQt = menuHelp->addAction(tr("About &Qt..."));
    actionHelpAboutQt->setMenuRole(QAction::AboutQtRole);
    connect(actionHelpAboutQt, SIGNAL(triggered()), this, SLOT(aboutQt()));
}

void MainWindow::makeToolBar()
{
    // TODO: populate
}

#if defined(Q_OS_WIN32)

#ifdef USE_QT5
bool MainWindow::nativeEvent(const QByteArray &eventType, void *msg, long *result)
{
    MSG *message = (MSG*)msg;
#else
bool MainWindow::winEvent(MSG *message, long *result)
{
#endif
    if (message->message == WM_SYSCOMMAND && message->wParam == SC_MINIMIZE)
    {
        if (Profile::instance().trayEnabled && Profile::instance().trayHideFromTaskBar)
            hide();
        if (Profile::instance().trayEnabled && !Profile::instance().trayAlwaysVisible && trayIcon)
            trayIcon->show();
        trayTalkAboutMe = false;
        if (Profile::instance().trayEnabled && Profile::instance().trayHideFromTaskBar)
        {
            (*result) = FALSE;
            return true;
        }
    }
#ifdef USE_QT5
    return QMainWindow::nativeEvent(eventType, msg, result);
#else
    return QMainWindow::winEvent(message, result);
#endif
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
    if (!session)
        return;

    if (session->isConnected() &&
        QMessageBox::question(this, tr("Confirmation"), tr("You seem to be already connected, do you really want to force a reconnection?"),
                              QMessageBox::Yes | QMessageBox::Cancel) != QMessageBox::Yes)
        return;

    session->resetBackupServers();
    session->start();
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
        tabWidgetMain->insertTab(0, SystemWidget::instance(), tr("System"));
        tabWidgetMain->setCurrentIndex(0);
    }
}

void MainWindow::about()
{
	QMessageBox::about(this, tr("About %1").arg(PRODUCT), tr("%1 %2 (%3)\n\nDevelopped by: Garou, Kervala\n\nSome Lua parts are from Ace, Drealmer and Ben").arg(PRODUCT).arg(VERSION).arg(BUILD_DATE));
}

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this);
}

ChannelWidget *MainWindow::connectTo(const SessionConfig &config)
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
    channelWidget = new ChannelWidget(session, this);
    connect(channelWidget, SIGNAL(whoUserDoubleClicked(const QString&)),
            this, SLOT(whoUserDoubleClicked(const QString&)));
    connect(channelWidget, SIGNAL(tellSessionAsked(const QString&)),
            this, SLOT(tellSessionAsked(const QString&)));
    connect(channelWidget, SIGNAL(showFileTransfers()), this, SLOT(showFileTransfers()));
    connect(channelWidget, SIGNAL(captionChanged()), this, SLOT(captionChanged()));

    tabWidgetMain->addTab(channelWidget, channelWidget->caption());
    tabWidgetMain->setCurrentWidget(channelWidget);

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
    if (trayIcon)
    {
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
            QApplication::alert(this);

            if (changeTray && trayIcon)
                trayIcon->setIcon(QIcon(":/images/tray-myself.png"));
            if (showWarningo && Profile::instance().warningoEnabled)
				(new DialogWarningo(session->config().name(), token.line()))->show();
        }
        else if (!trayTalkAboutMe && trayIcon)
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
    if (session == getCurrentSession())
        refreshStatusLabel();
}

void MainWindow::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason != QSystemTrayIcon::Trigger)
        return;

    if (Profile::instance().trayEnabled && Profile::instance().trayHideFromTaskBar)
        show();
    if (isMinimized())
        showNormal();
    raise();
    activateWindow();
    if (Profile::instance().trayEnabled && !Profile::instance().trayAlwaysVisible && trayIcon)
        trayIcon->hide();
}

bool MainWindow::event(QEvent *e)
{
    if (e->type() == QEvent::WindowActivate)
    {
        if (trayIcon)
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

#ifndef QT_PLUGIN
		if (Profile::instance().clientVersion != QApplication::applicationVersion())
            launchWhatsNew();

        // Default connect to
        if (!Profile::instance().sessionConfigs().count())
            newConnection();
#endif
    }
}

void MainWindow::onNewVersion(const QString &url, const QString &date, uint size, const QString &version)
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
    Session *session = getCurrentSession();
    if (!session)
        return;

    removeSessionWidgets(session);
    if (!SessionManager::destroyed())
        SessionManager::instance().removeSession(session);

    // Set focus
    QWidget *widget = tabWidgetMain->currentWidget();
    if (widget && widget->focusWidget())
        widget->focusWidget()->setFocus();
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

void MainWindow::closeTab(QWidget *tab)
{
    if (qobject_cast<ChannelWidget*>(tab))
    {
        if (QMessageBox::question(this, tr("Confirmation"), tr("If you close this tab, you session will be closed and all relative tabs too, do you want to continue?"),
                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
            closeCurrentSession();
        return;
    }

    if (qobject_cast<SystemWidget*>(tab))
    {
        showSystemLogs();
        return;
    }

    removeWidget(tab);
}

void MainWindow::closeCurrentTab()
{
    QWidget *widget = tabWidgetMain->currentWidget();
    if (!widget)
        return;

    closeTab(widget);
}

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
    QWidget *widget = tabWidgetMain->widget(index);
    if (widget)
        _focusStack.push(widget);

    SessionWidget *sessionWidget = qobject_cast<SessionWidget*>(tabWidgetMain->widget(index));
    if (sessionWidget)
    {
        sessionWidget->setStared(false);
        renameWidget(sessionWidget, sessionWidget->caption());
        changeWidgetColor(sessionWidget, tabWidgetMain->palette().color(QPalette::WindowText));
        sessionWidget->applyFirstShow();
    }

    refreshStatusLabel();
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

void MainWindow::showProfileDir()
{
    // Determine the QUrl of the logs path
    QUrl url = QUrl::fromLocalFile(Paths::profilePath());

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
	QString url = "http://dev.kervala.net/projects/cebmtpchat/issues/new?issue[tracker_id]=1&issue[description]=";
	
    QString body = tr("Hi!\n\nI'm happy (can be ironic) with your <Mtp> Chat! client but sometimes, it really sucks.\n\nHere are the problems:\n\n* ");

    MyTextEdit::openUrl(QUrl(url + body));
}

void MainWindow::launchFeatureReport()
{
	QString url = "http://dev.kervala.net/projects/cebmtpchat/issues/new?issue[tracker_id]=2&issue[description]=";

    QString body = tr("Hi!\n\nI'm happy (can be ironic) with your <Mtp> Chat! client but sometimes, it really sucks.\n\nHere are the missing features I'd like to see in the next version:\n\n* ");

    MyTextEdit::openUrl(QUrl(url + body));
}

void MainWindow::menuIconClicked()
{
    checkForUpdate();
}

void MainWindow::updateAccepted()
{
    QMessageBox::warning(this, tr("Warning"), tr("You must close every CeB instance to continue installation"));

    QString installer = qobject_cast<DialogUpdate*>(sender())->fileToLaunch();

	QDesktopServices::openUrl(QUrl::fromLocalFile(installer));
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == tabWidgetMain)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            QWidget *widget = tabWidgetMain->widgetByTabPosition(mouseEvent->globalPos());
            if (widget && mouseEvent->button() == Qt::MidButton)
                closeTab(widget);
        } else if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_W && keyEvent->modifiers() == Qt::ControlModifier)
            {
                closeCurrentTab();
                return true;
            }
        } else if (event->type() == QEvent::ShortcutOverride)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Left && keyEvent->modifiers() == Qt::AltModifier)
            {
                previousTab();
                return true;
            }
            else if ((keyEvent->key() == Qt::Key_Right && keyEvent->modifiers() == Qt::AltModifier) ||
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
    Session *session = getCurrentSession();
    switch ((Action::ActionType) action)
    {
    case Action::Action_ToggleMenuBar:
        menuBar()->setVisible(!menuBar()->isVisible()); break;
    case Action::Action_ToggleStatusBar:
        statusBar()->setVisible(!statusBar()->isVisible()); break;
    case Action::Action_PreviousTab:
        previousTab(); break;
    case Action::Action_NextTab:
        nextTab(); break;
    case Action::Action_MoveTabToPrevious:
        moveTabToPreviousPlace(); break;
    case Action::Action_MoveTabToNext:
        moveTabToNextPlace(); break;
    case Action::Action_CloseTab:
        closeCurrentTab(); break;
    case Action::Action_RefreshWhoColumn:
        if (!session || !session->isLogged())
            return;
        getChannelWidget(session)->refreshWhoColumn();
        break;
    case Action::Action_ToggleAway:
        if (!session || !session->isLogged())
            return;

        session->deactivateAutoAway();
        if (session->away())
            session->sendCommand("set away off");
        else
            session->sendCommand("set away on");
        break;
    case Action::Action_GlobalSend:
        showGlobalSendDialog();
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
	if (text.isEmpty())
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
    for (int i = tabWidgetMain->count() - 1; i >= 0; --i)
    {
        SessionWidget *sessionWidget = qobject_cast<SessionWidget*>(tabWidgetMain->widget(i));
        if (sessionWidget && sessionWidget->session() == session)
            removeWidget(sessionWidget);
    }
}

void MainWindow::removeWidget(QWidget *widget)
{
    int index = tabWidgetMain->indexOf(widget);
    if (index < 0)
        return;

    // Remove this widget from the focus stack
    for (int i = _focusStack.count() - 1; i > 0; --i)
        if (_focusStack[i] == widget)
            _focusStack.remove(i);

    // Focus the stack head
    if (!_focusStack.isEmpty())
        tabWidgetMain->setCurrentWidget(_focusStack.pop());

    tabWidgetMain->removeTab(index);
    widget->deleteLater();
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
    if (Profile::instance().topicWindowVisible)
        _actionToggleTopicVisibility->setText(tr("Hide &topic"));
    else
        _actionToggleTopicVisibility->setText(tr("Show &topic"));
    if (Profile::instance().usersWindowVisible)
        _actionToggleUsersVisibility->setText(tr("Hide &users"));
    else
        _actionToggleUsersVisibility->setText(tr("Show &users"));
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
    QShortcut *shortcut = shortcutByActionType(Action::Action_PreviousTab);
    if (shortcut)
        _actionPreviousTab->setShortcut(shortcut->key());
    shortcut = shortcutByActionType(Action::Action_NextTab);
    if (shortcut)
        _actionNextTab->setShortcut(shortcut->key());

    _actionMoveTabToPreviousPlace->setText(tr("Move the tab to the &left"));
    _actionMoveTabToNextPlace->setText(tr("Move the tab to the &right"));

    shortcut = shortcutByActionType(Action::Action_CloseTab);
    if (shortcut)
        _actionCloseCurrentTab->setShortcut(shortcut->key());

    if (tabWidgetMain->indexOf(SystemWidget::instance()) >= 0)
        _actionToggleSystemLogsVisibility->setText(tr("Hide &system logs"));
    else
        _actionToggleSystemLogsVisibility->setText(tr("Show &system logs"));
}

void MainWindow::aboutToShowEditMenu()
{
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
    QShortcut *shortcut = qobject_cast<QShortcut*>(actionSignalMapper->mapping(type));
    return shortcut;
}

void MainWindow::refreshStatusLabel()
{
    QString label;
    ChannelWidget *channelWidget = qobject_cast<ChannelWidget*>(tabWidgetMain->currentWidget());
    if (channelWidget)
    {
        Session *session = channelWidget->session();
        _statusMessageLabel->setText(session->socketStateCaption());
        _statusInfosLabel->setText(session->channel() + " - " +
                                   tr("%n user(s)", "", session->whoPopulation().users().count()) +
                                   " - " + session->serverAddress());
        return;
    }

    TellWidget *tellWidget = qobject_cast<TellWidget*>(tabWidgetMain->currentWidget());
    if (tellWidget)
    {
        Session *session = tellWidget->session();
        _statusMessageLabel->setText("");
        _statusInfosLabel->setText(tr("Conversation with %1 - %2").arg(
                                  tellWidget->login().compare(session->serverLogin(), Qt::CaseInsensitive) ? tellWidget->login() : tr("yourself")).arg(session->serverAddress()));
        return;
    }

    SystemWidget *systemWidget = qobject_cast<SystemWidget*>(tabWidgetMain->currentWidget());
    if (systemWidget)
    {
        _statusMessageLabel->setText("");
        _statusInfosLabel->setText(tr("System logs"));
        return;
    }
    _statusMessageLabel->setText("");
    _statusInfosLabel->setText(label);
}

void MainWindow::sessionConnecting(Session *session)
{
    if (session == getCurrentSession())
        refreshStatusLabel();
}

void MainWindow::sessionConnected(Session *session)
{
    if (session == getCurrentSession())
        refreshStatusLabel();
}

void MainWindow::sessionDisconnected(Session *session)
{
    if (session == getCurrentSession())
        refreshStatusLabel();
}

void MainWindow::moveTabToPreviousPlace()
{
    // TODO : wait for Qt4.5 and tabs movable
}

void MainWindow::moveTabToNextPlace()
{
    // TODO : wait for Qt4.5 and tabs movable
}

void MainWindow::tabWidgetMainCustomContextMenuRequested(const QPoint &pos)
{
    QPoint globalPos = tabWidgetMain->globalTabBarPos(pos);
    _contextMenuWidget = tabWidgetMain->widgetByTabPosition(globalPos);

    QMenu menu;

    QAction *actionCloseTab = new QAction(tr("&Close this tab"), &menu);
    menu.addAction(actionCloseTab);
    connect(actionCloseTab, SIGNAL(triggered()), this, SLOT(closeTabTriggered()));

    menu.exec(globalPos);
}

void MainWindow::closeTabTriggered()
{
    closeTab(_contextMenuWidget);
}

void MainWindow::showGlobalSendDialog()
{
    DialogBroadcast dialog(this);
    if (dialog.exec() == QDialog::Accepted)
        foreach (Session *session, SessionManager::instance().sessionsList())
            if (session->config().broadcast())
                session->send(dialog.text());
}

void MainWindow::sessionPrivateConversationIncoming(Session *session, const QString &login)
{
    TellWidget *widget = getTellWidget(session, login);
    if (!widget)
        newTellWidget(session, login);
}
