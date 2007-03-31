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
#include <QShortcut>

#ifdef Q_OS_WIN32
#include <windows.h>
#endif

#include "version.h"
#include "dialog_settings.h"
#include "profile_manager.h"
#include "dialog_about.h"
#include "dialog_session_config.h"
#include "session_manager.h"
#include "dialog_update.h"
#include "dialog_warningo.h"
#include "dialog_whatsnew.h"

#include "main_window.h"

MainWindow::MainWindow()
{
	Profile &profile = *ProfileManager::instance().currentProfile();
	setWindowTitle("CeB");

	trayMenu = new QMenu(this);
	trayIcon = new QSystemTrayIcon(QIcon(":/images/tray-neutral.png"), this);
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
	if (profile.trayEnabled && profile.trayAlwaysVisible)
		trayIcon->show();

	SessionManager &sessionManager = SessionManager::instance();
	connect(&sessionManager, SIGNAL(newSessionToken(Session *, const TokenEvent &)),
		this, SLOT(newSessionTokenForActivity(Session *, const TokenEvent &)));
	connect(&sessionManager, SIGNAL(newSessionToken(Session *, const TokenEvent &)),
		this, SLOT(newSessionToken(Session *, const TokenEvent &)));
	connect(&sessionManager, SIGNAL(sessionLoginChanged(Session *, const QString &, const QString &)),
		this, SLOT(sessionLoginChanged(Session *, const QString &, const QString &)));

	TransfersManager &transfersManager = TransfersManager::instance();
	connect(&transfersManager, SIGNAL(newTransferAdded(Transfer *)),
			this, SLOT(newTransfer(Transfer *)));

	layout()->setMargin(0);

	makeMenuBar();
	makeToolBar();
	makeStatusBar();

	// Create system dialog
	DialogSystem::init(this);
	systemDialog = DialogSystem::instance();
	connect(systemDialog, SIGNAL(hideSystemDialog()), this, SLOT(hideSystemDialog()));
	if (profile.systemLogsWidth != -1)
	{
		systemDialog->move(profile.systemLogsLeft, profile.systemLogsTop);
		systemDialog->resize(profile.systemLogsWidth, profile.systemLogsHeight);
	}
	if (profile.systemLogsVisible)
		systemDialog->show();

	int w, h, l, t;
	if (profile.mainWidth != -1)
	{
		w = profile.mainWidth;
		h = profile.mainHeight;
		l = profile.mainLeft;
		t = profile.mainTop;
	}
	else
	{
		w = 640;
		h = 480;
		l = 0;
		t = 0;
	}

	resize(w, h);
	move(l, t);

	// Create and init multitabwidget
	mtwMain = new MultiTabWidget;
	connect(mtwMain, SIGNAL(focusedWidgetChanged(QWidget*)), this, SLOT(focusedWidgetChanged(QWidget *)));
	mtwMain->installEventFilter(this);
	setCentralWidget(mtwMain);

	applyProfileOnMultiTabWidget();

	// Autoconnect connections
	foreach (SessionConfig *config, profile.sessionConfigs())
		if ((*config).autoconnect())
			connectTo(*config);

	connectionsSignalMapper = new QSignalMapper(this);
	connect(connectionsSignalMapper, SIGNAL(mapped(const QString &)),
			this, SLOT(connectToFromMenu(const QString &)));

	// Set focus
	ChannelWidget *channelWidget = qobject_cast<ChannelWidget*>(mtwMain->focusedWidget());
	if (channelWidget)
	{
		mtwMain->focusWidget(channelWidget);
		channelWidget->applyFirstShow();
	}

	// Start autoupdate stuff
	connect(&autoUpdate, SIGNAL(newVersion(const QDate &)), this, SLOT(newProgramVersion(const QDate &)));
	autoUpdate.checkForUpdate();

	animationTimer.setInterval(10);
	connect(&animationTimer, SIGNAL(timeout()), this, SLOT(animationTimeout()));
	animationTimer.start();

	m_firstShow = true;

	actionSignalMapper = new QSignalMapper(this);
	connect(actionSignalMapper, SIGNAL(mapped(int)),
			this, SLOT(executeAction(int)));
	for (int i = 0; i < profile.actionManager.actions().count(); ++i)
	{
		const Action &action = profile.actionManager.actions()[i];
		QShortcut *shortcut = new QShortcut(action.keySequence(), this);
		connect(shortcut, SIGNAL(activated()), actionSignalMapper, SLOT(map()));
		actionSignalMapper->setMapping(shortcut, action.actionType());
	}
}

