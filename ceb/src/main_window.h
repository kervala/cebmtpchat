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

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QMenu>
#include <QAction>
#include <QTabWidget>
#include <QSignalMapper>
#include <QSystemTrayIcon>

#include <multi_tab_widget.h>

#include "session.h"
#include "dialog_system.h"
#include "autoupdate.h"
#include "channel_widget.h"
#include "tell_widget.h"
#include "cmd_output_widget.h"
#include "message_widget.h"
#include "transfers_widget.h"
#include "my_menubar.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);
#if defined(Q_OS_WIN32)
    bool winEvent(MSG *message, long *result);
#endif
    bool event(QEvent *e);
    void showEvent(QShowEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    // Menu bar
    MyMenuBar *mbMain;
    // Connections menu
    QMenu *mConnectTo;
    QAction *actionReconnect;
    QAction *actionCloseConnection;
    QSignalMapper *connectionsSignalMapper;
    // Edit menu
    QAction *actionEditConnectionConfig;
    // View menu
    QAction *actionSystemLogs;
    QAction *actionViewTopic;
    // Help menu
#ifdef Q_OS_WIN32
    QAction *actionCheckForUpdate;
#endif // Q_OS_WIN32
    // Status bar
    QStatusBar *sbMain;
    // Main MultiTabWidget
    MultiTabWidget *mtwMain;
    // System dialog
    DialogSystem *systemDialog;
    QTimer animationTimer;
    QMap<QWidget*,int> tabToBlinkTime;
    QMap<QWidget*,bool> tabToAscendingOrder;
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    bool trayTalkAboutMe;
    AutoUpdate autoUpdate;
    QSignalMapper *actionSignalMapper;

    void makeMenuBar();
    void makeToolBar();
    void makeStatusBar();
    void makeConnectionsActions();

    void recordCurrentProfileDatas();
    Session *getCurrentSession();
    void closeCurrentSession();

    // Channel things
    ChannelWidget *getChannelWidget(Session *session);

    // Tell things
    TellWidget *getTellWidget(Session *session, const QString &login);
    TellWidget *newTellWidget(Session *session, const QString &login);

    // Cmd output things
    CmdOutputWidget *getCmdOutputWidget(Session *session, const QString &cmdName);
    CmdOutputWidget *newCmdOutputWidget(Session *session, const QString &cmdName);

    // File transfers things
    TransfersWidget *getTransfersWidget(Session *session);
    TransfersWidget *newTransfersWidget(Session *session);

    // Message things
    MessageWidget *getMessageWidget(Session *session);
    MessageWidget *newMessageWidget(Session *session);

    void refreshProfileSettings(); // Refresh profile settings on every widget
    void applyProfileOnMultiTabWidget();
    void refreshWidgets();

    ChannelWidget *connectTo(SessionConfig &config);

private:
    bool m_firstShow;

private slots:
    void newConnection();
    void reconnect();
    void closeConnection();
    void editConnectionConfig();
    void editSettings();
    void showSystemLogs();
    void showTopicWindow();
    void checkForUpdate();
    void about();
    void aboutQt();
    void connectTo(const QString &configName);
    void connectToFromMenu(const QString &configName);
    void hideSystemDialog();
    void aboutToShowConnectionsActions();
    void newSessionTokenForActivity(Session *session, const TokenEvent &event);
    void newSessionToken(Session *session, const TokenEvent &event);
    void sessionLoginChanged(Session *session, const QString &oldLogin, const QString &newLogin);
    void trayActivated(QSystemTrayIcon::ActivationReason reason);
    void newProgramVersion(const QDate &);
    void whoItemDblClicked(const QString &login);
    void closeTabWidget();
    void highlightSessionWidget();
    void focusedWidgetChanged(QWidget *widget);
    void whoUserDoubleClicked(const QString &login);
    void showLogsDir();
    void showMessages();
    void showTransfers();
    void launchWhatsNew();
    void launchBugReport();
    void launchFeatureReport();
    void menuIconClicked();
    void updateAccepted();
    void tellSessionAsked(const QString &login);
    void animationTimeout();
    void showFileTransfers();
    void newTransfer(Transfer *transfer);
    void executeAction(int action);
    void searchActionTriggered();
};

#endif // MAIN_WINDOW_H