MainWindow::~MainWindow()
{
	// Free some singletons
	SessionManager::free();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	// All finalization stuff must be done here
	recordCurrentProfileDatas();

	// Save current profile
	ProfileManager::instance().currentProfile()->save();

	// Hide tray
	trayIcon->hide();

	event->accept();
}

void MainWindow::recordCurrentProfileDatas()
{
	Profile &profile = *ProfileManager::instance().currentProfile();

	QPoint p = pos();
	QSize s = size();
	profile.mainWidth = s.width();
	profile.mainHeight = s.height();
	profile.mainLeft = p.x();
	profile.mainTop = p.y();

	// System logs
	profile.systemLogsVisible = systemDialog->isVisible();
	profile.systemLogsLeft = systemDialog->x();
	profile.systemLogsTop = systemDialog->y();
	profile.systemLogsWidth = systemDialog->width();
	profile.systemLogsHeight = systemDialog->height();
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
	actionReconnect->setStatusTip(tr("Reconnect"));
	actionReconnect->setEnabled(false);
	connect(actionReconnect, SIGNAL(triggered()), this, SLOT(reconnect()));

	// Close connection action
	actionCloseConnection = mConnections->addAction(tr("&Close connection"));
	actionCloseConnection->setStatusTip(tr("Close current connection"));
	actionCloseConnection->setEnabled(false);
	connect(actionCloseConnection, SIGNAL(triggered()), this, SLOT(closeConnection()));

	mConnections->addSeparator();
	QAction *action = mConnections->addAction(tr("&Quit"));
	connect(action, SIGNAL(triggered()), this, SLOT(close()));

	// Edit menu
	QMenu *mEdit = mbMain->addMenu(tr("&Edit"));
	actionEditConnectionConfig = mEdit->addAction(tr("&Connection configuration..."));
	actionEditConnectionConfig->setEnabled(false);
	connect(actionEditConnectionConfig, SIGNAL(triggered()), this, SLOT(editConnectionConfig()));
	mEdit->addSeparator();
	connect(mEdit->addAction(tr("&General settings...")), SIGNAL(triggered()), this, SLOT(editSettings()));

	// View menu
	QMenu *menuView = mbMain->addMenu(tr("&View"));

	Profile &profile = *ProfileManager::instance().currentProfile();

	actionViewTopic = menuView->addAction(tr("&Topic window"));
	actionViewTopic->setCheckable(true);
	connect(actionViewTopic, SIGNAL(triggered()), this, SLOT(showTopicWindow()));
	actionViewTopic->setChecked(profile.topicWindowVisible);

	menuView->addSeparator();

	actionSystemLogs = menuView->addAction(tr("&System logs"));
	actionSystemLogs->setCheckable(true);
	connect(actionSystemLogs, SIGNAL(triggered()), this, SLOT(showSystemLogs()));
	actionSystemLogs->setChecked(profile.systemLogsVisible);

	connect(menuView->addAction(tr("Open &logs directory")), SIGNAL(triggered()), this, SLOT(showLogsDir()));
	connect(menuView->addAction(tr("&Messages")), SIGNAL(triggered()), this, SLOT(showMessages()));
// TEMP	connect(menuView->addAction(tr("&File transfers")), SIGNAL(triggered()), this, SLOT(showTransfers()));

	// Help menu
	QMenu *menuHelp = mbMain->addMenu(tr("&Help"));
#ifdef Q_OS_WIN32
	connect(menuHelp->addAction(tr("&Check for update...")), SIGNAL(triggered()), this, SLOT(checkForUpdate()));
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
	Profile &profile = *ProfileManager::instance().currentProfile();

	if (message->message == WM_SYSCOMMAND && message->wParam == SC_MINIMIZE)
	{
		if (profile.trayEnabled && profile.trayHideFromTaskBar)
			hide();
		if (profile.trayEnabled && !profile.trayAlwaysVisible)
			trayIcon->show();
		trayTalkAboutMe = false;
		if (profile.trayEnabled && profile.trayHideFromTaskBar)
		{
			(*result) = false;
			return true;
		}
	}
	return QMainWindow::winEvent(message, result);
}

#endif

void MainWindow::makeStatusBar()
{
	sbMain = new QStatusBar(this);
	setStatusBar(sbMain);
	sbMain->showMessage("Loading...");
	sbMain->hide();
}

void MainWindow::makeConnectionsActions()
{
	mConnectTo->clear();

	const Profile &profile = *ProfileManager::instance().currentProfile();
	foreach (SessionConfig *config, profile.sessionConfigs())
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
	Profile &profile = *ProfileManager::instance().currentProfile();
	SessionConfig templateConfig = SessionConfig::getTemplate();
	templateConfig.setName(profile.getUniqSessionConfigName());
	DialogSessionConfig dialogSessionConfig(templateConfig, this);
	if (dialogSessionConfig.exec() == QDialog::Accepted)
	{
		SessionConfig newConfig;
		dialogSessionConfig.get(newConfig);
		profile.addSessionConfig(newConfig);

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
	Profile &profile = *ProfileManager::instance().currentProfile();
	SessionConfig *pConfig =
		profile.getSessionConfig(getCurrentSession()->config().name());

	DialogSessionConfig dialogSessionConfig(*pConfig, this);
	if (dialogSessionConfig.exec() == QDialog::Accepted)
	{
		QString oldSessionConfigName = pConfig->name();
		dialogSessionConfig.get(*pConfig);
		getCurrentSession()->setConfig(*pConfig);

		// Change super label
		mtwMain->renameSuperLabel(oldSessionConfigName, pConfig->name());
	}
}

void MainWindow::editSettings()
{
	DialogSettings dialog(this);
	if (dialog.exec() == QDialog::Accepted)
		refreshProfileSettings();
}

void MainWindow::showSystemLogs()
{
	if (systemDialog->isVisible())
		systemDialog->hide();
	else
		systemDialog->show();
}

void MainWindow::showTopicWindow()
{
	// Change profile datas
	Profile &profile = *ProfileManager::instance().currentProfile();
	profile.topicWindowVisible = actionViewTopic->isChecked();

	// Change controls
	for (int i = 0; i < mtwMain->count(); i++)
	{
		ChannelWidget *w = qobject_cast<ChannelWidget*>(mtwMain->widget(i));
		if (w)
		{
			if (profile.topicWindowVisible)
				w->showTopicWindow();
			else
				w->hideTopicWindow();
		}
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
	if (mtwMain->superLabels().indexOf(config.name()) >= 0)
	{
		SessionManager &sessionManager = SessionManager::instance();
		QList<Session*> &sessionList = sessionManager.sessionsList();
		foreach (Session *session, sessionList)
		{
			if (session->config().name() == config.name())
			{
				ChannelWidget *widget = getChannelWidget(session);
				mtwMain->focusWidget(widget);
				session->start();
				return 0;
			}
		}

		return 0;
	}

	// Create a session
	Session *session = SessionManager::instance().newSession(config);

	// Create a new ChannelWidget and add it
	ChannelWidget *channelWidget = new ChannelWidget(session);
	connect(channelWidget, SIGNAL(moveLeft()), mtwMain, SLOT(rotateCurrentPageToLeft()));
	connect(channelWidget, SIGNAL(moveRight()), mtwMain, SLOT(rotateCurrentPageToRight()));
	connect(channelWidget, SIGNAL(highlightMe()), this, SLOT(highlightSessionWidget()));
	connect(channelWidget, SIGNAL(whoUserDoubleClicked(const QString&)),
			this, SLOT(whoUserDoubleClicked(const QString&)));
	connect(channelWidget, SIGNAL(tellSessionAsked(const QString&)),
			this, SLOT(tellSessionAsked(const QString&)));
	connect(channelWidget, SIGNAL(showFileTransfers()), this, SLOT(showFileTransfers()));

	mtwMain->addWidget(config.name(), channelWidget, "Hall");

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
	const Profile &profile = *ProfileManager::instance().currentProfile();
	connectTo(*profile.getSessionConfig(configName));
}

void MainWindow::connectToFromMenu(const QString &configName)
{
	const Profile &profile = *ProfileManager::instance().currentProfile();
	ChannelWidget *widget = connectTo(*profile.getSessionConfig(configName));
	if (widget)
		widget->applyFirstShow();
}

void MainWindow::hideSystemDialog()
{
	actionSystemLogs->setChecked(false);
}

void MainWindow::refreshProfileSettings()
{
	Profile &profile = *ProfileManager::instance().currentProfile();

	// Tray stuffs
	if (profile.trayEnabled)
	{
		if (profile.trayHideFromTaskBar && isMinimized())
			hide();
		else if (!profile.trayHideFromTaskBar && !isVisible())
			trayIcon->show();
		else
		{
			if (profile.trayAlwaysVisible)
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

	// Tabs stuffs
	applyProfileOnMultiTabWidget();

	// Refresh widgets
	refreshWidgets();
}

void MainWindow::refreshWidgets()
{
	for (int i = 0; i < mtwMain->count(); i++)
	{
		ChannelWidget *w = qobject_cast<ChannelWidget*>(mtwMain->widget(i));
		if (w)
		{
			w->refreshKeepAlivePolicy();
			w->refreshFonts();
		}
		else
		{
			TellWidget *w = qobject_cast<TellWidget*>(mtwMain->widget(i));
			if (w)
				w->refreshFonts();
		}
	}
}

void MainWindow::aboutToShowConnectionsActions()
{
	makeConnectionsActions();
}

void MainWindow::newSessionTokenForActivity(Session *session, const TokenEvent &event)
{
	if (event.ticketID() >= 0)
		return;

	Profile &profile = *ProfileManager::instance().currentProfile();

	if ((profile.trayHideFromTaskBar && !isVisible()) || QApplication::activeWindow() != this)
	{
		bool showWarningo = false;
		bool changeTray = false;
		switch(event.token())
		{
		case Token_SomeoneTellsYou:
		case Token_SomeoneAsksYou:
		case Token_SomeoneReplies:
			showWarningo = profile.warningoPrivate;
			changeTray = true;
			trayTalkAboutMe = true;
			break;
		case Token_YouAreKicked:
			showWarningo = true;
			changeTray = true;
			trayTalkAboutMe = true;
			break;
		case Token_SomeoneSays:
			{
				// Someone talks about you?
				QString sentence = event.arguments()[2];
				if (sentence.indexOf(session->regExpAboutMe()) >= 0)
				{
					showWarningo = profile.warningoHighlight;
					changeTray = true;
					trayTalkAboutMe = true;
				}
			}
			break;
		default:;
		}

		if (showWarningo || changeTray)
		{
			if (changeTray)
			        trayIcon->setIcon(QIcon(":/images/tray-myself.png"));
			if (showWarningo && profile.warningoEnabled)
			{
				DialogWarningo *dialog = new DialogWarningo(session->config().name(), event.line(), 0);
				dialog->move(0, 0);
			}
		}
		else if (!trayTalkAboutMe)
			trayIcon->setIcon(QPixmap(":/images/tray-new.png"));
 	}
}

void MainWindow::newSessionToken(Session *session, const TokenEvent &event)
{
	Profile &profile = *ProfileManager::instance().currentProfile();

	switch(event.token())
	{
	case Token_LoginAsked:
		if (!session->config().furtiveMode() && !session->config().login().isEmpty())
			session->send(session->config().login());
		break;
	case Token_InvalidLogin:
		{
			SessionConfig newConfig = session->config();
			newConfig.setLogin("");
			session->setConfig(newConfig);
		}
		break;
	case Token_PasswordAsked:
		if (!session->config().furtiveMode() && !session->config().password().isEmpty())
			session->send(session->config().password());
		break;
	case Token_IncorrectPassword:
		{
			SessionConfig newConfig = session->config();
			newConfig.setLogin("");
			newConfig.setPassword("");
			session->setConfig(newConfig);
		}
		break;
	case Token_SomeoneTellsYou:
	case Token_SomeoneAsksYou:
	case Token_SomeoneReplies:
		if (profile.soundAboutMeEnabled)
		{
			QSound s(profile.getAboutMeFileName());
			s.play();
		}
	case Token_YouTellToSomeone:
	case Token_YouAskToSomeone:
	case Token_YouReply:
	{
		QString login = event.arguments()[1];
		TellWidget *widget = getTellWidget(session, login);
		if (!widget)
		{
			widget = newTellWidget(session, login);
			widget->newTokenFromSession(event);
		}
	}
	break;
	case Token_SomeoneBeepsYou:
		if (profile.soundBeepEnabled)
		{
			QSound s(profile.getBeepFileName());
			s.play();
		}
		break;
	case Token_WallBegin:
		// New tab?
		if (event.ticketID() == -1 && profile.tabForWall)
		{
			CmdOutputWidget *widget = getCmdOutputWidget(session, "wall");
			if (!widget)
			{
				widget = newCmdOutputWidget(session, "wall");
				widget->newTokenFromSession(event);
			}
			mtwMain->focusWidget(widget);
		}
		break;
	case Token_FingerBegin:
		// New tab?
		if (event.ticketID() == -1 && profile.tabForFinger)
		{
			CmdOutputWidget *widget = getCmdOutputWidget(session, "finger");
			if (!widget)
			{
				widget = newCmdOutputWidget(session, "finger");
				widget->newTokenFromSession(event);
			}
			mtwMain->focusWidget(widget);
		}
		break;
	case Token_WhoBegin:
		// New tab?
		if (event.ticketID() == -1 && profile.tabForWho)
		{
			CmdOutputWidget *widget = getCmdOutputWidget(session, "who");
			if (!widget)
			{
				widget = newCmdOutputWidget(session, "who");
				widget->newTokenFromSession(event);
			}
			mtwMain->focusWidget(widget);
		}
		break;
	case Token_WhoLine:
	{
		QString login = event.arguments()[1];
		TellWidget *tellWidget = getTellWidget(session, login);
		if (tellWidget)
		{
			if (event.arguments()[4] == "*Away*")
				mtwMain->renameLabel(tellWidget, login + " (away)");
			else
				mtwMain->renameLabel(tellWidget, login);
		}
	}
	break;
	case Token_WhoEnd:
		// Check for tell tabs
		for (int i = 0; i < mtwMain->count(); i++)
		{
			TellWidget *tellWidget = qobject_cast<TellWidget*>(mtwMain->widget(i));
			if (tellWidget && tellWidget->session() == session)
			{
				QString tellLogin = tellWidget->login();
				if (!session->users().contains(tellLogin))
					mtwMain->renameLabel(tellWidget, tellLogin + " (quit)");
			}
		}
		break;
	case Token_SomeoneAway:
	case Token_YouAway:
	{
		QString login;
		if (event.token() == Token_YouAway)
			login = session->serverLogin();
		else
			login = event.arguments()[1];

		// Tell widget
		TellWidget *tellWidget = getTellWidget(session, login);
		if (tellWidget)
		{
			tellWidget->setUserAway(true);
			mtwMain->renameLabel(tellWidget, tellWidget->caption());
		}
	}
	break;
	case Token_SomeoneBack:
	case Token_YouBack:
	{
		QString login;
		if (event.token() == Token_YouBack)
			login = session->serverLogin();
		else
			login = event.arguments()[1];

		// Tell widget
		TellWidget *tellWidget = getTellWidget(session, login);
		if (tellWidget)
		{
			tellWidget->setUserAway(false);
			mtwMain->renameLabel(tellWidget, tellWidget->caption());
		}
	}
	break;
	case Token_SomeoneAwayWarning:
	{
		// Tell widget
		TellWidget *tellWidget = getTellWidget(session, event.arguments()[1]);
		if (tellWidget)
			mtwMain->renameLabel(tellWidget, event.arguments()[1] + " (away)");
	}
	break;
	case Token_UserLoginRenamed:
		sessionLoginChanged(session, event.arguments()[1], event.arguments()[2]);
		break;
	case Token_SomeoneSays:
		{
			// Someone talks about you?
			QString sentence = event.arguments()[2];
			if (sentence.indexOf(session->regExpAboutMe()) >= 0)
			{
				if (profile.soundAboutMeEnabled)
				{
					QSound s(profile.getAboutMeFileName());
					s.play();
				}
			}
		}
		break;
	case Token_SomeoneComesIn:
	{
		TellWidget *tellWidget = getTellWidget(session, event.arguments()[1]);
		if (tellWidget)
			mtwMain->renameLabel(tellWidget, event.arguments()[1]);
	}
	break;
	case Token_SomeoneLeaves:
	case Token_SomeoneDisconnects:
	case Token_SomeoneIsKicked:
	case Token_YouKickSomeone:
	{
		TellWidget *tellWidget = getTellWidget(session, event.arguments()[1]);
		if (tellWidget)
			mtwMain->renameLabel(tellWidget, event.arguments()[1] + " (quit)");
	}
	break;
	case Token_YouJoinChannel:
		mtwMain->renameLabel(getChannelWidget(session), event.arguments()[1]);
		break;
	case Token_YouLeaveChannel:
		mtwMain->renameLabel(getChannelWidget(session), "Hall");
		break;
	break;
	default:;
	}
}

void MainWindow::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
	if (reason != QSystemTrayIcon::Trigger)
		return;

	Profile &profile = *ProfileManager::instance().currentProfile();
	if (profile.trayEnabled && profile.trayHideFromTaskBar)
		show();
	if (isMinimized())
		showNormal();
	activateWindow();
	if (profile.trayEnabled && !profile.trayAlwaysVisible)
		trayIcon->hide();
}

bool MainWindow::event(QEvent *e)
{
	if (e->type() == QEvent::WindowActivate)
	{
		trayIcon->setIcon(QIcon(":/images/tray-neutral.png"));
		trayTalkAboutMe = false;
	}
	return QMainWindow::event(e);
}

void MainWindow::showEvent(QShowEvent *)
{
	if (m_firstShow)
	{
		m_firstShow = false;

		Profile &profile = *ProfileManager::instance().currentProfile();
		if (VERSION != profile.clientVersion)
			launchWhatsNew();

		// Default connect to
		if (!profile.sessionConfigs().count())
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

void MainWindow::applyProfileOnMultiTabWidget()
{
	Profile &profile = *ProfileManager::instance().currentProfile();

	// Display mode
	mtwMain->setDisplayMode(profile.tabsAllInOne ? MultiTabWidget::DisplayMode_AllInOneRow :
		MultiTabWidget::DisplayMode_Hierarchical);

	// Tabs location
	mtwMain->setAllInOneRowLocation(profile.tabsAllInTop ? MultiTabWidget::TabLocation_North :
		MultiTabWidget::TabLocation_South);
	mtwMain->setSuperLocation(profile.tabsSuperOnTop ? MultiTabWidget::TabLocation_North :
		MultiTabWidget::TabLocation_South);
	mtwMain->setSubLocation(profile.tabsOnTop ? MultiTabWidget::TabLocation_North :
		MultiTabWidget::TabLocation_South);
}

Session *MainWindow::getCurrentSession()
{
	SessionWidget *w = qobject_cast<SessionWidget*>(mtwMain->focusedWidget());
	if (w)
		return w->session();
	return 0;
}

void MainWindow::closeCurrentSession()
{
	SessionWidget *w = qobject_cast<SessionWidget*>(mtwMain->focusedWidget());
	if (w)
	{
		Session *session = w->session();
		mtwMain->removeSuperWidgets(session->config().name());
		if (!SessionManager::destroyed())
			SessionManager::instance().removeSession(session);
	}
}

void MainWindow::whoItemDblClicked(const QString &login)
{
	Session *session = qobject_cast<ChannelWidget*>(sender())->session();

	// Initiate a tell widget
	TellWidget *w = getTellWidget(session, login);
	if (!w)
		w = newTellWidget(session, login);

	// Just focus the tell widget
	mtwMain->focusWidget(w);
}

TellWidget *MainWindow::getTellWidget(Session *session, const QString &login)
{
	for (int i = 0; i < mtwMain->count(); i++)
	{
		TellWidget *w = qobject_cast<TellWidget*>(mtwMain->widget(i));
		if (w && w->session() == session && w->login() == login)
			return w;
	}
	return 0;
}

TellWidget *MainWindow::newTellWidget(Session *session, const QString &login)
{
	TellWidget *w = new TellWidget(session, login);
	connect(w, SIGNAL(moveLeft()), mtwMain, SLOT(rotateCurrentPageToLeft()));
	connect(w, SIGNAL(moveRight()), mtwMain, SLOT(rotateCurrentPageToRight()));
	connect(w, SIGNAL(closeMe()), this, SLOT(closeTabWidget()));
	connect(w, SIGNAL(highlightMe()), this, SLOT(highlightSessionWidget()));

	mtwMain->addWidget(session->config().name(), w, login, false);

	return w;
}

void MainWindow::closeTabWidget()
{
	mtwMain->removeWidget(qobject_cast<QWidget*>(sender()));
}

void MainWindow::highlightSessionWidget()
{
	Profile &profile = *ProfileManager::instance().currentProfile();
	SessionWidget *widget = qobject_cast<SessionWidget*>(sender());

	switch (widget->highlightType())
	{
	case SessionWidget::NoHighlight:
		if (!profile.tabsIcons)
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

		if (!profile.tabsIcons)
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

		if (!profile.tabsIcons)
		{
			widget->setStared(true);
			mtwMain->renameLabel(widget, widget->caption());
		}
		tabToBlinkTime[widget] = 5000;
		tabToAscendingOrder[widget] = false;
		mtwMain->changeTabTextColor(widget, Qt::red);
		break;
	}
}

CmdOutputWidget *MainWindow::getCmdOutputWidget(Session *session, const QString &cmdName)
{
	for (int i = 0; i < mtwMain->count(); i++)
	{
		CmdOutputWidget *w = qobject_cast<CmdOutputWidget*>(mtwMain->widget(i));
		if (w && w->session() == session && w->cmdName() == cmdName)
			return w;
	}
	return 0;
}

CmdOutputWidget *MainWindow::newCmdOutputWidget(Session *session, const QString &cmdName)
{
	CmdOutputWidget *w = new CmdOutputWidget(session, cmdName);
	connect(w, SIGNAL(moveLeft()), mtwMain, SLOT(rotateCurrentPageToLeft()));
	connect(w, SIGNAL(moveRight()), mtwMain, SLOT(rotateCurrentPageToRight()));
	connect(w, SIGNAL(closeMe()), this, SLOT(closeTabWidget()));

	mtwMain->addWidget(session->config().name(), w, cmdName);

	return w;
}

TransfersWidget *MainWindow::getTransfersWidget(Session *session)
{
	for (int i = 0; i < mtwMain->count(); i++)
	{
		TransfersWidget *w = qobject_cast<TransfersWidget*>(mtwMain->widget(i));
		if (w && w->session() == session)
			return w;
	}
	return 0;	
}

TransfersWidget *MainWindow::newTransfersWidget(Session *session)
{
	TransfersWidget *w = new TransfersWidget(session);
	connect(w, SIGNAL(moveLeft()), mtwMain, SLOT(rotateCurrentPageToLeft()));
	connect(w, SIGNAL(moveRight()), mtwMain, SLOT(rotateCurrentPageToRight()));
	connect(w, SIGNAL(closeMe()), this, SLOT(closeTabWidget()));

	mtwMain->addWidget(session->config().name(), w, tr("File transfers"), false);

	return w;
}

void MainWindow::sessionLoginChanged(Session *session, const QString &oldLogin, const QString &newLogin)
{
	TellWidget *tellWidget = getTellWidget(session, oldLogin);
	if (tellWidget)
	{
		mtwMain->renameLabel(tellWidget, newLogin);
		tellWidget->setLogin(newLogin);
	}
}

void MainWindow::focusedWidgetChanged(QWidget *widget)
{
	Profile &profile = *ProfileManager::instance().currentProfile();

	SessionWidget *sessionWidget = qobject_cast<SessionWidget*>(widget);
	if (sessionWidget)
	{
		sessionWidget->focusIt();
		sessionWidget->setStared(false);
		if (!profile.tabsIcons)
			mtwMain->renameLabel(sessionWidget, sessionWidget->caption());
		mtwMain->changeTabTextColor(sessionWidget, mtwMain->palette().color(QPalette::WindowText));
		sessionWidget->applyFirstShow();
		return;
	}
}

ChannelWidget *MainWindow::getChannelWidget(Session *session)
{
	for (int i = 0; i < mtwMain->count(); i++)
	{
		ChannelWidget *w = qobject_cast<ChannelWidget*>(mtwMain->widget(i));
		if (w && w->session() == session)
			return w;
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
	mtwMain->focusWidget(widget);
}

void MainWindow::tellSessionAsked(const QString &login)
{
	whoUserDoubleClicked(login);
}

void MainWindow::showLogsDir()
{
	Profile &profile = *ProfileManager::instance().currentProfile();
	
	// Determine the QUrl of the logs path
	QUrl url;
	if (profile.logsDefaultDir)
		url = QUrl::fromLocalFile(QDir(QApplication::applicationDirPath()).filePath("logs"));
	else
		url = QUrl::fromLocalFile(profile.logsDir);

	// Launch it
	QDesktopServices::openUrl(url);
}

void MainWindow::showMessages()
{
	Session *session = getCurrentSession();

	// Initiate a message widget
	MessageWidget *widget = getMessageWidget(session);
	if (!widget)
		widget = newMessageWidget(session);

	// Just focus it
	mtwMain->focusWidget(widget);
}

void MainWindow::showTransfers()
{
	Session *session = getCurrentSession();

	// Initiate a message widget
	TransfersWidget *widget = getTransfersWidget(session);
	if (!widget)
		widget = newTransfersWidget(session);

	// Just focus it
	mtwMain->focusWidget(widget);
}

MessageWidget *MainWindow::getMessageWidget(Session *session)
{
	for (int i = 0; i < mtwMain->count(); i++)
	{
		MessageWidget *w = qobject_cast<MessageWidget*>(mtwMain->widget(i));
		if (w && w->session() == session)
			return w;
	}
	return 0;
}

MessageWidget *MainWindow::newMessageWidget(Session *session)
{
	MessageWidget *w = new MessageWidget(session);
	connect(w, SIGNAL(moveLeft()), mtwMain, SLOT(rotateCurrentPageToLeft()));
	connect(w, SIGNAL(moveRight()), mtwMain, SLOT(rotateCurrentPageToRight()));
	connect(w, SIGNAL(closeMe()), this, SLOT(closeTabWidget()));

	mtwMain->addWidget(session->config().name(), w, "Messages", false);

	return w;
}

void MainWindow::launchWhatsNew()
{
	DialogWhatsNew *dialog = new DialogWhatsNew(this);
	dialog->show();
}

void MainWindow::launchBugReport()
{
	QString subject = "Bug report";
	QString body = "Hi!\n\nI'm happy (can be ironic) with your <Mtp> Chat! client but sometimes, it really sucks.\n\n\
Here are the problems:\n\n* ";

	QDesktopServices::openUrl(QUrl("mailto:?To=cebmtp@free.fr&subject=" + subject + "&body=" + body));
}

void MainWindow::launchFeatureReport()
{
	QString subject = "Wanted feature report";
	QString body = "Hi!\n\nI'm happy (can be ironic) with your <Mtp> Chat! client but sometimes, it really sucks.\n\n\
Here are the missing features I'd like to see in the next version:\n\n* ";

	QDesktopServices::openUrl(QUrl("mailto:?To=cebmtp@free.fr&subject=" + subject + "&body=" + body));
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

void MainWindow::animationTimeout()
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
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
	if (watched == mtwMain)
	{
		if (event->type() == QEvent::MouseButtonPress)
		{
			QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
			QWidget *widget = mtwMain->widgetByTabLocation(QPoint(mouseEvent->globalX(), mouseEvent->globalY()));
			ChannelWidget *channelWidget = qobject_cast<ChannelWidget*>(widget);
			if (channelWidget)
				return true;
			SessionWidget *sessionWidget = qobject_cast<SessionWidget*>(widget);
			if (sessionWidget)
			{
				mtwMain->removeWidget(sessionWidget);
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
	mtwMain->focusWidget(widget);
}

void MainWindow::newTransfer(Transfer *transfer)
{
	// Initiate a transfers widget
	TransfersWidget *widget = getTransfersWidget(transfer->session());
	if (!widget)
		widget = newTransfersWidget(transfer->session());
	
	// Just focus it
	mtwMain->focusWidget(widget);
}

void MainWindow::executeAction(int action)
{
	// Get current session
	SessionWidget *sessionWidget = qobject_cast<SessionWidget*>(mtwMain->focusedWidget());
	if (!sessionWidget)
		return;

	Session *session = sessionWidget->session();

	switch ((Action::ActionType) action)
	{
	case Action::Action_RefreshWhoColumn:
		if (!session->isLogged())
			return;
		getChannelWidget(session)->refreshWhoColumn();
		break;
	case Action::Action_ToggleAway:
		if (!session->isLogged())
			return;
		
		if (session->away())
			session->send("set away off");
		else
			session->send("set away on");
		break;
	case Action::Action_Reconnect:
		session->resetBackupServers();
		session->start();
		break;
	default:;		
	}
}
